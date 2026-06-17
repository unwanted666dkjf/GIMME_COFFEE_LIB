#include "../headers/array_list.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct gc_ArrayList {
	gc_VoidPtr* array;
	gc_SizeT size;
	gc_SizeT length;
	gc_Floating expand_k;
};

gc_ArrayList* gc_ArrayList_init(gc_SizeT size, gc_Floating expand_k) {
	gc_ArrayList* self = (gc_ArrayList*)gc_malloc(gc_sizeof(gc_ArrayList));
	if (gc_not self) {
		gc_printf("Could not create gc_ArrayList!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->size = size + 1;
	self->array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * self->size);
	if (gc_not self->array) {
		gc_printf("Could not create array for gc_ArrayList!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (gc_SizeT i = 0; i < self->size; i++) {
		self->array[i] = NONE;
	}
	self->length = 0;
	self->expand_k = expand_k;
	return self;
}

gc_VoidPtr gc_ArrayList_del(gc_ArrayList* obj) {
	gc_free(obj->array);
	gc_free(obj);
	return NONE;
}

gc_SizeT gc_ArrayList_size(gc_ArrayList* self) {
	return self->size;
}

gc_SizeT gc_ArrayList_length(gc_ArrayList* self) {
	return self->length;
}

gc_Void gc_ArrayList_reverse(gc_ArrayList* self) {
	if (self->length > 1) {
		gc_SizeT i = 0;
		for (gc_SizeT j = self->length - 1; j > i; j--) {
			gc_VoidPtr item = self->array[i];
			self->array[i] = self->array[j];
			self->array[j] = item;
			i++;
		}
	}
}

gc_Floating gc_ArrayList_expand_k(gc_ArrayList* self) {
	return self->expand_k;
}

gc_Bool gc_ArrayList_is_empty(gc_ArrayList* self) {
	return self->length == 0;
}

gc_Bool gc_ArrayList_is_dense(gc_ArrayList* self) {
	gc_SizeT size = (gc_SizeT)(self->length * self->expand_k);
	return size > self->size;
}

gc_Bool gc_ArrayList_is_sparse(gc_ArrayList* self) {
	gc_SizeT length = (gc_SizeT)(self->size / self->expand_k);
	return self->length < length;
}

gc_Iterator* gc_ArrayList_iter(gc_ArrayList* self) {
	gc_Iterator* iter = gc_Iterator_init(self->length);
	for (gc_SizeT i = 0; i < self->length; i++) {
		gc_Iterator_append(iter, self->array[i]);
	}
	return iter;
}

gc_VoidPtr gc_ArrayList_first(gc_ArrayList* self) {
	return self->array[0];
}

gc_VoidPtr gc_ArrayList_last(gc_ArrayList* self) {
	return self->array[self->length - 1];
}

gc_VoidPtr gc_ArrayList_pop(gc_ArrayList* self, gc_SizeT index) {
	gc_VoidPtr item = self->array[index];
	self->array[index] = NONE;
	for (gc_SizeT i = index + 1; i < self->length; i++) {
		self->array[i - 1] = self->array[i];
	}
	self->length--;
	return item;
}

gc_VoidPtr gc_ArrayList_get(gc_ArrayList* self, gc_SizeT index) {
	return self->array[index];
}

gc_Void gc_ArrayList_append(gc_ArrayList* self, gc_VoidPtr item) {
	self->array[self->length] = item;
	self->length++;
}

gc_Void gc_ArrayList_expand(gc_ArrayList* self) {
	gc_SizeT new_size = (gc_SizeT)(self->size * self->expand_k) + 1;
	gc_VoidPtr* new_array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * new_size);
	if (gc_not new_array) {
		gc_printf("Could not expand gc_ArrayList!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (gc_SizeT i = 0; i < new_size; i++) {
		if (i < self->size) {
			new_array[i] = self->array[i];
		} else {
			new_array[i] = NONE;
		}
	}
	gc_free(self->array);
	self->size = new_size;
	self->array = new_array;
}

gc_Void gc_ArrayList_shrink(gc_ArrayList* self) {
	gc_SizeT new_size = (gc_SizeT)(self->size / self->expand_k);
	if (new_size > self->length) {
		gc_VoidPtr* new_array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * new_size);
		if (gc_not new_array) {
			gc_printf("Could not shrink gc_ArrayList!\n");
			gc_exit(EXIT_NULLPTR);
		}
		gc_SizeT new_length = 0;
		for (gc_SizeT i = 0; i < new_size; i++) {
			if (i < self->length) {
				new_array[i] = self->array[i];
				new_length++;
			} else {
				new_array[i] = NONE;
			}
		}
		gc_free(self->array);
		self->size = new_size;
		self->length = new_length;
		self->array = new_array;
	}
}

gc_Void gc_ArrayList_extend(gc_ArrayList* self, gc_ArrayList* other) {
	gc_SizeT new_size = self->length + other->length + 1;
	if (new_size < self->size) {
		for (gc_SizeT i = 0; i < other->length; i++) {
			self->array[self->length] = other->array[i];
			self->length++;
		}
	} else {
		gc_VoidPtr* new_array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * new_size);
		if (gc_not new_array) {
			gc_printf("Could not extend gc_ArrayList!\n");
			gc_exit(EXIT_NULLPTR);
		}
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

gc_VoidPtr gc_ArrayList_set(gc_ArrayList* self, gc_SizeT index, gc_VoidPtr item) {
	gc_VoidPtr old = self->array[index];
	self->array[index] = item;
	return old;
}

gc_Void gc_ArrayList_insert(gc_ArrayList* self, gc_SizeT index, gc_VoidPtr item) {
	for (gc_SizeT i = self->length; i > index; i--) {
		self->array[i] = self->array[i - 1];
	}
	self->array[index] = item;
	self->length++;
}

gc_ArrayList* gc_ArrayList_slice(gc_ArrayList* self, gc_SizeT start, gc_SizeT stop, gc_SizeT step) {
	if (start > stop) {
		gc_SizeT s = stop;
		stop = start;
		start = s;
	}
	gc_SizeT size;
	gc_Floating expected_size = 1. * (stop - start) / step;
	if (expected_size > (gc_SizeT)expected_size) {
		size = (gc_SizeT)expected_size + 1;
	} else {
		size = (gc_SizeT)expected_size;
	}
	gc_ArrayList* slice = gc_ArrayList_init(size, self->expand_k);
	while (start < stop) {
		gc_ArrayList_append(slice, self->array[start]);
		start += step;
	}
	return slice;
}
