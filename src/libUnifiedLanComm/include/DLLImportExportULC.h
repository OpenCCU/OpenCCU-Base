/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBUNIFIEDLANCOMM_DLLIMPORTEXPORT_H_
#define _LIBUNIFIEDLANCOMM_DLLIMPORTEXPORT_H_

#ifdef WIN32
#  ifdef LIBUNIFIEDLANCOMM_BUILD
#    define LIBUNIFIEDLANCOMM_API __declspec(dllexport)
#  else
#    define LIBUNIFIEDLANCOMM_API __declspec(dllimport)
#  endif
#else
#  define LIBUNIFIEDLANCOMM_API 
#endif

#endif