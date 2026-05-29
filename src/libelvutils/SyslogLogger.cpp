/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// SyslogLogger.cpp: Implementierung der Klasse SyslogLogger.
//
//////////////////////////////////////////////////////////////////////

#include "SyslogLogger.h"
#ifndef WIN32
#include <syslog.h>
static const int PRIORITY[]={LOG_DEBUG, LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING, LOG_ERR, LOG_CRIT};
#else
#define openlog(a,b,c)
#define closelog()
#define syslog(a,b)
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SyslogLogger::SyslogLogger(const char* id)
{
	openlog( id, 0, LOG_USER);
	log_timestamp=false;
}

SyslogLogger::~SyslogLogger()
{
	closelog();
}

bool SyslogLogger::DoLog(Logger::LogLevel l, const char *, const char *msg)
{
	syslog(PRIORITY[(int)l]|LOG_USER, "%s", msg);
	return true;
}
