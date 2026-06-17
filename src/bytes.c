#include "../headers/bytes.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


const gc_UInt8 ASCII_CODE_MIN = 0;

const gc_UInt8 ASCII_CODE_MAX = 255;

const gc_UInt8 READABLE_ASCII_CODE_MIN = 32;

const gc_UInt8 READABLE_ASCII_CODE_MAX = 126;

gc_ConstCharPtr digits = "0123456789\0";

gc_ConstCharPtr whitespace = " \t\n\r\x0b\x0c";

gc_ConstCharPtr hexdigits = "0123456789abcdefABCDEF\0";

gc_ConstCharPtr octdigits = "01234567\0";

gc_ConstCharPtr punctuation = "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~\0";

gc_ConstCharPtr ascii_lower = "abcdefghijklmnopqrstuvwxyz\0";

gc_ConstCharPtr ascii_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0";

gc_ConstCharPtr ascii_letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\0";


struct gc_Bytes {
	gc_CharPtr byte_str;
	gc_SizeT size;
	gc_SizeT hash;
	gc_SizeT length;
};

gc_Bytes* gc_BytesArray_cast(gc_BytesArray* array) {
	gc_Bytes* self = (gc_Bytes*)gc_malloc(gc_sizeof(gc_Bytes));
	if (gc_not self) {
		gc_printf("Could not create gc_Bytes!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->hash = gc_sizeof(gc_Bytes);
	gc_SizeT array_length = gc_BytesArray_length(array);
	if (array_length + 1 >= gc_BytesArray_size(array)) {
		gc_BytesArray_expand(array);
	}
	gc_Int last_ind = (gc_Int)array_length - 1;
	if (last_ind < 0) {
		gc_BytesArray_append(array, '\0');
	} else {
		if (gc_BytesArray_get(array, last_ind) != '\0') {
			gc_BytesArray_append(array, '\0');
		}
	}
	self->byte_str = gc_BytesArray_bytes(array);
	array = gc_BytesArray_del_ex(array, False);
	self->length = 0;
	for (; self->byte_str[self->length] != '\0'; self->length++) {
		self->hash += (gc_SizeT)self->byte_str[self->length];
	}
	self->byte_str[self->length] = '\0';
	self->size = self->length + 1;
	return self;
}

gc_Bytes* gc_Bytes_cast_bytes(gc_CharPtr bytes, gc_Bool copy) {
	if (copy) {
		return gc_Bytes_init(bytes);
	}
	gc_Bytes* self = (gc_Bytes*)gc_malloc(gc_sizeof(gc_Bytes));
	if (gc_not self) {
		gc_printf("Could not create gc_Bytes!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->hash = gc_sizeof(gc_Bytes);
	self->length = 0;
	self->byte_str = bytes;
	for (; self->byte_str[self->length] != '\0'; self->length++) {
		self->hash += (gc_SizeT)self->byte_str[self->length];
	}
	self->byte_str[self->length] = '\0';
	self->size = self->length + 1;
	return self;
}

gc_Bytes* gc_Bytes_init(gc_ConstCharPtr byte_str) {
	gc_Bytes* self = (gc_Bytes*)gc_malloc(gc_sizeof(gc_Bytes));
	if (gc_not self) {
		gc_printf("Could not create gc_Bytes!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->hash = gc_sizeof(gc_Bytes);
	gc_SizeT length = 0;
	if (byte_str) {
		for (; byte_str[length] != '\0'; length++) {
			self->hash += (gc_SizeT)byte_str[length];
		}
	}
	self->size = length + 1;
	self->length = length;
	self->hash *= self->size;
	self->byte_str = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * self->size);
	if (gc_not self->byte_str) {
		gc_printf("Could not create array for gc_Bytes!\n");
		gc_exit(EXIT_NULLPTR);
	}
	if (byte_str) {
		for (gc_SizeT i = 0; i < self->length; i++) {
			self->byte_str[i] = byte_str[i];
		}
		self->byte_str[self->length] = '\0';
	} else {
		self->byte_str[0] = '\0';
	}
	return self;
}

gc_VoidPtr gc_Bytes_del(gc_Bytes* obj) {
	return gc_Bytes_del_ex(obj, True);
}

gc_VoidPtr gc_Bytes_del_ex(gc_Bytes* obj, gc_Bool free_charr) {
	if (free_charr) {
		gc_free(obj->byte_str);
	}
	gc_free(obj);
	return NONE;
}

gc_SizeT gc_Bytes_size(gc_Bytes* self) {
	return self->size;
}

gc_SizeT gc_Bytes_hash(gc_Bytes* self) {
	return self->hash;
}

gc_SizeT gc_Bytes_length(gc_Bytes* self) {
	return self->length;
}

gc_SizeT gc_Bytes_count(gc_Bytes* self,
	gc_ConstCharPtr sub,
	gc_SizeT start,
	gc_SizeT stop
) {
	if (start > stop) {
		gc_SizeT s = start;
		start = stop;
		stop = s;
	}
	if (stop == self->size) {
		stop--;
	}
	gc_SizeT cntr = 0;
	gc_Int index = gc_Bytes_find(self, sub, start, stop);
	if (index == INDEX_NOT_FOUND) {
		return cntr;
	}
	cntr++;
	gc_SizeT sub_length = get_byte_str_length(sub);
	gc_SizeT ind_begin = index + sub_length;
	while (ind_begin < stop) {
		index = gc_Bytes_find(self, sub, ind_begin, stop);
		if (index == INDEX_NOT_FOUND) {
			break;
		}
		cntr++;
		ind_begin = index + sub_length;
	}
	return cntr;
}

gc_CharPtr gc_Bytes_bytes(gc_Bytes* self) {
	return self->byte_str;
}

gc_ConstCharPtr gc_Bytes_byte_str(gc_Bytes* self) {
	return (gc_ConstCharPtr)self->byte_str;
}

gc_Bool gc_Bytes_is_upper(gc_Bytes* self) {
	if (self->length == 0) {
		return False;
	}
	gc_Bool res = True;
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		if (gc_not is_ascii_letter_upper(self->byte_str[i])) {
			res = False;
			break;
		}
	}
	return res;
}

gc_Bool gc_Bytes_is_lower(gc_Bytes* self) {
	if (self->length == 0) {
		return False;
	}
	gc_Bool res = True;
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		if (gc_not is_ascii_letter_lower(self->byte_str[i])) {
			res = False;
			break;
		}
	}
	return res;
}

gc_Bool gc_Bytes_is_digit(gc_Bytes* self) {
	if (self->length == 0) {
		return False;
	}
	gc_Bool res = True;
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		if (gc_not is_letter_digit(self->byte_str[i])) {
			res = False;
			break;
		}
	}
	return res;
}

gc_Bool gc_Bytes_is_alpha(gc_Bytes* self) {
	if (self->length == 0) {
		return False;
	}
	gc_Bool res = True;
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		if (gc_not is_ascii_letter(self->byte_str[i])) {
			res = False;
			break;
		}
	}
	return res;
}

gc_Bool gc_Bytes_is_alnum(gc_Bytes* self) {
	if (self->length == 0) {
		return False;
	}
	gc_Bool res = True;
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		if (gc_not is_ascii_letter(self->byte_str[i])) {
			if (gc_not is_letter_digit(self->byte_str[i])) {
				res = False;
				break;
			}
		}
	}
	return res;
}

gc_Bool gc_Bytes_is_space(gc_Bytes* self) {
	if (self->length == 0) {
		return False;
	}
	gc_Bool res = True;
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		if (gc_not is_letter_whitespace(self->byte_str[i])) {
			res = False;
			break;
		}
	}
	return res;
}

gc_Bool gc_Bytes_is_title(gc_Bytes* self) {
	if (self->length == 0) {
		return False;
	}
	gc_Bool res = True;
	gc_Bool in_word = True;
	gc_Bool is_first_letter = True;
	gc_Bool worded = False;
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		if (is_letter_whitespace(self->byte_str[i]) gc_or is_letter_punctuation(self->byte_str[i])) {
			in_word = False;
		} else {
			in_word = True;
			worded = True;
		}
		if (in_word) {
			if (is_first_letter) {
				if (gc_not is_ascii_letter_upper(self->byte_str[i])) {
					res = False;
					break;
				}
				is_first_letter = False;
			} else {
				if (gc_not is_ascii_letter_lower(self->byte_str[i])) {
					res = False;
					break;
				}
			}
		} else {
			gc_SizeT j = i + 1;
			if (j < self->length) {
				if (is_ascii_letter(self->byte_str[j])) {
					is_first_letter = True;
				}
			}
		}
	}
	if (gc_not worded) {
		return False;
	}
	return res;
}

gc_Bool gc_Bytes_is_alspace(gc_Bytes* self) {
	if (self->length == 0) {
		return False;
	}
	gc_Bool res = True;
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		if (gc_not is_ascii_letter(self->byte_str[i])) {
			if (gc_not is_letter_whitespace(self->byte_str[i])) {
				res = False;
				break;
			}
		}
	}
	return res;
}

gc_Bool gc_Bytes_is_printable(gc_Bytes* self) {
	if (self->length == 0) {
		return False;
	}
	gc_Bool res = True;
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		if (gc_not is_letter_printable(self->byte_str[i])) {
			res = False;
			break;
		}
	}
	return res;
}

gc_Bool gc_Bytes_is_empty(gc_Bytes* self) {
	if (self->size == 1) {
		return self->byte_str[0] == '\0';
	}
	return self->length == 0;
}

gc_Bool gc_Bytes_contains(gc_Bytes* self,
	gc_ConstCharPtr sub,
	gc_SizeT start,
	gc_SizeT stop
) {
	if (self->length == 0 gc_and sub[0] != '\0') {
		return False;
	}
	if (self->length == 0 gc_and sub[0] == '\0') {
		return True;
	}
	gc_Int index;
	if (start <= self->length / 2) {
		index = gc_Bytes_find(self, sub, start, stop);
	} else {
		index = gc_Bytes_rfind(self, sub, start, stop);
	}
	return index != INDEX_NOT_FOUND;
}

gc_Bool gc_Bytes_endswith(gc_Bytes* self, gc_ConstCharPtr sub) {
	if (sub[0] == '\0' gc_and self->byte_str[0] != '\0') {
		return False;
	}
	if (self->length == 0 gc_and sub[0] == '\0') {
		return True;
	}
	gc_SizeT sub_length = get_byte_str_length(sub);
	if (sub_length > self->length) {
		return False;
	}
	if (sub_length == 1) {
		return self->byte_str[self->length - 1] == sub[0];
	}
	gc_Int start_ind = (gc_Int)self->length - (gc_Int)sub_length;
	if (start_ind < 0) {
		return False;
	}
	gc_SizeT cntr = 0;
	for (; cntr < sub_length; cntr++) {
		if (self->byte_str[start_ind + cntr] != sub[cntr]) {
			break;
		}
	}
	return cntr == sub_length;
}

gc_Bool gc_Bytes_startswith(gc_Bytes* self, gc_ConstCharPtr sub) {
	if (sub[0] == '\0' gc_and self->byte_str[0] != '\0') {
		return False;
	}
	if (self->length == 0 gc_and sub[0] == '\0') {
		return True;
	}
	gc_SizeT sub_length = get_byte_str_length(sub);
	if (sub_length > self->length) {
		return False;
	}
	gc_SizeT cntr = 0;
	for (; cntr < sub_length; cntr++) {
		if (self->byte_str[cntr] != sub[cntr]) {
			break;
		}
	}
	return cntr == sub_length;
}

gc_Bool gc_Bytes_is_equal(gc_Bytes* self, gc_Bytes* other) {
	if (self->hash != other->hash) {
		return False;
	}
	if (self->length != other->length) {
		return False;
	}
	return gc_Bytes_is_chars_equal(self, other->byte_str);
}

gc_Bool gc_Bytes_is_chars_equal(gc_Bytes* self, gc_ConstCharPtr other) {
	if (self->length < 1) {
		if (other[0] == '\0') {
			return True;
		}
		return False;
	}
	if (other[0] == '\0') {
		return False;
	}
	gc_SizeT i = 0;
	gc_Bool res = True;
	for(; self->byte_str[i] != '\0'; i++) {
		if (other[i] == '\0') {
			break;
		}
		if (self->byte_str[i] != other[i]) {
			res = False;
			break;
		}
	}
	if (i < self->length) {
		res = False;
	}
	return res;
}

gc_Char gc_Bytes_first(gc_Bytes* self) {
	if (self->length < 1) {
		return '\0';
	}
	return self->byte_str[0];
}

gc_Char gc_Bytes_last(gc_Bytes* self) {
	if (self->length < 1) {
		return '\0';
	}
	return self->byte_str[self->length - 1];
}

gc_Char gc_Bytes_get(gc_Bytes* self, gc_SizeT index) {
	return self->byte_str[index];
}

gc_Iterator* gc_Bytes_iter(gc_Bytes* self) {
	gc_Iterator* iter = gc_Iterator_init(self->length);
	for (gc_SizeT i = 0; self->byte_str[i] != '\0'; i++) {
		gc_Iterator_append(iter, &self->byte_str[i]);
	}
	return iter;
}

gc_ArrayList* gc_Bytes_split(gc_Bytes* self,
	gc_ConstCharPtr sep,
	gc_SizeT max_words
) {
	gc_SizeT num_words = (max_words > 0) ? max_words : 2;
	gc_ArrayList* words = gc_ArrayList_init(num_words, 1.5);
	if (max_words < 1) {
		return words;
	}
	gc_Int sep_ind = gc_Bytes_find(self, sep, 0, self->size);
	if (sep_ind == INDEX_NOT_FOUND) {
		gc_ArrayList_append(words, gc_Bytes_copy(self));
		if (gc_ArrayList_is_sparse(words)) {
			gc_ArrayList_shrink(words);
		}
		return words;
	}
	gc_SizeT sep_length = get_byte_str_length(sep);
	gc_ArrayList_append(
		words, gc_Bytes_slice(self, 0, sep_ind, 1)
	);
	gc_Int ind_begin = sep_ind + (gc_Int)sep_length;
	while (ind_begin < (gc_Int)self->size) {
		if (gc_ArrayList_is_dense(words)) {
			gc_ArrayList_expand(words);
		}
		if (gc_ArrayList_length(words) >= max_words) {
			break;
		}
		sep_ind = gc_Bytes_find(self, sep, ind_begin, self->size);
		if (sep_ind == INDEX_NOT_FOUND) {
			break;
		}
		gc_ArrayList_append(
			words, gc_Bytes_slice(self, ind_begin, sep_ind, 1)
		);
		ind_begin = sep_ind + (gc_Int)sep_length;
	}
	if (gc_ArrayList_length(words) < max_words) {
		if (ind_begin < (gc_Int)self->size) {
			gc_Bytes* last_sub = gc_Bytes_slice(self, ind_begin, self->size, 1);
			if (last_sub->length > 0) {
				gc_ArrayList_append(words, last_sub);
			} else {
				last_sub = gc_Bytes_del(last_sub);
			}
		}
	}
	if (gc_ArrayList_is_sparse(words)) {
		gc_ArrayList_shrink(words);
	}
	return words;
}

gc_ArrayList* gc_Bytes_chunks(gc_Bytes* self,
	gc_SizeT chunk_size,
	gc_SizeT max_chunks
) {
	gc_SizeT num_chunks = (max_chunks > 0) ? max_chunks : 2;
	gc_ArrayList* chunks = gc_ArrayList_init(num_chunks, 1.5);
	if (max_chunks < 1) {
		return chunks;
	}
	if (self->length < chunk_size) {
		gc_ArrayList_append(chunks, gc_Bytes_copy(self));
		return chunks;
	}
	gc_SizeT beg_index = 0;
	while (beg_index + chunk_size < self->size) {
		if (gc_ArrayList_length(chunks) >= max_chunks) {
			break;
		}
		if (gc_ArrayList_is_dense(chunks)) {
			gc_ArrayList_expand(chunks);
		}
		gc_ArrayList_append(
			chunks, gc_Bytes_slice(self, beg_index, beg_index + chunk_size, 1)
		);
		beg_index += chunk_size;
	}
	if (gc_ArrayList_length(chunks) < max_chunks) {
		if (beg_index < self->size) {
			gc_Bytes* last_chunk = gc_Bytes_slice(self, beg_index, self->size, 1);
			if (last_chunk->length > 0) {
				gc_ArrayList_append(chunks, last_chunk);
			} else {
				last_chunk = gc_Bytes_del(last_chunk);
			}
		}
	}
	if (gc_ArrayList_is_sparse(chunks)) {
		gc_ArrayList_shrink(chunks);
	}
	return chunks;
}

gc_Bytes* gc_Bytes_copy(gc_Bytes* self) {
	gc_Bytes* cp = (gc_Bytes*)gc_malloc(gc_sizeof(gc_Bytes));
	if (gc_not cp) {
		gc_printf("Could not create gc_Bytes!\n");
		gc_exit(EXIT_NULLPTR);
	}
	cp->size = self->size;
	cp->byte_str = copy_byte_str_ex(self->byte_str, self->length);
	cp->hash = self->hash;
	cp->length = self->length;
	cp->byte_str[self->length] = '\0';
	return cp;
}

gc_Bytes* gc_Bytes_upper(gc_Bytes* self) {
	gc_SizeT array_length = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_length + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_BytesArray_push_byte_str(array, self->byte_str);
	for (gc_SizeT i = 0; i < gc_BytesArray_length(array); i++) {
		gc_Char c = gc_BytesArray_get(array, i);
		if (gc_not is_ascii_letter_upper(c)) {
			c -= 32;
			gc_BytesArray_set(array, i, c);
		}
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_lower(gc_Bytes* self) {
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_BytesArray_push_byte_str(array, self->byte_str);
	for (gc_SizeT i = 0; i < gc_BytesArray_length(array); i++) {
		gc_Char c = gc_BytesArray_get(array, i);
		if (gc_not is_ascii_letter_lower(c)) {
			c += 32;
			gc_BytesArray_set(array, i, c);
		}
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_title(gc_Bytes* self) {
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_BytesArray_push_byte_str(array, self->byte_str);
	gc_Bool in_word = True;
	gc_Bool is_first_letter = True;
	gc_SizeT array_length = gc_BytesArray_length(array);
	for (gc_SizeT i = 0; i < array_length; i++) {
		gc_Char c = gc_BytesArray_get(array, i);
		if (is_letter_whitespace(c) gc_or is_letter_punctuation(self->byte_str[i])) {
			in_word = False;
		} else {
			in_word = True;
		}
		if (in_word) {
			if (is_first_letter) {
				if (gc_not is_ascii_letter_upper(c)) {
					c -= 32;
					gc_BytesArray_set(array, i, c);
				}
				is_first_letter = False;
			} else {
				if (gc_not is_ascii_letter_lower(c)) {
					c += 32;
					gc_BytesArray_set(array, i, c);
				}
			}
		} else {
			gc_SizeT j = i + 1;
			if (j < array_length) {
				gc_Char next_c = gc_BytesArray_get(array, j);
				if (is_ascii_letter(next_c)) {
					is_first_letter = True;
				}
			}
		}
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_swapcase(gc_Bytes* self) {
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_BytesArray_push_byte_str(array, self->byte_str);
	for (gc_SizeT i = 0; i < gc_BytesArray_length(array); i++) {
		gc_Char c = gc_BytesArray_get(array, i);
		if (gc_not is_ascii_letter(c)) {
			continue;
		}
		if (is_ascii_letter_lower(c)) {
			c -= 32;
		} else {
			if (is_ascii_letter_upper(c)) {
				c += 32;
			}
		}
		gc_BytesArray_set(array, i, c);
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_capitalize(gc_Bytes* self) {
	if (self->length == 1) {
		if (is_ascii_letter(self->byte_str[0])) {
			gc_Char buf[2];
			buf[0] = self->byte_str[0];
			if (is_ascii_letter_lower(buf[0])) {
				buf[0] -= 32;
			}
			buf[1] = '\0';
			return gc_Bytes_init(buf);
		} else {
			return gc_Bytes_copy(self);
		}
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_BytesArray_push_byte_str(array, self->byte_str);
	gc_Char c = gc_BytesArray_get(array, 0);
	if (is_ascii_letter_lower(c)) {
		if (is_ascii_letter(c)) {
			c -= 32;
		}
	}
	gc_BytesArray_set(array, 0, c);
	for (gc_SizeT i = 1; i < gc_BytesArray_length(array); i++) {
		gc_Char c = gc_BytesArray_get(array, i);
		if (gc_not is_ascii_letter_lower(c)) {
			c += 32;
			gc_BytesArray_set(array, i, c);
		}
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_reverse(gc_Bytes* self) {
	if (self->length <= 1) {
		return gc_Bytes_copy(self);
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_BytesArray_push_byte_str(array, self->byte_str);
	gc_BytesArray_reverse(array);
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_multiply(gc_Bytes* self, gc_SizeT num) {
	if (self->length == 0) {
		return gc_Bytes_copy(self);
	}
	if (num == 0) {
		return gc_Bytes_init("");
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size * num + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	for (gc_SizeT i = 0; i < num; i++) {
		gc_BytesArray_push_byte_str(array, self->byte_str);
	}
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_append(gc_Bytes* self, gc_Char c) {
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 2, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	if (self->byte_str[0] != '\0') {
		gc_BytesArray_push_byte_str(array, self->byte_str);
	}
	gc_BytesArray_append(array, c);
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_strip(gc_Bytes* self, gc_ConstCharPtr chars) {
	gc_CharPtr stripped = strip_byte_str_ex(self->byte_str, chars, self->length);
	gc_Bytes* res = gc_Bytes_cast_bytes(stripped, False);
	return res;
}

gc_Bytes* gc_Bytes_lstrip(gc_Bytes* self, gc_ConstCharPtr chars) {
	gc_CharPtr lstripped = lstrip_byte_str_ex(self->byte_str, chars, self->length);
	gc_Bytes* res = gc_Bytes_cast_bytes(lstripped, False);
	return res;
}

gc_Bytes* gc_Bytes_rstrip(gc_Bytes* self, gc_ConstCharPtr chars) {
	gc_CharPtr rstripped = rstrip_byte_str_ex(self->byte_str, chars, self->length);
	gc_Bytes* res = gc_Bytes_cast_bytes(rstripped, False);
	return res;
}

gc_Bytes* gc_Bytes_concat(gc_Bytes* self, gc_ConstCharPtr other) {
	if (other[0] == '\0') {
		return gc_Bytes_copy(self);
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	if (self->byte_str[0] != '\0') {
		gc_BytesArray_push_byte_str(array, self->byte_str);
	}
	gc_BytesArray_push_byte_str(array, other);
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_insert(gc_Bytes* self, gc_SizeT index, gc_Char c) {
	gc_CharPtr bytes = insert_byte_str(self->byte_str, self->length, index, c);
	gc_Bytes* res = gc_Bytes_cast_bytes(bytes, False);
	return res;
}

gc_Bytes* gc_Bytes_push_into(gc_Bytes* self, gc_SizeT index, gc_ConstCharPtr other) {
	if (self->length < 1 gc_or other[0] == '\0') {
		if (
			self->length < 1
			gc_and index == 0
			gc_and other[0] != '\0'
		) {
			return gc_Bytes_init(other);
		}
		return gc_Bytes_copy(self);
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->length);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_SizeT i = 0;
	for (; i < index; i++) {
		gc_BytesArray_append(array, self->byte_str[i]);
	}
	gc_BytesArray_push_byte_str(array, other);
	for (; self->byte_str[i] != '\0'; i++) {
		if (gc_BytesArray_is_dense(array)) {
			gc_BytesArray_expand(array);
		}
		gc_BytesArray_append(array, self->byte_str[i]);
	}
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_replace(gc_Bytes* self,
	gc_ConstCharPtr old_str,
	gc_ConstCharPtr new_str,
	gc_SizeT num_replaces
) {
	if (num_replaces < 1) {
		return gc_Bytes_copy(self);
	}
	if (gc_Bytes_length(self) < 1) {
		if (old_str[0] == '\0' gc_and new_str[0] != '\0') {
			return gc_Bytes_init(new_str);
		}
		return gc_Bytes_init("\0");
	}
	if (old_str[0] == '\0') {
		return gc_Bytes_copy(self);
	}
	gc_SizeT start = 0;
	gc_SizeT replaces = 0;
	gc_SizeT old_str_length = get_byte_str_length(old_str);
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	while (start < self->length) {
		gc_Int start_ind = gc_Bytes_find(self, old_str, start, self->size);
		if (start_ind == INDEX_NOT_FOUND) {
			break;
		}
		if (replaces >= num_replaces) {
			break;
		}
		gc_Bytes* sub = gc_Bytes_slice(self, start, start_ind, 1);
		gc_BytesArray_push_byte_str(array, sub->byte_str);
		gc_BytesArray_push_byte_str(array, new_str);
		start = (gc_SizeT)start_ind + old_str_length;
		sub = gc_Bytes_del(sub);
		replaces++;
	}
	if (start < self->length) {
		gc_Bytes* sub = gc_Bytes_slice(self, start, self->size, 1);
		gc_BytesArray_push_byte_str(array, sub->byte_str);
		sub = gc_Bytes_del(sub);
	}
	if (gc_BytesArray_length(array) == 0) {
		array = gc_BytesArray_del(array);
		return gc_Bytes_copy(self);
	}
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_slice(gc_Bytes* self,
	gc_SizeT start,
	gc_SizeT stop,
	gc_SizeT step
) {
	if (self->length < 1) {
		return gc_Bytes_init("\0");
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_BytesArray_push_byte_str(array, self->byte_str);
	gc_BytesArray* slice = gc_BytesArray_slice(array, start, stop, step);
	array = gc_BytesArray_del(array);
	gc_BytesArray_append(slice, '\0');
	gc_Bytes* res = gc_BytesArray_cast(slice);
	return res;
}

gc_Int gc_Bytes_index(gc_Bytes* self,
	gc_Char c,
	gc_SizeT start,
	gc_SizeT stop
) {
	gc_Int index = INDEX_NOT_FOUND;
	if (c == '\0') {
		return 0;
	}
	if (start > stop) {
		gc_SizeT s = stop;
		stop = start;
		start = s;
	}
	if (stop == self->size) {
		stop--;
	}
	for (gc_SizeT i = start; i < stop; i++) {
		if (self->byte_str[i] == c) {
			index = (gc_Int)i;
			break;
		}
	}
	return index;
}

gc_Int gc_Bytes_rindex(gc_Bytes* self,
	gc_Char c,
	gc_SizeT start,
	gc_SizeT stop
) {
	gc_Int index = INDEX_NOT_FOUND;
	if (c == '\0') {
		return gc_Bytes_length(self);
	}
	if (start > stop) {
		gc_SizeT s = stop;
		stop = start;
		start = s;
	}
	if (stop == self->size) {
		stop--;
	}
	gc_Int end_ind = (gc_Int)stop - 1;
	if (end_ind < 0) {
		return index;
	}
	for(; end_ind >= (gc_Int)start; end_ind--) {
		if (gc_Bytes_get(self, end_ind) == c) {
			index = end_ind;
			break;
		}
	}
	return index;
}

gc_Int gc_Bytes_find(gc_Bytes* self,
	gc_ConstCharPtr other,
	gc_SizeT start,
	gc_SizeT stop
) {
	gc_Int index = INDEX_NOT_FOUND;
	if (other[0] == '\0') {
		return 0;
	}
	gc_SizeT other_length = get_byte_str_length(other);
	if (other_length > self->length) {
		return index;
	}
	if (other_length == 1) {
		return gc_Bytes_index(self, other[0], start, stop);
	}
	if (start > stop) {
		gc_SizeT s = stop;
		stop = start;
		start = s;
	}
	if (stop == self->size) {
		stop--;
	}
	for (gc_SizeT i = start; i < stop; i++) {
		if (self->length - i < other_length) {
			break;
		}
		if (self->byte_str[i] == other[0]) {
			gc_SizeT j = i + 1, k = 1;
			for (; other[k] != '\0'; k++) {
				if (self->byte_str[j] != other[k]) {
					break;
				}
				j++;
			}
			if (k == other_length) {
				index = (gc_Int)i;
				break;
			}
		}
	}
	return index;
}

gc_Int gc_Bytes_rfind(gc_Bytes* self,
	gc_ConstCharPtr other,
	gc_SizeT start,
	gc_SizeT stop
) {
	gc_Int index = INDEX_NOT_FOUND;
	if (other[0] == '\0') {
		return gc_Bytes_length(self);
	}
	gc_SizeT other_length = get_byte_str_length(other);
	if (other_length > self->length) {
		return index;
	}
	if (other_length == 1) {
		return gc_Bytes_rindex(self, other[0], start, stop);
	}
	if (start > stop) {
		gc_SizeT s = stop;
		stop = start;
		start = s;
	}
	if (stop == self->size) {
		stop--;
	}
	gc_Int start_ind = (gc_Int)stop - 1;
	if (start_ind < (gc_Int)start) {
		return index;
	}
	gc_Int other_start = (gc_Int)other_length - 1;
	for(; start_ind >= (gc_Int)start; start_ind--) {
		if (start_ind + 1 < other_length) {
			break;
		}
		if (self->byte_str[start_ind] == other[other_start]) {
			gc_SizeT cntr = 1;
			for(; cntr < other_length; cntr++) {
				if (self->byte_str[start_ind - cntr] != other[other_start - cntr]) {
					break;
				}
			}
			if (cntr == other_length) {
				index = start_ind - (gc_Int)other_length + 1;
				break;
			}
		}
	}
	return index;
}


gc_CharPtr copy_byte_str(gc_ConstCharPtr byte_str) {
	gc_SizeT initial_length = get_byte_str_length(byte_str);
	return copy_byte_str_ex(byte_str, initial_length);
}

gc_CharPtr lstrip_byte_str(gc_ConstCharPtr byte_str, gc_ConstCharPtr chars) {
	gc_SizeT initial_length = get_byte_str_length(byte_str);
	return lstrip_byte_str_ex(byte_str, chars, initial_length);
}

gc_CharPtr rstrip_byte_str(gc_ConstCharPtr byte_str, gc_ConstCharPtr chars) {
	gc_SizeT initial_length = get_byte_str_length(byte_str);
	return rstrip_byte_str_ex(byte_str, chars, initial_length);
}

gc_CharPtr strip_byte_str(gc_ConstCharPtr byte_str, gc_ConstCharPtr chars) {
	gc_SizeT initial_length = get_byte_str_length(byte_str);
	return strip_byte_str_ex(byte_str, chars, initial_length);
}

gc_CharPtr copy_byte_str_ex(gc_ConstCharPtr byte_str, gc_SizeT byte_str_length) {
	gc_CharPtr cp = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * (byte_str_length + 1));
	if (gc_not cp) {
		gc_printf("Could not create gc_CharPtr in copy_byte_str_ex!\n");
		gc_exit(EXIT_NULLPTR);
	}
	if (byte_str) {
		gc_SizeT i = 0;
		for (; byte_str[i] != '\0'; i++) {
			cp[i] = byte_str[i];
		}
		cp[i] = '\0';
	} else {
		cp[0] = '\0';
	}
	return cp;
}

gc_CharPtr lstrip_byte_str_ex(
	gc_ConstCharPtr byte_str,
	gc_ConstCharPtr chars,
	gc_SizeT byte_str_length
) {
	if (chars[0] == '\0') {
		return copy_byte_str_ex(byte_str, byte_str_length);
	}
	if (byte_str[0] == '\0') {
		gc_CharPtr r = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char));
		if (gc_not r) {
			gc_printf("Could not create new byte_str for lstrip_byte_str!\n");
			gc_exit(EXIT_NULLPTR);
		}
		r[0] = '\0';
		return r;
	}
	gc_CharPtr res = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * (byte_str_length + 1));
	if (gc_not res) {
		gc_printf("Could not create new byte_str for lstrip_byte_str!\n");
		gc_exit(EXIT_NULLPTR);
	}
	gc_SizeT i = 0, j = 0;
	gc_Bool stripped = False;
	for (; byte_str[i] != '\0'; i++) {
		if (stripped) {
			res[j] = byte_str[i];
			j++;
		} else {
			gc_Bool in_strip = False;
			for (gc_SizeT k = 0; chars[k] != '\0'; k++) {
				if (byte_str[i] == chars[k]) {
					in_strip = True;
					break;
				}
			}
			if (gc_not in_strip) {
				stripped = True;
				res[j] = byte_str[i];
				j++;
			}
		}
	}
	res[j] = '\0';
	return res;
}

gc_CharPtr rstrip_byte_str_ex(
	gc_ConstCharPtr byte_str,
	gc_ConstCharPtr chars,
	gc_SizeT byte_str_length
) {
	if (chars[0] == '\0') {
		return copy_byte_str_ex(byte_str, byte_str_length);
	}
	if (byte_str[0] == '\0') {
		gc_CharPtr r = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char));
		if (gc_not r) {
			gc_printf("Could not create new byte_str for rstrip_byte_str!\n");
			gc_exit(EXIT_NULLPTR);
		}
		r[0] = '\0';
		return r;
	}
	gc_SizeT size = byte_str_length + 1;
	gc_CharPtr res = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * size);
	if (gc_not res) {
		gc_printf("Could not create new byte_str for rstrip_byte_str!\n");
		gc_exit(EXIT_NULLPTR);
	}
	gc_Int end_ind = byte_str_length - 1;
	for (; end_ind >= 0; end_ind--) {
		gc_Bool in_strip = False;
		for (gc_SizeT i = 0; chars[i] != '\0'; i++) {
			if (byte_str[end_ind] == chars[i]) {
				in_strip = True;
				break;
			}
		}
		if (gc_not in_strip) {
			break;
		}
	}
	gc_SizeT length = 0;
	if (end_ind >= 0) {
		for (gc_SizeT i = 0; i <= end_ind; i++) {
			res[i] = byte_str[i];
			length++;
		}
	}
	res[length] = '\0';
	return res;
}

gc_CharPtr strip_byte_str_ex(
	gc_ConstCharPtr byte_str,
	gc_ConstCharPtr chars,
	gc_SizeT byte_str_length
) {
	if (chars[0] == '\0') {
		return copy_byte_str_ex(byte_str, byte_str_length);
	}
	gc_CharPtr res;
	gc_Int stop_ind = (gc_Int)byte_str_length - 1;
	if (stop_ind < 0) {
		res = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * 1);
		if (gc_not res) {
			gc_printf("Could not create gc_CharPtr in strip_byte_str_ex!\n");
			gc_exit(EXIT_NULLPTR);
		}
		res[0] = '\0';
		return res;
	}
	gc_SizeT start_ind = 0;
	for(; start_ind < byte_str_length; start_ind++) {
		gc_Bool in = False;
		for (gc_SizeT i = 0; chars[i] != '\0'; i++) {
			if (byte_str[start_ind] == chars[i]) {
				in = True;
				break;
			}
		}
		if (gc_not in) {
			break;
		}
	}
	for (; stop_ind >= (gc_Int)start_ind; stop_ind--) {
		gc_Bool in = False;
		for (gc_SizeT i = 0; chars[i] != '\0'; i++) {
			if (byte_str[stop_ind] == chars[i]) {
				in = True;
				break;
			}
		}
		if (gc_not in) {
			break;
		}
	}
	gc_SizeT i = 0;
	gc_SizeT res_length = stop_ind - start_ind + 1;
	res = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * (res_length + 1));
	if (gc_not res) {
		gc_printf("Could not create gc_CharPtr in strip_byte_str_ex!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (; (gc_Int)start_ind <= stop_ind; start_ind++) {
		res[i] = byte_str[start_ind];
		i++;
	}
	res[res_length] = '\0';
	return res;
}

gc_CharPtr insert_byte_str(
	gc_ConstCharPtr byte_str,
	gc_SizeT byte_str_length,
	gc_SizeT index,
	gc_Char c
) {
	if (c == '\0' gc_or byte_str[0] == '\0') {
		if (
			c != '\0'
			gc_and byte_str[0] == '\0'
			gc_and index == 0
		) {
			gc_Char buf[2];
			buf[0] = c;
			buf[1] = '\0';
			return copy_byte_str_ex(buf, 1);
		}
		return copy_byte_str_ex(byte_str, byte_str_length);
	}
	gc_SizeT res_length = byte_str_length + 1;
	gc_CharPtr res = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * (res_length + 1));
	if (gc_not res) {
		gc_printf("Could not create gc_CharPtr in insert_byte_str!\n");
		gc_exit(EXIT_NULLPTR);
	}
	gc_SizeT i = 0;
	for (; i < index; i++) {
		res[i] = byte_str[i];
	}
	res[index] = c;
	gc_SizeT j = index + 1;
	for (; byte_str[i] != '\0'; j++) {
		res[j] = byte_str[i];
		i++;
	}
	res[j] = '\0';
	return res;
}

gc_SizeT get_byte_str_length(gc_ConstCharPtr byte_str) {
	gc_SizeT length = 0;
	for (; byte_str[length] != '\0'; length++);
	return length;
}

gc_Bool is_letter_digit(gc_Char c) {
	if (c == '\0') {
		return False;
	}
	gc_UInt8 code = (gc_UInt8)c;
	return code >= (gc_UInt8)'0' gc_and code <= (gc_UInt8)'9';
}

gc_Bool is_ascii_letter(gc_Char c) {
	if (c == '\0') {
		return False;
	}
	gc_UInt8 code = (gc_UInt8)c;
	if (
		gc_not (
			(code >= (gc_UInt8)'A' gc_and code <= (gc_UInt8)'Z')
			gc_or (code >= (gc_UInt8)'a' gc_and code <= (gc_UInt8)'z')
		)
	) {
		return False;
	}
	return True;
}

gc_Bool is_letter_whitespace(gc_Char c) {
	gc_UInt8 code = (gc_UInt8)c;
	if (code == 32) {
		return True;
	}
	return code >= 9 gc_and code <= 13;
}

gc_Bool is_letter_printable(gc_Char c) {
	if (c == '\0') {
		return False;
	}
	gc_UInt8 code = (gc_UInt8)c;
	if (code == 32) {
		return True;
	}
	if (code >= 9 gc_and code <= 13) {
		return True;
	}
	return code >= 33 gc_and code <= 126;
}

gc_Bool is_letter_punctuation(gc_Char c) {
	if (c == '\0') {
		return False;
	}
	gc_UInt8 code = (gc_UInt8)c;
	if (code >= 33 gc_and code <= 47) {
		return True;
	}
	if (code >= 58 gc_and code <= 64) {
		return True;
	}
	if (code >= 91 gc_and code <= 96) {
		return True;
	}
	return code >= 123 gc_and code <= 126;
}

gc_Bool is_ascii_letter_upper(gc_Char c) {
	if (gc_not is_ascii_letter(c)) {
		return True;
	}
	return is_ascii_letter_upper_strict(c);
}

gc_Bool is_ascii_letter_lower(gc_Char c) {
	if (gc_not is_ascii_letter(c)) {
		return True;
	}
	return is_ascii_letter_lower_strict(c);
}

gc_Bool is_ascii_letter_upper_strict(gc_Char c) {
	gc_UInt8 code = (gc_UInt8)c;
	return code >= (gc_UInt8)'A' gc_and code <= (gc_UInt8)'Z';
}

gc_Bool is_ascii_letter_lower_strict(gc_Char c) {
	gc_UInt8 code = (gc_UInt8)c;
	return code >= (gc_UInt8)'a' gc_and code <= (gc_UInt8)'z';
}
