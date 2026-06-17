#include <stdio.h>

#include "../headers/inifile.h"

#include "../headers/file.h"

#include "../headers/bytes_utils.h"

#include "../headers/bytes_concrete_strip.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


const gc_Int GC_INI_FILE_SECTION_NOT_FOUND = -1;

const gc_Int GC_INI_FILE_OPTION_NOT_FOUND = -2;


static gc_Bool is_section(gc_Bytes* stripped_line);

static gc_Bytes* get_complete_section_name(gc_ConstCharPtr section_name);

static gc_INIFileOption* get_option(
	gc_Bytes* stripped_line,
	gc_Bytes* section_name,
	gc_ConstCharPtr name_valsep,
	gc_SizeT min_opt_len
);


struct gc_INIFile {
	gc_File* file;
	gc_Bytes* lsep;
	gc_Bytes* nameval_sep;
	gc_ArrayList* sections;
};

struct gc_INIFileCreator {
	gc_ArrayList* sections;
	gc_Bytes* lsep;
	gc_Bytes* nameval_sep;
};

struct gc_INIFileSection {
	gc_Bytes* name;
	gc_Int position;
};

struct gc_INIFileOption {
	gc_Bytes* name;
	gc_Bytes* value;
	gc_Bytes* section_name;
};

struct gc_INIFileOptionItem{
	gc_Bytes* option_name;
	gc_Bytes* option_value;
};

typedef struct {
	gc_Bytes* section_name;
	gc_ArrayList* section_options;
} gc_INIFileSectionItem;


static gc_INIFileSection* gc_INIFileSection_init(gc_Bytes* name, gc_Int position);

static gc_VoidPtr gc_INIFileSection_del(gc_INIFileSection* obj);


static gc_INIFileOption* gc_INIFileOption_init(gc_Bytes* name, gc_Bytes* value, gc_Bytes* section_name);


static gc_INIFileOptionItem* gc_INIFileOptionItem_init(gc_Bytes* name, gc_Bytes* value);

static gc_VoidPtr gc_INIFileOptionItem_del(gc_INIFileOptionItem* obj);


static gc_INIFileSectionItem* gc_INIFileSectionItem_init(gc_Bytes* name);

static gc_VoidPtr gc_INIFileSectionItem_del(gc_INIFileSectionItem* obj);

static gc_Void gc_INIFileSectionItem_add_option(gc_INIFileSectionItem* self, gc_INIFileOptionItem* option);

static gc_Bool gc_INIFileSectionItem_remove_option(gc_INIFileSectionItem* self, gc_ConstCharPtr option_name);

static gc_Bool gc_INIFileSectionItem_set_option(gc_INIFileSectionItem* self,
	gc_ConstCharPtr option_name,
	gc_ConstCharPtr option_value
);

static gc_Int gc_INIFileSectionItem_option_index(gc_INIFileSectionItem* self, gc_ConstCharPtr option_name);


gc_INIFile* gc_INIFile_init(
	gc_Path* path,
	gc_ConstCharPtr lsep,
	gc_ConstCharPtr nameval_sep
) {
	gc_INIFile* self = (gc_INIFile*)gc_malloc(gc_sizeof(gc_INIFile));
	if (gc_not self) {
		gc_printf("Could not create gc_INIFile!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->file = gc_File_init(path, "r");
	self->lsep = gc_Bytes_init(lsep);
	self->nameval_sep = gc_Bytes_init(nameval_sep);
	self->sections = gc_ArrayList_init(10, 1.5);
	gc_Int pos = 0;
	gc_Int sep_length = (gc_Int)gc_Bytes_length(self->lsep);
	gc_FileSplitlinesGen* split_gen = gc_FileSplitlinesGen_init(self->file, gc_Bytes_byte_str(self->lsep));
	while (gc_FileSplitlinesGen_has_next(split_gen)) {
		gc_Bytes* line = gc_FileSplitlinesGen_next(split_gen);
		gc_Bytes* stripped = gc_Bytes_concrete_strip(line, gc_LetterWhitespace);
		gc_SizeT line_length = gc_Bytes_length(line);
		line = gc_Bytes_del(line);
		if (is_section(stripped)) {
			gc_SizeT stripped_len = gc_Bytes_length(stripped);
			gc_Bytes* section_name = gc_Bytes_slice(stripped, 1, stripped_len - 1, 1);
			gc_INIFileSection* section = gc_INIFileSection_init(section_name, pos);
			if (gc_ArrayList_is_dense(self->sections)) {
				gc_ArrayList_expand(self->sections);
			}
			gc_ArrayList_append(self->sections, section);
		}
		stripped = gc_Bytes_del(stripped);
		pos += (gc_Int)line_length + sep_length;
	}
	split_gen = gc_FileSplitlinesGen_del(split_gen);
	gc_File_seek(self->file, 0, SEEK_SET);
	return self;
}

gc_VoidPtr gc_INIFile_del(gc_INIFile* obj) {
	obj->file = gc_File_del(obj->file);
	obj->lsep = gc_Bytes_del(obj->lsep);
	obj->nameval_sep = gc_Bytes_del(obj->nameval_sep);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(obj->sections); i++) {
		gc_INIFileSection* section = (gc_INIFileSection*)gc_ArrayList_get(obj->sections, i);
		section = gc_INIFileSection_del(section);
	}
	obj->sections = gc_ArrayList_del(obj->sections);
	gc_free(obj);
	return NONE;
}

gc_ArrayList* gc_INIFile_sections(gc_INIFile* self) {
	return self->sections;
}

gc_ArrayList* gc_INIFile_options(gc_INIFile* self, gc_ConstCharPtr section_name) {
	gc_ArrayList* options = gc_ArrayList_init(10, 1.5);
	gc_Int ind = gc_INIFile_get_section_index(self, section_name);
	if (ind == GC_INI_FILE_SECTION_NOT_FOUND) {
		return options;
	}
	gc_INIFileSection* section = (gc_INIFileSection*)gc_ArrayList_get(self->sections, ind);
	if (gc_not gc_File_seek(self->file, section->position, SEEK_SET)) {
		return options;
	}
	gc_Bool started = False;
	gc_ConstCharPtr lsep_str = gc_Bytes_byte_str(self->lsep);
	gc_ConstCharPtr name_valsep = gc_Bytes_byte_str(self->nameval_sep);
	gc_SizeT min_op_len = gc_Bytes_length(self->nameval_sep) + 2;
	gc_FileSplitlinesGen* split_gen = gc_FileSplitlinesGen_init(self->file, lsep_str);
	while (gc_FileSplitlinesGen_has_next(split_gen)) {
		gc_Bytes* line = gc_FileSplitlinesGen_next(split_gen);
		gc_Bytes* stripped = gc_Bytes_concrete_strip(line, gc_LetterWhitespace);
		line = gc_Bytes_del(line);
		if (is_section(stripped)) {
			stripped = gc_Bytes_del(stripped);
			if (started) {
				break;
			}
			started = True;
			continue;
		}
		gc_INIFileOption* option = get_option(stripped, section->name, name_valsep, min_op_len);
		stripped = gc_Bytes_del(stripped);
		if (gc_not option) {
			continue;
		}
		if (gc_ArrayList_is_dense(options)) {
			gc_ArrayList_expand(options);
		}
		gc_ArrayList_append(options, option);
	}
	split_gen = gc_FileSplitlinesGen_del(split_gen);
	gc_File_seek(self->file, 0, SEEK_SET);
	return options;
}

gc_Bool gc_INIFile_has_section(gc_INIFile* self, gc_ConstCharPtr section_name) {
	gc_Int ind = gc_INIFile_get_section_index(self, section_name);
	return ind != GC_INI_FILE_SECTION_NOT_FOUND;
}

gc_Bool gc_INIFile_has_option(gc_INIFile* self,
	gc_ArrayList* options,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name
) {
	gc_Int ind = gc_INIFile_get_option_index(self, options, section_name, option_name);
	return ind != GC_INI_FILE_OPTION_NOT_FOUND;
}

gc_Int gc_INIFile_get_section_index(gc_INIFile* self, gc_ConstCharPtr section_name) {
	gc_Int res = GC_INI_FILE_SECTION_NOT_FOUND;
	for(gc_SizeT i = 0; i < gc_ArrayList_length(self->sections); i++) {
		gc_INIFileSection* section = (gc_INIFileSection*)gc_ArrayList_get(self->sections, i);
		if (gc_Bytes_is_chars_equal(section->name, section_name)) {
			res = (gc_Int)i;
			break;
		}
	}
	return res;
}

gc_Int gc_INIFile_get_section_pos(
	gc_INIFile* self,
	gc_Int offset,
	gc_ConstCharPtr section_name
) {
	if (gc_not gc_File_is_open(self->file)) {
		return GC_INI_FILE_SECTION_NOT_FOUND;
	}
	gc_Bytes* name = get_complete_section_name(section_name);
	gc_SizeT name_length = gc_Bytes_length(name);
	if (name_length < 3) {
		name = gc_Bytes_del(name);
		return GC_INI_FILE_SECTION_NOT_FOUND;
	}
	if (gc_not gc_File_seek(self->file, 0, SEEK_SET)) {
		name = gc_Bytes_del(name);
		return GC_INI_FILE_SECTION_NOT_FOUND;
	}
	gc_Int pos = offset;
	gc_Bool found = False;
	gc_ConstCharPtr lsep_str = gc_Bytes_byte_str(self->lsep);
	gc_FileSplitlinesGen* split_gen = gc_FileSplitlinesGen_init(self->file, lsep_str);
	while (gc_FileSplitlinesGen_has_next(split_gen)) {
		gc_Bytes* line = gc_FileSplitlinesGen_next(split_gen);
		gc_SizeT line_length = gc_Bytes_length(line);
		if (line_length >= name_length) {
			gc_Int ind = gc_Bytes_find(line, gc_Bytes_byte_str(name), 0, line_length);
			if (ind != INDEX_NOT_FOUND) {
				line = gc_Bytes_del(line);
				pos += ind;
				found = True;
				break;
			}
		}
		line = gc_Bytes_del(line);
		pos += (gc_Int)line_length + (gc_Int)gc_Bytes_length(self->lsep);
	}
	name = gc_Bytes_del(name);
	split_gen = gc_FileSplitlinesGen_del(split_gen);
	gc_File_seek(self->file, 0, SEEK_SET);
	if (gc_not found) {
		pos = GC_INI_FILE_SECTION_NOT_FOUND;
	}
	return pos;
}

gc_Int gc_INIFile_get_option_index(gc_INIFile* self,
	gc_ArrayList* options,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name
) {
	gc_Int ind = GC_INI_FILE_OPTION_NOT_FOUND;
	for (gc_SizeT i = 0; i < gc_ArrayList_length(options); i++) {
		gc_INIFileOption* option = (gc_INIFileOption*)gc_ArrayList_get(options, i);
		if (gc_not gc_Bytes_is_chars_equal(option->section_name, section_name)) {
			continue;
		}
		if (gc_Bytes_is_chars_equal(option->name, option_name)) {
			ind = (gc_Int)i;
			break;
		}
	}
	return ind;
}


gc_INIFileCreator* gc_INIFileCreator_from_inifile(gc_INIFile* file) {
	gc_INIFileCreator* self = (gc_INIFileCreator*)gc_malloc(gc_sizeof(gc_INIFileCreator));
	if (gc_not self) {
		gc_printf("Could not create gc_INIFileCreator!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->lsep = gc_Bytes_copy(file->lsep);
	self->nameval_sep = gc_Bytes_copy(file->nameval_sep);
	self->sections = gc_ArrayList_init(10, 1.5);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(file->sections); i++) {
		gc_INIFileSection* section = (gc_INIFileSection*)gc_ArrayList_get(file->sections, i);
		gc_ConstCharPtr section_name = gc_Bytes_byte_str(section->name);

		gc_INIFileCreator_add_section(self, section_name);
		gc_Int last_section_ind = (gc_Int)gc_ArrayList_length(self->sections) - 1;
		if (last_section_ind < 0) {
			continue;
		}
		gc_INIFileSectionItem* last_section = (gc_INIFileSectionItem*)gc_ArrayList_get(
			self->sections,
			last_section_ind
		);

		gc_ArrayList* options = gc_INIFile_options(file, section_name);
		for (gc_SizeT j = 0; j < gc_ArrayList_length(options); j++) {
			gc_INIFileOption* option = gc_ArrayList_get(options, j);
			gc_INIFileOptionItem* option_item = gc_INIFileOptionItem_init(
				gc_Bytes_copy(option->name),
				gc_Bytes_copy(option->value)
			);
			option = gc_INIFileOption_del(option);
			gc_INIFileSectionItem_add_option(last_section, option_item);
		}
		options = gc_ArrayList_del(options);
	}
	return self;
}

gc_INIFileCreator* gc_INIFileCreator_init(gc_ConstCharPtr lsep, gc_ConstCharPtr nameval_sep) {
	gc_INIFileCreator* self = (gc_INIFileCreator*)gc_malloc(gc_sizeof(gc_INIFileCreator));
	if (gc_not self) {
		gc_printf("Could not create gc_INIFileCreator!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->lsep = gc_Bytes_init(lsep);
	self->nameval_sep = gc_Bytes_init(nameval_sep);
	self->sections = gc_ArrayList_init(10, 1.5);
	return self;
}

gc_VoidPtr gc_INIFileCreator_del(gc_INIFileCreator* obj) {
	obj->lsep = gc_Bytes_del(obj->lsep);
	obj->nameval_sep = gc_Bytes_del(obj->nameval_sep);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(obj->sections); i++) {
		gc_INIFileSectionItem* item = (gc_INIFileSectionItem*)gc_ArrayList_get(obj->sections, i);
		item = gc_INIFileSectionItem_del(item);
	}
	obj->sections = gc_ArrayList_del(obj->sections);
	gc_free(obj);
	return NONE;
}

gc_ArrayList* gc_INIFileCreator_sections(gc_INIFileCreator* self) {
	gc_ArrayList* sections = gc_ArrayList_init(10, 1.5);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(self->sections); i++) {
		gc_INIFileSectionItem* section = gc_ArrayList_get(self->sections, i);
		if (gc_ArrayList_is_dense(sections)) {
			gc_ArrayList_expand(sections);
		}
		gc_ArrayList_append(sections, section->section_name);
	}
	return sections;
}

gc_ArrayList* gc_INIFileCreator_options(gc_INIFileCreator* self, gc_ConstCharPtr section_name) {
	gc_ArrayList* options = gc_ArrayList_init(10, 1.5);
	gc_Int index = gc_INIFileCreator_section_index(self, section_name);
	if (index == GC_INI_FILE_SECTION_NOT_FOUND) {
		return options;
	}
	gc_INIFileSectionItem* section = gc_ArrayList_get(self->sections, index);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(section->section_options); i++) {
		gc_INIFileOptionItem* item = (gc_INIFileOptionItem*)gc_ArrayList_get(section->section_options, i);
		if (gc_ArrayList_is_dense(options)) {
			gc_ArrayList_expand(options);
		}
		gc_ArrayList_append(options, item);
	}
	return options;
}

gc_Void gc_INIFileCreator_add_section(gc_INIFileCreator* self, gc_ConstCharPtr section_name) {
	if (gc_ArrayList_is_dense(self->sections)) {
		gc_ArrayList_expand(self->sections);
	}
	gc_INIFileSectionItem* section = gc_INIFileSectionItem_init(gc_Bytes_init(section_name));
	gc_ArrayList_append(self->sections, section);
}

gc_Bool gc_INIFileCreator_save(gc_INIFileCreator* self, gc_Path* path) {
	if (gc_Path_exists(path)) {
		return False;
	}
	gc_File* file = gc_File_init(path, "w");
	if (gc_not gc_File_is_open(file)) {
		file = gc_File_del(file);
		return False;
	}
	gc_ConstCharPtr lsep = gc_Bytes_byte_str(self->lsep);
	gc_ConstCharPtr name_valsep = gc_Bytes_byte_str(self->nameval_sep);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(self->sections); i++) {
		gc_INIFileSectionItem* section = (gc_INIFileSectionItem*)gc_ArrayList_get(self->sections, i);
		gc_Bytes* section_name = get_complete_section_name(gc_Bytes_byte_str(section->section_name));
		gc_ConstCharPtr sect_name = gc_Bytes_byte_str(section_name);

		gc_File_write(file, sect_name);
		section_name = gc_Bytes_del(section_name);
		gc_File_write(file, lsep);
		for (gc_SizeT j = 0; j < gc_ArrayList_length(section->section_options); j++) {
			gc_INIFileOptionItem* option = (gc_INIFileOptionItem*)gc_ArrayList_get(section->section_options, j);
			gc_ConstCharPtr opt_name = gc_Bytes_byte_str(option->option_name);
			gc_ConstCharPtr opt_value = gc_Bytes_byte_str(option->option_value);

			gc_File_write(file, opt_name);
			gc_File_write(file, name_valsep);
			gc_Bytes* stripped_val = gc_Bytes_concrete_strip(option->option_value, gc_LetterWhitespace);
			if (gc_Bytes_length(stripped_val) > 0) {
				gc_File_write(file, opt_value);
			} else {
				gc_File_write(file, "\"");
				gc_File_write(file, opt_value);
				gc_File_write(file, "\"");
			}
			stripped_val = gc_Bytes_del(stripped_val);
			gc_File_write(file, lsep);
		}
	}
	file = gc_File_del(file);
	return True;
}

gc_Bool gc_INIFileCreator_remove_section(gc_INIFileCreator* self, gc_ConstCharPtr section_name) {
	gc_Int index = gc_INIFileCreator_section_index(self, section_name);
	if (index == GC_INI_FILE_SECTION_NOT_FOUND) {
		return False;
	}
	gc_INIFileSectionItem* section = gc_ArrayList_pop(self->sections, index);
	section = gc_INIFileSectionItem_del(section);
	if (gc_ArrayList_is_sparse(self->sections)) {
		gc_ArrayList_shrink(self->sections);
	}
	return True;
}

gc_Bool gc_INIFileCreator_remove_option(gc_INIFileCreator* self,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name
) {
	gc_Int index = gc_INIFileCreator_section_index(self, section_name);
	if (index == GC_INI_FILE_SECTION_NOT_FOUND) {
		return False;
	}
	gc_INIFileSectionItem* section = gc_ArrayList_get(self->sections, index);
	return gc_INIFileSectionItem_remove_option(section, option_name);
}

gc_Bool gc_INIFileCreator_add_option(gc_INIFileCreator* self,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name,
	gc_ConstCharPtr option_value
) {
	gc_Int index = gc_INIFileCreator_section_index(self, section_name);
	if (index == GC_INI_FILE_SECTION_NOT_FOUND) {
		return False;
	}
	gc_INIFileSectionItem* section = gc_ArrayList_get(self->sections, index);
	gc_INIFileOptionItem* item = gc_INIFileOptionItem_init(
		gc_Bytes_init(option_name),
		gc_Bytes_init(option_value)
	);
	gc_INIFileSectionItem_add_option(section, item);
	return True;
}

gc_Bool gc_INIFileCreator_set_option(gc_INIFileCreator* self,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name,
	gc_ConstCharPtr option_value
) {
	gc_Int index = gc_INIFileCreator_section_index(self, section_name);
	if (index == GC_INI_FILE_SECTION_NOT_FOUND) {
		return False;
	}
	gc_INIFileSectionItem* section = (gc_INIFileSectionItem*)gc_ArrayList_get(self->sections, index);
	return gc_INIFileSectionItem_set_option(section, option_name, option_value);
}

gc_Int gc_INIFileCreator_section_index(gc_INIFileCreator* self, gc_ConstCharPtr section_name) {
	gc_Int res = GC_INI_FILE_SECTION_NOT_FOUND;
	for (gc_SizeT i = 0; i < gc_ArrayList_length(self->sections); i++) {
		gc_INIFileSectionItem* item = (gc_INIFileSectionItem*)gc_ArrayList_get(self->sections, i);
		if (gc_Bytes_is_chars_equal(item->section_name, section_name)) {
			res = (gc_Int)i;
			break;
		}
	}
	return res;
}

gc_Int gc_INIFileCreator_option_index(gc_INIFileCreator* self,
	gc_ArrayList* options,
	gc_ConstCharPtr option_name
) {
	gc_Int res = GC_INI_FILE_OPTION_NOT_FOUND;
	for (gc_SizeT i = 0; i < gc_ArrayList_length(options); i++) {
		gc_INIFileOptionItem* item = (gc_INIFileOptionItem*)gc_ArrayList_get(options, i);
		if (gc_Bytes_is_chars_equal(item->option_name, option_name)) {
			res = (gc_Int)i;
			break;
		}
	}
	return res;
}


gc_INIFileSection* gc_INIFileSection_init(gc_Bytes* name, gc_Int position) {
	gc_INIFileSection* self = (gc_INIFileSection*)gc_malloc(gc_sizeof(gc_INIFileSection));
	if (gc_not self) {
		gc_printf("Could not create gc_INIFileSection!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->name = name;
	self->position = position;
	return self;
}

gc_VoidPtr gc_INIFileSection_del(gc_INIFileSection* obj) {
	obj->name = gc_Bytes_del(obj->name);
	gc_free(obj);
	return NONE;
}

gc_Int gc_INIFileSection_position(gc_INIFileSection* self) {
	return self->position;
}

gc_Bytes* gc_INIFileSection_name(gc_INIFileSection* self) {
	return self->name;
}


gc_INIFileOption* gc_INIFileOption_init(gc_Bytes* name, gc_Bytes* value, gc_Bytes* section_name) {
	gc_INIFileOption* self = (gc_INIFileOption*)gc_malloc(gc_sizeof(gc_INIFileOption));
	if (gc_not self) {
		gc_printf("Could not create gc_INIFileOption!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->name = name;
	self->value = value;
	self->section_name = gc_Bytes_copy(section_name);
	return self;
}

gc_VoidPtr gc_INIFileOption_del(gc_INIFileOption* obj) {
	obj->name = gc_Bytes_del(obj->name);
	obj->value = gc_Bytes_del(obj->value);
	obj->section_name = gc_Bytes_del(obj->section_name);
	gc_free(obj);
	return NONE;
}

gc_Bytes* gc_INIFileOption_name(gc_INIFileOption* self) {
	return self->name;
}

gc_Bytes* gc_INIFileOption_value(gc_INIFileOption* self) {
	return self->value;
}

gc_Bytes* gc_INIFileOption_section_name(gc_INIFileOption* self) {
	return self->section_name;
}


gc_INIFileOptionItem* gc_INIFileOptionItem_init(gc_Bytes* name, gc_Bytes* value) {
	gc_INIFileOptionItem* self = (gc_INIFileOptionItem*)gc_malloc(gc_sizeof(gc_INIFileOptionItem));
	if (gc_not self) {
		gc_printf("Could not create gc_INIFileOptionItem!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->option_name = name;
	self->option_value = value;
	return self;
}

gc_VoidPtr gc_INIFileOptionItem_del(gc_INIFileOptionItem* obj) {
	obj->option_name = gc_Bytes_del(obj->option_name);
	obj->option_value = gc_Bytes_del(obj->option_value);
	gc_free(obj);
	return NONE;
}

gc_Bytes* gc_INIFileOptionItem_name(gc_INIFileOptionItem* self) {
	return self->option_name;
}

gc_Bytes* gc_INIFileOptionItem_value(gc_INIFileOptionItem* self) {
	return self->option_value;
}


gc_INIFileSectionItem* gc_INIFileSectionItem_init(gc_Bytes* name) {
	gc_INIFileSectionItem* self = (gc_INIFileSectionItem*)gc_malloc(gc_sizeof(gc_INIFileSectionItem));
	if (gc_not self) {
		gc_printf("Could not create gc_INIFileSectionItem!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->section_name = name;
	self->section_options = gc_ArrayList_init(10, 1.5);
	return self;
}

gc_VoidPtr gc_INIFileSectionItem_del(gc_INIFileSectionItem* obj) {
	obj->section_name = gc_Bytes_del(obj->section_name);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(obj->section_options); i++) {
		gc_INIFileOptionItem* item = (gc_INIFileOptionItem*)gc_ArrayList_get(obj->section_options, i);
		item = gc_INIFileOptionItem_del(item);
	}
	obj->section_options = gc_ArrayList_del(obj->section_options);
	gc_free(obj);
	return NONE;
}

gc_Void gc_INIFileSectionItem_add_option(gc_INIFileSectionItem* self, gc_INIFileOptionItem* option) {
	if (gc_ArrayList_is_dense(self->section_options)) {
		gc_ArrayList_expand(self->section_options);
	}
	gc_ArrayList_append(self->section_options, option);
}

gc_Bool gc_INIFileSectionItem_remove_option(gc_INIFileSectionItem* self, gc_ConstCharPtr option_name) {
	gc_Int index = gc_INIFileSectionItem_option_index(self, option_name);
	if (index == GC_INI_FILE_OPTION_NOT_FOUND) {
		return False;
	}
	gc_INIFileOptionItem* item = (gc_INIFileOptionItem*)gc_ArrayList_pop(self->section_options, index);
	item = gc_INIFileOptionItem_del(item);
	if (gc_ArrayList_is_sparse(self->section_options)) {
		gc_ArrayList_shrink(self->section_options);
	}
	return True;
}

gc_Bool gc_INIFileSectionItem_set_option(gc_INIFileSectionItem* self,
	gc_ConstCharPtr option_name,
	gc_ConstCharPtr option_value
) {
	gc_Int index = gc_INIFileSectionItem_option_index(self, option_name);
	if (index == GC_INI_FILE_OPTION_NOT_FOUND) {
		return False;
	}
	gc_INIFileOptionItem* item = (gc_INIFileOptionItem*)gc_ArrayList_get(self->section_options, index);
	item->option_value = gc_Bytes_del(item->option_value);
	item->option_value = gc_Bytes_init(option_value);
	return True;
}

gc_Int gc_INIFileSectionItem_option_index(gc_INIFileSectionItem* self, gc_ConstCharPtr option_name) {
	gc_Int res = GC_INI_FILE_OPTION_NOT_FOUND;
	for (gc_SizeT i = 0; i < gc_ArrayList_length(self->section_options); i++) {
		gc_INIFileOptionItem* item = (gc_INIFileOptionItem*)gc_ArrayList_get(self->section_options, i);
		if (gc_Bytes_is_chars_equal(item->option_name, option_name)) {
			res = (gc_Int)i;
			break;
		}
	}
	return res;
}


gc_Bool is_section(gc_Bytes* stripped_line) {
	gc_SizeT line_length = gc_Bytes_length(stripped_line);
	if (line_length < 3) {
		return False;
	}
	if (gc_Bytes_get(stripped_line, 0) != '[') {
		return False;
	}
	return gc_Bytes_get(stripped_line, line_length - 1) == ']';
}

gc_Bytes* get_complete_section_name(gc_ConstCharPtr section_name) {
	if (section_name[0] == '\0') {
		return gc_Bytes_init("\0");
	}
	gc_BytesArray* name_array = gc_BytesArray_init(
		MIN_GC_BYTES_ARRAY_SIZE_SAFE,
		MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE
	);
	if (section_name[0] != '[') {
		gc_BytesArray_append(name_array, '[');
	}
	gc_BytesArray_push_byte_str(name_array, section_name);
	if (gc_BytesArray_is_dense(name_array)) {
		gc_BytesArray_expand(name_array);
	}
	gc_Int last_ind = (gc_Int)gc_BytesArray_length(name_array) - 1;
	if (last_ind >= 0) {
		if (gc_BytesArray_get(name_array, last_ind) != ']') {
			gc_BytesArray_append(name_array, ']');
		}
	} else {
		gc_BytesArray_append(name_array, ']');
	}
	gc_BytesArray_append(name_array, '\0');
	gc_Bytes* res = gc_BytesArray_cast(name_array);
	return res;
}

gc_INIFileOption* get_option(
	gc_Bytes* stripped_line,
	gc_Bytes* section_name,
	gc_ConstCharPtr name_valsep,
	gc_SizeT min_opt_len
) {
	gc_INIFileOption* res = NONE;
	gc_SizeT stripped_length = gc_Bytes_length(stripped_line);
	if (stripped_length < min_opt_len) {
		return res;
	}
	gc_SizeT numsplit = gc_Bytes_count(stripped_line, name_valsep, 0, stripped_length);
	numsplit++;
	gc_ArrayList* splitline = gc_Bytes_split(stripped_line, name_valsep, numsplit);
	gc_SizeT num_parts = gc_ArrayList_length(splitline);
	if (num_parts != 2) {
		for (gc_SizeT i = 0; i < num_parts; i++) {
			gc_Bytes* part = (gc_Bytes*)gc_ArrayList_get(splitline, i);
			part = gc_Bytes_del(part);
		}
		splitline = gc_ArrayList_del(splitline);
		return res;
	}
	gc_Bytes* name = (gc_Bytes*)gc_ArrayList_get(splitline, 0);
	gc_Bytes* value = (gc_Bytes*)gc_ArrayList_get(splitline, 1);
	name = gc_Bytes_concrete_strip(name, gc_LetterWhitespace);
	value = gc_Bytes_concrete_strip(value, gc_LetterWhitespace);
	for (gc_SizeT i = 0; i < num_parts; i++) {
		gc_Bytes* part = (gc_Bytes*)gc_ArrayList_get(splitline, i);
		part = gc_Bytes_del(part);
	}
	splitline = gc_ArrayList_del(splitline);
	gc_SizeT val_length = gc_Bytes_length(value);
	if (val_length >= 2) {
		if (gc_Bytes_get(value, 0) == gc_Bytes_get(value, val_length - 1)) {
			gc_Bytes* no_par = gc_Bytes_remove_external_parentheses(value);
			value = gc_Bytes_del(value);
			value = no_par;
		}
	}
	return gc_INIFileOption_init(name, value, section_name);
}
