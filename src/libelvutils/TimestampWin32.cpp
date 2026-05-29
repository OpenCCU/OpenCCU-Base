/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "Timestamp.h"

#include <ctime>
#include <cstdio>

char* Timestamp::GetAsString(char* result, size_t size)
{
	struct tm* t;
	time_t ti=time(NULL);
	t=localtime(&ti);
	strftime(result, size, "%d.%m.%Y %H:%M:%S", t);

  return result;
}

