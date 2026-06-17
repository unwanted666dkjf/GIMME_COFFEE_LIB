#ifndef GIMME_COFFEE_BYTES_ARRAY_H
#define GIMME_COFFEE_BYTES_ARRAY_H


#include "./iterator.h"


extern const gc_SizeT MIN_GC_BYTES_ARRAY_SIZE_SAFE;

extern const gc_Floating MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Returns 'bytes_size' if 'bytes_size' is greater than 'MIN_GC_BYTES_ARRAY_SIZE_SAFE',
 * otherwise MIN_GC_BYTES_ARRAY_SIZE_SAFE.
 */
GIMME_COFFEE_API gc_SizeT get_min_size_for_gc_bytes_array(gc_SizeT bytes_size);


struct GIMME_COFFEE_API gc_BytesArray;

/**
 * Mutable array of characters with dynamic size.
 * In short, gc_ArrayList specifically for characters.
 * Allows casting into gc_CharPtr*.
 */
typedef struct gc_BytesArray gc_BytesArray;


/**
 * Creates character array with given size and expansion k.
 */
GIMME_COFFEE_API gc_BytesArray* gc_BytesArray_init(gc_SizeT size, gc_Floating expand_k);

/**
 * Deletes object and frees it's internal array of characters.
 */
GIMME_COFFEE_API gc_VoidPtr gc_BytesArray_del(gc_BytesArray* obj);

/**
 * Deletes object.
 * If free_charr is True, frees it's internal array of characters.
 * If free_charr is False, internal array of characters remains.
 */
GIMME_COFFEE_API gc_VoidPtr gc_BytesArray_del_ex(gc_BytesArray* obj, gc_Bool free_charr);

/**
 * Returns size of an array.
 */
GIMME_COFFEE_API gc_SizeT gc_BytesArray_size(gc_BytesArray* self);

/**
 * Returns length(amount of characters) in array.
 */
GIMME_COFFEE_API gc_SizeT gc_BytesArray_length(gc_BytesArray* self);

/**
 * Returns expansion k of an array.
 */
GIMME_COFFEE_API gc_Floating gc_BytesArray_expand_k(gc_BytesArray* self);

/**
 * Returns True if array is empty, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_BytesArray_is_empty(gc_BytesArray* self);

/**
 * Return True if array needs to be expanded, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_BytesArray_is_dense(gc_BytesArray* self);

/**
 * Return True if array needs to be shrinked, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_BytesArray_is_sparse(gc_BytesArray* self);

/**
 * Set self[index] to c if index lesser than length.
 * Returns True if successful, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_BytesArray_set(gc_BytesArray* self, gc_SizeT index, gc_Char c);

/**
 * Insert character before index if index lesser than length.
 * Returns True if successful, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_BytesArray_insert(gc_BytesArray* self, gc_SizeT index, gc_Char c);

/**
 * Returns first character of the array.
 * If array is empty, returns '\0'.
 */
GIMME_COFFEE_API gc_Char gc_BytesArray_first(gc_BytesArray* self);

/**
 * Returns last character of the array.
 * If array is empty, returns '\0'.
 */
GIMME_COFFEE_API gc_Char gc_BytesArray_last(gc_BytesArray* self);

/**
 * Returns character at the given index if index lesser than length, otherwise
 * returns '\0'.
 */
GIMME_COFFEE_API gc_Char gc_BytesArray_get(gc_BytesArray* self, gc_SizeT index);

/**
 * Removes and returns character at index if index lesser than length, otherwise
 * returns '\0'.
 */
GIMME_COFFEE_API gc_Char gc_BytesArray_pop(gc_BytesArray* self, gc_SizeT index);

/**
 * Returns internal character array as mutable object.
 */
GIMME_COFFEE_API gc_CharPtr gc_BytesArray_bytes(gc_BytesArray* self);

/**
 * Returns internal character array as immutable object.
 */
GIMME_COFFEE_API gc_ConstCharPtr gc_BytesArray_char_ptr(gc_BytesArray* self);

/**
 * Returns iterator for given array.
 * Doesn't copy characters, iterator contains references.
 */
GIMME_COFFEE_API gc_Iterator* gc_BytesArray_iter(gc_BytesArray* self);

/**
 * Creates new gc_BytesArray, using start, stop and step values.
 * Stop will be always greater or equal to start.
 * start -- start index.
 * stop -- end index.
 */
GIMME_COFFEE_API gc_BytesArray* gc_BytesArray_slice(gc_BytesArray* self,
	gc_SizeT start,
	gc_SizeT stop,
	gc_SizeT step
);

/**
 * In place reverse.
 */
GIMME_COFFEE_API gc_Void gc_BytesArray_reverse(gc_BytesArray* self);

/**
 * Shrinks size of the array.
 * Uses expansion k.
 * Cannot shrink if future size will be lesser than current length.
 */
GIMME_COFFEE_API gc_Void gc_BytesArray_shrink(gc_BytesArray* self);

/**
 * Expands size of the array.
 * Uses expansion k.
 */
GIMME_COFFEE_API gc_Void gc_BytesArray_expand(gc_BytesArray* self);

/**
 * Appends character to the end of the array.
 */
GIMME_COFFEE_API gc_Void gc_BytesArray_append(gc_BytesArray* self, gc_Char c);

/**
 * Appends elements from other gc_BytesArray to self.
 * Expands automatically if needed.
 */
GIMME_COFFEE_API gc_Void gc_BytesArray_extend(gc_BytesArray* self, gc_BytesArray* other);

/**
 * Appends characters from given string to self.
 * Expands automatically if needed.
 * String must be terminated with '\0'.
 */
GIMME_COFFEE_API gc_Void gc_BytesArray_push_byte_str(gc_BytesArray* self, gc_ConstCharPtr byte_str);


#ifdef __cplusplus
}
#endif


#endif
