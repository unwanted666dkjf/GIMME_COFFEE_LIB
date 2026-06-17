#ifndef GIMME_COFFEE_STACK_H
#define GIMME_COFFEE_STACK_H


#include "./iterator.h"


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_Stack;

/**
 * Simple stack.
 * Does not owns it's elements.
 * Allows casting into gc_VoidPtr**.
 */
typedef struct gc_Stack gc_Stack;


/**
 * Creates stack with size 'size'.
 */
GIMME_COFFEE_API gc_Stack* gc_Stack_init(gc_SizeT size);

/**
 * Deletes stack and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Stack_del(gc_Stack* obj);

/**
 * Returns size of the stack.
 */
GIMME_COFFEE_API gc_SizeT gc_Stack_size(gc_Stack* self);

/**
 * Returns length(number of elements) of the stack.
 */
GIMME_COFFEE_API gc_SizeT gc_Stack_length(gc_Stack* self);

/**
 * Returns True if stack length is greater or equal to stack size.
 */
GIMME_COFFEE_API gc_Bool gc_Stack_is_full(gc_Stack* self);

/**
 * Returns True if stack is empty.
 */
GIMME_COFFEE_API gc_Bool gc_Stack_is_empty(gc_Stack* self);

/**
 * Returns iterator for this stack.
 * Iterator does not owns it's elements.
 */
GIMME_COFFEE_API gc_Iterator* gc_Stack_iter(gc_Stack* self);

/**
 * Returns last element of the stack without removing.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Stack_top(gc_Stack* self);

/**
 * Removes and returns last element of the stack.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Stack_pop(gc_Stack* self);

/**
 * Adds element to the end of the stack.
 */
GIMME_COFFEE_API gc_Void gc_Stack_push(gc_Stack* self, gc_VoidPtr item);


#ifdef __cplusplus
}
#endif


#endif
