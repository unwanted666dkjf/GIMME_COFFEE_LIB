#ifndef GIMME_COFFEE_ARRAY_LIST_H
#define GIMME_COFFEE_ARRAY_LIST_H


#include "./iterator.h"


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_ArrayList;

/**
 * Array with dynamic size.
 * Does not owns it's elements.
 * Allows casting into gc_VoidPtr**.
 */
typedef struct gc_ArrayList gc_ArrayList;

/**
 * Creates array list with given size and expansion k.
 */
GIMME_COFFEE_API gc_ArrayList* gc_ArrayList_init(gc_SizeT size, gc_Floating expand_k);

/**
 * Deletes object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_ArrayList_del(gc_ArrayList* obj);

/**
 * Returns size of an gc_ArrayList.
 */
GIMME_COFFEE_API gc_SizeT gc_ArrayList_size(gc_ArrayList* self);

/**
 * Returns length(amount of elements) in gc_ArrayList.
 */
GIMME_COFFEE_API gc_SizeT gc_ArrayList_length(gc_ArrayList* self);

/**
 * In place reverse.
 */
GIMME_COFFEE_API gc_Void gc_ArrayList_reverse(gc_ArrayList* self);

/**
 * Returns expansion k of an gc_ArrayList.
 */
GIMME_COFFEE_API gc_Floating gc_ArrayList_expand_k(gc_ArrayList* self);

/**
 * Returns True if gc_ArrayList is empty, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_ArrayList_is_empty(gc_ArrayList* self);

/**
 * Return True if gc_ArrayList needs to be expanded, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_ArrayList_is_dense(gc_ArrayList* self);

/**
 * Return True if gc_ArrayList needs to be shrinked, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_ArrayList_is_sparse(gc_ArrayList* self);

/**
 * Return iterator for the given object.
 */
GIMME_COFFEE_API gc_Iterator* gc_ArrayList_iter(gc_ArrayList* self);

/**
 * Returns first element of gc_ArrayList.
 */
GIMME_COFFEE_API gc_VoidPtr gc_ArrayList_first(gc_ArrayList* self);

/**
 * Return last element of gc_ArrayList.
 */
GIMME_COFFEE_API gc_VoidPtr gc_ArrayList_last(gc_ArrayList* self);

/**
 * Removes and returns item at index.
 */
GIMME_COFFEE_API gc_VoidPtr gc_ArrayList_pop(gc_ArrayList* self, gc_SizeT index);

/**
 * Returns element at index.
 */
GIMME_COFFEE_API gc_VoidPtr gc_ArrayList_get(gc_ArrayList* self, gc_SizeT index);

/**
 * Appends object to the end of the list.
 */
GIMME_COFFEE_API gc_Void gc_ArrayList_append(gc_ArrayList* self, gc_VoidPtr item);

/**
 * Expands size of gc_ArrayList.
 * Uses expansion k.
 */
GIMME_COFFEE_API gc_Void gc_ArrayList_expand(gc_ArrayList* self);

/**
 * Shrinks size of gc_ArrayList.
 * Uses expansion k.
 * Cannot shrink if future size will be lesser than current length.
 */
GIMME_COFFEE_API gc_Void gc_ArrayList_shrink(gc_ArrayList* self);

/**
 * Appends elements from other gc_ArrayList to self.
 * Expands automatically if needed.
 */
GIMME_COFFEE_API gc_Void gc_ArrayList_extend(gc_ArrayList* self, gc_ArrayList* other);

/**
 * Set self[index] to item.
 */
GIMME_COFFEE_API gc_VoidPtr gc_ArrayList_set(gc_ArrayList* self, gc_SizeT index, gc_VoidPtr item);

/**
 * Insert object before index.
 */
GIMME_COFFEE_API gc_Void gc_ArrayList_insert(gc_ArrayList* self, gc_SizeT index, gc_VoidPtr item);

/**
 * Creates new gc_ArrayList, using start, stop and step values.
 * Stop will be always greater or equal to start.
 * start -- start index.
 * stop -- end index.
 */
GIMME_COFFEE_API gc_ArrayList* gc_ArrayList_slice(gc_ArrayList* self, gc_SizeT start, gc_SizeT stop, gc_SizeT step);


#ifdef __cplusplus
}
#endif


#endif
