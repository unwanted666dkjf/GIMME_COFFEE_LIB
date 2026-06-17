#include <stdio.h>

#include "../headers/file.h"

#include "../headers/bytes_utils.h"

#include "../headers/bytes_array.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct gc_File {
	FILE* file;
	gc_Path* path;
	gc_Bool is_open;
};

gc_File* gc_File_init(gc_Path* path, gc_ConstCharPtr mode) {
	gc_File* self = (gc_File*)gc_malloc(gc_sizeof(gc_File));
	if (gc_not self) {
		gc_printf("Could not create gc_File!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->path = path;
	gc_ConstCharPtr path_str = gc_Path_byte_str(self->path);
	self->file = fopen(path_str, mode);
	if (self->file) {
		self->is_open = True;
	} else {
		self->is_open = False;
	}
	return self;
}

gc_VoidPtr gc_File_del(gc_File* obj) {
	if (obj->is_open) {
		fseek(obj->file, 0, SEEK_SET);
		fclose(obj->file);
	}
	gc_free(obj);
	return NONE;
}

gc_Void gc_File_close(gc_File* self) {
	if (self->is_open) {
		fseek(self->file, 0, SEEK_SET);
		fclose(self->file);
		self->is_open = False;
	}
}

gc_Void gc_File_rewind(gc_File* self) {
	if (self->is_open) {
		rewind(self->file);
	}
}

gc_Bool gc_File_is_open(gc_File* self) {
	return self->is_open;
}

gc_Bool gc_File_write(gc_File* self, gc_ConstCharPtr text) {
	if (gc_not self->is_open) {
		return False;
	}
	gc_Int res = fputs(text, self->file);
	if (res == EOF) {
		return False;
	}
	return True;
}

gc_Bool gc_File_seek(gc_File* self, gc_Int offset, gc_Int whence) {
	if (gc_not self->is_open) {
		return False;
	}
	if (fseek(self->file, offset, whence) != 0) {
		return False;
	}
	return True;
}

gc_Bool gc_File_writelines(gc_File* self,
	gc_ConstCharPtr lsep,
	gc_ArrayList* bytes_lst
) {
	gc_Bytes* content = gc_Bytes_join(lsep, bytes_lst);
	gc_Bool res = gc_File_write(self, gc_Bytes_byte_str(content));
	content = gc_Bytes_del(content);
	return res;
}

gc_Bytes* gc_File_read(gc_File* self) {
	gc_SizeT file_size;
	if (self->path) {
		gc_Stat file_stat = gc_Path_stat(self->path);
		file_size = file_stat.size;
	} else {
		file_size = MIN_GC_BYTES_ARRAY_SIZE_SAFE;
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(file_size);
	gc_BytesArray* array = gc_BytesArray_init(array_size + 1, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_Int c;
	while ((c = fgetc(self->file)) != EOF) {
		if (gc_BytesArray_is_dense(array)) {
			gc_BytesArray_expand(array);
		}
		gc_BytesArray_append(array, (gc_Char)c);
	}
	if (gc_BytesArray_is_dense(array)) {
		gc_BytesArray_expand(array);
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}

gc_ArrayList* gc_File_readlines(gc_File* self, gc_ConstCharPtr lsep) {
	gc_ArrayList* lines = gc_ArrayList_init(15, 1.5);
	gc_FileLineGen* line_gen = gc_FileLineGen_init(self);
	while (gc_FileLineGen_has_next(line_gen)) {
		if (gc_ArrayList_is_dense(lines)) {
			gc_ArrayList_expand(lines);
		}
		gc_Bytes* line = gc_FileLineGen_next(line_gen);
		gc_ArrayList_append(lines, line);
	}
	line_gen = gc_FileLineGen_del(line_gen);
	return lines;
}


struct gc_FileLineGen {
	gc_File* file;
	gc_Bool is_exhausted;
};

gc_FileLineGen* gc_FileLineGen_init(gc_File* file) {
	gc_FileLineGen* self = (gc_FileLineGen*)gc_malloc(gc_sizeof(gc_FileLineGen));
	if (gc_not self) {
		gc_printf("Could not create gc_FileLineGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->file = file;
	if (self->file->is_open) {
		self->is_exhausted = False;
	} else {
		self->is_exhausted = True;
	}
	return self;
}

gc_VoidPtr gc_FileLineGen_del(gc_FileLineGen* obj) {
	gc_free(obj);
	return NONE;
}

gc_Bool gc_FileLineGen_has_next(gc_FileLineGen* self) {
	return gc_not self->is_exhausted;
}

gc_Bytes* gc_FileLineGen_next(gc_FileLineGen* self) {
	gc_Int c;
	gc_BytesArray* array = gc_BytesArray_init(
		MIN_GC_BYTES_ARRAY_SIZE_SAFE,
		MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE
	);
	while ((c = fgetc(self->file->file)) != (gc_Int)'\n') {
		if (c == EOF) {
			self->is_exhausted = True;
			break;
		}
		if (gc_BytesArray_is_dense(array)) {
			gc_BytesArray_expand(array);
		}
		gc_BytesArray_append(array, (gc_Char)c);
	}
	if (gc_BytesArray_is_dense(array)) {
		gc_BytesArray_expand(array);
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}


struct gc_FileChunkGen {
	gc_File* file;
	gc_SizeT chunk_size;
	gc_Bool is_exhausted;
};

gc_FileChunkGen* gc_FileChunkGen_init(gc_File* file, gc_SizeT chunk_size) {
	gc_FileChunkGen* self = (gc_FileChunkGen*)gc_malloc(gc_sizeof(gc_FileChunkGen));
	if (gc_not self) {
		gc_printf("Could not create gc_FileChunkGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->file = file;
	self->chunk_size = chunk_size;
	if (self->chunk_size == 0 gc_or gc_not self->file->is_open) {
		self->is_exhausted = True;
	} else {
		self->is_exhausted = False;
	}
	return self;
}

gc_VoidPtr gc_FileChunkGen_del(gc_FileChunkGen* obj) {
	gc_free(obj);
	return NONE;
}

gc_Bool gc_FileChunkGen_has_next(gc_FileChunkGen* self) {
	return gc_not self->is_exhausted;
}

gc_Bytes* gc_FileChunkGen_next(gc_FileChunkGen* self) {
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self->chunk_size + 1);
	gc_BytesArray* array = gc_BytesArray_init(
		array_size + 1,
		MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE
	);
	for (gc_SizeT i = 0; i < self->chunk_size; i++) {
		gc_Int c = fgetc(self->file->file);
		if (c == EOF) {
			self->is_exhausted = True;
			break;
		}
		gc_BytesArray_append(array, (gc_Char)c);
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}


struct gc_FileSplitlinesGen {
	gc_File* file;
	gc_Bytes* lsep;
	gc_Bool is_exhausted;
};

gc_FileSplitlinesGen* gc_FileSplitlinesGen_init(gc_File* file, gc_ConstCharPtr lsep) {
	gc_FileSplitlinesGen* self = (gc_FileSplitlinesGen*)gc_malloc(gc_sizeof(gc_FileSplitlinesGen));
	if (gc_not self) {
		gc_printf("Could not create gc_FileSplitlinesGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->file = file;
	self->lsep = gc_Bytes_init(lsep);
	if (gc_not (self->file->is_open gc_and gc_Bytes_length(self->lsep) >= 1)) {
		self->is_exhausted = True;
	} else {
		self->is_exhausted = False;
	}
	return self;
}

gc_VoidPtr gc_FileSplitlinesGen_del(gc_FileSplitlinesGen* obj) {
	obj->lsep = gc_Bytes_del(obj->lsep);
	gc_free(obj);
	return NONE;
}

gc_Bool gc_FileSplitlinesGen_has_next(gc_FileSplitlinesGen* self) {
	return gc_not self->is_exhausted;
}

gc_Bytes* gc_FileSplitlinesGen_next(gc_FileSplitlinesGen* self) {
	gc_SizeT sep_length = gc_Bytes_length(self->lsep);
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(sep_length);
	gc_BytesArray* array = gc_BytesArray_init(
		array_size + 1,
		MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE
	);
	gc_Int c;
	while (True) {
		c = fgetc(self->file->file);
		if (c == EOF) {
			self->is_exhausted = True;
			break;
		}
		if (gc_BytesArray_is_dense(array)) {
			gc_BytesArray_expand(array);
		}
		if (c != gc_Bytes_get(self->lsep, 0)) {
			gc_BytesArray_append(array, c);
			continue;
		}
		gc_CharPtr buf = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * (sep_length + 1));
		buf[0] = c;
		gc_SizeT i = 1;
		for (; i < sep_length; i++) {
			gc_Int code = fgetc(self->file->file);
			if (code == EOF) {
				buf[i] = '\0';
				self->is_exhausted = True;
				break;
			} else {
				buf[i] = (gc_Char)code;
			}
			gc_Char sym = gc_Bytes_get(self->lsep, i);
			if (buf[i] != sym) {
				break;
			}
		}
		if (i >= sep_length) {
			gc_free(buf);
			break;
		}
		if (i > 1) {
			buf[i + 1] = '\0';
		} else {
			buf[i] = '\0';
		}
		gc_BytesArray_push_byte_str(array, buf);
		gc_free(buf);
		if (self->is_exhausted) {
			break;
		}
	}
	if (gc_BytesArray_is_dense(array)) {
		gc_BytesArray_expand(array);
	}
	gc_BytesArray_append(array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(array);
	return res;
}
