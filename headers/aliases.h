#ifndef GIMME_COFFEE_ALIASES_H
#define GIMME_COFFEE_ALIASES_H


#include <wchar.h>

#include <stdint.h>


#define gc_or ||

#define gc_and &&

#define gc_not !

#define gc_sizeof(x) (sizeof(x))

#define gc_typedef(type, alias) typedef type alias


gc_typedef(void, gc_Void);

gc_typedef(void*, gc_VoidPtr);

gc_typedef(char, gc_Char);

gc_typedef(char*, gc_CharPtr);

gc_typedef(const char*, gc_ConstCharPtr);

gc_typedef(wchar_t, gc_WChar);

gc_typedef(wchar_t*, gc_WCharPtr);

gc_typedef(const wchar_t*, gc_ConstWcharPtr);

gc_typedef(unsigned char, gc_UChar);

gc_typedef(unsigned char*, gc_UCharPtr);

gc_typedef(int8_t, gc_Int8);

gc_typedef(int16_t, gc_Int16);

gc_typedef(int32_t, gc_Int32);

gc_typedef(int64_t, gc_Int64);

gc_typedef(uint8_t, gc_UInt8);

gc_typedef(uint16_t, gc_UInt16);

gc_typedef(uint32_t, gc_UInt32);

gc_typedef(uint64_t, gc_UInt64);

gc_typedef(float, gc_Float);

gc_typedef(double, gc_Double);

gc_typedef(long long, gc_Long);

gc_typedef(unsigned long long, gc_ULong);

#if defined(__x86_64__) || defined(__amd64__)
	gc_typedef(long, gc_Int);
	
	gc_typedef(gc_UInt64, gc_SizeT);
	
	gc_typedef(gc_Double, gc_Floating);
	
	typedef unsigned long gc_UInt;
#else
	gc_typedef(int, gc_Int);

	gc_typedef(gc_UInt32, gc_SizeT);
	
	gc_typedef(gc_Float, gc_Floating);
	
	typedef unsigned int gc_UInt;
#endif


/**
 * Pseudo-boolean type.
 * False is 0, True is 1.
 */
typedef enum {False=0, True=1} gc_Bool;

/**
 * Pseudo-boolean for comparison.
 * Lesser is -1, Equal is 0, Greater is 1.
 */
typedef enum {Lesser=-1, Equal=0, Greater=1} gc_ComparatorT;

typedef gc_ComparatorT (*gc_Comparator)(gc_VoidPtr, gc_VoidPtr);


#endif
