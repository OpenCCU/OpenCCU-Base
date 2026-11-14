/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

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