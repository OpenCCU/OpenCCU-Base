/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
*
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "TrafficLogger.h"
#include "SerialFrame/SerialFrame.h"
#include "SerialFrame/LowLevelMacFrame.h"
#include "SerialFrame/HmIpStackFrame.h"
#include <Logger.h>
#ifndef WIN32
#include <sys/time.h>
#endif

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
}

TrafficLogger::~TrafficLogger()
{
	if( _file )
	{
		fclose( _file );
	}
}

void TrafficLogger::Configure( bool enabled, const std::string& directory )
{
	LockGuard lock( _mutex );
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
}

bool TrafficLogger::IsEnabled()const
{
	return _enabled;
}

void TrafficLogger::OnUpstreamFrame( const SerialFrame& frame )
{
	if( !_enabled )
	{
		return;
	}
	switch( frame.GetSubsystem() )
	{
	case SerialFrame::FrameSubsystemType_LowLevelMac:
		if( frame.GetCommand() == LowLevelMacFrame::FrameType_RxTelegram )
		{
			//telegram data starts after frame end time (2 bytes), options (1 byte) and rssi (1 byte)
			if( frame.Data().size() > 4 )
			{
				LogBidcosTelegram( "RX", frame.Data().GetRange( 4, frame.Data().size() - 4 ) );
			}
		}
		break;
	case SerialFrame::FrameSubsystemType_HmIpStack:
		if( frame.GetCommand() == HmIpStackFrame::FrameType_RxFrameEvent )
		{
			LogHmIpFrame( "RX", frame.Data() );
		}
		break;
	default:
		break;
	}
}

void TrafficLogger::OnDownstreamFrame( const SerialFrame& frame )
{
	if( !_enabled )
	{
		return;
	}
	switch( frame.GetSubsystem() )
	{
	case SerialFrame::FrameSubsystemType_LowLevelMac:
		switch( frame.GetCommand() )
		{
		case LowLevelMacFrame::FrameType_Tx:
		case LowLevelMacFrame::FrameType_TxWitRxModeSet:
			//telegram data starts after frame start time (2 bytes) and options (1 byte)
			if( frame.Data().size() > 3 )
			{
				LogBidcosTelegram( "TX", frame.Data().GetRange( 3, frame.Data().size() - 3 ) );
			}
			break;
		default:
			break;
		}
		break;
	case SerialFrame::FrameSubsystemType_HmIpStack:
		switch( frame.GetCommand() )
		{
		case HmIpStackFrame::FrameType_SendProtocolFrame:
		case HmIpStackFrame::FrameType_OtauFrameSend:
			LogHmIpFrame( "TX", frame.Data() );
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void TrafficLogger::LogBidcosTelegram( const char* direction, const BinaryData& telegramData )
{
	//telegram layout: [0]=counter, [1]=flags, [2]=type, [3..5]=sender, [6..8]=receiver, [9..]=payload
	//the length equals the on-air length byte, which counts all bytes following it
	char buffer[80];
	snprintf( buffer, sizeof(buffer), "LEN=%02X CNT=%02X FLAGS=%02X TYPE=%02X FROM=%06X TO=%06X PAYLOAD=",
		(unsigned)telegramData.size(),
		telegramData.GetUInt8Value( 0 ),
		telegramData.GetUInt8Value( 1 ),
		telegramData.GetUInt8Value( 2 ),
		telegramData.GetUInt24Value( 3 ),
		telegramData.GetUInt24Value( 6 ) );
	std::string fields = buffer;
	if( telegramData.size() > 9 )
	{
		fields += ToHex( telegramData.GetRange( 9, telegramData.size() - 9 ) );
	}
	WriteLine( direction, "BIDCOS", fields );
}

void TrafficLogger::LogHmIpFrame( const char* direction, const BinaryData& frameData )
{
	//the HmIP stack frame content is passed through this daemon transparently,
	//so it is logged unparsed
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "LEN=%02X DATA=", (unsigned)frameData.size() );
	WriteLine( direction, "HMIP", std::string( buffer ) + ToHex( frameData ) );
}

void TrafficLogger::WriteLine( const char* direction, const char* protocol, const std::string& fields )
{
	struct tm localTime;
	int milliseconds;
#ifndef WIN32
	struct timeval tv;
	gettimeofday( &tv, NULL );
	localtime_r( &tv.tv_sec, &localTime );
	milliseconds = tv.tv_usec / 1000;
#else
	time_t now = time( NULL );
	localTime = *localtime( &now );
	milliseconds = 0;
#endif

	LockGuard lock( _mutex );
	if( !_enabled )
	{
		return;
	}
	FILE* file = GetLogFile( localTime );
	if( !file )
	{
		return;
	}
	fprintf( file, "%04d-%02d-%02d %02d:%02d:%02d.%03d %s %s %s\n",
		localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
		localTime.tm_hour, localTime.tm_min, localTime.tm_sec, milliseconds,
		direction, protocol, fields.c_str() );
	fflush( file );
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
	snprintf( filename, sizeof(filename), "multimacd-traffic-%04d-%02d-%02d.log",
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

/*static*/ std::string TrafficLogger::ToHex( const BinaryData& data )
{
	static const char* HEX_DIGITS = "0123456789ABCDEF";
	std::string s;
	s.reserve( data.size() * 2 );
	for( size_t i = 0; i < data.size(); i++ )
	{
		s += HEX_DIGITS[data.at( i ) >> 4];
		s += HEX_DIGITS[data.at( i ) & 0x0f];
	}
	return s;
}
