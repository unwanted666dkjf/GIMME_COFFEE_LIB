#ifndef GIMME_COFFEE_BYTES_H
#define GIMME_COFFEE_BYTES_H


#include "./iterator.h"

#include "./array_list.h"

#include "./bytes_array.h"


extern const gc_UInt8 ASCII_CODE_MIN;

extern const gc_UInt8 ASCII_CODE_MAX;

extern const gc_UInt8 READABLE_ASCII_CODE_MIN;

extern const gc_UInt8 READABLE_ASCII_CODE_MAX;

extern gc_ConstCharPtr digits;

extern gc_ConstCharPtr whitespace;

extern gc_ConstCharPtr hexdigits;

extern gc_ConstCharPtr octdigits;

extern gc_ConstCharPtr punctuation;

extern gc_ConstCharPtr printable;

extern gc_ConstCharPtr ascii_lower;

extern gc_ConstCharPtr ascii_upper;

extern gc_ConstCharPtr ascii_letters;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Returns True if the character is a digit character, False otherwise.
 */
GIMME_COFFEE_API gc_Bool is_letter_digit(gc_Char c);

/**
 * Returns True if the character is in the English alphabet, False otherwise.
 */
GIMME_COFFEE_API gc_Bool is_ascii_letter(gc_Char c);

/**
 * Returns True if the character is a whitespace character, False otherwise.
 */
GIMME_COFFEE_API gc_Bool is_letter_whitespace(gc_Char c);

/**
 * Returns True if the character is printable, False otherwise.
 */
GIMME_COFFEE_API gc_Bool is_letter_printable(gc_Char c);

/**
 * Returns True if the character is a punctuation character, False otherwise.
 */
GIMME_COFFEE_API gc_Bool is_letter_punctuation(gc_Char c);

/**
 * Returns True if the character is uppercase or not in English alphabet, False otherwise.
 */
GIMME_COFFEE_API gc_Bool is_ascii_letter_upper(gc_Char c);

/**
 * Returns True if the character is lowercase or not in English alphabet, False otherwise.
 */
GIMME_COFFEE_API gc_Bool is_ascii_letter_lower(gc_Char c);

/**
 * Returns True if the character is uppercase, False otherwise.
 */
GIMME_COFFEE_API gc_Bool is_ascii_letter_upper_strict(gc_Char c);

/**
 * Returns True if the character is lowercase, False otherwise.
 */
GIMME_COFFEE_API gc_Bool is_ascii_letter_lower_strict(gc_Char c);

/**
 * Returns length(amount of characters before '\0') of the given string.
 */
GIMME_COFFEE_API gc_SizeT get_byte_str_length(gc_ConstCharPtr byte_str);

/**
 * Copies characters(before '\0') from given string.
 * Calculates the length of given string itself.
 * Result string is '\0'-terminated.
 */
GIMME_COFFEE_API gc_CharPtr copy_byte_str(gc_ConstCharPtr byte_str);

/**
 * Returns a copy of the string with leading characters in chars removed.
 * Calculates the length of given string itself.
 */
GIMME_COFFEE_API gc_CharPtr lstrip_byte_str(gc_ConstCharPtr byte_str, gc_ConstCharPtr chars);

/**
 * Returns a copy of the string with trailing characters in chars removed.
 * Calculates the length of given string itself.
 */
GIMME_COFFEE_API gc_CharPtr rstrip_byte_str(gc_ConstCharPtr byte_str, gc_ConstCharPtr chars);

/**
 * Returns a copy of the string with leading and trailing characters in chars removed.
 * Calculates the length of given string itself.
 */
GIMME_COFFEE_API gc_CharPtr strip_byte_str(gc_ConstCharPtr byte_str, gc_ConstCharPtr chars);

/**
 * Copies characters(before '\0') from given string.
 */
GIMME_COFFEE_API gc_CharPtr copy_byte_str_ex(gc_ConstCharPtr byte_str, gc_SizeT byte_str_length);

/**
 * Returns a copy of the string with leading characters in chars removed.
 */
GIMME_COFFEE_API gc_CharPtr lstrip_byte_str_ex(
	gc_ConstCharPtr byte_str,
	gc_ConstCharPtr chars,
	gc_SizeT byte_str_length
);

/**
 * Returns a copy of the string with trailing characters in chars removed.
 */
GIMME_COFFEE_API gc_CharPtr rstrip_byte_str_ex(
	gc_ConstCharPtr byte_str,
	gc_ConstCharPtr chars,
	gc_SizeT byte_str_length
);

/**
 * Returns a copy of the string with leading and trailing characters in chars removed.
 */
GIMME_COFFEE_API gc_CharPtr strip_byte_str_ex(
	gc_ConstCharPtr byte_str,
	gc_ConstCharPtr chars,
	gc_SizeT byte_str_length
);

/**
 * Returns a copy of the string with character inserted before index.
 * Works as copy if c is equal to '\0'.
 * It is possible to insert a character into empty string if index is 0.
 */
GIMME_COFFEE_API gc_CharPtr insert_byte_str(
	gc_ConstCharPtr byte_str,
	gc_SizeT byte_str_length,
	gc_SizeT index,
	gc_Char c
);


struct GIMME_COFFEE_API gc_Bytes;

/**
 * Basically, a better version of usual string.
 * Hash, size and length are precalculated.
 * '\0' at the end is guaranteed.
 * Allows casting into gc_CharPtr*.
 */
typedef struct gc_Bytes gc_Bytes;


gc_typedef(const gc_Bytes*, gc_ConstBytesPtr);


/**
 * Creates gc_Bytes from gc_BytesArray without copying characters.
 * Deletes given gc_BytesArray.
 */
GIMME_COFFEE_API gc_Bytes* gc_BytesArray_cast(gc_BytesArray* array);

/**
 * Creates gc_Bytes from given string.
 * If copy is True, uses gc_Bytes_init.
 * If copy is False, does not copy characters and becomes owner of given string.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_cast_bytes(gc_CharPtr bytes, gc_Bool copy);

/**
 * Copies characters from given string and creates gc_Bytes.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_init(gc_ConstCharPtr byte_str);

/**
 * Deletes object and frees it's internal array of characters.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Bytes_del(gc_Bytes* obj);

/**
 * Deletes object.
 * If free_charr is True, frees it's internal array of characters.
 * If free_charr is False, internal array of characters remains.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Bytes_del_ex(gc_Bytes* obj, gc_Bool free_charr);

/**
 * Returns size(amount of characters with '\0') of gc_Bytes.
 */
GIMME_COFFEE_API gc_SizeT gc_Bytes_size(gc_Bytes* self);

/**
 * Returns hash.
 */
GIMME_COFFEE_API gc_SizeT gc_Bytes_hash(gc_Bytes* self);

/**
 * Returns number of characters(before '\0') in self.
 */
GIMME_COFFEE_API gc_SizeT gc_Bytes_length(gc_Bytes* self);

/**
 * Returns the number of non-overlapping occurrences of substring sub in string S[start:end].
 */
GIMME_COFFEE_API gc_SizeT gc_Bytes_count(gc_Bytes* self,
	gc_ConstCharPtr sub,
	gc_SizeT start,
	gc_SizeT stop
);

/**
 * Returns internal array of characters as mutable object.
 */
GIMME_COFFEE_API gc_CharPtr gc_Bytes_bytes(gc_Bytes* self);

/**
 * Returns internal array of characters as immutable object.
 */
GIMME_COFFEE_API gc_ConstCharPtr gc_Bytes_byte_str(gc_Bytes* self);

/**
 * Returns True if the string is an uppercase string, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_upper(gc_Bytes* self);

/**
 * Returns True if the string is a lowercase string, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_lower(gc_Bytes* self);

/**
 * Returns True if the string is a digit string, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_digit(gc_Bytes* self);

/**
 * Returns True if the string is an alphabetic string, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_alpha(gc_Bytes* self);

/**
 * Returns True if the string is an alpha-numeric string, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_alnum(gc_Bytes* self);

/**
 * Returns True if the string is a whitespace string, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_space(gc_Bytes* self);

/**
 * Returns True if the string is a title-cased string, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_title(gc_Bytes* self);

/**
 * Returns True if the string is an alpha-space string, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_alspace(gc_Bytes* self);

/**
 * Returns True if the string is printable string, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_printable(gc_Bytes* self);

/**
 * Returns True if the string is empty, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_empty(gc_Bytes* self);

/**
 * Returns True if self[start:stop] contains string sub, False otherwise.
 * Order matters.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_contains(gc_Bytes* self,
	gc_ConstCharPtr sub,
	gc_SizeT start,
	gc_SizeT stop
);

/**
 * Returns True if the string ends with the specified suffix, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_endswith(gc_Bytes* self, gc_ConstCharPtr sub);

/**
 * Returns True if the string starts with the specified prefix, False otherwise.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_startswith(gc_Bytes* self, gc_ConstCharPtr sub);

/**
 * Returns True if self equal to other, False otherwise.
 * String are equal if their lengths, hashes and characters are equal.
 * Order matters.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_equal(gc_Bytes* self, gc_Bytes* other);

/**
 * Returns True if characters in internal char array are equal to other. Otherwise False.
 * Order matters.
 */
GIMME_COFFEE_API gc_Bool gc_Bytes_is_chars_equal(gc_Bytes* self, gc_ConstCharPtr other);

/**
 * Returns first character of the string.
 * If string is empty, returns '\0'.
 */
GIMME_COFFEE_API gc_Char gc_Bytes_first(gc_Bytes* self);

/**
 * Returns last character of the string.
 * If string is empty, returns '\0'.
 */
GIMME_COFFEE_API gc_Char gc_Bytes_last(gc_Bytes* self);

/**
 * Returns character at given index.
 * If index is greater than length, returns '\0'.
 */
GIMME_COFFEE_API gc_Char gc_Bytes_get(gc_Bytes* self, gc_SizeT index);

/**
 * Returns iterator for given string.
 * Doesn't copy characters, iterator contains references.
 */
GIMME_COFFEE_API gc_Iterator* gc_Bytes_iter(gc_Bytes* self);

/**
 * Returns a list of the substrings in the string, using sep as the separator string.
 * Maximum number of splits is equal to max_words value.
 */
GIMME_COFFEE_API gc_ArrayList* gc_Bytes_split(gc_Bytes* self,
	gc_ConstCharPtr sep,
	gc_SizeT max_words
);

/**
 * Returns a list of the substrings(chunks) in the string with 'chunk_size' length or lesser.
 * Maximum number of chunks is equal to 'max_chunks' value.
 */
GIMME_COFFEE_API gc_ArrayList* gc_Bytes_chunks(gc_Bytes* self,
	gc_SizeT chunk_size,
	gc_SizeT max_chunks
);

/**
 * Returns a copy of self.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_copy(gc_Bytes* self);

/**
 * Returns a copy of the string converted to uppercase.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_upper(gc_Bytes* self);

/**
 * Returns a copy of the string converted to lowercase.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_lower(gc_Bytes* self);

/**
 * Returns a version of the string where each word is titlecased.
 * More specifically, words start with uppercased characters and
 * all remaining cased characters have lower case.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_title(gc_Bytes* self);

/**
 * Returns a copy of the string where uppercase converted characters
 * to lowercase and lowercase characters converted to uppercase.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_swapcase(gc_Bytes* self);

/**
 * Returns a capitalized version of the string.
 * More specifically, make the first character have upper case
 * and the rest lowercase.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_capitalize(gc_Bytes* self);

/**
 * Returns a reversed version of the string.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_reverse(gc_Bytes* self);

/**
 * Returns a string, that is a duplicated version of original string
 * 'num' times.
 * If num is 0, returns empty string.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_multiply(gc_Bytes* self, gc_SizeT num);

/**
 * Returns a copy of original string, where character appended to the end.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_append(gc_Bytes* self, gc_Char c);

/**
 * Returns a copy of the string with leading and trailing characters in chars removed.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_strip(gc_Bytes* self, gc_ConstCharPtr chars);

/**
 * Returns a copy of the string with leading characters in chars removed.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_lstrip(gc_Bytes* self, gc_ConstCharPtr chars);

/**
 * Returns a copy of the string with trailing characters in chars removed.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_rstrip(gc_Bytes* self, gc_ConstCharPtr chars);

/**
 * Concatenates two strings into one new.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_concat(gc_Bytes* self, gc_ConstCharPtr other);

/**
 * Returns a copy of the string with character inserted before index.
 * Works as copy if c is equal to '\0'.
 * It is possible to insert a character into empty string if index is 0.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_insert(gc_Bytes* self, gc_SizeT index, gc_Char c);

/**
 * Returns a copy of the string with other string inserted before index.
 * Works as copy if other is empty.
 * It is possible to insert other into empty string if index is 0.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_push_into(gc_Bytes* self, gc_SizeT index, gc_ConstCharPtr other);

/**
 * Returns a copy with all occurrences of substring old replaced by new.
 * Maximum number of occurrences to replace is value of 'num_replaces'.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_replace(gc_Bytes* self,
	gc_ConstCharPtr old_str,
	gc_ConstCharPtr new_str,
	gc_SizeT num_replaces
);

/**
 * Creates new gc_Bytes, using start, stop and step values.
 * Stop will be always greater or equal to start.
 * start -- start index.
 * stop -- end index.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_slice(gc_Bytes* self,
	gc_SizeT start,
	gc_SizeT stop,
	gc_SizeT step
);

/**
 * Returns the lowest index in S where character is found, such that character is contained within S[start:end].
 * If character is not found, returns 'INDEX_NOT_FOUND'.
 */
GIMME_COFFEE_API gc_Int gc_Bytes_index(gc_Bytes* self,
	gc_Char c,
	gc_SizeT start,
	gc_SizeT stop
);

/**
 * Returns the highest index in S where character is found, such that character is contained within S[start:end].
 * If character is not found, returns 'INDEX_NOT_FOUND'.
 */
GIMME_COFFEE_API gc_Int gc_Bytes_rindex(gc_Bytes* self,
	gc_Char c,
	gc_SizeT start,
	gc_SizeT stop
);

/**
 * Returns the lowest index in S where substring sub is found, such that sub is contained within S[start:end].
 * If sub is not found, returns 'INDEX_NOT_FOUND'.
 */
GIMME_COFFEE_API gc_Int gc_Bytes_find(gc_Bytes* self,
	gc_ConstCharPtr other,
	gc_SizeT start,
	gc_SizeT stop
);

/**
 * Returns the highest index in S where substring sub is found, such that sub is contained within S[start:end].
 * If sub is not found, returns 'INDEX_NOT_FOUND'.
 */
GIMME_COFFEE_API gc_Int gc_Bytes_rfind(gc_Bytes* self,
	gc_ConstCharPtr other,
	gc_SizeT start,
	gc_SizeT stop
);


#ifdef __cplusplus
}
#endif


#endif
