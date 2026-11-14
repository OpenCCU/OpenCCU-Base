/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "IUpstreamSerialFrameSink.h"
#include "tinythread.h"

class Subsystem;

class UpstreamCharConnection : public IUpstreamSerialFrameSink
{
public:
	UpstreamCharConnection(Subsystem& subsystem);
	~UpstreamCharConnection(void);
	bool Start( const char* masterdev, const char* slavedev );
	bool Stop();
	void OnUpstreamFrame( SerialFrame* frame );
private:
	enum{
		SlaveEventOpen = 1,
		SlaveEventClose = 2,
		SlaveStateOpen = 0x8000,

	};
	int SetupDevices( const char* masterdev, const char* slavedev );
	void ThreadFunction();
	static void sThreadFunction(void* arg)
	{
		reinterpret_cast<UpstreamCharConnection*>(arg)->ThreadFunction();
	}
	tthread::thread _thread;
	bool _exit;
	tthread::mutex _mutex;
	typedef tthread::lock_guard<tthread::mutex> LockGuard;
	tthread::condition_variable _condition;

	unsigned char _txBuf[1024];
	int _fd;
	std::string _slaveDevice;
	Subsystem& _subsystem;
};

