#ifndef GIMME_COFFEE_IMPORT_MACRO_H
#define GIMME_COFFEE_IMPORT_MACRO_H


#if defined(_WIN32) || defined(_WIN64)
	#ifdef GIMME_COFFEE_EXPORTS
		#define GIMME_COFFEE_API __declspec(dllexport)
	#else
		#define GIMME_COFFEE_API
	#endif
#else
	#ifdef GIMME_COFFEE_EXPORTS
		#define GIMME_COFFEE_API __attribute__((visibility("default")))
	#else
		#define GIMME_COFFEE_API
	#endif
#endif


#endif
