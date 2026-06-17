#ifndef GIMME_COFFEE_WRAPPERS_H
#define GIMME_COFFEE_WRAPPERS_H


#include "./import_macro.h"

#include "./aliases.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Simply:
 * gc_UInt gc_time_null_seed() { return (gc_UInt)time(NULL);}
 */
GIMME_COFFEE_API gc_UInt gc_time_null_seed();

/**
 * Wrapper for rand.
 * Returns a random integer from inclusive range: [low; high].
 */
GIMME_COFFEE_API gc_Int gc_random(gc_Int low, gc_Int high);

/**
 * Wrapper for malloc.
 * Allocate 'size' bytes of memory.
 */
GIMME_COFFEE_API gc_VoidPtr gc_malloc(gc_SizeT size);

/**
 * Wrapper for srand.
 * Seed the random number generator with the given number.
 */
GIMME_COFFEE_API gc_Void gc_srand(gc_UInt seed);

/**
 * Wrapper for free.
 * Free a block allocated by `malloc', `realloc' or `calloc'.
 */
GIMME_COFFEE_API gc_Void gc_free(gc_VoidPtr ptr);

/**
 * Wrapper for exit.
 * Call all functions registered with `atexit' and `on_exit',
 *in the reverse of the order in which they were registered,
 *perform stdio cleanup, and terminate program execution with STATUS.
 */
GIMME_COFFEE_API gc_Void gc_exit(gc_Int status);

/**
 * Custom printf.
 * Write formatted output to stdout.
 */
GIMME_COFFEE_API gc_Void gc_printf(gc_CharPtr msg, ...);


#ifdef __cplusplus
}
#endif


#endif
