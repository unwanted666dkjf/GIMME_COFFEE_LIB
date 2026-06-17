#include "../headers/bytes_array.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


const gc_SizeT MIN_GC_BYTES_ARRAY_SIZE_SAFE = 10;

const gc_Floating MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE = 1.5;


struct gc_BytesArray {
	gc_CharPtr array;
	gc_SizeT size;
	gc_SizeT length;
	gc_Floating expand_k;
};

gc_BytesArray* gc_BytesArray_init(gc_SizeT size, gc_Floating expand_k) {
	gc_BytesArray* self = (gc_BytesArray*)gc_malloc(gc_sizeof(gc_BytesArray));
	if (gc_not self) {
		gc_printf("Could not create gc_BytesArray!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->size = size + 1;
	self->array = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * self->size);
	if (gc_not self->array) {
		gc_printf("Could not create array for gc_BytesArray!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->array[0] = '\0';
	self->length = 0;
	self->expand_k = expand_k;
	return self;
}

gc_VoidPtr gc_BytesArray_del(gc_BytesArray* obj) {
	return gc_BytesArray_del_ex(obj, True);
}

gc_VoidPtr gc_BytesArray_del_ex(gc_BytesArray* obj, gc_Bool free_charr) {
	if (free_charr) {
		gc_free(obj->array);
	}
	gc_free(obj);
	return NONE;
}

gc_SizeT gc_BytesArray_size(gc_BytesArray* self) {
	return self->size;
}

gc_SizeT gc_BytesArray_length(gc_BytesArray* self) {
	return self->length;
}

gc_Floating gc_BytesArray_expand_k(gc_BytesArray* self) {
	return self->expand_k;
}

gc_Bool gc_BytesArray_is_empty(gc_BytesArray* self) {
	return self->length == 0;
}

gc_Bool gc_BytesArray_is_dense(gc_BytesArray* self) {
	gc_SizeT size = (gc_SizeT)(self->length * self->expand_k);
	return size > self->size;
}

gc_Bool gc_BytesArray_is_sparse(gc_BytesArray* self) {
	gc_SizeT length = (gc_SizeT)(self->size / self->expand_k);
	return self->length < length;
}

gc_Bool gc_BytesArray_set(gc_BytesArray* self, gc_SizeT index, gc_Char c) {
	if (index < self->length) {
		self->array[index] = c;
		return True;
	}
	return False;
}

gc_Bool gc_BytesArray_insert(gc_BytesArray* self, gc_SizeT index, gc_Char c) {
	if (index < self->length) {
		for (gc_SizeT i = self->length; i > index; i--) {
			self->array[i] = self->array[i - 1];
		}
		self->array[index] = c;
		self->length++;
		return True;
	}
	return False;
}

gc_Char gc_BytesArray_first(gc_BytesArray* self) {
	if (self->length < 1) {
		return '\0';
	}
	return self->array[0];
}

gc_Char gc_BytesArray_last(gc_BytesArray* self) {
	if (self->length < 1) {
		return '\0';
	}
	return self->array[self->length - 1];
}

gc_Char gc_BytesArray_get(gc_BytesArray* self, gc_SizeT index) {
	if (index >= self->length) {
		return '\0';
	}
	return self->array[index];
}

gc_Char gc_BytesArray_pop(gc_BytesArray* self, gc_SizeT index) {
	if (index >= self->length) {
		return '\0';
	}
	gc_Char c = self->array[index];
	for (gc_SizeT i = index + 1; i < self->length; i++) {
		self->array[i - 1] = self->array[i];
	}
	self->length--;
	return c;
}

gc_CharPtr gc_BytesArray_bytes(gc_BytesArray* self) {
	return self->array;
}

gc_ConstCharPtr gc_BytesArray_char_ptr(gc_BytesArray* self) {
	if (self->length < 1) {
		return "\0";
	}
	return (gc_ConstCharPtr)self->array;
}

gc_Iterator* gc_BytesArray_iter(gc_BytesArray* self) {
	gc_Iterator* iter = gc_Iterator_init(self->length);
	for (gc_SizeT i = 0; i < self->length; i++) {
		gc_Iterator_append(iter, &(self->array[i]));
	}
	return iter;
}

gc_BytesArray* gc_BytesArray_slice(gc_BytesArray* self,
	gc_SizeT start,
	gc_SizeT stop,
	gc_SizeT step
) {
	if (start > stop) {
		gc_SizeT s = start;
		start = stop;
		stop = s;
	}
	if (stop == self->length + 1) {
		stop--;
	}
	gc_Floating size = 1. * (stop - start) / step;
	gc_SizeT expected_size = (gc_SizeT)size;
	if (size > expected_size) {
		expected_size++;
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(expected_size);
	gc_BytesArray* slice = gc_BytesArray_init(array_size, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	while (start < stop) {
		gc_BytesArray_append(slice, self->array[start]);
		start += step;
	}
	return slice;
}

gc_Void gc_BytesArray_reverse(gc_BytesArray* self) {
	gc_Int j = (gc_Int)self->length - 1;
	if (j >= 1) {
		for (gc_SizeT i = 0; (gc_Int)i < j; i++) {
			gc_Char c = self->array[j];
			self->array[j] = self->array[i];
			self->array[i] = c;
			j--;
		}
	}
}

gc_Void gc_BytesArray_shrink(gc_BytesArray* self) {
	gc_SizeT new_size = (gc_SizeT)(self->size / self->expand_k);
	if (new_size > self->length) {
		gc_CharPtr new_array = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * new_size);
		if (gc_not new_array) {
			gc_printf("Could not shrink gc_BytesArray!\n");
			gc_exit(EXIT_NULLPTR);
		}
		gc_SizeT new_length = 0;
		for (gc_SizeT i = 0; i < new_size; i++) {
			if (i < self->length) {
				new_array[i] = self->array[i];
				new_length++;
			} else {
				new_array[i] = '\0';
				break;
			}
		}
		gc_free(self->array);
		self->size = new_size;
		self->length = new_length;
		self->array = new_array;
	}
}

gc_Void gc_BytesArray_expand(gc_BytesArray* self) {
	gc_SizeT new_size = (gc_SizeT)(self->size * self->expand_k) + 1;
	gc_CharPtr new_array = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * new_size);
	if (gc_not new_array) {
		gc_printf("Could not expand gc_BytesArray!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (gc_SizeT i = 0; i < new_size; i++) {
		if (i < self->length) {
			new_array[i] = self->array[i];
		} else {
			new_array[i] = '\0';
			break;
		}
	}
	gc_free(self->array);
	self->size = new_size;
	self->array = new_array;
}

gc_Void gc_BytesArray_append(gc_BytesArray* self, gc_Char c) {
	self->array[self->length] = c;
	self->length++;
}

gc_Void gc_BytesArray_extend(gc_BytesArray* self, gc_BytesArray* other) {
	gc_SizeT new_size = self->length + other->length + 1;
	if (new_size < self->size) {
		for (gc_SizeT i = 0; i < other->length; i++) {
			gc_BytesArray_append(self, other->array[i]);
		}
	} else {
		gc_CharPtr new_array = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * new_size);
		gc_SizeT new_length = 0;
		for (gc_SizeT i = 0; i < self->length; i++) {
			new_array[new_length] = self->array[i];
			new_length++;
		}
		for (gc_SizeT i = 0; i < other->length; i++) {
			new_array[new_length] = other->array[i];
			new_length++;
		}
		gc_free(self->array);
		self->size = new_size;
		self->length = new_length;
		self->array = new_array;
	}
}

gc_Void gc_BytesArray_push_byte_str(gc_BytesArray* self, gc_ConstCharPtr byte_str) {
	for (gc_SizeT i = 0; byte_str[i] != '\0'; i++) {
		if (gc_BytesArray_is_dense(self)) {
			gc_BytesArray_expand(self);
		}
		gc_BytesArray_append(self, byte_str[i]);
	}
}


gc_SizeT get_min_size_for_gc_bytes_array(gc_SizeT bytes_size) {
	return (bytes_size > MIN_GC_BYTES_ARRAY_SIZE_SAFE) ? bytes_size : MIN_GC_BYTES_ARRAY_SIZE_SAFE;
}
