/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "Sysutils.h"
#ifdef WIN32
#include <WinSock2.h>
#include <windows.h>
#include <io.h>
#else
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <time.h>
#endif
#include "tinythread.h"
#include <string.h>
#include <errno.h>
#include <Logger.h>

/*static*/ uint32_t Sysutils::GetMonotonicTime()
{
#ifdef WIN32
	return GetTickCount();
#else
	struct timespec ts;
	clock_gettime( CLOCK_MONOTONIC, &ts );
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
}

/*static*/ uint32_t Sysutils::GetMonotonicTimeSeconds()
{
#ifdef WIN32
	return uint32_t(GetTickCount64() / 1000);
#else
	struct timespec ts;
	clock_gettime( CLOCK_MONOTONIC, &ts );
	return ts.tv_sec;
#endif
}

/*static*/ bool Sysutils::ThreadSetSchedulingPriority( uint8_t belowHighest )
{
#ifndef WIN32
	struct sched_param param;
	memset( &param, 0, sizeof( param ) );
	param.sched_priority = sched_get_priority_max( SCHED_RR ) - belowHighest;
	if( param.sched_priority < sched_get_priority_min( SCHED_RR ) )
	{
		param.sched_priority = sched_get_priority_min( SCHED_RR );
	}
	if( sched_setscheduler(0, SCHED_RR, &param ) < 0 )
	{
		LOG( Logger::LOG_WARNING, "Could not set scheduling priority: %s", strerror( errno ) ); 
		return false;
	}
#endif
	return true;
}

