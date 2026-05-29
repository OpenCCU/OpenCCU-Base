/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// FileLogger.cpp: Implementierung der Klasse FileLogger.
//
//////////////////////////////////////////////////////////////////////

#include "FileLogger.h"
#include <fstream>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

FileLogger::FileLogger()
{
	stream=0;
}

FileLogger::~FileLogger()
{

}

bool FileLogger::DoLog(Logger::LogLevel l, const char *t, const char *msg)
{
	std::ostream* os;
	std::ofstream fileStream;
	if(!stream){
        char filename[256];
		struct tm tm_local;
		time_t ti=time(NULL);
		localtime_r(&ti, &tm_local);
		strftime(filename, sizeof(filename), m_strFilename.c_str(), &tm_local);
		fileStream.open(filename, std::ios::app);
		if(!fileStream.good())return false;
		os=&fileStream;
	}else{
		os=stream;
	}
	(*os)<<t<<" ";
	switch(l){
	case LOG_DEBUG:
		(*os)<<"<Debug>";
		break;
	case LOG_INFO:
		(*os)<<"<Info>";
		break;
	case LOG_NOTICE:
		(*os)<<"<Notice>";
		break;
	case LOG_WARNING:
		(*os)<<"<Warning>";
		break;
	case LOG_ERROR:
		(*os)<<"<Error>";
		break;
	case LOG_FATAL_ERROR:
		(*os)<<"<Fatal error>";
		break;
	default:
		break;
	}
	(*os)<<" "<<msg<<std::endl;
	return (*os).good();
}

void FileLogger::SetFilename(const char *fn)
{
	m_strFilename=fn;
}

void FileLogger::SetStream(std::ostream* of)
{
	stream=of;
}
