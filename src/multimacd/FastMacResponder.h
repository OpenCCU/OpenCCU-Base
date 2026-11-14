/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "tinythread.h"

class SerialFrame;
class IUpstreamSerialFrameSink;
class FastMacResponder
{
public:
	FastMacResponder(void);
	~FastMacResponder(void);
	bool Start( const std::string& device );
	bool Stop();

	void SetUpstreamFrameSink( IUpstreamSerialFrameSink& frameSink );

private:
    int OpenPort(const char* device);
    bool SetPriority( int fd, unsigned int latency );
	void OnFrameFromCoprocessor( SerialFrame* frame );

	void ThreadFunction();
	static void sThreadFunction(void* arg)
	{
		reinterpret_cast<FastMacResponder*>(arg)->ThreadFunction();
	}
	tthread::thread _thread;
	int _fd;
	volatile bool _exit;

	IUpstreamSerialFrameSink* _frameSink;

};

