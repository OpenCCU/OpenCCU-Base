/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "TimeZoneInfo.h"
#include <cstdint>

TimeZoneInfo::TimeZoneInfo(void)
{
    m_time=time(NULL);
}

TimeZoneInfo::TimeZoneInfo(time_t t)
{
    m_time=t;
}

TimeZoneInfo::~TimeZoneInfo(void)
{
}

bool TimeZoneInfo::IsDst(time_t t)
{
  struct tm tm_local;
  localtime_r(&t, &tm_local);
  return tm_local.tm_isdst != 0;
}

bool TimeZoneInfo::CalcNextChange(int days_advance, time_t* change_time, time_t* change_offset)
{
  time_t t1=(m_time+CHANGE_RESOLUTION-1)/CHANGE_RESOLUTION;
  time_t t2=t1+(days_advance*86400/CHANGE_RESOLUTION);

  if( IsDst(t2*CHANGE_RESOLUTION) == IsDst(m_time) ){
      return false;
  }

  int64_t diff;
  while( (diff=int64_t(t2-t1))>1 ){
     if( IsDst((t2-diff/2)*CHANGE_RESOLUTION) != IsDst(t1*CHANGE_RESOLUTION) )t2=t2-diff/2;
     else t1=t2-diff/2;
  }

  *change_time=t2*CHANGE_RESOLUTION;

  struct tm t_tm;
  localtime_r(&m_time, &t_tm);
  int64_t t_minutes=t_tm.tm_min+t_tm.tm_hour*60-int64_t(m_time%86400)/60;

  struct tm t2_tm;
  localtime_r(change_time, &t2_tm);
  int64_t t2_minutes=t2_tm.tm_min+t2_tm.tm_hour*60-int64_t(*change_time%86400)/60;

  int64_t offset_minutes=t2_minutes - t_minutes;
  if(offset_minutes < -(12*60))offset_minutes+=(24*60);
  if(offset_minutes > (12*60))offset_minutes-=(24*60);

  *change_offset=offset_minutes*60;
  return true;
}

time_t TimeZoneInfo::GetUTCOffset()
{
  struct tm tm_utc;
  gmtime_r(&m_time, &tm_utc);
  struct tm tm_local;
  localtime_r(&m_time, &tm_local);
  int64_t offset_minutes=(tm_local.tm_hour*60+tm_local.tm_min) - (tm_utc.tm_hour*60+tm_utc.tm_min);

  if(offset_minutes < -(12*60))offset_minutes+=(24*60);
  if(offset_minutes > (12*60))offset_minutes-=(24*60);
  return offset_minutes*60;
}
