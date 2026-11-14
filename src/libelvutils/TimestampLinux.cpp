/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "Timestamp.h"

#include <sys/time.h>
#include <ctime>
#include <cstdio>

char* Timestamp::GetAsString(char* result, size_t size)
{
  char* timestr = result;

  struct timespec now_timespec;
  clock_gettime(CLOCK_REALTIME, &now_timespec);

  struct tm now_localtime;
  if (localtime_r(&now_timespec.tv_sec, &now_localtime) == NULL) {
    return result;
  }

  if (strftime(timestr, size, "%Y/%m/%d %H:%M:%S", &now_localtime) != 19) {
    return result;
  }
  timestr += 19;
  size -= 19;

  // Add milliseconds after a dot. Should result in exactly 4 characters
  // written.
  if (snprintf(timestr, size, ".%03ld", now_timespec.tv_nsec) != 4) {
    return result;
  }

  return result;
}
