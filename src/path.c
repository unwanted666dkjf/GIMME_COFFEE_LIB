#include <stdint.h>

#include "../headers/os.h"

#include "../headers/path.h"

#include "../headers/bytes_array.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


#ifndef _WIN32


#include <time.h>

#include <stdlib.h>

#include <unistd.h>

#include <limits.h>

#include <sys/stat.h>

#include <sys/types.h>


#else


#include <stdio.h>

#include <string.h>

#include <errno.h>

#include <tchar.h>

#include <windows.h>


static long filetime_to_time_t(const FILETIME *ft) {
	const unsigned long long EPOCH_DIFF = 11644473600ULL;
	unsigned long long v = ((unsigned long long)ft->dwHighDateTime << 32) | ft->dwLowDateTime;
	return (long)(v / 10000000ULL - EPOCH_DIFF);
}


#endif


struct gc_Path {
	gc_Bytes* bytes;
	gc_SizeT hash;
	gc_Bool ignore_case;
};

struct gc_SplittedPath {
	gc_Path* dirname;
	gc_Path* basename;
};

struct gc_PathSplitextT {
	gc_Bytes* name;
	gc_Bytes* ext;
};

struct gc_FullSplittedPath {
	gc_Path* dirname;
	gc_Bytes* ext;
	gc_Bytes* basename;
};


static gc_SplittedPath* gc_SplittedPath_init(gc_Path* full_path);


static gc_PathSplitextT* gc_PathSplitextT_init(gc_Path* full_path);


static gc_FullSplittedPath* gc_FullSplittedPath_init(gc_Path* full_path);


static gc_Path* gc_Path_get_uninit() {
	gc_Path* self = (gc_Path*)gc_malloc(gc_sizeof(gc_Path));
	if (gc_not self) {
		gc_printf("Could not create gc_Path!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->hash = gc_sizeof(gc_Path);
	#ifndef _WIN32
		self->ignore_case = False;
	#else
		self->ignore_case = True;
	#endif
	return self;
}

gc_Path* gc_Path_from_splitext(gc_ConstCharPtr name, gc_ConstCharPtr ext) {
	gc_BytesArray* array = gc_BytesArray_init(
		MIN_GC_BYTES_ARRAY_SIZE_SAFE,
		MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE
	);
	if (name[0] != '\0') {
		gc_BytesArray_push_byte_str(array, name);
	}
	if (ext[0] != '\0') {
		if (gc_BytesArray_last(array) != extsep[0] gc_and ext[0] != extsep[0]) {
			gc_BytesArray_push_byte_str(array, extsep);
		}
		gc_BytesArray_push_byte_str(array, ext);
	}
	gc_Path* path = gc_BytesArray_path_cast(array);
	return path;
}

gc_Path* gc_Path_from_split(gc_ConstCharPtr dirname, gc_ConstCharPtr basename) {
	gc_BytesArray* array = gc_BytesArray_init(
		MIN_GC_BYTES_ARRAY_SIZE_SAFE,
		MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE
	);
	if (dirname[0] != '\0') {
		gc_BytesArray_push_byte_str(array, dirname);
	}
	if (basename[0] != '\0') {
		gc_Char last = gc_BytesArray_last(array);
		if (
			last != '\\'
			gc_and last != '/'
			gc_and basename[0] != '\\'
			gc_and basename[0] != '/'
			gc_and dirname[0] != '\0'
		) {
			gc_BytesArray_push_byte_str(array, sep);
		}
		gc_BytesArray_push_byte_str(array, basename);
	}
	gc_Path* path = gc_BytesArray_path_cast(array);
	return path;
}

gc_Path* gc_Path_from_full_split(
	gc_ConstCharPtr dirname,
	gc_ConstCharPtr basename,
	gc_ConstCharPtr ext
) {
	gc_BytesArray* array = gc_BytesArray_init(
		MIN_GC_BYTES_ARRAY_SIZE_SAFE,
		MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE
	);
	if (dirname[0] != '\0') {
		gc_BytesArray_push_byte_str(array, dirname);
	}
	if (basename[0] != '\0') {
		gc_Char last = gc_BytesArray_last(array);
		if (
			last != '\\'
			gc_and last != '/'
			gc_and basename[0] != '\\'
			gc_and basename[0] != '/'
			gc_and dirname[0] != '\0'
		) {
			gc_BytesArray_push_byte_str(array, sep);
		}
		gc_BytesArray_push_byte_str(array, basename);
	}
	if (ext[0] != '\0') {
		if (gc_BytesArray_last(array) != extsep[0] gc_and ext[0] != extsep[0]) {
			gc_BytesArray_push_byte_str(array, extsep);
		}
		gc_BytesArray_push_byte_str(array, ext);
	}
	gc_Path* path = gc_BytesArray_path_cast(array);
	return path;
}

gc_Path* gc_Path_from_gc_bytes(gc_Bytes* path_bytes) {
	gc_CharPtr formatted = gc_format_path(
		gc_Bytes_byte_str(path_bytes),
		gc_Bytes_length(path_bytes)
	);
	gc_Path* self = gc_Path_cast_formatted(formatted);
	return self;
}

gc_Path* gc_Path_cast_formatted(gc_CharPtr bytes) {
	gc_Path* self = gc_Path_get_uninit();
	self->bytes = gc_Bytes_cast_bytes(bytes, False);
	self->hash += gc_Bytes_hash(self->bytes);
	return self;
}

gc_Path* gc_BytesArray_path_cast(gc_BytesArray* array) {
	gc_SizeT arr_length = gc_BytesArray_length(array);
	gc_Int last_ind = (gc_Int)arr_length - 1;
	if (last_ind < 0) {
		gc_BytesArray_append(array, '\0');
	}
	if (gc_BytesArray_get(array, last_ind) != '\0') {
		if (arr_length + 1 >= gc_BytesArray_size(array)) {
			gc_BytesArray_expand(array);
		}
		gc_BytesArray_append(array, '\0');
	}
	gc_CharPtr formatted = gc_format_path(
		gc_BytesArray_char_ptr(array),
		gc_BytesArray_length(array) - 1
	);
	gc_Path* self = gc_Path_cast_formatted(formatted);
	array = gc_BytesArray_del(array);					// to keep old behaviour
	return self;
}

gc_Path* gc_Path_cast_bytes(gc_CharPtr bytes, gc_Bool copy) {
	if (copy) {
		return gc_Path_init(bytes);
	}
	gc_SizeT bytes_length = get_byte_str_length(bytes);
	gc_CharPtr formatted = gc_format_path(bytes, bytes_length);
	gc_Path* self = gc_Path_cast_formatted(formatted);
	gc_free(bytes);												// to keep old behaviour
	return self;
}

gc_Path* gc_Path_cast_formatted_gc_bytes(gc_Bytes* bytes) {
	gc_Path* self = gc_Path_get_uninit();
	self->bytes = bytes;
	self->hash += gc_Bytes_hash(self->bytes);
	return self;
}

gc_Path* gc_Path_init(gc_ConstCharPtr path) {
	gc_SizeT bytes_length = get_byte_str_length(path);
	gc_CharPtr formatted = gc_format_path(path, bytes_length);
	gc_Path* self = gc_Path_cast_formatted(formatted);
	return self;
}

gc_VoidPtr gc_Path_del(gc_Path* obj) {
	return gc_Path_del_ex(obj, True);
}

gc_VoidPtr gc_Path_del_ex(gc_Path* obj, gc_Bool free_bytes) {
	if (free_bytes) {
		obj->bytes = gc_Bytes_del(obj->bytes);
	}
	gc_free(obj);
	return NONE;
}

gc_Bytes* gc_Path_bytes(gc_Path* self) {
	return self->bytes;
}

gc_Bytes* gc_Path_ext(gc_Path* self) {
	gc_SizeT bytes_length = gc_Bytes_length(self->bytes);
	if (bytes_length < 1) {
		return gc_Bytes_init("\0");
	}
	if (gc_Path_is_dir(self)) {
		return gc_Bytes_init("\0");
	}
	gc_Int start_ind = gc_Bytes_rfind(self->bytes, extsep, 0, bytes_length);
	if (start_ind == INDEX_NOT_FOUND) {
		return gc_Bytes_init("\0");
	}
	return gc_Bytes_slice(self->bytes, start_ind + 1, bytes_length, 1);
}

gc_ConstCharPtr gc_Path_byte_str(gc_Path* self) {
	return gc_Bytes_byte_str(self->bytes);
}

gc_SizeT gc_Path_hash(gc_Path* self) {
	return self->hash;
}

gc_SizeT gc_Path_length(gc_Path* self) {
	return gc_Bytes_length(self->bytes);
}

gc_Stat gc_Path_stat(gc_Path* self) {
	gc_Stat gc_stat;
	gc_stat.mode = 0;
	gc_stat.ino = 0;
	gc_stat.dev = 0;
	gc_stat.nlink = 0;
	gc_stat.uid = 0;
	gc_stat.gid = 0;
	gc_stat.size = 0;
	gc_stat.atime = 0;
	gc_stat.mtime = 0;
	gc_stat.ctime = 0;
	if (gc_Bytes_length(self->bytes) < 1) {
		return gc_stat;
	}
	if (gc_not gc_Path_exists(self)) {
		return gc_stat;
	}
	gc_ConstCharPtr byte_str = gc_Bytes_byte_str(self->bytes);
	#ifndef _WIN32
		struct stat st;
		if (lstat(byte_str, &st) != 0) {
			return gc_stat;
		}
		gc_stat.mode = (gc_UInt)st.st_mode;
		gc_stat.ino = (gc_ULong)st.st_ino;
		gc_stat.dev = (gc_ULong)st.st_dev;
		gc_stat.nlink = (gc_ULong)st.st_nlink;
		gc_stat.uid = (gc_UInt)st.st_uid;
		gc_stat.gid = (gc_UInt)st.st_gid;
		gc_stat.size = (gc_Long)st.st_size;
		#ifdef st_atime
			gc_stat.atime = (gc_Long)st.st_atim.tv_sec;
			gc_stat.mtime = (gc_Long)st.st_mtim.tv_sec;
			gc_stat.ctime = (gc_Long)st.st_ctim.tv_sec;
		#else
			gc_stat.atime = (gc_Long)st.st_atime;
			gc_stat.mtime = (gc_Long)st.st_mtime;
			gc_stat.ctime = (gc_Long)st.st_ctime;
		#endif
		return gc_stat;
	#else						// Windows really sucks
		HANDLE h = CreateFileA(
			byte_str,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL
		);
		if (h == INVALID_HANDLE_VALUE) {
			return gc_stat;
		}

		BY_HANDLE_FILE_INFORMATION info;
		if (gc_not GetFileInformationByHandle(h, &info)) {
			CloseHandle(h);
			return gc_stat;
		}

		DWORD attrs = info.dwFileAttributes;
		if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
			gc_stat.mode |= 0040000;
		} else {
			gc_stat.mode |= 0100000;
		}

		if (gc_not (attrs & FILE_ATTRIBUTE_READONLY)) {
			gc_stat.mode |= 0222;
		}
		gc_stat.dev = (gc_ULong)info.dwVolumeSerialNumber;
		gc_UInt64 fileindex = ((gc_UInt64)info.nFileIndexHigh << 32) | info.nFileIndexLow;
		gc_stat.ino = (gc_ULong)fileindex;

		gc_stat.nlink = (gc_ULong)info.nNumberOfLinks;
		gc_UInt64 size = ((gc_UInt64)info.nFileSizeHigh << 32) | info.nFileSizeLow;
		gc_stat.size = (gc_Long)size;

		FILETIME ftCreate, ftAccess, ftWrite;
		if (GetFileTime(h, &ftCreate, &ftAccess, &ftWrite)) {
			gc_stat.ctime = filetime_to_time_t(&ftCreate);
			gc_stat.atime = filetime_to_time_t(&ftAccess);
			gc_stat.mtime = filetime_to_time_t(&ftWrite);
		}

		CloseHandle(h);
		return gc_stat;
	#endif
}

gc_Bool gc_Path_exists(gc_Path* self) {
	gc_ConstCharPtr byte_str = gc_Bytes_byte_str(self->bytes);
	#ifndef _WIN32
		struct stat sb;
		return (stat(byte_str, &sb) == 0);
	#else
		DWORD attr = GetFileAttributesA(byte_str);
		return (attr != INVALID_FILE_ATTRIBUTES);
	#endif
}

gc_Bool gc_Path_is_dir(gc_Path* self) {
	gc_ConstCharPtr self_byte_str = gc_Bytes_byte_str(self->bytes);
	#ifndef _WIN32
		struct stat sb;
		if (stat(self_byte_str, &sb) != 0) {
			return False;
		}
		return S_ISDIR(sb.st_mode);
	#else
		DWORD attr = GetFileAttributesA(self_byte_str);
		if (attr == INVALID_FILE_ATTRIBUTES) {
			return False;
		}
		return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
	#endif
}

gc_Bool gc_Path_is_file(gc_Path* self) {
	if (gc_not gc_Path_exists(self)) {
		return False;
	}
	return gc_not gc_Path_is_dir(self);
}

gc_Bool gc_Path_is_relative(gc_Path* self) {
	return gc_not gc_Path_is_absolute(self);
}

gc_Bool gc_Path_is_absolute(gc_Path* self) {
	gc_SizeT path_length = gc_Path_length(self);
	if (path_length < 1) {
		return False;
	}
	gc_ConstCharPtr byte_str = gc_Path_byte_str(self);
	#ifndef _WIN32
		return byte_str[0] == sep[0];
	#else
		if (byte_str[0] == sep[0]) {
			return True;
		}
		if (path_length < 3) {
			return False;
		}
		if (gc_not is_ascii_letter(byte_str[0])) {
			return False;
		}
		if (byte_str[1] != ':') {
			return False;
		}
		return byte_str[2] == sep[0];
	#endif
}

gc_Bool gc_Path_is_equal(gc_Path* self, gc_Path* other) {
	if (gc_Path_length(self) < 1) {
		if (gc_Path_length(other) < 1) {
			return True;
		} else {
			return False;
		}
	}
	if (gc_Path_length(other) < 1) {
		return False;
	}
	gc_Bool res;
	gc_Path* self_real = gc_Path_realpath(self);
	gc_Path* other_real = gc_Path_realpath(other);
	if (self->ignore_case) {
		gc_Bytes* slower = gc_Bytes_lower(self_real->bytes);
		gc_Bytes* olower = gc_Bytes_lower(other_real->bytes);
		res = gc_Bytes_is_equal(slower, olower);
		slower = gc_Bytes_del(slower);
		olower = gc_Bytes_del(olower);
	} else {
		res = gc_Bytes_is_equal(self_real->bytes, other_real->bytes);
	}
	self_real = gc_Path_del(self_real);
	other_real = gc_Path_del(other_real);
	return res;
}

gc_Bool gc_Path_is_relative_to(gc_Path* self, gc_Path* other) {
	gc_SizeT self_length = gc_Path_length(self);
	gc_SizeT other_length = gc_Path_length(other);
	if (self_length < 1 gc_or other_length < 1) {
		return False;
	}
	gc_Path* tmp = gc_Path_add_path(other, self);
	gc_Bool res = gc_Path_exists(tmp);
	tmp = gc_Path_del(tmp);
	return res;
}

gc_SplittedPath* gc_Path_split(gc_Path* self) {
	return gc_SplittedPath_init(self);
}

gc_PathSplitextT* gc_Path_splitext(gc_Path* self) {
	return gc_PathSplitextT_init(self);
}

gc_FullSplittedPath* gc_Path_full_split(gc_Path* self) {
	return gc_FullSplittedPath_init(self);
}

gc_Path* gc_Path_copy(gc_Path* self) {
	gc_Path* cp = (gc_Path*)gc_malloc(gc_sizeof(gc_Path));
	if (gc_not cp) {
		gc_printf("Could not create gc_Path!\n");
		gc_exit(EXIT_NULLPTR);
	}
	cp->bytes = gc_Bytes_copy(self->bytes);
	cp->hash = self->hash;
	cp->ignore_case = self->ignore_case;
	return cp;
}

gc_Path* gc_Path_abspath(gc_Path* self) {
	return gc_Path_realpath(self);
}

gc_Path* gc_Path_realpath(gc_Path* self) {
	if (gc_Bytes_length(self->bytes) < 1) {
		return gc_Path_init("\0");
	}
	gc_ConstCharPtr byte_str = gc_Bytes_byte_str(self->bytes);
	#ifndef _WIN32
		gc_CharPtr buf = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * PATH_MAX);
		gc_CharPtr res = realpath(byte_str, buf);
		if (gc_not res) {
			gc_free(buf);
			return gc_Path_init(byte_str);
		}
		gc_Path* abs_path = gc_Path_cast_bytes(buf, False);
		return abs_path;
	#else
		gc_CharPtr buf = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * MAX_PATH);
		if (GetFullPathName(byte_str, MAX_PATH, buf, NULL) == 0) {
			return gc_Path_init(byte_str);
		}
		gc_Path* res = gc_Path_cast_bytes(buf, False);
		return res;
	#endif
}

gc_Path* gc_Path_dirname(gc_Path* self) {
	gc_SizeT path_length = gc_Bytes_length(self->bytes);
	if (path_length < 1) {
		return gc_Path_init("\0");
	}
	gc_SizeT sep_cntr = gc_Bytes_count(self->bytes, sep, 0, path_length);
	if (sep_cntr == 0) {
		if (gc_Path_is_dir(self)) {
			return gc_Path_copy(self);
		} else {
			return gc_Path_init("\0");
		}
	}
	gc_Int stop_ind = (gc_Int)path_length - 1;
	for (; stop_ind >= 0; stop_ind--) {
		if (gc_Bytes_get(self->bytes, stop_ind) == sep[0]) {
			break;
		}
	}
	if (stop_ind < 0) {
		return gc_Path_init("\0");
	}
	gc_Bytes* slice = gc_Bytes_slice(self->bytes, 0, stop_ind + 1, 1);
	gc_Path* res = gc_Path_from_gc_bytes(slice);
	slice = gc_Bytes_del(slice);
	return res;
}

gc_Path* gc_Path_basename(gc_Path* self) {
	gc_SizeT path_length = gc_Path_length(self);
	gc_Path* tmp_dirname = gc_Path_dirname(self);
	gc_SizeT dirname_length = gc_Path_length(tmp_dirname);
	tmp_dirname = gc_Path_del(tmp_dirname);
	if (dirname_length < 1) {
		return gc_Path_copy(self);
	}
	if (dirname_length >= path_length) {
		return gc_Path_init("\0");
	}
	gc_Bytes* slice = gc_Bytes_slice(self->bytes, dirname_length, path_length, 1);
	if (gc_Bytes_startswith(slice, sep)) {
		gc_Bytes* strip_slice = gc_Bytes_lstrip(slice, sep);
		slice = gc_Bytes_del(slice);
		slice = strip_slice;
	}
	gc_Path* res = gc_Path_from_gc_bytes(slice);
	slice = gc_Bytes_del(slice);
	return res;
}

gc_Path* gc_Path_add(gc_Path* self, gc_ConstCharPtr other) {
	if (other[0] == '\0') {
		return gc_Path_copy(self);
	}
	gc_Path* other_path = gc_Path_init(other);
	gc_Path* res = gc_Path_add_path(self, other_path);
	other_path = gc_Path_del(other_path);
	return res;
}

gc_Path* gc_Path_add_path(gc_Path* self, gc_Path* other) {
	gc_SizeT self_length = gc_Bytes_length(self->bytes);
	gc_SizeT other_length = gc_Bytes_length(other->bytes);
	if (self_length < 1) {
		return gc_Path_copy(other);
	}
	if (other_length < 1) {
		return gc_Path_copy(self);
	}
	if (gc_Bytes_is_equal(self->bytes, other->bytes)) {
		return gc_Path_copy(self);
	}
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(self_length + other_length + 2);
	gc_BytesArray* array = gc_BytesArray_init(array_size, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	gc_BytesArray_push_byte_str(array, gc_Bytes_byte_str(self->bytes));
	if (gc_Bytes_get(other->bytes, 0) != '/' gc_and gc_Bytes_get(other->bytes, 0) != '\\') {
		gc_BytesArray_append(array, '/');
	}
	if (other_length > 0) {
		gc_BytesArray_push_byte_str(array, gc_Bytes_byte_str(other->bytes));
	}
	gc_BytesArray_append(array, '\0');
	gc_Path* res = gc_BytesArray_path_cast(array);
	return res;
}

gc_Path* gc_Path_join(gc_Path* self, gc_ArrayList* gc_path_lst) {
	gc_SizeT array_length = gc_ArrayList_length(gc_path_lst);
	if (array_length < 1) {
		return gc_Path_copy(self);
	}
	gc_SizeT self_length = gc_Bytes_length(self->bytes);
	gc_SizeT max_length = self_length ? self_length > array_length : array_length;
	gc_SizeT array_size = get_min_size_for_gc_bytes_array(max_length + 2);
	gc_BytesArray* array = gc_BytesArray_init(array_size, MIN_GC_BYTES_ARRAY_EXPAND_K_SAFE);
	if (self_length > 0) {
		gc_BytesArray_push_byte_str(array, gc_Bytes_byte_str(self->bytes));
	}
	for (gc_SizeT i = 0; i < array_length; i++) {
		gc_Path* path = (gc_Path*)gc_ArrayList_get(gc_path_lst, i);
		gc_SizeT path_length = gc_Bytes_length(path->bytes);
		if (path_length < 1) {
			continue;
		}
		if (gc_BytesArray_is_dense(array)) {
			gc_BytesArray_expand(array);
		}
		gc_SizeT current_array_length = gc_BytesArray_length(array);
		if (current_array_length > 1) {
			gc_SizeT ind_cntr = 0;
			for (; ind_cntr < current_array_length; ind_cntr++) {
				gc_Char c1 = gc_BytesArray_get(array, i);
				gc_Char c2 = gc_Bytes_get(path->bytes, ind_cntr);
				if (c2 == '\0') {
					break;
				}
				if (c1 != c2) {
					break;
				}
			}
			if (ind_cntr == current_array_length) {
				continue;
			}
		}
		gc_ConstCharPtr byte_str = gc_Bytes_byte_str(path->bytes);
		if (byte_str[0] != '/' gc_and byte_str[0] != '\\') {
			if (current_array_length > 1) {
				gc_BytesArray_append(array, '/');
			}
		}
		gc_BytesArray_push_byte_str(array, byte_str);
	}
	if (gc_BytesArray_is_dense(array)) {
		gc_BytesArray_expand(array);
	}
	gc_BytesArray_append(array, '\0');
	gc_Path* res = gc_BytesArray_path_cast(array);
	return res;
}


gc_SplittedPath* gc_SplittedPath_init(gc_Path* full_path) {
	gc_SplittedPath* self = (gc_SplittedPath*)gc_malloc(gc_sizeof(gc_SplittedPath));
	if (gc_not self) {
		gc_printf("Could not create gc_SplittedPath!\n");
		gc_exit(EXIT_NULLPTR);
	}
	gc_Path* dirname = gc_Path_dirname(full_path);
	gc_Path* basename = gc_Path_basename(full_path);
	self->dirname = dirname;
	self->basename = basename;
	if (gc_Path_length(self->dirname) < 1) {
		if (gc_Path_is_dir(self->basename)) {
			self->dirname = basename;
			self->basename = dirname;
		}
	}
	return self;
}

gc_VoidPtr gc_SplittedPath_del(gc_SplittedPath* obj) {
	obj->dirname = gc_Path_del(obj->dirname);
	obj->basename = gc_Path_del(obj->basename);
	gc_free(obj);
	return NONE;
}

gc_Path* gc_SplittedPath_dirname(gc_SplittedPath* self) {
	return self->dirname;
}

gc_Path* gc_SplittedPath_basename(gc_SplittedPath* self) {
	return self->basename;
}

gc_PathSplitextT* gc_PathSplitextT_init(gc_Path* full_path) {
	gc_PathSplitextT* self = (gc_PathSplitextT*)gc_malloc(gc_sizeof(gc_PathSplitextT));
	if (gc_not self) {
		gc_printf("Could not create gc_PathSplitextT!\n");
		gc_exit(EXIT_NULLPTR);
	}
	gc_Bytes* path = gc_Bytes_copy(full_path->bytes);
	if (gc_Path_is_dir(full_path)) {
		self->name = path;
		self->ext = gc_Bytes_init("\0");
		return self;
	}
	gc_SizeT path_length = gc_Bytes_length(path);
	gc_Int last_ind = (gc_Int)path_length - 1;
	if (last_ind < 2) {
		self->name = path;
		self->ext = gc_Bytes_init("\0");
		return self;
	}
	for(; last_ind >= 0; last_ind--) {
		if (gc_Bytes_get(path, last_ind) == extsep[0]) {
			break;
		}
	}
	if (last_ind < 1) {
		self->name = path;
		self->ext = gc_Bytes_init("\0");
		return self;
	}
	self->ext = gc_Bytes_slice(path, last_ind + 1, path_length, 1);
	self->name = gc_Bytes_slice(path, 0, last_ind, 1);
	path = gc_Bytes_del(path);
	return self;
}

gc_VoidPtr gc_PathSplitextT_del(gc_PathSplitextT* obj) {
	obj->ext = gc_Bytes_del(obj->ext);
	obj->name = gc_Bytes_del(obj->name);
	gc_free(obj);
	return NONE;
}

gc_Bytes* gc_PathSplitextT_ext(gc_PathSplitextT* self) {
	return self->ext;
}

gc_Bytes* gc_PathSplitextT_name(gc_PathSplitextT* self) {
	return self->name;
}


gc_FullSplittedPath* gc_FullSplittedPath_init(gc_Path* full_path) {
	gc_FullSplittedPath* self = (gc_FullSplittedPath*)gc_malloc(gc_sizeof(gc_FullSplittedPath));
	if (gc_not self) {
		gc_printf("Could not create gc_FullSplittedPath!\n");
		gc_exit(EXIT_NULLPTR);
	}
	gc_Path* dirname = gc_Path_dirname(full_path);
	gc_Path* basename = gc_Path_basename(full_path);
	if (gc_Path_length(dirname) < 1) {
		if (gc_Path_is_dir(basename)) {
			gc_Path* t = basename;
			basename = dirname;
			dirname = t;
		}
	}
	gc_Bytes* bname = basename->bytes;
	basename = gc_Path_del_ex(basename, False);
	self->dirname = dirname;
	gc_SizeT bname_len = gc_Bytes_length(bname);
	gc_Int last_ind = (gc_Int)bname_len - 1;
	if (last_ind < 2) {
		self->basename = bname;
		self->ext = gc_Bytes_init("\0");
		return self;
	}
	for (; last_ind >= 0; last_ind--) {
		if (gc_Bytes_get(bname, last_ind) == extsep[0]) {
			break;
		}
	}
	if (last_ind < 1) {
		self->basename = bname;
		self->ext = gc_Bytes_init("\0");
		return self;
	}
	self->ext = gc_Bytes_slice(bname, last_ind + 1, bname_len, 1);
	self->basename = gc_Bytes_slice(bname, 0, last_ind, 1);
	bname = gc_Bytes_del(bname);
	return self;
}

gc_VoidPtr gc_FullSplittedPath_del(gc_FullSplittedPath* obj) {
	obj->ext = gc_Bytes_del(obj->ext);
	obj->dirname = gc_Path_del(obj->dirname);
	obj->basename = gc_Bytes_del(obj->basename);
	gc_free(obj);
	return NONE;
}

gc_Path* gc_FullSplittedPath_dirname(gc_FullSplittedPath* self) {
	return self->dirname;
}

gc_Bytes* gc_FullSplittedPath_ext(gc_FullSplittedPath* self) {
	return self->ext;
}

gc_Bytes* gc_FullSplittedPath_basename(gc_FullSplittedPath* self) {
	return self->basename;
}


gc_CharPtr gc_format_path(gc_ConstCharPtr path, gc_SizeT path_length) {
	if (path[0] == '\0' gc_or path_length < 1) {
		return copy_byte_str_ex(path, 0);
	}
	gc_Int stop_ind = (gc_Int)path_length - 1;
	gc_SizeT start_ind = 0;
	for (; start_ind < path_length; start_ind++) {
		if (gc_not is_letter_whitespace(path[start_ind])) {
			break;
		}
	}
	for (; stop_ind >= (gc_Int)start_ind; stop_ind--) {
		if (gc_not is_letter_whitespace(path[stop_ind])) {
			break;
		}
	}
	if (start_ind >= path_length gc_or stop_ind < 0) {
		return copy_byte_str_ex("\0", 1);
	}
	if (
		path[start_ind] == path[stop_ind]
		gc_and start_ind != stop_ind
	) {
		if (path[start_ind] == '\'' gc_or path[start_ind] == '\"') {
			start_ind++;
			stop_ind--;
		}
	}
	if (start_ind >= path_length gc_or stop_ind < 0) {
		return copy_byte_str_ex("\0", 1);
	}
	gc_SizeT res_length = stop_ind - start_ind + 1;
	gc_CharPtr res = (gc_CharPtr)gc_malloc(gc_sizeof(gc_Char) * (res_length + 1));
	if (gc_not res) {
		gc_printf("Could not create gc_CharPtr in gc_format_path!\n");
		gc_exit(EXIT_NULLPTR);
	}
	if (start_ind > stop_ind) {
		res[0] = '\0';
		return res;
	}
	gc_SizeT i = 0;
	gc_Bool is_seps = True;
	for (; start_ind <= stop_ind; start_ind++) {
		gc_Char c = path[start_ind];
		if (c == '/' gc_or c == '\\') {
			c = sep[0];
		}
		if (c != sep[0]) {
			is_seps = False;
		}
		res[i] = c;
		i++;
	}
	if (gc_not is_seps) {
		while (((gc_Int)res_length - 1) >= 0 gc_and res[res_length - 1] == sep[0]) {
			res_length--;
		}
	}
	res[res_length] = '\0';
	return res;
}

gc_ArrayList* gc_ArrayList_sort_paths(gc_ArrayList* gc_paths_lst) {
	gc_Int lst_length = (gc_Int)gc_ArrayList_length(gc_paths_lst);
	if (lst_length <= 1) {
		return gc_paths_lst;
	}
	gc_Int half_ind = lst_length / 2;
	gc_ArrayList* left;
	gc_ArrayList* right;
	if (half_ind <= 1) {
		left = gc_ArrayList_init(2, 1.5);
		gc_ArrayList_append(left, gc_ArrayList_get(gc_paths_lst, 0));
	} else {
		left = gc_ArrayList_slice(gc_paths_lst, 0, half_ind, 1);
	}
	right = gc_ArrayList_slice(gc_paths_lst, half_ind, lst_length, 1);
	gc_Int left_length = (gc_Int)gc_ArrayList_length(left);
	gc_Int right_length = (gc_Int)gc_ArrayList_length(right);
	left = gc_ArrayList_sort_paths(left);
	right = gc_ArrayList_sort_paths(right);
	gc_Int i = 0, j = 0, k = 0;
	while (i < right_length gc_and j < left_length) {
		gc_Path* el_i = (gc_Path*)gc_ArrayList_get(right, i);
		gc_Path* el_j = (gc_Path*)gc_ArrayList_get(left, j);
		if (gc_Path_length(el_j) < gc_Path_length(el_i)) {
			gc_ArrayList_set(gc_paths_lst, k, el_j);
			j++;
		} else {
			gc_ArrayList_set(gc_paths_lst, k, el_i);
			i++;
		}
		k++;
	}
	while (j < left_length) {
		gc_ArrayList_set(gc_paths_lst, k, gc_ArrayList_get(left, j));
		j++;
		k++;
	}
	while (i < right_length) {
		gc_ArrayList_set(gc_paths_lst, k, gc_ArrayList_get(right, i));
		i++;
		k++;
	}
	left = gc_ArrayList_del(left);
	right = gc_ArrayList_del(right);
	return gc_paths_lst;
}
