#ifndef GIMME_COFFEE_BYTES_UTILS_H
#define GIMME_COFFEE_BYTES_UTILS_H


#include "./bytes.h"

#include "./hash_map.h"

#include "./hash_set.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Concatenates strings in the list.
 * 'sep' will be inserted in between each given string,
 * but not at the end of resulting string.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_join(gc_ConstCharPtr sep, gc_ArrayList* gc_bytes_lst);

/**
 * Returns a string of length 'length' with random characters from 'alphabet'.
 * Calculates the length of 'alphabet' itself.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_random_bytes(gc_SizeT length, gc_ConstCharPtr alphabet);

/**
 * Returns a string of length 'length' with random characters from 'alphabet'.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_random_bytes_ex(
	gc_SizeT length,
	gc_SizeT alphabet_length,
	gc_ConstCharPtr alphabet
);

/**
 * Returns a string, that has beginning and end quotes(' and ") removed.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_remove_external_parentheses(gc_Bytes* self);

/**
 * Returns a string with translated characters.
 * Uses 'chars_translate_table' for translation. Character 'key' will be translated into
 *'value'.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_translate(gc_Bytes* self, gc_HashMap* chars_translate_table);

/**
 * Returns a string with characters from 'char_set' removed.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_remove_chars(gc_Bytes* self, gc_HashSet* char_set);

/**
 * Returns longest common substring.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_longest_common_substr(gc_Bytes* self, gc_ConstCharPtr other);

/**
 * Returns common prefix length.
 */
GIMME_COFFEE_API gc_SizeT gc_Bytes_common_prefix_length(gc_Bytes* self, gc_ConstCharPtr other);

/**
 * Returns common postfix length.
 */
GIMME_COFFEE_API gc_SizeT gc_Bytes_common_postfix_length(gc_Bytes* self, gc_ConstCharPtr other);

/**
 * Returns True if characters in string are in 'char_set', otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_contains_all_chars(gc_Bytes* self, gc_HashSet* char_set);

/**
 * Returns True if string contains all substrings from sub.
 * To check, first the sub is split into 'and', then into 'or'.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_sub_in(gc_Bytes* self,
	gc_ConstCharPtr sub,
	gc_ConstCharPtr or_sep,
	gc_ConstCharPtr and_sep,
	gc_Bool ignore_case
);


struct GIMME_COFFEE_API gc_BytesCodesGen;

struct GIMME_COFFEE_API gc_BytesSplitGen;

struct GIMME_COFFEE_API gc_BytesChunksGen;

struct GIMME_COFFEE_API gc_RandomBytesGen;

struct GIMME_COFFEE_API gc_BytesSlidingWindowsGen;


/**
 * Generator.
 * Generates characters codes from string.
 */
typedef struct gc_BytesCodesGen gc_BytesCodesGen;

/**
 * Generator.
 * Generates substrings from string.
 */
typedef struct gc_BytesSplitGen gc_BytesSplitGen;

/**
 * Generator.
 * Generates substrings from string.
 */
typedef struct gc_BytesChunksGen gc_BytesChunksGen;

/**
 * Generator.
 * Generates random strings from given alphabet.
 */
typedef struct gc_RandomBytesGen gc_RandomBytesGen;

/**
 * Generator.
 * Generates substrings from string.
 */
typedef struct gc_BytesSlidingWindowsGen gc_BytesSlidingWindowsGen;


/**
 * Creates gc_BytesCodesGen*.
 * Does not copy given string, but uses pointer to it.
 */
GIMME_COFFEE_API gc_BytesCodesGen* gc_BytesCodesGen_init(gc_Bytes* bytes);

/**
 * Deletes given object.
 */
GIMME_COFFEE_API gc_VoidPtr gc_BytesCodesGen_del(gc_BytesCodesGen* obj);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_BytesCodesGen_has_next(gc_BytesCodesGen* self);

/**
 * Creates and returns next element.
 */
GIMME_COFFEE_API gc_UInt8 gc_BytesCodesGen_next(gc_BytesCodesGen* self);


/**
 * Creates gc_BytesSplitGen*.
 * Does not copy given string, but uses pointer to it.
 * 'sep' will be used to generate substring.
 * Maximum number of splits is equal to max_words value.
 */
GIMME_COFFEE_API gc_BytesSplitGen* gc_BytesSplitGen_init(
	gc_Bytes* bytes,
	gc_ConstCharPtr sep,
	gc_SizeT max_words
);

/**
 * Deletes object.
 */
GIMME_COFFEE_API gc_VoidPtr gc_BytesSplitGen_del(gc_BytesSplitGen* obj);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_BytesSplitGen_has_next(gc_BytesSplitGen* self);

/**
 * Creates and returns next element.
 */
GIMME_COFFEE_API gc_Bytes* gc_BytesSplitGen_next(gc_BytesSplitGen* self);


/**
 * Creates gc_BytesChunksGen*.
 * Does not copy given string, but uses pointer to it.
 * Generates substrings(chunks) from string with 'chunk_size' length or lesser.
 * Maximum number of chunks is equal to 'max_chunks' value.
 */
GIMME_COFFEE_API gc_BytesChunksGen* gc_BytesChunksGen_init(
	gc_Bytes* bytes,
	gc_SizeT chunk_size,
	gc_SizeT max_chunks
);

/**
 * Deletes object.
 */
GIMME_COFFEE_API gc_VoidPtr gc_BytesChunksGen_del(gc_BytesChunksGen* obj);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_BytesChunksGen_has_next(gc_BytesChunksGen* self);

/**
 * Creates and returns next element.
 */
GIMME_COFFEE_API gc_Bytes* gc_BytesChunksGen_next(gc_BytesChunksGen* self);


/**
 * Creates gc_RandomBytesGen*.
 * Generates 'num' random string of length 'length'
 *with characters from 'alphabet'.
 */
GIMME_COFFEE_API gc_RandomBytesGen* gc_RandomBytesGen_init(
	gc_SizeT num,
	gc_SizeT length,
	gc_ConstCharPtr alphabet
);

/**
 * Deletes object.
 */
GIMME_COFFEE_API gc_VoidPtr gc_RandomBytesGen_del(gc_RandomBytesGen* obj);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_RandomBytesGen_has_next(gc_RandomBytesGen* self);

/**
 * Creates and returns next element.
 */
GIMME_COFFEE_API gc_Bytes* gc_RandomBytesGen_next(gc_RandomBytesGen* self);


/**
 * Creates gc_BytesSlidingWindowsGen*.
 * Does not copy given string, but uses pointer to it.
 * Generates substrings of length 'window_length' or lesser from given string.
 */
GIMME_COFFEE_API gc_BytesSlidingWindowsGen* gc_BytesSlidingWindowsGen_init(gc_Bytes* bytes, gc_SizeT window_length);

/**
 * Deletes object.
 */
GIMME_COFFEE_API gc_VoidPtr gc_BytesSlidingWindowsGen_del(gc_BytesSlidingWindowsGen* obj);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_BytesSlidingWindowsGen_has_next(gc_BytesSlidingWindowsGen* self);

/**
 * Creates and returns next element.
 */
GIMME_COFFEE_API gc_Bytes* gc_BytesSlidingWindowsGen_next(gc_BytesSlidingWindowsGen* self);


#ifdef __cplusplus
}
#endif


#endif
