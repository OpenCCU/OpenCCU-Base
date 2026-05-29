/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// ConsoleLogger.cpp: Implementierung der Klasse ConsoleLogger.
//
//////////////////////////////////////////////////////////////////////

#include "ConsoleLogger.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ConsoleLogger::ConsoleLogger()
{
	log_timestamp=true;
}

ConsoleLogger::~ConsoleLogger()
{
}

bool ConsoleLogger::DoLog(Logger::LogLevel l, const char *t, const char *msg)
{
	std::cerr<<t<<" ";
	switch(l){
	case LOG_DEBUG:
		std::cerr<<"<Debug>";
		break;
	case LOG_INFO:
		std::cerr<<"<Info>";
		break;
	case LOG_NOTICE:
		std::cerr<<"<Notice>";
		break;
	case LOG_WARNING:
		std::cerr<<"<Warning>";
		break;
	case LOG_ERROR:
		std::cerr<<"<Error>";
		break;
	case LOG_FATAL_ERROR:
		std::cerr<<"<Fatal error>";
		break;
	default:
		break;
	}
	std::cerr<<" "<<msg<<std::endl;
	return true;
}
