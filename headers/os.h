#ifndef GIMME_COFFEE_OS_H
#define GIMME_COFFEE_OS_H


#include "./path.h"


extern gc_ConstCharPtr sep;

extern gc_ConstCharPtr pathsep;

extern gc_ConstCharPtr defsep;

extern gc_ConstCharPtr name;

extern gc_ConstCharPtr curdir;

extern gc_ConstCharPtr pardir;

extern gc_ConstCharPtr linesep;

extern gc_ConstCharPtr extsep;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Returns absolute path to a current working directory.
 */
GIMME_COFFEE_API gc_Path* gc_get_cwd();

/**
 * Returns absolute path to 'user home' directory if success, otherwise returns 'default_' path.
 */
GIMME_COFFEE_API gc_Path* gc_get_home_dir(gc_ConstCharPtr default_);

/**
 * Returns absolute path to 'windows startup' directory if it exists, otherwise returns 'default_' path.
 * If 'current_user' is True, returns 'windows startup' for current user, otherwise returns global startup.
 */
GIMME_COFFEE_API gc_Path* gc_get_windows_startup_dir(gc_ConstCharPtr default_, gc_Bool current_user);

/**
 * Returns environment variable value with given name if success, otherwise returns 'default_'.
 */
GIMME_COFFEE_API gc_Bytes* gc_getenv(gc_ConstCharPtr varname, gc_ConstCharPtr default_);

/**
 * Sets new value to the variable with name 'varname'.
 * If 'overwrite' is True, overwrites existing value.
 */
GIMME_COFFEE_API gc_Void gc_setenv(gc_ConstCharPtr varname, gc_ConstCharPtr varval, gc_Bool overwrite);


/**
 * Creates(not recursively) a directory.
 * Default directory mode: 0777.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_mkdir(gc_Path* path);

/**
 * Removes(not recursively) empty directory.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_rmdir(gc_Path* path);

/**
 * Removes file.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_remove(gc_Path* path);

/**
 * Not recursively removes directory and it's content.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_rem_dir(gc_Path* start_dir);

/**
 * Creates a leaf directory and all intermediate ones.
 * Default directory mode: 0777.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_makedirs(gc_Path* start_dir);

/**
 * Recursively removes directory and it's content.
 * The depth of recursion is specified by value of 'depth_limit'.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_rmtree(gc_Path* start_dir, gc_SizeT depth_limit);

/**
 * Changes the access permissions of a file.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_chmod(gc_Path* path, gc_Int mode);

/**
 * Renames/moves a file or directory.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_rename(gc_Path* src, gc_Path* dst);

/**
 * Creates a symbolic link pointing to src named link_path.
 * Unix only.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_symlink(gc_Path* src, gc_Path* link_path);

/**
 * Changes the owner and group id of path to the numeric uid and gid.
 * Unix only.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_chown(gc_Path* path, gc_Int uid, gc_Int gid);

/**
 * Copies(not recursively) directory src(with it's content) to dst.
 * Files are copied chunk by chunk, 'file_chunk_size' specifies chunk size.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_copydir(gc_Path* src, gc_Path* dst, gc_SizeT file_chunk_size);

/**
 * Copies src file to dst chunk by chunk.
 * 'file_chunk_size' specifies chunk size.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_copyfile(gc_Path* src, gc_Path* dst, gc_SizeT chunk_size);

/**
 * Recursively copies directory 'src'(with it's content) to 'dst'.
 * Files are copied chunk by chunk, 'file_chunk_size' specifies chunk size.
 * Returns True if success, otherwise returns False.
 */
GIMME_COFFEE_API gc_Bool gc_os_copytree(
	gc_Path* src,
	gc_Path* dst,
	gc_SizeT depth_limit,
	gc_SizeT file_chunk_size
);


struct GIMME_COFFEE_API gc_PathWalkGen;

struct GIMME_COFFEE_API gc_PathWalkItem;

struct GIMME_COFFEE_API gc_PathListDirGen;


/**
 * Generator.
 * Recursively traverses the directory and it's subdirectories.
 * Generates a gc_PathWalkItem with paths: root, files, directories.
 * Root is absolute path, files and dirs are relative.
 * Note: will not generate '.' and '..'.
 */
typedef struct gc_PathWalkGen gc_PathWalkGen;

/**
 * Struct with gc_Path pointers.
 * root -- absolute path.
 * files -- list of relative(to root) paths to files.
 * dirs -- list of relative(to root) to directories.
 * Owns root, files and dirs.
 */
typedef struct gc_PathWalkItem gc_PathWalkItem;

/**
 * Generator.
 * Generates relative(to start dir) paths to files and folders in the directory.
 * Not recursive.
 * Note: will generate '.' and '..'.
 */
typedef struct gc_PathListDirGen gc_PathListDirGen;


/**
 * Creates generator from 'start_dir' to it's sub-dirs.
 * 'depth_limit' specifies traversal depth.
 */
GIMME_COFFEE_API gc_PathWalkGen* gc_PathWalkGen_init(gc_ConstCharPtr start_dir, gc_SizeT depth_limit);

/**
 * Deletes gc_PathWalkGen object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_PathWalkGen_del(gc_PathWalkGen* obj);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_PathWalkGen_has_next(gc_PathWalkGen* self);

/**
 * Generates and returns next element.
 */
GIMME_COFFEE_API gc_PathWalkItem* gc_PathWalkGen_next(gc_PathWalkGen* self);


/**
 * Deletes gc_PathWalkItem and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_PathWalkItem_del(gc_PathWalkItem* obj);

/**
 * Returns absolute path to root.
 */
GIMME_COFFEE_API gc_Path* gc_PathWalkItem_root(gc_PathWalkItem* self);

/**
 * Returns list of relative(to root) paths to directories.
 */
GIMME_COFFEE_API gc_ArrayList* gc_PathWalkItem_dirs(gc_PathWalkItem* self);

/**
 * Returns list of relative(to root) paths to files.
 */
GIMME_COFFEE_API gc_ArrayList* gc_PathWalkItem_files(gc_PathWalkItem* self);


/**
 * Returns pointer to a new gc_PathListDirGen object.
 */
GIMME_COFFEE_API gc_PathListDirGen* gc_PathListDirGen_init(gc_ConstCharPtr start_dir);

/**
 * Deletes gc_PathListDirGen object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_PathListDirGen_del(gc_PathListDirGen* obj);

/**
 * Returns path to a start dir.
 */
GIMME_COFFEE_API gc_Path* gc_PathListDirGen_start_dir(gc_PathListDirGen* self);

/**
 * Generates and returns next element.
 */
GIMME_COFFEE_API gc_Path* gc_PathListDirGen_next(gc_PathListDirGen* self);

/**
 * Returns True if directory is open, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_PathListDirGen_is_open(gc_PathListDirGen* self);

/**
 * Returns True if generator can create next element, otherwise False.
 */
GIMME_COFFEE_API gc_Bool gc_PathListDirGen_has_next(gc_PathListDirGen* self);


#ifdef __cplusplus
}
#endif


#endif
