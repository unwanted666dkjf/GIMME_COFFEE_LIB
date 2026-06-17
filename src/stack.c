#include "../headers/stack.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct gc_Stack {
	gc_VoidPtr* array;
	gc_SizeT size;
	gc_SizeT length;
};

gc_Stack* gc_Stack_init(gc_SizeT size) {
	gc_Stack* self = (gc_Stack*)gc_malloc(gc_sizeof(gc_Stack));
	if (gc_not self) {
		gc_printf("Could not create gc_Stack!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->size = size;
	self->array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * self->size);
	if (gc_not self->array) {
		gc_printf("Could not create array for gc_Stack!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (gc_SizeT i = 0; i < self->size; i++) {
		self->array[i] = NONE;
	}
	self->length = 0;
	return self;
}

gc_VoidPtr gc_Stack_del(gc_Stack* obj) {
	gc_free(obj->array);
	gc_free(obj);
	return NONE;
}

gc_SizeT gc_Stack_size(gc_Stack* self) {
	return self->size;
}

gc_SizeT gc_Stack_length(gc_Stack* self) {
	return self->length;
}

gc_Bool gc_Stack_is_full(gc_Stack* self) {
	return self->length >= self->size;
}

gc_Bool gc_Stack_is_empty(gc_Stack* self) {
	return self->length == 0;
}

gc_Iterator* gc_Stack_iter(gc_Stack* self) {
	gc_Iterator* iter = gc_Iterator_init(self->length);
	for (gc_SizeT i = 0; i < self->length; i++) {
		gc_Iterator_append(iter, self->array[i]);
	}
	return iter;
}

gc_VoidPtr gc_Stack_top(gc_Stack* self) {
	return self->array[self->length - 1];
}

gc_VoidPtr gc_Stack_pop(gc_Stack* self) {
	gc_VoidPtr item = gc_Stack_top(self);
	self->array[self->length - 1] = NONE;
	self->length--;
	return item;
}

gc_Void gc_Stack_push(gc_Stack* self, gc_VoidPtr item) {
	self->array[self->length] = item;
	self->length++;
}
