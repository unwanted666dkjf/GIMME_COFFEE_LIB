#ifndef GIMME_COFFEE_QUEUE_H
#define GIMME_COFFEE_QUEUE_H


#include "./iterator.h"


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_Queue;

/**
 * Simple queue.
 * Does not owns it's elements.
 * Allows casting into gc_VoidPtr**.
 */
typedef struct gc_Queue gc_Queue;


/**
 * Creates a queue with size 'size'.
 */
GIMME_COFFEE_API gc_Queue* gc_Queue_init(gc_SizeT size);

/**
 * Deletes queue object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Queue_del(gc_Queue* obj);

/**
 * Returns queue size.
 */
GIMME_COFFEE_API gc_SizeT gc_Queue_size(gc_Queue* self);

/**
 * Returns queue length(number of elements in queue).
 */
GIMME_COFFEE_API gc_SizeT gc_Queue_length(gc_Queue* self);

/**
 * Returns True is queue length is greater or equal to size,
 *otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Queue_is_full(gc_Queue* self);

/**
 * Returns True if queue is empty, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Queue_is_empty(gc_Queue* self);

/**
 * Returns iterator for this queue.
 * Iterator does not owns it's elements.
 */
GIMME_COFFEE_API gc_Iterator* gc_Queue_iter(gc_Queue* self);

/**
 * Returns first element of the queue without removing.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Queue_front(gc_Queue* self);

/**
 * Removes and returns first element of the queue.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Queue_dequeue(gc_Queue* self);

/**
 * Adds element to the end of the queue.
 */
GIMME_COFFEE_API gc_Void gc_Queue_enqueue(gc_Queue* self, gc_VoidPtr item);


#ifdef __cplusplus
}
#endif


#endif
