/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// ConsoleLogger.h: Schnittstelle für die Klasse ConsoleLogger.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CONSOLELOGGER_H_
#define _CONSOLELOGGER_H_

#include "dllexport.h"
#include "Logger.h"
#include <string>

class ELVUTILS_DLLEXPORT ConsoleLogger : public Logger  
{
public:
	ConsoleLogger();
	virtual ~ConsoleLogger();

protected:
	std::string m_strFilename;
	bool DoLog(LogLevel l, const char *t, const char *msg);
};

#endif
