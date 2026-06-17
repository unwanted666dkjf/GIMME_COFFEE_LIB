#ifndef GIMME_COFFEE_PATH_H
#define GIMME_COFFEE_PATH_H


#include "./bytes.h"

#include "./array_list.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns nomralized copy of a given path.
 * Strips whitespaces.
 * Removes external quotes if path is quoted.
 * Replaces path separator with the platform's one.
 * rstrip's separator if the path is not only separators.
 */
GIMME_COFFEE_API gc_CharPtr gc_format_path(gc_ConstCharPtr path, gc_SizeT path_length);

/**
 * Sorts gc_ArrayList of gc_Path pointers using merge sort.
 * Sorting takes place according to the length of the path,
 *from the smalles to largest.
 */
GIMME_COFFEE_API gc_ArrayList* gc_ArrayList_sort_paths(gc_ArrayList* gc_paths_lst);


/**
 * Cross-platform 'stat' structure.
 */
typedef struct {
	gc_UInt mode;
	gc_ULong ino;
	gc_ULong dev;
	gc_ULong nlink;
	gc_UInt uid;
	gc_UInt gid;
	gc_Long size;
	gc_Long atime;
	gc_Long mtime;
	gc_Long ctime;
} GIMME_COFFEE_API gc_Stat;


struct GIMME_COFFEE_API gc_Path;

struct GIMME_COFFEE_API gc_SplittedPath;

struct GIMME_COFFEE_API gc_PathSplitextT;

struct GIMME_COFFEE_API gc_FullSplittedPath;


/**
 * Wrapper of gc_Bytes with formatted string.
 */
typedef struct gc_Path gc_Path;

/**
 * Struct with dirname and basename as gc_Path pointers.
 */
typedef struct gc_SplittedPath gc_SplittedPath;

/**
 * Struct with name(full, including path) and extension as gc_Bytes pointers.
 */
typedef struct gc_PathSplitextT gc_PathSplitextT;

/**
 * Struct with dirname, basename(without extension) and extension.
 * dirname is gc_Path pointer. basename and extension are gc_Bytes pointers.
 */
typedef struct gc_FullSplittedPath gc_FullSplittedPath;


/**
 * Creates and returns gc_Path pointer using given name and extension.
 */
GIMME_COFFEE_API gc_Path* gc_Path_from_splitext(gc_ConstCharPtr name, gc_ConstCharPtr ext);

/**
 * Creates and returns gc_Path pointer using given dirname and basename.
 */
GIMME_COFFEE_API gc_Path* gc_Path_from_split(gc_ConstCharPtr dirname, gc_ConstCharPtr basename);

/**
 * Creates and returns gc_Path pointer using given dirname, basename(without extension) and extension.
 */
GIMME_COFFEE_API gc_Path* gc_Path_from_full_split(
	gc_ConstCharPtr dirname,
	gc_ConstCharPtr basename,
	gc_ConstCharPtr ext
);

/**
 * Creates and returns gc_Path pointer from gc_Bytes pointer.
 */
GIMME_COFFEE_API gc_Path* gc_Path_from_gc_bytes(gc_Bytes* path_bytes);

/**
 * Creates and returns gc_Path pointer from given string.
 * Does not copy string, becomes new owner.
 * String must be formatted for correct behavior.
 */
GIMME_COFFEE_API gc_Path* gc_Path_cast_formatted(gc_CharPtr bytes);

/**
 * Creates and returns gc_Path pointer from bytes array.
 * Deletes array after creation.
 */
GIMME_COFFEE_API gc_Path* gc_BytesArray_path_cast(gc_BytesArray* array);

/**
 * Creates and returns gc_Path pointer from bytes.
 * First, it creates formatted string from given bytes, then owns it.
 * Deletes original string after creation.
 */
GIMME_COFFEE_API gc_Path* gc_Path_cast_bytes(gc_CharPtr bytes, gc_Bool copy);

/**
 * Creates and returns gc_Path pointer from bytes.
 * Does not copy string, neither format it. Just becomes it's owner.
 */
GIMME_COFFEE_API gc_Path* gc_Path_cast_formatted_gc_bytes(gc_Bytes* bytes);

/**
 * Creates and returns gc_Path pointer from given string.
 */
GIMME_COFFEE_API gc_Path* gc_Path_init(gc_ConstCharPtr path);

/**
 * Deletes gc_Path object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Path_del(gc_Path* obj);

/**
 * Deletes gc_Path object and returns NONE.
 * If free_bytes is False, will not delete internal gc_Bytes string.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Path_del_ex(gc_Path* obj, gc_Bool free_bytes);

/**
 * Returns pointer to the internal gc_Bytes string.
 */
GIMME_COFFEE_API gc_Bytes* gc_Path_bytes(gc_Path* self);

/**
 * Returns file extension if path is not a directory,
 *otherwise returns empty string.
 */
GIMME_COFFEE_API gc_Bytes* gc_Path_ext(gc_Path* self);

/**
 * Returns internal 'c-string' as immutable object.
 */
GIMME_COFFEE_API gc_ConstCharPtr gc_Path_byte_str(gc_Path* self);

/**
 * Returns hash.
 */
GIMME_COFFEE_API gc_SizeT gc_Path_hash(gc_Path* self);

/**
 * Returns length of the internal gc_Bytes string.
 */
GIMME_COFFEE_API gc_SizeT gc_Path_length(gc_Path* self);

/**
 * Creates and returns gc_Stat object with information about path.
 */
GIMME_COFFEE_API gc_Stat gc_Path_stat(gc_Path* self);

/**
 * Returns True if path exists, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Path_exists(gc_Path* self);

/**
 * Returns True if path exists and it's a directory, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Path_is_dir(gc_Path* self);

/**
 * Returns True if path exists and it's not a directory, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Path_is_file(gc_Path* self);

/**
 * Returns True if the path is not absolute, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Path_is_relative(gc_Path* self);

/**
 * Returns True if the path is absolute, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_Path_is_absolute(gc_Path* self);

/**
 * Returns True if paths are equal, otherwise False.
 * Paths are equal, if their absolute paths are equal.
 * On Windows ignores string cases.
 */
GIMME_COFFEE_API gc_Bool gc_Path_is_equal(gc_Path* self, gc_Path* other);

/**
 * Returns True if path 'self' is relative to path 'other', otherwise returns False.
 * If 'self' or 'other' does not exists, returns False anyway.
 * If paths are equal, 'self' is not relative to 'other', except '.' or '..'.
 */
GIMME_COFFEE_API gc_Bool gc_Path_is_relative_to(gc_Path* self, gc_Path* other);

/**
 * Creates and returns gc_SplittedPath pointer from path.
 */
GIMME_COFFEE_API gc_SplittedPath* gc_Path_split(gc_Path* self);

/**
 * Creates and returns gc_PathSplitextT pointer from path.
 */
GIMME_COFFEE_API gc_PathSplitextT* gc_Path_splitext(gc_Path* self);

/**
 * Creates and returns gc_FullSplittedPath pointer from path.
 */
GIMME_COFFEE_API gc_FullSplittedPath* gc_Path_full_split(gc_Path* self);

/**
 * Returns copy of the self.
 */
GIMME_COFFEE_API gc_Path* gc_Path_copy(gc_Path* self);

/**
 * Creates and returns absolute path for the given.
 */
GIMME_COFFEE_API gc_Path* gc_Path_abspath(gc_Path* self);

/**
 * Creates and returns absolute path for the given.
 */
GIMME_COFFEE_API gc_Path* gc_Path_realpath(gc_Path* self);

/**
 * Returns the directory component of a pathname.
 */
GIMME_COFFEE_API gc_Path* gc_Path_dirname(gc_Path* self);

/**
 * Returns the final component of a pathname.
 */
GIMME_COFFEE_API gc_Path* gc_Path_basename(gc_Path* self);

/**
 * Concatenate strings from self and other, returns pointer to a new gc_Path.
 */
GIMME_COFFEE_API gc_Path* gc_Path_add(gc_Path* self, gc_ConstCharPtr other);

/**
 * Concatenate strings from self and other, returns pointer to a new gc_Path.
 */
GIMME_COFFEE_API gc_Path* gc_Path_add_path(gc_Path* self, gc_Path* other);

/**
 * Joins gc_Path pointers from list with self, returns pointer to a new gc_Path.
 */
GIMME_COFFEE_API gc_Path* gc_Path_join(gc_Path* self, gc_ArrayList* gc_path_lst);


/**
 * Deletes gc_SplittedPath object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_SplittedPath_del(gc_SplittedPath* obj);

/**
 * Returns directory component.
 */
GIMME_COFFEE_API gc_Path* gc_SplittedPath_dirname(gc_SplittedPath* self);

/**
 * Returns basename component.
 */
GIMME_COFFEE_API gc_Path* gc_SplittedPath_basename(gc_SplittedPath* self);


/**
 * Deletes gc_PathSplitextT object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_PathSplitextT_del(gc_PathSplitextT* obj);

/**
 * Returns file extension or empty string.
 */
GIMME_COFFEE_API gc_Bytes* gc_PathSplitextT_ext(gc_PathSplitextT* self);

/**
 * Returns name(including path).
 */
GIMME_COFFEE_API gc_Bytes* gc_PathSplitextT_name(gc_PathSplitextT* self);


/**
 * Deletes gc_FullSplittedPath object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_FullSplittedPath_del(gc_FullSplittedPath* obj);

/**
 * Returns directory name component.
 */
GIMME_COFFEE_API gc_Path* gc_FullSplittedPath_dirname(gc_FullSplittedPath* self);

/**
 * Returns file extension or empty string.
 */
GIMME_COFFEE_API gc_Bytes* gc_FullSplittedPath_ext(gc_FullSplittedPath* self);

/**
 * Returns basename component(without extension).
 */
GIMME_COFFEE_API gc_Bytes* gc_FullSplittedPath_basename(gc_FullSplittedPath* self);


#ifdef __cplusplus
}
#endif


#endif
