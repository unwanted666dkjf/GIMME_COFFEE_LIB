#ifndef GIMME_COFFEE_TIMER_H
#define GIMME_COFFEE_TIMER_H


#include "./time_holder.h"


#ifdef __cplusplus
extern "C" {
#endif


struct GIMME_COFFEE_API gc_Timer;

/**
 * Timer structure.
 * Stores and returns time.
 * Allows casting into gc_UInt64*. Which is microseconds.
 */
typedef struct gc_Timer gc_Timer;


/**
 * Creates timer and starts timer.
 */
GIMME_COFFEE_API gc_Timer* gc_Timer_init();

/**
 * Deletes timer object and returns NONE.
 */
GIMME_COFFEE_API gc_VoidPtr gc_Timer_del(gc_Timer* obj);

/**
 * Adds the elapsed time span into internal buffer and returns
 *the cumulative elapsed time as microseconds.
 */
GIMME_COFFEE_API gc_UInt64 gc_Timer_wait(gc_Timer* self);

/**
 * Restarts timer and returns elapsed time as gc_TimeHolder pointer.
 */
GIMME_COFFEE_API gc_TimeHolder* gc_Timer_restart(gc_Timer* self);

/**
 * Returns elapsed time as gc_TimeHolder pointer.
 */
GIMME_COFFEE_API gc_TimeHolder* gc_Timer_elapsed_time(gc_Timer* self);


#ifdef __cplusplus
}
#endif


#endif
