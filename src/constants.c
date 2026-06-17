#include "../headers/constants.h"


const gc_VoidPtr NONE = (gc_VoidPtr)0;

const gc_Int EXIT_OK = 0;

const gc_Int EXIT_ERR = 1;

const gc_Int EXIT_NULLPTR = 2;

const gc_Int EXIT_LOGICAL_ERROR = 3;

const gc_Int INDEX_NOT_FOUND = -1;


#if defined(__x86_64__) || defined(__amd64__)
	const gc_SizeT INF = 1844674407370955161;
#else
	const gc_SizeT INF = 4294967295;
#endif
