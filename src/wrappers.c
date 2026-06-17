#include <time.h>

#include <stdarg.h>

#include <stdio.h>

#include <stdlib.h>

#include "../headers/wrappers.h"


gc_UInt gc_time_null_seed() {
	return (gc_UInt)time(NULL);
}


gc_Int gc_random(gc_Int low, gc_Int high) {
	if (low > high) {
		gc_Int l = low;
		low = high;
		high = l;
	}
	gc_Int range = high - low + 1;
	gc_Int delta = (gc_Int)((gc_Floating)rand() / ((gc_Floating)RAND_MAX + 1) * range);
	return low + delta;
}


gc_VoidPtr gc_malloc(gc_SizeT size) {
	return malloc(size);
}


gc_Void gc_srand(gc_UInt seed) {
	srand(seed);
}


gc_Void gc_free(gc_VoidPtr ptr) {
	free(ptr);
}


gc_Void gc_exit(gc_Int status) {
	exit(status);
}


gc_Void gc_printf(gc_CharPtr msg, ...) {
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}
