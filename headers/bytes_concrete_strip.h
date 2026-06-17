#ifndef GIMME_COFFEE_BYTES_CONCRETE_STRIP
#define GIMME_COFFEE_BYTES_CONCRETE_STRIP


#include "./bytes.h"


/**
 * Constants for specified types.
 */
typedef enum {
	gc_LetterDigit,
	gc_LetterPrintable,
	gc_LetterWhitespace,
	gc_LetterPunctuation,
	gc_AsciiLetter,
	gc_AsciiLetterLower,
	gc_AsciiLetterUpper
} gc_LetterCheckT;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Returns True if character in specified type 'type_', otherwise False.
 */
GIMME_COFFEE_API gc_Bool is_concrete_letter(gc_Char c, gc_LetterCheckT type_);


/**
 * Returns a copy of the string with leading and trailing characters of specified type removed.
 * Faster than ordinary strip, because it checks whether the characters codes in corresponding
 *range or not.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_concrete_strip(gc_Bytes* self, gc_LetterCheckT type_);

/**
 * Returns a copy of the string with leading characters of specified type removed.
 * Faster than ordinary lstrip, because it checks whether the characters codes in corresponding
 *range or not.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_concrete_lstrip(gc_Bytes* self, gc_LetterCheckT type_);

/**
 * Returns a copy of the string with trailing characters of specified type removed.
 * Faster than ordinary rstrip, because it checks whether the characters codes in corresponding
 *range or not.
 */
GIMME_COFFEE_API gc_Bytes* gc_Bytes_concrete_rstrip(gc_Bytes* self, gc_LetterCheckT type_);


#ifdef __cplusplus
}
#endif


#endif
