#ifndef GIMME_COFFEE_ITERATOR_H
#define GIMME_COFFEE_ITERATOR_H


#include "./import_macro.h"

#include "./aliases.h"


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_Iterator;

/**
 * Simple iterator object.
 * Iterator does not owns it's elements.
 */
typedef struct gc_Iterator gc_Iterator;


/**
 * Creates iterator with given size.
 */
GIMME_COFFEE_API gc_Iterator* gc_Iterator_init(gc_SizeT length);

/**
 * Deletes gc_Iterator object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Iterator_del(gc_Iterator* obj);

/**
 * Returns True if iterator can give next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Iterator_has_next(gc_Iterator* self);

/**
 * Returns True if iterator can give last element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Iterator_has_last(gc_Iterator* self);

/**
 * Returns self.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Iterator_iter(gc_Iterator* self);

/**
 * Returns next element from iterator.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Iterator_next(gc_Iterator* self);

/**
 * Returns last element from iterator.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Iterator_last(gc_Iterator* self);

/**
 * Appends element to an iterator.
 */
GIMME_COFFEE_API gc_Void gc_Iterator_append(gc_Iterator* self, gc_VoidPtr item);


#ifdef __cplusplus
}
#endif


#endif
