#include <stdio.h>

#include <stdlib.h>

#include "../headers/os.h"

#include "../headers/bytes_utils.h"

#include "../headers/array_list.h"

#include "../headers/linked_list.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


#ifdef _WIN32


#define WIN32_LEAN_AND_MEAN


#include <string.h>

#include <windows.h>

#else


#include <dirent.h>

#include <unistd.h>

#include <fcntl.h>

#include <sys/types.h>

#include <sys/stat.h>


#endif


struct gc_PathWalkItem {
	gc_Path* root;
	gc_ArrayList* dirs;
	gc_ArrayList* files;
};

static gc_PathWalkItem* gc_PathWalkItem_init(gc_Path* root) {
	gc_PathWalkItem* self = (gc_PathWalkItem*)gc_malloc(gc_sizeof(gc_PathWalkItem));
	if (gc_not self) {
		gc_printf("Could not create gc_PathWalkItem!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->root = gc_Path_copy(root);
	self->dirs = gc_ArrayList_init(10, 1.5);
	self->files = gc_ArrayList_init(15, 1.5);
	return self;
}

gc_VoidPtr gc_PathWalkItem_del(gc_PathWalkItem* obj) {
	obj->root = gc_Path_del(obj->root);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(obj->dirs); i++) {
		gc_Path* path = (gc_Path*)gc_ArrayList_get(obj->dirs, i);
		path = gc_Path_del(path);
	}
	obj->dirs = gc_ArrayList_del(obj->dirs);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(obj->files); i++) {
		gc_Path* path = (gc_Path*)gc_ArrayList_get(obj->files, i);
		path = gc_Path_del(path);
	}
	obj->files = gc_ArrayList_del(obj->files);
	gc_free(obj);
	return NONE;
}

gc_Path* gc_PathWalkItem_root(gc_PathWalkItem* self) {
	return self->root;
}

gc_ArrayList* gc_PathWalkItem_dirs(gc_PathWalkItem* self) {
	return self->dirs;
}

gc_ArrayList* gc_PathWalkItem_files(gc_PathWalkItem* self) {
	return self->files;
}

static gc_Void gc_PathWalkItem_append_dir(gc_PathWalkItem* self, gc_Path* dir) {
	if (gc_ArrayList_is_dense(self->dirs)) {
		gc_ArrayList_expand(self->dirs);
	}
	gc_ArrayList_append(self->dirs, dir);
}

static gc_Void gc_PathWalkItem_append_file(gc_PathWalkItem* self, gc_Path* file) {
	if (gc_ArrayList_is_dense(self->files)) {
		gc_ArrayList_expand(self->files);
	}
	gc_ArrayList_append(self->files, file);
}


struct gc_PathWalkGen {
	gc_LinkedList* dirs_left;
	gc_SizeT depth_limit;
	gc_SizeT depth_cntr;
};

gc_PathWalkGen* gc_PathWalkGen_init(gc_ConstCharPtr start_dir, gc_SizeT depth_limit) {
	gc_PathWalkGen* self = (gc_PathWalkGen*)gc_malloc(gc_sizeof(gc_PathWalkGen));
	if (gc_not self) {
		gc_printf("Could not create gc_PathWalkGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->depth_limit = depth_limit;
	self->depth_cntr = 0;
	self->dirs_left = gc_LinkedList_init();
	gc_LinkedList_append(self->dirs_left, gc_Path_init(start_dir));
	return self;
}

gc_VoidPtr gc_PathWalkGen_del(gc_PathWalkGen* obj) {
	gc_Int last_ind = (gc_Int)gc_LinkedList_length(obj->dirs_left) - 1;
	if (last_ind >= 0) {
		for(; last_ind >= 0; last_ind--) {
			gc_Path* p = (gc_Path*)gc_LinkedList_pop(obj->dirs_left, last_ind);
			p = gc_Path_del(p);
		}
	}
	obj->dirs_left = gc_LinkedList_del(obj->dirs_left);
	gc_free(obj);
	return NONE;
}

gc_Bool gc_PathWalkGen_has_next(gc_PathWalkGen* self) {
	if (self->depth_cntr >= self->depth_limit) {
		return False;
	}
	return gc_LinkedList_length(self->dirs_left) >= 1;
}

gc_PathWalkItem* gc_PathWalkGen_next(gc_PathWalkGen* self) {
	gc_SizeT index = gc_LinkedList_length(self->dirs_left) - 1;
	gc_Path* start_dir = (gc_Path*)gc_LinkedList_pop(self->dirs_left, index);
	gc_PathWalkItem* item = gc_PathWalkItem_init(start_dir);
	gc_PathListDirGen* list_dir = gc_PathListDirGen_init(gc_Path_byte_str(start_dir));
	self->depth_cntr++;
	while (gc_PathListDirGen_has_next(list_dir)) {
		gc_Path* path = gc_PathListDirGen_next(list_dir);
		gc_Bytes* path_bytes = gc_Path_bytes(path);
		if (
			gc_Bytes_is_chars_equal(path_bytes, curdir)
			gc_or gc_Bytes_is_chars_equal(path_bytes, pardir)
		) {
			path = gc_Path_del(path);
			continue;
		}
		gc_Path* full_path = gc_Path_add_path(start_dir, path);
		if (gc_Path_is_dir(full_path)) {
			gc_LinkedList_append(self->dirs_left, full_path);
			gc_PathWalkItem_append_dir(item, path);
		} else {
			gc_PathWalkItem_append_file(item, path);
			full_path = gc_Path_del(full_path);
		}
	}
	start_dir = gc_Path_del(start_dir);
	list_dir = gc_PathListDirGen_del(list_dir);
	return item;
}


#ifdef _WIN32


gc_ConstCharPtr sep = "\\";

gc_ConstCharPtr pathsep = ";";

gc_ConstCharPtr defsep = ";";

gc_ConstCharPtr name = "nt";

gc_ConstCharPtr curdir = ".";

gc_ConstCharPtr pardir = "..";

gc_ConstCharPtr linesep = "\n";

gc_ConstCharPtr extsep = ".";


struct gc_PathListDirGen {
	gc_Path* start_dir;
	gc_Bool is_open;
	gc_Bool exhausted;
	HANDLE h;
	WIN32_FIND_DATAA fd;
};

gc_PathListDirGen* gc_PathListDirGen_init(gc_ConstCharPtr start_dir) {
	gc_PathListDirGen* self = (gc_PathListDirGen*)gc_malloc(gc_sizeof(gc_PathListDirGen));
	if (gc_not self) {
		gc_printf("Could not create gc_PathListDirGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->start_dir = gc_Path_init(start_dir);
	gc_Bytes* path_bytes = gc_Path_bytes(self->start_dir);
	gc_Char search[MAX_PATH + 3];							// Hate Windows, btw
	for (gc_SizeT i = 0; i < MAX_PATH; i++) {
		if (i < gc_Bytes_length(path_bytes)) {
			gc_Char c = gc_Bytes_get(path_bytes, i);
			search[i] = c;
		} else {
			if (search[i - 1] != '\\') {
				search[i] = '\\';
				search[i + 1] = '*';
				search[i + 2] = '\0';
			} else {
				search[i] = '*';
				search[i + 1] = '\0';
			}
			break;
		}
	}
	self->h = FindFirstFileA(search, &(self->fd));
	if (self->h == INVALID_HANDLE_VALUE) {
		self->is_open = False;
	} else {
		self->is_open = True;
	}
	if (self->is_open) {
		self->exhausted = False;
	} else {
		self->exhausted = True;
	}
	return self;
}

gc_VoidPtr gc_PathListDirGen_del(gc_PathListDirGen* obj) {
	if (obj->is_open) {
		FindClose(obj->h);
	}
	obj->start_dir = gc_Path_del(obj->start_dir);
	gc_free(obj);
	return NONE;
}

gc_Path* gc_PathListDirGen_start_dir(gc_PathListDirGen* self) {
	return self->start_dir;
}

gc_Path* gc_PathListDirGen_next(gc_PathListDirGen* self) {
	gc_ConstCharPtr name = self->fd.cFileName;
	gc_Path* path = gc_Path_init(name);
	self->exhausted = gc_not FindNextFileA(self->h, &(self->fd));
	return path;
}

gc_Bool gc_PathListDirGen_is_open(gc_PathListDirGen* self) {
	return self->is_open;
}

gc_Bool gc_PathListDirGen_has_next(gc_PathListDirGen* self) {
	if (gc_not self->is_open) {
		return False;
	}
	return gc_not self->exhausted;
}


#else


gc_ConstCharPtr sep = "/";

gc_ConstCharPtr pathsep = ":";

gc_ConstCharPtr defsep = ":";

gc_ConstCharPtr name = "posix";

gc_ConstCharPtr curdir = ".";

gc_ConstCharPtr pardir = "..";

gc_ConstCharPtr linesep = "\n";

gc_ConstCharPtr extsep = ".";


struct gc_PathListDirGen {
	gc_Path* start_dir;
	gc_Bool is_open;
	DIR* dir;
	struct dirent* e;
};

gc_PathListDirGen* gc_PathListDirGen_init(gc_ConstCharPtr start_dir) {
	gc_PathListDirGen* self = (gc_PathListDirGen*)gc_malloc(gc_sizeof(gc_PathListDirGen));
	if (gc_not self) {
		gc_printf("Could not create gc_PathListDirGen!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->start_dir = gc_Path_init(start_dir);
	self->dir = opendir(gc_Path_byte_str(self->start_dir));
	if (gc_not self->dir) {
		self->is_open = False;
		self->e = NULL;
	} else {
		self->is_open = True;
		self->e = NONE;
	}
	return self;
}

gc_VoidPtr gc_PathListDirGen_del(gc_PathListDirGen* obj) {
	if (obj->is_open) {
		closedir(obj->dir);
	}
	obj->start_dir = gc_Path_del(obj->start_dir);
	gc_free(obj);
	return NONE;
}

gc_Path* gc_PathListDirGen_start_dir(gc_PathListDirGen* self) {
	return self->start_dir;
}

gc_Path* gc_PathListDirGen_next(gc_PathListDirGen* self) {
	gc_ConstCharPtr name = self->e->d_name;
	return gc_Path_init(name);
}

gc_Bool gc_PathListDirGen_is_open(gc_PathListDirGen* self) {
	return self->is_open;
}

gc_Bool gc_PathListDirGen_has_next(gc_PathListDirGen* self) {
	if (gc_not self->is_open) {
		return False;
	}
	self->e = readdir(self->dir);
	return self->e != NULL;
}

#endif


gc_Path* gc_get_cwd() {
	gc_Path* cd = gc_Path_init(curdir);
	gc_Path* res = gc_Path_realpath(cd);
	cd = gc_Path_del(cd);
	return res;
}


gc_Path* gc_get_home_dir(gc_ConstCharPtr default_) {
	#ifndef _WIN32
		gc_ConstCharPtr homeval = getenv("HOME");
	#else
		gc_ConstCharPtr homeval = getenv("USERPROFILE");
	#endif
	if (homeval) {
		return gc_Path_init(homeval);
	}
	return gc_Path_init(default_);
}


gc_Path* gc_get_windows_startup_dir(gc_ConstCharPtr default_, gc_Bool current_user) {
	gc_Path* default_path = gc_Path_init(default_);
	#ifndef _WIN32
		return default_path;
	#endif
	gc_Path* res;
	if (current_user) {
		gc_ConstCharPtr app_data = getenv("APPDATA");
		if (gc_not app_data) {
			return default_path;
		}
		gc_Path* appd_path = gc_Path_init(app_data);
		res = gc_Path_add(appd_path, "Microsoft/Windows/Start Menu/Programs/Startup");
		appd_path = gc_Path_del(appd_path);
	} else {
		gc_ConstCharPtr prog_data = getenv("PROGRAMDATA");
		if (gc_not prog_data) {
			return default_path;
		}
		gc_Path* progd_path = gc_Path_init(prog_data);
		res = gc_Path_add(progd_path, "Microsoft/Windows/Start Menu/Programs/Startup");
		progd_path = gc_Path_del(progd_path);
	}
	if (gc_not gc_Path_is_dir(res)) {
		res = gc_Path_del(res);
		res = default_path;
	} else {
		default_path = gc_Path_del(default_path);
	}
	return res;
}


gc_Bytes* gc_getenv(gc_ConstCharPtr varname,  gc_ConstCharPtr default_) {
	gc_ConstCharPtr varval = getenv(varname);
	if (varval) {
		return gc_Bytes_init(varval);
	}
	return gc_Bytes_init(default_);
}


gc_Void gc_setenv(gc_ConstCharPtr varname, gc_ConstCharPtr varval, gc_Bool overwrite) {
	#ifndef _WIN32
		setenv(varname, varval, overwrite);
	#else
		DWORD len = GetEnvironmentVariableA(varname, NULL, 0);
		if (len == 0) {
			SetEnvironmentVariableA(varname, varval);
		} else {
			if (overwrite) {
				SetEnvironmentVariableA(varname, varval);
			}
		}
	#endif
}


gc_Bool gc_os_mkdir(gc_Path* path) {
	gc_ConstCharPtr byte_str = gc_Path_byte_str(path);
	if (gc_Path_is_dir(path)) {
		return False;
	}
	#ifndef _WIN32
		if (mkdir(byte_str, 0777) != 0) {
			return False;
		}
	#else
		if (gc_not CreateDirectory(byte_str, NULL)) {
			return False;
		}
	#endif
	return True;
}

gc_Bool gc_os_rmdir(gc_Path* path) {
	gc_ConstCharPtr byte_str = gc_Path_byte_str(path);
	#ifndef _WIN32
		if (rmdir(byte_str) == 0) {
			return True;
		}
		return False;
	#else
		RemoveDirectoryA(byte_str);
		return gc_not gc_Path_exists(path);	// Because there is some issues with return value
	#endif
}

gc_Bool gc_os_remove(gc_Path* path) {
	gc_ConstCharPtr byte_str = gc_Path_byte_str(path);
	#ifndef _WIN32
		struct stat st;
		if (lstat(byte_str, &st) != 0) {
			return False;
		}
		if (S_ISLNK(st.st_mode)) {
			if (unlink(byte_str) == 0) {
				return True;
			}
		} else {
			if (remove(byte_str) == 0) {
				return True;
			}
		}
		return False;
	#else
		DeleteFileA(byte_str);
		return gc_not gc_Path_exists(path);	// Because there is some issues with return value
	#endif
}

gc_Bool gc_os_rem_dir(gc_Path* start_dir) {
	if (gc_not gc_Path_is_dir(start_dir)) {
		return False;
	}
	gc_ConstCharPtr start_dir_str = gc_Path_byte_str(start_dir);
	if (start_dir_str[0] == '\0') {
		return False;
	}
	gc_PathListDirGen* list_dir = gc_PathListDirGen_init(start_dir_str);
	while (gc_PathListDirGen_has_next(list_dir)) {
		gc_Path* path = gc_PathListDirGen_next(list_dir);
		gc_Path* full_path = gc_Path_add_path(start_dir, path);
		path = gc_Path_del(path);
		if (gc_Path_is_dir(full_path)) {
			gc_os_rmdir(full_path);
		} else {
			gc_os_remove(full_path);
		}
		full_path = gc_Path_del(full_path);
	}
	list_dir = gc_PathListDirGen_del(list_dir);
	return gc_os_rmdir(start_dir);
}

gc_Bool gc_os_rmtree(gc_Path* start_dir, gc_SizeT depth_limit) {
	if (gc_not gc_Path_is_dir(start_dir)) {
		return False;
	}
	gc_Bytes* start_dir_bytes = gc_Path_bytes(start_dir);
	gc_ConstCharPtr start_dir_str = gc_Bytes_byte_str(start_dir_bytes);
	if (start_dir_str[0] == '\0') {
		return False;
	}
	gc_PathWalkGen* walk_gen = gc_PathWalkGen_init(start_dir_str, depth_limit);
	gc_ArrayList* dirs_left = gc_ArrayList_init(15, 1.5);
	while (gc_PathWalkGen_has_next(walk_gen)) {
		gc_PathWalkItem* item = gc_PathWalkGen_next(walk_gen);
		gc_Path* root = gc_PathWalkItem_root(item);

		gc_ArrayList* files = gc_PathWalkItem_files(item);
		for (gc_SizeT i = 0; i < gc_ArrayList_length(files); i++) {
			gc_Path* file = (gc_Path*)gc_ArrayList_get(files, i);
			gc_Path* full_path = gc_Path_add_path(root, file);
			gc_os_remove(full_path);
			full_path = gc_Path_del(full_path);
		}

		gc_ArrayList* dirs = gc_PathWalkItem_dirs(item);
		for (gc_SizeT i = 0; i < gc_ArrayList_length(dirs); i++) {
			gc_Path* dir = (gc_Path*)gc_ArrayList_get(dirs, i);
			gc_Path* full_path = gc_Path_add_path(root, dir);
			if (gc_not gc_os_rmdir(full_path)) {
				if (gc_ArrayList_is_dense(dirs_left)) {
					gc_ArrayList_expand(dirs_left);
				}
				gc_ArrayList_append(dirs_left, full_path);
			} else {
				full_path = gc_Path_del(full_path);
			}
		}

		item = gc_PathWalkItem_del(item);
	}
	walk_gen = gc_PathWalkGen_del(walk_gen);
	dirs_left = gc_ArrayList_sort_paths(dirs_left);
	gc_ArrayList_reverse(dirs_left);
	for (gc_SizeT i = 0; i < gc_ArrayList_length(dirs_left); i++) {
		gc_Path* path = (gc_Path*)gc_ArrayList_get(dirs_left, i);
		gc_os_rem_dir(path);
		path = gc_Path_del(path);
	}
	dirs_left = gc_ArrayList_del(dirs_left);
	return gc_os_rmdir(start_dir);
}

gc_Bool gc_os_makedirs(gc_Path* start_dir) {
	if (gc_Path_length(start_dir) < 1) {
		return False;
	}
	if (gc_Path_is_dir(start_dir)) {
		return False;
	}
	if (gc_os_mkdir(start_dir)) {
		return True;
	}
	gc_Bytes* start_bytes = gc_Path_bytes(start_dir);
	gc_Path* current = NONE;
	gc_BytesSplitGen* split_gen = gc_BytesSplitGen_init(start_bytes, sep, INF);
	while (gc_BytesSplitGen_has_next(split_gen)) {
		gc_Bytes* part = gc_BytesSplitGen_next(split_gen);
		if (
			gc_Bytes_is_chars_equal(part, curdir)
			gc_or gc_Bytes_is_chars_equal(part, pardir)
		) {
			if (gc_not current) {
				current = gc_Path_from_gc_bytes(part);
			}
			part = gc_Bytes_del(part);
			continue;
		}
		if (gc_not current) {
			current = gc_Path_from_gc_bytes(part);
			if (gc_not gc_Path_is_dir(current)) {
				if (gc_not gc_os_mkdir(current)) {
					break;
				}
			}
			part = gc_Bytes_del(part);
		} else {
			gc_Path* full_path = gc_Path_add(current, gc_Bytes_byte_str(part));
			part = gc_Bytes_del(part);
			if (gc_not gc_Path_is_dir(full_path)) {
				if (gc_not gc_os_mkdir(full_path)) {
					full_path = gc_Path_del(full_path);
					break;
				} else {
					current = gc_Path_del(current);
					current = full_path;
				}
			} else {
				current = gc_Path_del(current);
				current = full_path;
			}
		}
	}
	split_gen = gc_BytesSplitGen_del(split_gen);
	if (current) {
		current = gc_Path_del(current);
	}
	return gc_Path_is_dir(start_dir);
}

gc_Bool gc_os_chmod(gc_Path* path, gc_Int mode) {
	gc_ConstCharPtr byte_str = gc_Path_byte_str(path);
	#ifndef _WIN32
		if (chmod(byte_str, (mode_t)mode) == -1) {
			perror("chmod");
			return False;
		}
	#else
		DWORD attrs = GetFileAttributesA(byte_str);
		if (attrs == INVALID_FILE_ATTRIBUTES) {
			return False;
		}
		gc_Bool readonly = (mode & 0200) == 0;
		if (readonly) {
			attrs |= FILE_ATTRIBUTE_READONLY;
		} else {
			attrs &= ~FILE_ATTRIBUTE_READONLY;
		}
		return SetFileAttributesA(byte_str, attrs);
	#endif
	return True;
}

gc_Bool gc_os_rename(gc_Path* src, gc_Path* dst) {
	if (gc_Path_exists(dst)) {
		return False;
	}
	if (gc_Path_is_equal(src, dst)) {
		return False;
	}
	if (rename(gc_Path_byte_str(src), gc_Path_byte_str(dst)) == 0) {
		return True;
	}
	return False;
}

gc_Bool gc_os_symlink(gc_Path* src, gc_Path* link_path) {
	#ifdef _WIN32
		return False;
	#else
		if (symlink(gc_Path_byte_str(src), gc_Path_byte_str(link_path)) == -1) {
			perror("symlink");
			return False;
		}
		return True;
	#endif
}

gc_Bool gc_os_chown(gc_Path* path, gc_Int uid, gc_Int gid) {
	#ifdef _WIN32
		return False;
	#else
		gc_ConstCharPtr byte_str = gc_Path_byte_str(path);
		if (chown(byte_str, (uid_t)uid, (gid_t)gid) == -1) {
			perror("chown");
			return False;
		}
		return True;
	#endif
}

gc_Bool gc_os_copydir(gc_Path* src, gc_Path* dst, gc_SizeT file_chunk_size) {
	if (gc_Path_length(src) < 1 gc_or gc_Path_length(dst) < 1) {
		return False;
	}
	if (gc_Path_is_dir(dst)) {
		return False;
	}
	if (gc_not gc_Path_is_dir(src)) {
		return False;
	}
	if (gc_not gc_os_makedirs(dst)) {
		return False;
	}
	gc_PathListDirGen* list_dir = gc_PathListDirGen_init(gc_Path_byte_str(src));
	while (gc_PathListDirGen_has_next(list_dir)) {
		gc_Path* path = gc_PathListDirGen_next(list_dir);
		gc_Path* dst_full_path = gc_Path_add_path(dst, path);
		gc_Path* src_full_path = gc_Path_add_path(src, path);
		path = gc_Path_del(path);
		if (gc_Path_is_dir(src_full_path)) {
			gc_os_makedirs(dst_full_path);
		} else {
			gc_os_copyfile(src_full_path, dst_full_path, file_chunk_size);
		}
		src_full_path = gc_Path_del(src_full_path);
		dst_full_path = gc_Path_del(dst_full_path);
	}
	list_dir = gc_PathListDirGen_del(list_dir);
	return gc_Path_is_dir(dst);
}

gc_Bool gc_os_copyfile(gc_Path* src, gc_Path* dst, gc_SizeT chunk_size) {
	if (gc_Path_length(src) < 1 gc_or gc_Path_length(dst) < 1) {
		return False;
	}
	if (gc_Path_is_file(dst)) {
		return False;
	}
	if (gc_not gc_Path_is_file(src)) {
		return False;
	}
	if (chunk_size < 1) {
		return False;
	}
	if (gc_Path_is_equal(src, dst)) {
		return False;
	}
	FILE* fsrc = NONE;
	FILE* fdst = NONE;
	fsrc = fopen(gc_Path_byte_str(src), "rb");
	if (gc_not fsrc) {
		perror("fopen source");
		goto fail;
	}
	fdst = fopen(gc_Path_byte_str(dst), "wb");
	if (gc_not fdst) {
		perror("fopen dest");
		goto fail;
	}
	gc_UCharPtr buf = (gc_UCharPtr)gc_malloc(gc_sizeof(gc_UChar) * chunk_size);
	if (gc_not buf) {
		goto fail_buf;
	}
	gc_SizeT n;
	gc_SizeT item_size = gc_sizeof(gc_UChar);
	while ((n = fread(buf, item_size, chunk_size, fsrc)) > 0) {
		if (fwrite(buf, item_size, n, fdst) != n) {
			perror("fwrite");
			goto fail;
		}
	}
	gc_free(buf);
	if (ferror(fsrc)) {
		perror("fread");
		goto fail;
	}
	fclose(fsrc);
	fclose(fdst);
	return True;

	fail:
		if (fsrc) {
			fclose(fsrc);
		}
		if (fdst) {
			fclose(fdst);
		}
		return False;
	fail_buf:
		gc_printf("Could not create buffer in gc_os_copyfile!\n");
		fclose(fsrc);
		fclose(fdst);
		gc_exit(EXIT_NULLPTR);
		return False;
}

gc_Bool gc_os_copytree(
	gc_Path* src,
	gc_Path* dst,
	gc_SizeT depth_limit,
	gc_SizeT file_chunk_size
) {
	if (gc_Path_length(src) < 1 gc_or gc_Path_length(dst) < 1) {
		return False;
	}
	if (gc_Path_is_dir(dst)) {
		return False;
	}
	if (gc_not gc_Path_is_dir(src)) {
		return False;
	}
	if (gc_not gc_os_makedirs(dst)) {
		return False;
	}
	if (file_chunk_size < 1) {
		return False;
	}
	gc_SizeT src_length = gc_Path_length(src);
	gc_PathWalkGen* walk_gen = gc_PathWalkGen_init(gc_Path_byte_str(src), depth_limit);
	while (gc_PathWalkGen_has_next(walk_gen)) {
		gc_PathWalkItem* item = gc_PathWalkGen_next(walk_gen);

		gc_Path* root = gc_PathWalkItem_root(item);
		gc_Bytes* root_bytes = gc_Path_bytes(root);
		gc_Path* dst_root;
		gc_SizeT root_length = gc_Bytes_length(root_bytes);
		gc_Int start_relative = (gc_Int)root_length - (gc_Int)src_length;
		if (start_relative > 0) {
			gc_Bytes* no_src = gc_Bytes_slice(root_bytes, src_length, root_length, 1);
			dst_root = gc_Path_add(dst, gc_Bytes_byte_str(no_src));
			no_src = gc_Bytes_del(no_src);
		} else {
			dst_root = dst;
		}

		gc_ArrayList* dirs = gc_PathWalkItem_dirs(item);
		for (gc_SizeT i = 0; i < gc_ArrayList_length(dirs); i++) {
			gc_Path* path = (gc_Path*)gc_ArrayList_get(dirs, i);
			gc_Path* dst_full_path = gc_Path_add_path(dst_root, path);
			if (gc_not gc_Path_is_dir(dst_full_path)) {
				gc_os_makedirs(dst_full_path);
			}
			dst_full_path = gc_Path_del(dst_full_path);
		}

		gc_ArrayList* files = gc_PathWalkItem_files(item);
		for (gc_SizeT i = 0; i < gc_ArrayList_length(files); i++) {
			gc_Path* path = (gc_Path*)gc_ArrayList_get(files, i);
			gc_Path* dst_full_path = gc_Path_add_path(dst_root, path);
			if (gc_not gc_Path_is_dir(dst_root)) {
				dst_full_path = gc_Path_del(dst_full_path);
				continue;
			}
			gc_Path* src_full_path = gc_Path_add_path(root, path);
			if (gc_not gc_Path_is_file(dst_full_path)) {
				gc_os_copyfile(src_full_path, dst_full_path, file_chunk_size);
			}
			src_full_path = gc_Path_del(src_full_path);
			dst_full_path = gc_Path_del(dst_full_path);
		}

		item = gc_PathWalkItem_del(item);
		if (start_relative > 0) {
			dst_root = gc_Path_del(dst_root);
		}
	}
	walk_gen = gc_PathWalkGen_del(walk_gen);
	return gc_Path_is_dir(dst);
}
