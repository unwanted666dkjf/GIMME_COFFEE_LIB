#ifndef GIMME_COFFEE_HASH_MAP_H
#define GIMME_COFFEE_HASH_MAP_H


#include "./iterator.h"


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_HashMapItem;

struct GIMME_COFFEE_API gc_HashMap;

struct GIMME_COFFEE_API gc_HashMapItems;


/**
 * Struct, which contains key, value and hash.
 * Key and value are pointers, it does not owns them.
 * Allows casting into gc_VoidPtr**.
 */
typedef struct gc_HashMapItem gc_HashMapItem;

/**
 * Simple collection of keys and their values.
 * Allows collisions.
 * Owns gc_HashMapItem objects.
 * Allows casting into gc_VoidPtr*.
 */
typedef struct gc_HashMap gc_HashMap;

/**
 * gc_HashMap iterator with gc_HashMapItem objects in it.
 * Does not owns gc_HashMapItem objects.
 * Allows casting into gc_HashMapItem***.
 */
typedef struct gc_HashMapItems gc_HashMapItems;


/**
 * Returns key of gc_HashMapItem.
 */
GIMME_COFFEE_API gc_VoidPtr gc_HashMapItem_key(gc_HashMapItem* self);

/**
 * Returns value of gc_HashMapItem.
 */
GIMME_COFFEE_API gc_VoidPtr gc_HashMapItem_value(gc_HashMapItem* self);

/**
 * Returns hash of gc_HashMapItem.
 */
GIMME_COFFEE_API gc_SizeT gc_HashMapItem_hash(gc_HashMapItem* self);


/**
 * Returns pointer to a new gc_HashMap with size 'size', expansion k 'expand_k',
 *and occupation k 'occupation_k'.
 */
GIMME_COFFEE_API gc_HashMap* gc_HashMap_init(gc_SizeT size, gc_Floating expand_k, gc_Floating occupation_k);

/**
 * Deletes gc_HashMap, it's gc_HashMapItem objects and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_HashMap_del(gc_HashMap* obj);

/**
 * Returns size of gc_HashMap.
 */
GIMME_COFFEE_API gc_SizeT gc_HashMap_size(gc_HashMap* self);

/**
 * Returns length(number of elements) of gc_HashMap.
 */
GIMME_COFFEE_API gc_SizeT gc_HashMap_length(gc_HashMap* self);

/**
 * Returns True if gc_HashMap is empty, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashMap_is_empty(gc_HashMap* self);

/**
 * Returns True if gc_HashMap needs to be expanded, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashMap_is_dense(gc_HashMap* self);

/**
 * Returns True if gc_HashMap can be shrinked, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashMap_is_sparse(gc_HashMap* self);

/**
 * Returns True if gc_HashMapItem with given hash exists in self, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashMap_in(gc_HashMap* self, gc_SizeT hash);

/**
 * Shrinks gc_HashMap if new_size will be greater than current length.
 * Can't shrink if future size will be smaller than the minimum.
 * Minimum size depends on occupation_k.
 */
GIMME_COFFEE_API gc_Void gc_HashMap_shrink(gc_HashMap* self);

/**
 * Expands gc_HashMap size using expand_k.
 */
GIMME_COFFEE_API gc_Void gc_HashMap_expand(gc_HashMap* self);

/**
 * Simply creates new gc_HashMapItem and adds it to gc_HashMap.
 * No collision checks.
 */
GIMME_COFFEE_API gc_Void gc_HashMap_add(gc_HashMap* self, gc_SizeT hash, gc_VoidPtr key, gc_VoidPtr value);

/**
 * Returns iterator with gc_HashMap keys in it.
 * Iterator does not owns it's items.
 */
GIMME_COFFEE_API gc_Iterator* gc_HashMap_iter(gc_HashMap* self);

/**
 * Returns iterator with gc_HashMap keys in it.
 * Iterator does not owns it's items.
 */
GIMME_COFFEE_API gc_Iterator* gc_HashMap_keys(gc_HashMap* self);

/**
 * Returns iterator with gc_HashMap values in it.
 * Iterator does not owns it's items.
 */
GIMME_COFFEE_API gc_Iterator* gc_HashMap_values(gc_HashMap* self);

/**
 * Returns value of gc_HashMapItem with given hash.
 */
GIMME_COFFEE_API gc_VoidPtr gc_HashMap_get(gc_HashMap* self, gc_SizeT hash);

/**
 * Removes and deletes gc_HashMapItem with given hash.
 * Returns value of removed gc_HashMapItem.
 */
GIMME_COFFEE_API gc_VoidPtr gc_HashMap_pop(gc_HashMap* self, gc_SizeT hash);

/**
 * Returns pointer to gc_HashMapItems object.
 * Object does not owns it's gc_HashMapItem items.
 */
GIMME_COFFEE_API gc_HashMapItems* gc_HashMap_items(gc_HashMap* self);


/**
 * Just deletes gc_HashMapItems object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_HashMapItems_del(gc_HashMapItems* obj);

/**
 * Returns True if iterator can give next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashMapItems_has_next(gc_HashMapItems* self);

/**
 * Returns True if iterator can give last element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_HashMapItems_has_last(gc_HashMapItems* self);

/**
 * Returns next element of the iterator.
 */
GIMME_COFFEE_API gc_HashMapItem* gc_HashMapItems_next(gc_HashMapItems* self);

/**
 * Returns last element of the iterator.
 */
GIMME_COFFEE_API gc_HashMapItem* gc_HashMapItems_last(gc_HashMapItems* self);


#ifdef __cplusplus
}
#endif


#endif
