/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _ELVUTILS_DLLEXPORT_H_
#define _ELVUTILS_DLLEXPORT_H_

#ifdef WIN32
  #ifdef ELVUTILS_BUILD_DLL
    #define ELVUTILS_DLLEXPORT __declspec(dllexport)
    #pragma warning( disable: 4251 )
  #else
    #ifdef BUILD_LIB
      #define ELVUTILS_DLLEXPORT
    #else
      #define ELVUTILS_DLLEXPORT __declspec(dllimport)
      #pragma warning( disable: 4251 )
    #endif
  #endif
#else
  #define ELVUTILS_DLLEXPORT
#endif

#endif

