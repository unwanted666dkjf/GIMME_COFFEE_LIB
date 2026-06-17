#ifndef GIMME_COFFEE_HASH_SET_H
#define GIMME_COFFEE_HASH_SET_H


#include "./iterator.h"


extern const gc_SizeT SAFE_HASH_SET_SIZE;

extern const gc_Floating SAFE_HASH_SET_EXPAND_K;

extern const gc_Floating SAFE_HASH_SET_OCCUPATION_K;


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_HashSet;

/**
 * Simple collection of items and their hashes.
 * Does not owns items.
 * Collisions are allowed.
 * Allows casting into gc_VoidPtr**.
 */
typedef struct gc_HashSet gc_HashSet;


/**
 * Returns pointer to a new gc_HashSet with given size, expand_k and occupation_k.
 */
GIMME_COFFEE_API gc_HashSet* gc_HashSet_init(gc_SizeT size, gc_Floating expand_k, gc_Floating occupation_k);

/**
 * Deletes gc_HashSet and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_HashSet_del(gc_HashSet* obj);

/**
 * Returns size of gc_HashSet.
 */
GIMME_COFFEE_API gc_SizeT gc_HashSet_size(gc_HashSet* self);

/**
 * Returns length(number of elements) of gc_HashSet.
 */
GIMME_COFFEE_API gc_SizeT gc_HashSet_length(gc_HashSet* self);

/**
 * Expands size of gc_HashSet using expand_k.
 */
GIMME_COFFEE_API gc_Void gc_HashSet_expand(gc_HashSet* self);

/**
 * Shrinks gc_HashSet if size will be greater than current length.
 * Can't shrink if future size will be smaller than the minimum.
 * Minimum size depends on occupation_k.
 */
GIMME_COFFEE_API gc_Void gc_HashSet_shrink(gc_HashSet* self);

/**
 * Returns iterator with values of items in gc_HashSet.
 * Does not owns values.
 */
GIMME_COFFEE_API gc_Iterator* gc_HashSet_iter(gc_HashSet* self);

/**
 * Simply creates and adds new item to a gc_HashSet.
 * Does not checks collisions.
 */
GIMME_COFFEE_API gc_Void gc_HashSet_add(gc_HashSet* self, gc_SizeT hash, gc_VoidPtr item);

/**
 * Returns value of item with given hash.
 */
GIMME_COFFEE_API gc_VoidPtr gc_HashSet_get(gc_HashSet* self, gc_SizeT hash);

/**
 * Removes and deletes gc_HashSet item with given hash.
 * Returns item value.
 */
GIMME_COFFEE_API gc_VoidPtr gc_HashSet_pop(gc_HashSet* self, gc_SizeT hash);

/**
 * Returns True if gc_HashSet is empty, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashSet_is_empty(gc_HashSet* self);

/**
 * Returns True if gc_HashSet needs to be expanded, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashSet_is_dense(gc_HashSet* self);

/**
 * Returns True if gc_HashSet can be shrinked, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashSet_is_sparse(gc_HashSet* self);

/**
 * Returns True if gc_HashSet contains item with given hash, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashSet_in(gc_HashSet* self, gc_SizeT hash);


#ifdef __cplusplus
}
#endif


#endif
