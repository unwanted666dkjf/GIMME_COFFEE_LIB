#include "../headers/iterator.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct gc_Iterator {
	gc_VoidPtr* array;
	gc_SizeT size;
	gc_SizeT length;
	gc_SizeT next_ind;
	gc_Int last_ind;
};

gc_Iterator* gc_Iterator_init(gc_SizeT size) {
	gc_Iterator* self = (gc_Iterator*)gc_malloc(gc_sizeof(gc_Iterator));
	if (gc_not self) {
		gc_printf("Could not create gc_Iterator!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->size = size;
	self->array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * self->size);
	if (gc_not self->array) {
		gc_printf("Could not create array for gc_Iterator!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->length = 0;
	self->next_ind = 0;
	self->last_ind = (gc_Int)self->length - 1;
	return self;
}

gc_VoidPtr gc_Iterator_del(gc_Iterator* obj) {
	gc_free(obj->array);
	gc_free(obj);
	return NONE;
}

gc_Bool gc_Iterator_has_next(gc_Iterator* self) {
	return self->next_ind < self->length;
}

gc_Bool gc_Iterator_has_last(gc_Iterator* self) {
	return self->last_ind >= 0;
}

gc_VoidPtr gc_Iterator_iter(gc_Iterator* self) {
	return self;
}

gc_VoidPtr gc_Iterator_next(gc_Iterator* self) {
	gc_VoidPtr item = self->array[self->next_ind];
	self->array[self->next_ind] = NONE;
	self->next_ind++;
	return item;
}

gc_VoidPtr gc_Iterator_last(gc_Iterator* self) {
	gc_VoidPtr item = self->array[self->last_ind];
	self->array[self->last_ind] = NONE;
	self->last_ind--;
	return item;
}

gc_Void gc_Iterator_append(gc_Iterator* self, gc_VoidPtr item) {
	self->array[self->length] = item;
	self->length++;
	self->last_ind++;
}
