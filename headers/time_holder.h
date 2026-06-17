#ifndef GIMME_COFFEE_TIME_HOLDER_H
#define GIMME_COFFEE_TIME_HOLDER_H


#include "./import_macro.h"

#include "./aliases.h"


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_TimeHolder;

/**
 * Structure for storing time.
 * Stores time as microseconds, type gc_UInt64.
 * Allows casting into gc_UInt64*. Which is microseconds.
 */
typedef struct gc_TimeHolder gc_TimeHolder;


/**
 * Returns object with 0 microseconds.
 */
GIMME_COFFEE_API gc_TimeHolder* gc_TimeHolder_get_empty();

/**
 * Deletes object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_TimeHolder_del(gc_TimeHolder* obj);

/**
 * Creates time object from given microseconds.
 */
GIMME_COFFEE_API gc_TimeHolder* gc_TimeHolder_microseconds(gc_UInt64 amount);

/**
 * Creates time object from given milliseconds.
 */
GIMME_COFFEE_API gc_TimeHolder* gc_TimeHolder_milliseconds(gc_UInt32 amount);

/**
 * Creates time object from given seconds.
 */
GIMME_COFFEE_API gc_TimeHolder* gc_TimeHolder_seconds(gc_Floating amount);

/**
 * Returns time as seconds.
 */
GIMME_COFFEE_API gc_Floating gc_TimeHolder_as_seconds(gc_TimeHolder* self);

/**
 * Returns time as milliseconds.
 */
GIMME_COFFEE_API gc_UInt32 gc_TimeHolder_as_milliseconds(gc_TimeHolder* self);

/**
 * Returns time as microseconds.
 */
GIMME_COFFEE_API gc_UInt64 gc_TimeHolder_as_microseconds(gc_TimeHolder* self);


#ifdef __cplusplus
}
#endif


#endif
