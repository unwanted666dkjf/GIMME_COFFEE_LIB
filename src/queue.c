#include "../headers/queue.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct gc_Queue {
	gc_VoidPtr* array;
	gc_SizeT size;
	gc_SizeT length;
	gc_SizeT head;
	gc_SizeT tail;
};

gc_Queue* gc_Queue_init(gc_SizeT size) {
	gc_Queue* self = (gc_Queue*)gc_malloc(gc_sizeof(gc_Queue));
	if (gc_not self) {
		gc_printf("Could not create gc_Queue!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->size = size;
	self->array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * self->size);
	if (gc_not self->array) {
		gc_printf("Could not create array for gc_Queue!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (gc_SizeT i = 0; i < self->size; i++) {
		self->array[i] = NONE;
	}
	self->head = 0;
	self->tail = 0;
	self->length = 0;
	return self;
}

gc_VoidPtr gc_Queue_del(gc_Queue* obj) {
	gc_free(obj->array);
	gc_free(obj);
	return NONE;
}

gc_SizeT gc_Queue_size(gc_Queue* self) {
	return self->size;
}

gc_SizeT gc_Queue_length(gc_Queue* self) {
	return self->length;
}

gc_Bool gc_Queue_is_full(gc_Queue* self) {
	return self->length >= self->size;
}

gc_Bool gc_Queue_is_empty(gc_Queue* self) {
	return self->length == 0;
}

gc_Iterator* gc_Queue_iter(gc_Queue* self) {
	gc_Iterator* iter = gc_Iterator_init(self->length);
	for (gc_SizeT i = 0; i < self->length; i++) {
		gc_Iterator_append(iter, self->array[i]);
	}
	return iter;
}

gc_VoidPtr gc_Queue_front(gc_Queue* self) {
	return self->array[self->head];
}

gc_VoidPtr gc_Queue_dequeue(gc_Queue* self) {
	gc_VoidPtr item = gc_Queue_front(self);
	self->array[self->head] = NONE;
	self->head = (self->head + 1) % self->size;
	self->length--;
	return item;
}

gc_Void gc_Queue_enqueue(gc_Queue* self, gc_VoidPtr item) {
	self->array[self->tail] = item;
	self->tail = (self->tail + 1) % self->size;
	self->length++;
}
