#include <time.h>

#include "../headers/timer.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct gc_Timer {
	gc_UInt64 microseconds;
	time_t start_time;
};

gc_Timer* gc_Timer_init() {
	gc_Timer* self = (gc_Timer*)gc_malloc(gc_sizeof(gc_Timer));
	if (gc_not self) {
		gc_printf("Could not create gc_Timer!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->start_time = time(NULL);
	self->microseconds = (gc_UInt64)0;
	return self;
}

gc_VoidPtr gc_Timer_del(gc_Timer* obj) {
	gc_free(obj);
	return NONE;
}

gc_UInt64 gc_Timer_wait(gc_Timer* self) {
	time_t end_time = time(NULL);
	gc_Floating elapsed = difftime(end_time, self->start_time);
	self->microseconds += (gc_UInt64)(elapsed * 1000000.);
	self->start_time = end_time;
	return self->microseconds;
}

gc_TimeHolder* gc_Timer_restart(gc_Timer* self) {
	gc_TimeHolder* holder = gc_TimeHolder_microseconds(self->microseconds);
	self->microseconds = (gc_UInt64)0;
	self->start_time = time(NULL);
	return holder;
}

gc_TimeHolder* gc_Timer_elapsed_time(gc_Timer* self) {
	gc_TimeHolder* holder = gc_TimeHolder_microseconds(self->microseconds);
	return holder;
}
