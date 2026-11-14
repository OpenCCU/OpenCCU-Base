/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// Logger.h: Schnittstelle f�r die Klasse Logger.
//
//////////////////////////////////////////////////////////////////////

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "dllexport.h"
#include <pthread.h>
#include <map>
#include <string>

class ELVUTILS_DLLEXPORT Logger  
{
public:
	void SetExtraInfo(const char* s);
	typedef enum {LOG_ALL, LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING, LOG_ERROR, LOG_FATAL_ERROR} LogLevel;
	LogLevel GetLevel();
	LogLevel GetLevelRealm(const char* realm);
	void SetLevel(LogLevel l);
	void SetLevelRealm(const char* realm, LogLevel l);
	void SetLogTimestamp(bool log_timestamp);
	static bool Log(LogLevel l, const char* format ...);
	static bool LogRealm(const char* realm, LogLevel l, const char* format ...);
	static bool WouldLog(LogLevel l);
	Logger();
	virtual ~Logger();
	static Logger* s_logger;
protected:
	char m_buffer[1024];
	char* bufpointer;
	pthread_mutex_t mutex;
	virtual bool DoLog(LogLevel l, const char* t, const char* msg);
	LogLevel m_level;
	typedef std::map<std::string, LogLevel> t_level_by_realm;
	t_level_by_realm m_level_by_realm;
	bool log_timestamp;
};

#define logger Logger::s_logger

#ifndef NO_LOGGING

#  ifndef WIN32
#    ifdef LOGGING_REALM
#      define LOG(level, fmt, args...) Logger::LogRealm(LOGGING_REALM, level, fmt, ## args)
#    else
#      define LOG(level, fmt, args...) Logger::Log(level, fmt, ## args)
#    endif
#  else
#    if _MSC_VER >= 1400 //VS2005 and above support variable argument macros
#      ifdef LOGGING_REALM
#        define LOG(level, fmt, ...) Logger::LogRealm(LOGGING_REALM, level, fmt, __VA_ARGS__)
#      else
#        define LOG(level, fmt, ...) Logger::Log(level, fmt, __VA_ARGS__)
#      endif
#    else
#      define LOG Logger::Log
#    endif
#  endif
#else
#  ifndef WIN32
#    define LOG(level, fmt, args...)
#  else
     inline void __DUMMY_LOG_FUNCTION(Logger::LogLevel l, const char* fmt ...){};
#    define LOG __DUMMY_LOG_FUNCTION
#  endif
#endif


#endif
