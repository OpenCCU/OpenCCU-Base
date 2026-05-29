/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// Logger.cpp: Implementierung der Klasse Logger.
//
//////////////////////////////////////////////////////////////////////

#include "Logger.h"
#include "Timestamp.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#ifdef WIN32
#define vsnprintf _vsnprintf
#endif

/*static*/ Logger* Logger::s_logger=NULL;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

Logger::Logger()
{
	m_level=LOG_ERROR;
	pthread_mutex_init(&mutex, NULL);
	bufpointer=m_buffer;
	log_timestamp=true;
}

Logger::~Logger()
{
	pthread_mutex_destroy(&mutex);
}

bool Logger::Log(Logger::LogLevel l, const char *format ...)
{
	if(!s_logger)return false;
	if(l<s_logger->GetLevel())return false;
	char* timestamp=NULL;
	if(s_logger->log_timestamp){
    timestamp=new char[128];
    Timestamp::GetAsString(timestamp, 128);
	}
	pthread_mutex_lock(&s_logger->mutex);
	va_list args;
	va_start(args, format);
	vsnprintf(s_logger->bufpointer, sizeof(s_logger->m_buffer)-(s_logger->bufpointer-s_logger->m_buffer), format, args);
	va_end(args);	
	bool retval=s_logger->DoLog(l, timestamp, s_logger->m_buffer);
	pthread_mutex_unlock(&s_logger->mutex);
    if(timestamp)delete[] timestamp;
	return retval;
}

bool Logger::WouldLog(Logger::LogLevel l) {
	if(!s_logger) {
		return false;
	}
	if(l < s_logger->GetLevel()) {
		return false;
	}
	else {
		return true;
	}
}

bool Logger::LogRealm(const char* realm, Logger::LogLevel l, const char *format ...)
{
	if(!s_logger)return false;
	if(l<s_logger->GetLevelRealm(realm))return false;
	char* timestamp=NULL;
	if(s_logger->log_timestamp){
		struct tm tm_local;
		time_t ti=time(NULL);
		localtime_r(&ti, &tm_local);
		timestamp=new char[128];
		strftime(timestamp, 128, "%d.%m.%Y %H:%M:%S", &tm_local);
	}
	pthread_mutex_lock(&s_logger->mutex);
	va_list args;
	va_start(args, format);
	vsnprintf(s_logger->bufpointer, sizeof(s_logger->m_buffer)-(s_logger->bufpointer-s_logger->m_buffer), format, args);
	va_end(args);	
	bool retval=s_logger->DoLog(l, timestamp, s_logger->m_buffer);
	pthread_mutex_unlock(&s_logger->mutex);
	if(timestamp)delete[] timestamp;
	return retval;
}

bool Logger::DoLog(Logger::LogLevel l, const char *t, const char *msg)
{
	return false;
}

void Logger::SetLevel(Logger::LogLevel l)
{
	pthread_mutex_lock(&mutex);
	m_level=l;
	pthread_mutex_unlock(&mutex);
}

void Logger::SetLevelRealm(const char* realm, Logger::LogLevel l)
{
	pthread_mutex_lock(&mutex);
	m_level_by_realm[realm] = l;
	pthread_mutex_unlock(&mutex);
}

Logger::LogLevel Logger::GetLevel()
{
	LogLevel l;
	pthread_mutex_lock(&mutex);
	l=m_level;
	pthread_mutex_unlock(&mutex);
	return l;
}

Logger::LogLevel Logger::GetLevelRealm(const char* realm)
{
	LogLevel l;
	pthread_mutex_lock(&mutex);
	t_level_by_realm::iterator it = m_level_by_realm.find(realm);
	if( it != m_level_by_realm.end() )l = it->second;
	else l=m_level;
	pthread_mutex_unlock(&mutex);
	return l;
}

void Logger::SetExtraInfo(const char *s)
{
	snprintf(m_buffer, sizeof(m_buffer), "%s: ", s);
	bufpointer=m_buffer+strlen(m_buffer);
}

void Logger::SetLogTimestamp(bool log_timestamp)
{
	this->log_timestamp = log_timestamp;
}
