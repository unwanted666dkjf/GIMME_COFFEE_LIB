#include <stdio.h>

#include "headers/array_list.h"
#include "headers/os.h"

#include "headers/path.h"

#include "headers/file.h"

#include "headers/timer.h"

#include "headers/bytes.h"

#include "headers/inifile.h"

#include "headers/bytes_utils.h"

#include "headers/bytes_concrete_strip.h"

#include "headers/aliases.h"

#include "headers/wrappers.h"

#include "headers/constants.h"


#define ARRAY_SIZE 3


int main() {
	gc_PathWalkGen* walk_gen = gc_PathWalkGen_init("C:/Users", 1024 * 9);
	while (gc_PathWalkGen_has_next(walk_gen)) {
		gc_PathWalkItem* item = gc_PathWalkGen_next(walk_gen);
		gc_Path* root = gc_PathWalkItem_root(item);
		gc_printf("\nroot: %s\n", gc_Path_byte_str(root));

		gc_ArrayList* dirs = gc_PathWalkItem_dirs(item);
		gc_printf("dirs:\n");
		for (gc_SizeT i = 0; i < gc_ArrayList_length(dirs); i++) {
			gc_Path* p = (gc_Path*)gc_ArrayList_get(dirs, i);
			gc_printf("\t%s\n", gc_Path_byte_str(p));
		}

		gc_ArrayList* files = gc_PathWalkItem_files(item);
		gc_printf("files:\n");
		for (gc_SizeT i = 0; i < gc_ArrayList_length(files); i++) {
			gc_Path* p = (gc_Path*)gc_ArrayList_get(files, i);
			gc_printf("\t%s\n", gc_Path_byte_str(p));
		}

		item = gc_PathWalkItem_del(item);
	}

	walk_gen = gc_PathWalkGen_del(walk_gen);
	gc_printf("End!\n");
	return 0;
}
