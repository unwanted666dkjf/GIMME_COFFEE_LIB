#ifndef GIMME_COFFEE_FILE_H
#define GIMME_COFFEE_FILE_H


#include "./path.h"


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_File;

struct GIMME_COFFEE_API gc_FileLineGen;

struct GIMME_COFFEE_API gc_FileChunkGen;

struct GIMME_COFFEE_API gc_FileSplitlinesGen;


/**
 * A wrapper for FILE* object.
 * Allows casting into FILE**.
 */
typedef struct gc_File gc_File;

/**
 * Generator.
 * Does not resets the file position indicator.
 * Generates string lines from file.
 */
typedef struct gc_FileLineGen gc_FileLineGen;

/**
 * Generator.
 * Does not resets the file position indicator.
 * Generates string chunks from file.
 */
typedef struct gc_FileChunkGen gc_FileChunkGen;

/**
 * Generator.
 * Does not resets the file position indicator.
 * Generates string lines from file, using custom separator.
 */
typedef struct gc_FileSplitlinesGen gc_FileSplitlinesGen;


/**
 * Creates gc_File* with specified mode.
 * Modes:
 * 'r'       open for reading
 * 'w'       open for writing, truncating the file first
 * 'a'       open for writing, appending to the end of the file if it exists
 * 'b'       binary mode
 * 't'       text mode (default)
 * '+'       open a disk file for updating (reading and writing)
 */
GIMME_COFFEE_API gc_File* gc_File_init(gc_Path* path, gc_ConstCharPtr mode);

/**
 * Closes file, deletes object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_File_del(gc_File* obj);

/**
 * Closes file.
 */
GIMME_COFFEE_API gc_Void gc_File_close(gc_File* self);

/**
 * Resets file position indicator to the begining of a
 *file stream and clears any error indicators.
 */
GIMME_COFFEE_API gc_Void gc_File_rewind(gc_File* self);

/**
 * Returns True if files is open, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_File_is_open(gc_File* self);

/**
 * Writes a text to a file.
 */
GIMME_COFFEE_API gc_Bool gc_File_write(gc_File* self, gc_ConstCharPtr text);

/**
 * 'join's bytes from list using 'lsep' and writes line to a file.
 */
GIMME_COFFEE_API gc_Bool gc_File_writelines(gc_File* self,
	gc_ConstCharPtr lsep,
	gc_ArrayList* bytes_lst
);

/**
 * Sets the file position of the stream to the given offset.
 * 'whence' is one of the following values:
 * SEEK_SET -- beginning of the file;
 * SEEK_CUR -- current position of the file pointer;
 * SEEK_END -- end of the file.
 */
GIMME_COFFEE_API gc_Bool gc_File_seek(gc_File* self, gc_Int offset, gc_Int whence);

/**
 * Reads the file to the end and returns it's contents as a string.
 */
GIMME_COFFEE_API gc_Bytes* gc_File_read(gc_File* self);

/**
 * Reads the file to the end and returns it's contents as a list of string(gc_Bytes*).
 */
GIMME_COFFEE_API gc_ArrayList* gc_File_readlines(gc_File* self, gc_ConstCharPtr lsep);


/**
 * Creates gc_FileLineGen*.
 * Does not copy gc_File, but uses pointer to it.
 */
GIMME_COFFEE_API gc_FileLineGen* gc_FileLineGen_init(gc_File* file);

/**
 * Deletes object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_FileLineGen_del(gc_FileLineGen* obj);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_FileLineGen_has_next(gc_FileLineGen* self);

/**
 * Creates and returns next element.
 */
GIMME_COFFEE_API gc_Bytes* gc_FileLineGen_next(gc_FileLineGen* self);


/**
 * Creates gc_FileChunkGen*.
 * Does not copy gc_File, but uses pointer to it.
 * 'chunk_size' is maximum chunk size.
 */
GIMME_COFFEE_API gc_FileChunkGen* gc_FileChunkGen_init(gc_File* file, gc_SizeT chunk_size);

/**
 * Deletes object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_FileChunkGen_del(gc_FileChunkGen* obj);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_FileChunkGen_has_next(gc_FileChunkGen* self);

/**
 * Creates and returns next element.
 */
GIMME_COFFEE_API gc_Bytes* gc_FileChunkGen_next(gc_FileChunkGen* self);


/**
 * Creates gc_FileSplitlinesGen*.
 * Does not copy gc_File, but uses pointer to it.
 * 'lsep' is line separator.
 */
GIMME_COFFEE_API gc_FileSplitlinesGen* gc_FileSplitlinesGen_init(gc_File* file, gc_ConstCharPtr lsep);

/**
 * Deletes object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_FileSplitlinesGen_del(gc_FileSplitlinesGen* obj);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_FileSplitlinesGen_has_next(gc_FileSplitlinesGen* self);

/**
 * Creates and returns next element.
 */
GIMME_COFFEE_API gc_Bytes* gc_FileSplitlinesGen_next(gc_FileSplitlinesGen* self);


#ifdef __cplusplus
}
#endif


#endif
