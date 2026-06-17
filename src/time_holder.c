#include "../headers/time_holder.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct gc_TimeHolder {
	gc_UInt64 microseconds;
};

gc_TimeHolder* gc_TimeHolder_get_empty() {
	gc_TimeHolder* self = (gc_TimeHolder*)gc_malloc(gc_sizeof(gc_TimeHolder));
	if (gc_not self) {
		gc_printf("Could not create gc_TimeHolder!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->microseconds = (gc_UInt64)0;
	return self;
}

gc_VoidPtr gc_TimeHolder_del(gc_TimeHolder* obj) {
	gc_free(obj);
	return NONE;
}

gc_TimeHolder* gc_TimeHolder_microseconds(gc_UInt64 amount) {
	gc_TimeHolder* self = gc_TimeHolder_get_empty();
	self->microseconds = amount;
	return self;
}

gc_TimeHolder* gc_TimeHolder_milliseconds(gc_UInt32 amount) {
	gc_TimeHolder* self = gc_TimeHolder_get_empty();
	self->microseconds = (gc_UInt64)1000 * (gc_UInt64)amount;
	return self;
}

gc_TimeHolder* gc_TimeHolder_seconds(gc_Floating amount) {
	gc_TimeHolder* self = gc_TimeHolder_get_empty();
	self->microseconds = (gc_UInt64)(1000000. * amount);
	return self;
}

gc_Floating gc_TimeHolder_as_seconds(gc_TimeHolder* self) {
	return (gc_Floating)self->microseconds / 1000000;
}

gc_UInt32 gc_TimeHolder_as_milliseconds(gc_TimeHolder* self) {
	return (gc_UInt32)((gc_Floating)self->microseconds / 1000);
}

gc_UInt64 gc_TimeHolder_as_microseconds(gc_TimeHolder* self) {
	return self->microseconds;
}
