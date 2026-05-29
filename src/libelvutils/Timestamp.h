/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include "dllexport.h"
#include <cstddef>

class ELVUTILS_DLLEXPORT Timestamp
{
  public:
    static char* GetAsString(char* result, size_t size);
};


#endif

