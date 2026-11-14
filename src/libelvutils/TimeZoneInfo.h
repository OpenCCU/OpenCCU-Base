/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _TIMEZONEINFO_H_
#define _TIMEZONEINFO_H_

#include "dllexport.h"
#include <time.h>

class ELVUTILS_DLLEXPORT TimeZoneInfo
{
public:
    TimeZoneInfo(void);
    TimeZoneInfo(time_t t);
    ~TimeZoneInfo(void);
    bool CalcNextChange(int days_advance, time_t* change_time, time_t* change_offset);
    time_t GetUTCOffset();
private:
    enum{
        CHANGE_RESOLUTION=1800
    };
    static bool IsDst(time_t t);
    time_t m_time;
};

#endif
