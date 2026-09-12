/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
*
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "TrafficLogger.h"
#include "BidcosFrame.h"
#include <Logger.h>
#include <sys/time.h>

/*static*/ TrafficLogger* TrafficLogger::_instance = NULL;

/*static*/ TrafficLogger& TrafficLogger::Instance()
{
	if( !_instance )
	{
		_instance = new TrafficLogger();
	}
	return *_instance;
}

TrafficLogger::TrafficLogger()
{
	_enabled = false;
	_directory = "/var/log";
	_file = NULL;
	_fileYear = 0;
	_fileMonth = 0;
	_fileDay = 0;
	pthread_mutex_init( &_mutex, NULL );
}

TrafficLogger::~TrafficLogger()
{
	if( _file )
	{
		fclose( _file );
	}
	pthread_mutex_destroy( &_mutex );
}

void TrafficLogger::Configure( bool enabled, const std::string& directory )
{
	pthread_mutex_lock( &_mutex );
	_enabled = enabled;
	if( !directory.empty() )
	{
		_directory = directory;
	}
	if( _file )
	{
		fclose( _file );
		_file = NULL;
		_fileYear = _fileMonth = _fileDay = 0;
	}
	pthread_mutex_unlock( &_mutex );
}

bool TrafficLogger::IsEnabled()const
{
	return _enabled;
}

void TrafficLogger::LogTelegram( const char* direction, const BidcosFrame& frame, const std::string& interfaceId )
{
	if( !_enabled )
	{
		return;
	}
	//Telegrammlayout im Datenblock: [0]=Zaehler, [1]=Flags, [2]=Typ,
	//[3..5]=Absender, [6..8]=Empfaenger, [9..]=Payload (wie beim multimacd-Logger)
	int rssi = frame.GetRSSI();
	if( rssi == (int)BidcosFrame::INVALID_RSSI_VALUE )
	{
		rssi = 0;
	}
	char buffer[128];
	snprintf( buffer, sizeof(buffer), "LEN=%02X CNT=%02X FLAGS=%02X TYPE=%02X FROM=%06X TO=%06X RSSI=%d IFACE=%s PAYLOAD=",
		frame.GetSize(),
		frame.GetTelegramCounter() & 0xff,
		frame.GetCtrl() & 0xff,
		frame.GetByteData( 2 ),
		frame.GetSenderAddress() & 0xffffff,
		frame.GetReceiverAddress() & 0xffffff,
		rssi,
		interfaceId.c_str() );
	std::string fields = buffer;
	if( frame.GetSize() > 9 )
	{
		fields += frame.ToString().substr( 18 );
	}
	WriteLine( direction, fields );
}

void TrafficLogger::WriteLine( const char* direction, const std::string& fields )
{
	struct tm localTime;
	struct timeval tv;
	gettimeofday( &tv, NULL );
	localtime_r( &tv.tv_sec, &localTime );

	pthread_mutex_lock( &_mutex );
	if( !_enabled )
	{
		pthread_mutex_unlock( &_mutex );
		return;
	}
	FILE* file = GetLogFile( localTime );
	if( !file )
	{
		pthread_mutex_unlock( &_mutex );
		return;
	}
	fprintf( file, "%04d-%02d-%02d %02d:%02d:%02d.%03d %s BIDCOS %s\n",
		localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
		localTime.tm_hour, localTime.tm_min, localTime.tm_sec, (int)(tv.tv_usec / 1000),
		direction, fields.c_str() );
	fflush( file );
	pthread_mutex_unlock( &_mutex );
}

FILE* TrafficLogger::GetLogFile( const struct tm& localTime )
{
	if( _file && (localTime.tm_year == _fileYear) && (localTime.tm_mon == _fileMonth) && (localTime.tm_mday == _fileDay) )
	{
		return _file;
	}
	if( _file )
	{
		fclose( _file );
		_file = NULL;
	}
	_fileYear = localTime.tm_year;
	_fileMonth = localTime.tm_mon;
	_fileDay = localTime.tm_mday;

	char filename[64];
	snprintf( filename, sizeof(filename), "rfd-traffic-%04d-%02d-%02d.log",
		localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday );
	std::string path = _directory;
	if( (!path.empty()) && (path[path.length()-1] != '/') )
	{
		path += '/';
	}
	path += filename;
	_file = fopen( path.c_str(), "a" );
	if( !_file )
	{
		LOG( Logger::LOG_WARNING, "TrafficLogger: unable to open %s", path.c_str() );
	}
	return _file;
}
