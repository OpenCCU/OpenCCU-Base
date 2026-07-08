/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
*
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "BinaryData.h"
#include "tinythread.h"
#include <stdio.h>
#include <time.h>
#include <string>

class SerialFrame;

/// <summary>
/// Logs all radio rx/tx traffic passing between the subsystems and the rf module
/// into one log file per day (e.g. /var/log/multimacd-traffic-2026-07-06.log).
/// Bidcos telegrams are logged with their decoded fields (length, counter, flags,
/// type, sender, receiver, payload), HmIP frames are logged as raw frame data.
/// </summary>
class TrafficLogger
{
public:
	static TrafficLogger& Instance();
	void Configure( bool enabled, const std::string& directory );
	bool IsEnabled()const;
	/// frame received from the rf module (rx path)
	void OnUpstreamFrame( const SerialFrame& frame );
	/// frame sent towards the rf module (tx path)
	void OnDownstreamFrame( const SerialFrame& frame );
private:
	TrafficLogger();
	~TrafficLogger();
	void LogBidcosTelegram( const char* direction, const BinaryData& telegramData );
	void LogHmIpFrame( const char* direction, const BinaryData& frameData );
	void WriteLine( const char* direction, const char* protocol, const std::string& fields );
	FILE* GetLogFile( const struct tm& localTime );
	static std::string ToHex( const BinaryData& data );

	bool _enabled;
	std::string _directory;
	FILE* _file;
	int _fileYear;
	int _fileMonth;
	int _fileDay;
	tthread::mutex _mutex;
	typedef tthread::lock_guard<tthread::mutex> LockGuard;
	static TrafficLogger* _instance;
};
