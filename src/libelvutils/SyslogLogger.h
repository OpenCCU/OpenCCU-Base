/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// SyslogLogger.h: Schnittstelle für die Klasse SyslogLogger.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SYSLOGLOGGER_H_
#define _SYSLOGLOGGER_H_

#include "dllexport.h"
#include "Logger.h"

class ELVUTILS_DLLEXPORT SyslogLogger : public Logger  
{
public:
	SyslogLogger(const char* id="");
	virtual ~SyslogLogger();
protected:
	virtual bool DoLog(LogLevel l, const char* t, const char* msg);

};

#endif
