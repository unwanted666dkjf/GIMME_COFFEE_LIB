#include "../headers/bytes_concrete_strip.h"

#include "../headers/bytes_array.h"


static gc_SizeT get_first_nonconcrete(gc_Bytes* bytes, gc_LetterCheckT type_);

static gc_Int get_last_nonconcrete(gc_Bytes* bytes, gc_LetterCheckT type_, gc_SizeT start_ind);


gc_Bytes* gc_Bytes_concrete_strip(gc_Bytes* self, gc_LetterCheckT type_) {
	gc_SizeT start_ind = get_first_nonconcrete(self, type_);
	gc_Int stop_ind = get_last_nonconcrete(self, type_, start_ind);
	if (stop_ind < 0) {
		return gc_Bytes_init("\0");
	}
	gc_Bytes* res = gc_Bytes_slice(self, start_ind, stop_ind + 1, 1);
	return res;
}

gc_Bytes* gc_Bytes_concrete_lstrip(gc_Bytes* self, gc_LetterCheckT type_) {
	gc_SizeT self_length = gc_Bytes_length(self);
	gc_SizeT start_ind = get_first_nonconcrete(self, type_);
	if (start_ind >= self_length) {
		return gc_Bytes_init("\0");
	}
	gc_Bytes* res = gc_Bytes_slice(self, start_ind, self_length, 1);
	return res;
}

gc_Bytes* gc_Bytes_concrete_rstrip(gc_Bytes* self, gc_LetterCheckT type_) {
	gc_SizeT self_length = gc_Bytes_length(self);
	if (self_length < 1) {
		return gc_Bytes_init("\0");
	}
	gc_SizeT arr_size = get_min_size_for_gc_bytes_array(self_length);
	gc_BytesArray* array = gc_BytesArray_init(arr_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_BytesArray_push_byte_str(array, gc_Bytes_byte_str(self));
	gc_Char c;
	gc_Int last_ind;
	do {
		last_ind = (gc_Int)gc_BytesArray_length(array) - 1;
		if (last_ind < 0) {
			break;
		}
		c = gc_BytesArray_pop(array, last_ind);
	} while (is_concrete_letter(c, type_));
	if (last_ind >= 0) {
		gc_BytesArray_append(array, c);
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}


gc_Bool is_concrete_letter(gc_Char c, gc_LetterCheckT type_) {	// Fuck 'clean code' lovers, I prefer performance
	switch(type_) {
		case gc_LetterDigit:
			return is_letter_digit(c);
			break;
		case gc_LetterPrintable:
			return is_letter_printable(c);
			break;
		case gc_LetterWhitespace:
			return is_letter_whitespace(c);
			break;
		case gc_LetterPunctuation:
			return is_letter_punctuation(c);
			break;
		case gc_AsciiLetter:
			return is_ascii_letter(c);
			break;
		case gc_AsciiLetterLower:
			return is_ascii_letter_lower_strict(c);
			break;
		case gc_AsciiLetterUpper:
			return is_ascii_letter_upper_strict(c);
			break;
		default:
			return False;
	}
}


gc_SizeT get_first_nonconcrete(gc_Bytes* bytes, gc_LetterCheckT type_) {
	gc_SizeT start_ind = 0;
	gc_SizeT self_length = gc_Bytes_length(bytes);
	for (; start_ind < self_length; start_ind++) {
		gc_Char c = gc_Bytes_get(bytes, start_ind);
		if (gc_not is_concrete_letter(c, type_)) {
			break;
		}
	}
	return start_ind;
}

gc_Int get_last_nonconcrete(gc_Bytes* bytes, gc_LetterCheckT type_, gc_SizeT start_ind) {
	gc_SizeT self_length = gc_Bytes_length(bytes);
	gc_Int stop_ind = (gc_Int)self_length - 1;
	if (stop_ind < 0) {
		return stop_ind;
	}
	for(; stop_ind >= (gc_Int)start_ind; stop_ind--) {
		gc_Char c = gc_Bytes_get(bytes, stop_ind);
		if (gc_not is_concrete_letter(c, type_)) {
			break;
		}
	}
	return stop_ind;
}
