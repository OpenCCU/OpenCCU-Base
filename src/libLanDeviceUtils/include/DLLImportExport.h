#ifndef _LIBLANDEVICEUTILS_DLLIMPORTEXPORT_H_
#define _LIBLANDEVICEUTILS_DLLIMPORTEXPORT_H_

#ifdef WIN32
#  ifdef LIBLANDEVICEUTILS_BUILD
#    define LIBLANDEVICEUTILS_API __declspec(dllexport)
#  else
#    define LIBLANDEVICEUTILS_API __declspec(dllimport)
#  endif
#else
#  define LIBLANDEVICEUTILS_API 
#endif

#endif