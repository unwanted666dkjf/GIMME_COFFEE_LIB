#ifndef GIMME_COFFEE_INIFILE_H
#define GIMME_COFFEE_INIFILE_H


#include "./path.h"


extern const gc_Int GC_INI_FILE_SECTION_NOT_FOUND;

extern const gc_Int GC_INI_FILE_OPTION_NOT_FOUND;


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_INIFile;

struct GIMME_COFFEE_API gc_INIFileCreator;

struct GIMME_COFFEE_API gc_INIFileSection;

struct GIMME_COFFEE_API gc_INIFileOption;

struct GIMME_COFFEE_API gc_INIFileOptionItem;


/**
 * Wrapper of gc_File for reading ini-files.
 * Example of ini-file content for reading:
 * '''
 * [section]
 *
 * option1 = value1
 *
 * option2 = value2
 * ...
 * '''
 * Allows casting into gc_File**.
 */
typedef struct gc_INIFile gc_INIFile;

/**
 * Struct for creating ini-files.
 * Allows casting into gc_ArrayList**.
 */
typedef struct gc_INIFileCreator gc_INIFileCreator;

/**
 * Struct with information(name, position) about section.
 * Allows casting into gc_Bytes**. Which is name.
 */
typedef struct gc_INIFileSection gc_INIFileSection;

/**
 * Struct with information(section name, option name, value) about option.
 * Allows casting into gc_Bytes**. Which is option name.
 */
typedef struct gc_INIFileOption gc_INIFileOption;

/**
 * Struct with only option name and option value.
 * Allows casting into gc_Bytes**. Which is name.
 */
typedef struct gc_INIFileOptionItem gc_INIFileOptionItem;


/**
 * Returns pointer to a gc_INIFile.
 * Opens file for reading.
 * Uses 'lsep' as lines separator.
 * Uses 'nameval_sep' as separator between values and options.
 */
GIMME_COFFEE_API gc_INIFile* gc_INIFile_init(
	gc_Path* path,
	gc_ConstCharPtr lsep,
	gc_ConstCharPtr nameval_sep
);

/**
 * Deletes gc_INIFile object, closes the file and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_INIFile_del(gc_INIFile* obj);

/**
 * Returns list of gc_INIFileSection pointers.
 * List is owned by self completely.
 */
GIMME_COFFEE_API gc_ArrayList* gc_INIFile_sections(gc_INIFile* self);

/**
 * Creates gc_INIFileOption pointers and list for them.
 * Returns list with pointers. You have to do cleanup yourself.
 */
GIMME_COFFEE_API gc_ArrayList* gc_INIFile_options(gc_INIFile* self, gc_ConstCharPtr section_name);

/**
 * Returns True if section exists, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_INIFile_has_section(gc_INIFile* self, gc_ConstCharPtr section_name);

/**
 * Returns True if options list has option with given section name and option name, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_INIFile_has_option(gc_INIFile* self,
	gc_ArrayList* options,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name
);

/**
 * Returns index of section with given name if it exists in internal sections list.
 * Otherwise, returns 'GC_INI_FILE_SECTION_NOT_FOUND'.
 */
GIMME_COFFEE_API gc_Int gc_INIFile_get_section_index(gc_INIFile* self, gc_ConstCharPtr section_name);

/**
 * Searches section with given name in the file.
 * Returns it's position if found, otherwise returns 'GC_INI_FILE_SECTION_NOT_FOUND'.
 */
GIMME_COFFEE_API gc_Int gc_INIFile_get_section_pos(gc_INIFile* self,
	gc_Int offset,
	gc_ConstCharPtr section_name
);

/**
 * Returns option index if option with given section name and option name exist in options list.
 * Otherwise, returns 'GC_INI_FILE_OPTION_NOT_FOUND'.
 */
GIMME_COFFEE_API gc_Int gc_INIFile_get_option_index(gc_INIFile* self,
	gc_ArrayList* options,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name
);


/**
 * Creates new gc_INIFileCreator pointer from opened gc_INIFile.
 * Internal list of sections and their options will be filled using gc_INIFile.
 */
GIMME_COFFEE_API gc_INIFileCreator* gc_INIFileCreator_from_inifile(gc_INIFile* file);

/**
 * Creates new gc_INIFileCreator pointer.
 * 'lsep' will be used as lines separator.
 * 'nameval_sep' will be used as separator between 'option' and 'value'.
 */
GIMME_COFFEE_API gc_INIFileCreator* gc_INIFileCreator_init(gc_ConstCharPtr lsep, gc_ConstCharPtr nameval_sep);

/**
 * Deletes gc_INIFileCreator object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_INIFileCreator_del(gc_INIFileCreator* obj);

/**
 * Creates new list of sections names and returns it.
 * List does not owns it's elements.
 */
GIMME_COFFEE_API gc_ArrayList* gc_INIFileCreator_sections(gc_INIFileCreator* self);

/**
 * Creates and returns list of gc_INIFileOptionItem pointers for given section name.
 * List does not owns it's elements.
 */
GIMME_COFFEE_API gc_ArrayList* gc_INIFileCreator_options(gc_INIFileCreator* self, gc_ConstCharPtr section_name);

/**
 * Appends section name to an internal sections list.
 */
GIMME_COFFEE_API gc_Void gc_INIFileCreator_add_section(gc_INIFileCreator* self, gc_ConstCharPtr section_name);

/**
 * Creates new ini-file if path does not exists.
 * Returns True if successful, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_INIFileCreator_save(gc_INIFileCreator* self, gc_Path* path);

/**
 * Removes and deletes section(with it's options) with given name.
 * Returns True if successful, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_INIFileCreator_remove_section(gc_INIFileCreator* self, gc_ConstCharPtr section_name);

/**
 * Removes and deletes option with name 'option_name' from section with name 'section_name'.
 * Returns True if successful, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_INIFileCreator_remove_option(gc_INIFileCreator* self,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name
);

/**
 * Creates option with name 'option_name' and value 'option_value' for section with name 'section_name'.
 * Returns True if successful, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_INIFileCreator_add_option(gc_INIFileCreator* self,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name,
	gc_ConstCharPtr option_value
);

/**
 * Sets new value to the option with name 'option_name' from section with name 'section_name'.
 * Returns True if successful, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_INIFileCreator_set_option(gc_INIFileCreator* self,
	gc_ConstCharPtr section_name,
	gc_ConstCharPtr option_name,
	gc_ConstCharPtr option_value
);

/**
 * Returns index of section from internal sections array.
 * If section with given name does not exists, returns 'GC_INI_FILE_SECTION_NOT_FOUND'.
 */
GIMME_COFFEE_API gc_Int gc_INIFileCreator_section_index(gc_INIFileCreator* self, gc_ConstCharPtr section_name);

/**
 * Return index of option from options list.
 * If option with given name does not exists, returns 'GC_INI_FILE_OPTION_NOT_FOUND'.
 */
GIMME_COFFEE_API gc_Int gc_INIFileCreator_option_index(gc_INIFileCreator* self,
	gc_ArrayList* options,
	gc_ConstCharPtr option_name
);


/**
 * Returns the position of the section in the file.
 */
GIMME_COFFEE_API gc_Int gc_INIFileSection_position(gc_INIFileSection* self);

/**
 * Returns section name.
 */
GIMME_COFFEE_API gc_Bytes* gc_INIFileSection_name(gc_INIFileSection* self);


/**
 * Deletes gc_INIFileOption object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_INIFileOption_del(gc_INIFileOption* obj);

/**
 * Returns option name.
 */
GIMME_COFFEE_API gc_Bytes* gc_INIFileOption_name(gc_INIFileOption* self);

/**
 * Returns option value.
 */
GIMME_COFFEE_API gc_Bytes* gc_INIFileOption_value(gc_INIFileOption* self);

/**
 * Returns section name.
 */
GIMME_COFFEE_API gc_Bytes* gc_INIFileOption_section_name(gc_INIFileOption* self);


/**
 * Returns option name.
 */
GIMME_COFFEE_API gc_Bytes* gc_INIFileOptionItem_name(gc_INIFileOptionItem* self);

/**
 * Returns option value.
 */
GIMME_COFFEE_API gc_Bytes* gc_INIFileOptionItem_value(gc_INIFileOptionItem* self);


#ifdef __cplusplus
}
#endif


#endif
