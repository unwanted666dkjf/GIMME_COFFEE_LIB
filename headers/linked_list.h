#ifndef GIMME_COFFEE_LINKED_LIST_H
#define GIMME_COFFEE_LINKED_LIST_H


#include "./iterator.h"


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_LinkedList;

/**
 * Double linked list.
 */
typedef struct gc_LinkedList gc_LinkedList;


/**
 * Returns pointer to a new gc_LinkedList.
 * Does not owns it's elements.
 */
GIMME_COFFEE_API gc_LinkedList* gc_LinkedList_init();

/**
 * Deletes gc_LinkedList object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_LinkedList_del(gc_LinkedList* obj);

/**
 * Returns length(number of elements) in gc_LinkedList.
 */
GIMME_COFFEE_API gc_SizeT gc_LinkedList_length(gc_LinkedList* self);

/**
 * Returns True if gc_LinkedList is empty, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_LinkedList_is_empty(gc_LinkedList* self);

/**
 * Returns iterator for gc_LinkedList.
 */
GIMME_COFFEE_API gc_Iterator* gc_LinkedList_iter(gc_LinkedList* self);

/**
 * Returns first element of gc_LinkedList.
 */
GIMME_COFFEE_API gc_VoidPtr gc_LinkedList_first(gc_LinkedList* self);

/**
 * Returns last element of gc_LinkedList.
 */
GIMME_COFFEE_API gc_VoidPtr gc_LinkedList_last(gc_LinkedList* self);

/**
 * Removes and returns element at index.
 */
GIMME_COFFEE_API gc_VoidPtr gc_LinkedList_pop(gc_LinkedList* self, gc_SizeT index);

/**
 * Returns element at index.
 */
GIMME_COFFEE_API gc_VoidPtr gc_LinkedList_get(gc_LinkedList* self, gc_SizeT index);

/**
 * Appends item to the end of the list.
 */
GIMME_COFFEE_API gc_Void gc_LinkedList_append(gc_LinkedList* self, gc_VoidPtr item);

/**
 * Set self[index] to item.'
 */
GIMME_COFFEE_API gc_VoidPtr gc_LinkedList_set(gc_LinkedList* self, gc_SizeT index, gc_VoidPtr item);

/**
 * Insert item before index.
 */
GIMME_COFFEE_API gc_Void gc_LinkedList_insert(gc_LinkedList* self, gc_SizeT index, gc_VoidPtr item);


#ifdef __cplusplus
}
#endif


#endif
