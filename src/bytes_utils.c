#include "../headers/bytes_utils.h"

#include "../headers/bytes_array.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct gc_BytesCodesGen {
	gc_Bytes* bytes;
	gc_SizeT current_ind;
};

gc_BytesCodesGen* gc_BytesCodesGen_init(gc_Bytes* bytes) {
	gc_BytesCodesGen* self = (gc_BytesCodesGen*)gc_malloc(gc_sizeof(gc_BytesCodesGen));
	if (gc_not self) {
		gc_printf("Could not create gc_BytesCodesGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->bytes = bytes;
	self->current_ind = 0;
	return self;
}

gc_VoidPtr gc_BytesCodesGen_del(gc_BytesCodesGen* obj) {
	gc_free(obj);
	return NONE;
}

gc_Bool gc_BytesCodesGen_has_next(gc_BytesCodesGen* self) {
	gc_SizeT bytes_length = gc_Bytes_length(self->bytes);
	if (bytes_length < 1) {
		return False;
	}
	return self->current_ind < bytes_length;
}

gc_UInt8 gc_BytesCodesGen_next(gc_BytesCodesGen* self) {
	gc_UInt8 code = (gc_UInt8)gc_Bytes_get(self->bytes, self->current_ind);
	self->current_ind++;
	return code;
}


struct gc_BytesSplitGen {
	gc_Bytes* bytes;
	gc_Bytes* sep;
	gc_Int ind_begin;
	gc_SizeT word_cntr;
	gc_SizeT max_words;
	gc_Bool exhausted;
};

gc_BytesSplitGen* gc_BytesSplitGen_init(
	gc_Bytes* bytes,
	gc_ConstCharPtr sep,
	gc_SizeT max_words
) {
	gc_BytesSplitGen* self = (gc_BytesSplitGen*)gc_malloc(gc_sizeof(gc_BytesSplitGen));
	if (gc_not self) {
		gc_printf("Could not create gc_BytesSplitGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->bytes = bytes;
	self->sep = gc_Bytes_init(sep);
	self->max_words = max_words;
	self->word_cntr = 0;
	self->exhausted = False;
	gc_Int sep_ind = gc_Bytes_find(
		self->bytes,
		gc_Bytes_byte_str(self->sep),
		0,
		gc_Bytes_size(self->bytes)
	);
	if (sep_ind == INDEX_NOT_FOUND) {
		self->ind_begin = INDEX_NOT_FOUND;
	} else {
		self->ind_begin = sep_ind + (gc_Int)gc_Bytes_length(self->sep);
	}
	return self;
}

gc_VoidPtr gc_BytesSplitGen_del(gc_BytesSplitGen* obj) {
	obj->sep = gc_Bytes_del(obj->sep);
	gc_free(obj);
	return NONE;
}

gc_Bool gc_BytesSplitGen_has_next(gc_BytesSplitGen* self) {
	if (self->word_cntr >= self->max_words) {
		return False;
	}
	if (self->ind_begin >= (gc_Int)gc_Bytes_size(self->bytes)) {
		return False;
	}
	return gc_not self->exhausted;
}

gc_Bytes* gc_BytesSplitGen_next(gc_BytesSplitGen* self) {
	if (self->ind_begin == INDEX_NOT_FOUND gc_and gc_not self->exhausted) {
		self->exhausted = True;
		return gc_Bytes_copy(self->bytes);
	}
	gc_Bytes* next;
	gc_Int sep_ind;
	if (self->word_cntr == 0) {
		sep_ind = (gc_SizeT)(self->ind_begin - gc_Bytes_length(self->sep));
		next = gc_Bytes_slice(self->bytes, 0, sep_ind, 1);
	} else {
		sep_ind = gc_Bytes_find(
			self->bytes,
			gc_Bytes_byte_str(self->sep),
			self->ind_begin,
			gc_Bytes_size(self->bytes)
		);
		if (sep_ind != INDEX_NOT_FOUND) {
			next = gc_Bytes_slice(self->bytes, self->ind_begin, sep_ind, 1);
			self->ind_begin = sep_ind + (gc_Int)gc_Bytes_length(self->sep);
		} else {
			next = gc_Bytes_slice(self->bytes, self->ind_begin, gc_Bytes_size(self->bytes), 1);
			self->ind_begin = INDEX_NOT_FOUND;
			self->exhausted = True;
		}
	}
	self->word_cntr++;
	return next;
}


struct gc_BytesChunksGen {
	gc_Bytes* bytes;
	gc_SizeT chunk_size;
	gc_SizeT chunks_cntr;
	gc_SizeT max_chunks;
	gc_SizeT beg_index;
	gc_Bool exhausted;
};

gc_BytesChunksGen* gc_BytesChunksGen_init(
	gc_Bytes* bytes,
	gc_SizeT chunk_size,
	gc_SizeT max_chunks
) {
	gc_BytesChunksGen* self = (gc_BytesChunksGen*)gc_malloc(gc_sizeof(gc_BytesChunksGen));
	if (gc_not self) {
		gc_printf("Could not create gc_BytesChunksGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->bytes = bytes;
	self->chunk_size = chunk_size;
	self->max_chunks = max_chunks;
	self->chunks_cntr = 0;
	self->beg_index = 0;
	self->exhausted = False;
	return self;
}

gc_VoidPtr gc_BytesChunksGen_del(gc_BytesChunksGen* obj) {
	gc_free(obj);
	return NONE;
}

gc_Bool gc_BytesChunksGen_has_next(gc_BytesChunksGen* self) {
	if (self->chunks_cntr >= self->max_chunks) {
		return False;
	}
	return gc_not self->exhausted;
}

gc_Bytes* gc_BytesChunksGen_next(gc_BytesChunksGen* self) {
	gc_Bytes* next;
	if (self->beg_index + self->chunk_size >= gc_Bytes_size(self->bytes)) {
		if (self->beg_index < gc_Bytes_size(self->bytes)) {
			next = gc_Bytes_slice(
				self->bytes,
				self->beg_index,
				gc_Bytes_size(self->bytes),
				1
			);
		} else {
			next = gc_Bytes_init("\0");
		}
		self->exhausted = True;
		return next;
	}
	gc_SizeT end_index = self->beg_index + self->chunk_size;
	next = gc_Bytes_slice(self->bytes, self->beg_index, end_index, 1);
	self->beg_index = end_index;
	self->chunks_cntr++;
	return next;
}


struct gc_RandomBytesGen {
	gc_ConstCharPtr alphabet;
	gc_SizeT length;
	gc_Int num;
	gc_Int alphabet_length;
};

gc_RandomBytesGen* gc_RandomBytesGen_init(
	gc_SizeT num,
	gc_SizeT length,
	gc_ConstCharPtr alphabet
) {
	gc_RandomBytesGen* self = (gc_RandomBytesGen*)gc_malloc(gc_sizeof(gc_RandomBytesGen));
	if (gc_not self) {
		gc_printf("Could not create gc_RandomBytesGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->num = (gc_Int)num;
	self->length = length;
	self->alphabet = alphabet;
	self->alphabet_length = -1;
	return self;
}

gc_VoidPtr gc_RandomBytesGen_del(gc_RandomBytesGen* obj) {
	gc_free(obj);
	return NONE;
}

gc_Bool gc_RandomBytesGen_has_next(gc_RandomBytesGen* self) {
	if (self->length < 1 gc_or self->alphabet[0] == '\0') {
		return False;
	}
	return self->num > 0;
}

gc_Bytes* gc_RandomBytesGen_next(gc_RandomBytesGen* self) {
	if (self->alphabet_length < 0) {
		self->alphabet_length = (gc_Int)get_byte_str_length(self->alphabet);
	}
	gc_Bytes* next = gc_Bytes_random_bytes_ex(
		self->length,
		self->alphabet_length,
		self->alphabet
	);
	self->num--;
	return next;
}


struct gc_BytesSlidingWindowsGen {
	gc_Bytes* bytes;
	gc_SizeT window_length;
	gc_SizeT beg_index;
	gc_Bool exhausted;
};

gc_BytesSlidingWindowsGen* gc_BytesSlidingWindowsGen_init(gc_Bytes* bytes, gc_SizeT window_length) {
	gc_BytesSlidingWindowsGen* self = (gc_BytesSlidingWindowsGen*)gc_malloc(gc_sizeof(gc_BytesSlidingWindowsGen));
	if (gc_not self) {
		gc_printf("Could not create gc_BytesSlidingWindowsGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->bytes = bytes;
	self->window_length = window_length;
	self->beg_index = 0;
	if (self->window_length < 1) {
		self->exhausted = True;
	} else {
		self->exhausted = False;
	}
	return self;
}

gc_VoidPtr gc_BytesSlidingWindowsGen_del(gc_BytesSlidingWindowsGen* obj) {
	gc_free(obj);
	return NONE;
}

gc_Bool gc_BytesSlidingWindowsGen_has_next(gc_BytesSlidingWindowsGen* self) {
	if (self->beg_index >= gc_Bytes_length(self->bytes)) {
		return False;
	}
	return gc_not self->exhausted;
}

gc_Bytes* gc_BytesSlidingWindowsGen_next(gc_BytesSlidingWindowsGen* self) {
	gc_Bytes* next;
	gc_SizeT bytes_length = gc_Bytes_length(self->bytes);
	gc_SizeT end_index = self->beg_index + self->window_length;
	if (end_index > bytes_length) {
		next = gc_Bytes_slice(self->bytes, self->beg_index, bytes_length, 1);
		self->exhausted = True;
		return next;
	}
	next = gc_Bytes_slice(self->bytes, self->beg_index, end_index, 1);
	self->beg_index++;
	return next;
}


gc_Bytes* gc_Bytes_join(gc_ConstCharPtr sep, gc_ArrayList* gc_bytes_lst) {
	gc_SizeT array_length = gc_ArrayList_length(gc_bytes_lst);
	if (array_length < 1) {
		return gc_Bytes_init("\0");
	}
	gc_SizeT sep_length = get_byte_str_length(sep);
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(sep_length * array_length);
	gc_BytesArray* array = gc_BytesArray_init(
		1 + array_size,
		MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE
	);
	for (gc_SizeT i = 0; i < array_length; i++) {
		gc_Bytes* bytes = (gc_Bytes*)gc_ArrayList_get(gc_bytes_lst, i);
		if (gc_Bytes_get(bytes, 0) == '\0') {
			continue;
		}
		gc_BytesArray_push_byte_str(array, gc_Bytes_byte_str(bytes));
		if (i != array_length - 1) {
			if(sep[0] != '\0') {
				gc_BytesArray_push_byte_str(array, sep);
			}
		}
	}
	if (gc_BytesArray_is_dense(array)) {
		gc_BytesArray_expand(array);
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_random_bytes(gc_SizeT length, gc_ConstCharPtr alphabet) {
	gc_SizeT alphabet_length = get_byte_str_length(alphabet);
	return gc_Bytes_random_bytes_ex(length, alphabet_length, alphabet);
}

gc_Bytes* gc_Bytes_random_bytes_ex(
	gc_SizeT length,
	gc_SizeT alphabet_length,
	gc_ConstCharPtr alphabet
) {
	if (length < 1 gc_or alphabet[0] == '\0') {
		return gc_Bytes_init("\0");
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(length + 1);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	while (gc_BytesArray_length(array) < length) {
		gc_Int index = gc_random(0, alphabet_length - 1);
		if (alphabet[index] != '\0') {
			gc_BytesArray_append(array, alphabet[index]);
		}
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_remove_external_parentheses(gc_Bytes* self) {
	gc_SizeT i = 0, j = gc_Bytes_length(self);
	if (j < 1) {
		return gc_Bytes_init("\0");
	}
	if (gc_Bytes_startswith(self, "\'") gc_or gc_Bytes_startswith(self, "\"")) {
		i = 1;
	}
	if (gc_Bytes_endswith(self, "\'") gc_or gc_Bytes_endswith(self, "\"")) {
		j--;
	}
	gc_Bytes* res;
	if (i < j) {
		res = gc_Bytes_slice(self, i, j, 1);
	} else {
		res = gc_Bytes_init("\0");
	}
	return res;
}

gc_Bytes* gc_Bytes_translate(gc_Bytes* self, gc_HashMap* chars_translate_table) {
	gc_SizeT self_length = gc_Bytes_length(self);
	if (self_length < 1) {
		return gc_Bytes_init("\0");
	}
	if (gc_HashMap_length(chars_translate_table) < 1) {
		return gc_Bytes_copy(self);
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(gc_Bytes_size(self));
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	for (gc_SizeT i = 0; i < self_length; i++) {
		gc_Char c = gc_Bytes_get(self, i);
		gc_SizeT hash_c = (gc_SizeT)c;
		if (gc_HashMap_in(chars_translate_table, hash_c)) {
			gc_Char* new_c = (gc_Char*)gc_HashMap_get(chars_translate_table, hash_c);
			gc_BytesArray_append(array, *new_c);
		} else {
			gc_BytesArray_append(array, c);
		}
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_remove_chars(gc_Bytes* self, gc_HashSet* char_set) {
	gc_SizeT self_length = gc_Bytes_length(self);
	if (self_length < 1) {
		return gc_Bytes_init("\0");
	}
	if (gc_HashSet_length(char_set) < 1) {
		return gc_Bytes_copy(self);
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(gc_Bytes_size(self));
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	for (gc_SizeT i = 0; i < self_length; i++) {
		gc_Char c = gc_Bytes_get(self, i);
		if (gc_not gc_HashSet_in(char_set, (gc_UInt8)c)) {
			gc_BytesArray_append(array, c);
		}
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_Bytes* gc_Bytes_longest_common_substr(gc_Bytes* self, gc_ConstCharPtr other) {
	gc_SizeT self_length = gc_Bytes_length(self);
	if (self_length < 1 gc_or other[0] == '\0') {
		return gc_Bytes_init("\0");
	}
	gc_SizeT other_length = get_byte_str_length(other);
	gc_SizeT prev_arr_size = other_length + 1;
	gc_SizeT* prev = (gc_SizeT*)gc_malloc(gc_sizeof(gc_SizeT) * prev_arr_size);
	if (gc_not prev) {
		gc_printf("Could not create prev array for gc_Bytes_common_prefix_length func!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (gc_SizeT i = 0; i < prev_arr_size; i++) {
		prev[i] = 0;
	}
	gc_SizeT best_len = 0, best_end = 0;
	for (gc_SizeT i = 1; i < self_length + 1; i++) {
		gc_SizeT* cur = (gc_SizeT*)gc_malloc(gc_sizeof(gc_SizeT) * prev_arr_size);
		if (gc_not cur) {
			gc_printf("Could not create cur array for gc_Bytes_common_prefix_length func!\n");
			gc_exit(EXIT_NULLPTR);
		}
		for (gc_SizeT j = 0; j < prev_arr_size; j++) {
			cur[j] = 0;
		}
		gc_Char ai = gc_Bytes_get(self, i - 1);
		for (gc_SizeT j = 1; j < other_length + 1; j++) {
			gc_Char c = other[j - 1];
			if (ai == c) {
				cur[j] = prev[j - 1] + 1;
				if (cur[j] > best_len) {
					best_len = cur[j];
					best_end = i;
				}
			}
		}
		gc_free(prev);
		prev = cur;
	}
	gc_free(prev);
	gc_Bytes* res = gc_Bytes_slice(self, best_end - best_len, best_end, 1);
	return res;
}

gc_SizeT gc_Bytes_common_prefix_length(gc_Bytes* self, gc_ConstCharPtr other) {
	gc_SizeT self_length = gc_Bytes_length(self);
	if (self_length < 1) {
		if (other[0] == '\0') {
			return 1;
		} else {
			return 0;
		}
	}
	if (other[0] == '\0') {
		return 0;
	}
	gc_SizeT prefix_len = 0;
	for (; prefix_len < self_length; prefix_len++) {
		if (other[prefix_len] == '\0') {
			break;
		}
		gc_Char c = gc_Bytes_get(self, prefix_len);
		if (c != other[prefix_len]) {
			break;
		}
	}
	return prefix_len;
}

gc_SizeT gc_Bytes_common_postfix_length(gc_Bytes* self, gc_ConstCharPtr other) {
	gc_SizeT self_length = gc_Bytes_length(self);
	if (other[0] == '\0') {
		if (self_length < 1) {
			return 1;
		} else {
			return 0;
		}
	}
	gc_SizeT postfix_length = 0;
	gc_SizeT other_length = get_byte_str_length(other);
	gc_Int j = (gc_Int)other_length - 1;
	for (gc_Int i = (gc_Int)(self_length - 1); i >= 0; i--) {
		if (other[j] == '\0' gc_or j < 0) {
			break;
		}
		gc_Char c = gc_Bytes_get(self, i);
		if (c == other[j]) {
			postfix_length++;
		} else {
			break;
		}
		j--;
	}
	return postfix_length;
}

gc_Bool gc_Bytes_contains_all_chars(gc_Bytes* self, gc_HashSet* char_set) {
	gc_SizeT self_length = gc_Bytes_length(self);
	if (self_length < 1) {
		if (gc_HashSet_length(char_set) < 1) {
			return True;
		}
		return False;
	}
	gc_Bool res = True;
	for (gc_SizeT i = 0; i < gc_Bytes_length(self); i++) {
		gc_Char c = gc_Bytes_get(self, i);
		if (gc_not gc_HashSet_in(char_set, (gc_UInt8)c)) {
			res = False;
			break;
		}
	}
	return res;
}

gc_Bool gc_Bytes_is_sub_in(gc_Bytes* self,
	gc_ConstCharPtr sub,
	gc_ConstCharPtr or_sep,
	gc_ConstCharPtr and_sep,
	gc_Bool ignore_case
) {
	gc_Bytes* sub_bytes = gc_Bytes_init(sub);
	gc_SizeT sub_length = gc_Bytes_length(sub_bytes);
	gc_SizeT self_length = gc_Bytes_length(self);
	if (sub_length == 0) {
		sub_bytes = gc_Bytes_del(sub_bytes);
		return True;
	}
	gc_Bytes* sor = gc_Bytes_init(or_sep);
	gc_Bytes* sand = gc_Bytes_init(and_sep);
	gc_Bytes* str;
	gc_Bytes* substr;
	gc_Bytes* sep_or;
	gc_Bytes* sep_and;
	if (ignore_case) {
		str = gc_Bytes_lower(self);
		substr = gc_Bytes_lower(sub_bytes);
		sub_bytes = gc_Bytes_del(sub_bytes);
		sep_or = gc_Bytes_lower(sor);
		sor = gc_Bytes_del(sor);
		sep_and = gc_Bytes_lower(sand);
		sand = gc_Bytes_del(sand);
	} else {
		str = self;
		substr = sub_bytes;
		sep_or = sor;
		sep_and = sand;
	}
	gc_BytesSplitGen* split_gen = gc_BytesSplitGen_init(substr, gc_Bytes_byte_str(sep_and), INF);
	gc_Bool and_res = True;
	while (gc_BytesSplitGen_has_next(split_gen)) {
		gc_Bytes* and_split = gc_BytesSplitGen_next(split_gen);
		gc_SizeT and_split_length = gc_Bytes_length(and_split);
		if (and_split_length < 1) {
			and_split = gc_Bytes_del(and_split);
			continue;
		}
		gc_Bool or_res = False;
		gc_BytesSplitGen* or_split_gen = gc_BytesSplitGen_init(and_split, gc_Bytes_byte_str(sep_or), INF);
		while (gc_BytesSplitGen_has_next(or_split_gen)) {
			gc_Bytes* or_split = gc_BytesSplitGen_next(or_split_gen);
			gc_SizeT or_split_length = gc_Bytes_length(or_split);
			if (or_split_length < 1) {
				or_split = gc_Bytes_del(or_split);
				or_res = True;
				continue;
			}
			if (or_split_length > self_length) {
				or_split = gc_Bytes_del(or_split);
				continue;
			}
			or_res = gc_Bytes_contains(
				str,
				gc_Bytes_byte_str(or_split),
				0,
				gc_Bytes_length(str)
			);
			or_split = gc_Bytes_del(or_split);
			if (or_res) {
				break;
			}
		}
		or_split_gen = gc_BytesSplitGen_del(or_split_gen);
		and_split = gc_Bytes_del(and_split);
		if (gc_not or_res) {
			and_res = False;
			break;
		}
	}
	split_gen = gc_BytesSplitGen_del(split_gen);
	sep_or = gc_Bytes_del(sep_or);
	sep_and = gc_Bytes_del(sep_and);
	substr = gc_Bytes_del(substr);
	if (ignore_case) {
		str = gc_Bytes_del(str);
	}
	return and_res;
}
