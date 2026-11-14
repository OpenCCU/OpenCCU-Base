/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include <queue>
#include "Enums.h"
#include "tinythread.h"
#include <stdint.h>
#include <map>


class UpstreamCharConnection;
class SerialFrame;
class IUpstreamSerialFrameSink;
class IDownstreamSerialFrameSink;
class Subsystem
{
public:
	virtual ~Subsystem(void);
	bool Start();
	bool Stop();
	bool OnUpstreamFrame( SerialFrame* frame );
	MacSubsystemType GetMacSubsystemType();
	bool UpstreamConnect( IUpstreamSerialFrameSink* frameSink );
	bool UpstreamDisconnect( IUpstreamSerialFrameSink* frameSink );
	void SetDownstreamFrameSink( IDownstreamSerialFrameSink* frameSink );
	virtual std::size_t ConsumeDownstreamBytes( uint8_t* buf, std::size_t len ) = 0;
protected:
	Subsystem(MacSubsystemType subsystemType, const char* subsystemName);
	virtual bool CheckIfResponsibleForUpstreamFrame( const SerialFrame* frame ) = 0;
	virtual bool ProcessUpstreamFrame( SerialFrame* frame ) = 0;
	virtual bool ProcessDownstreamFrame( SerialFrame* frame ) = 0;
	virtual bool OnUpstreamConnect() { return true; }
	void SendFrameUpstream( SerialFrame* frame );
	void SendFrameDownstream( SerialFrame* frame );
	void OnDownstreamFrame( SerialFrame* frame );
	virtual uint32_t CanSleep();
	virtual void OnWorkerThreadStarted(){};
private:
	uint8_t GetNextUpstreamSequenceCounter();
	MacSubsystemType _subsystemType;
	void ThreadFunction();
	static void sThreadFunction(void* arg)
	{
		reinterpret_cast<Subsystem*>(arg)->ThreadFunction();
	}
	tthread::thread _thread;
	bool _exit;
	tthread::mutex _mutex;
	typedef tthread::lock_guard<tthread::mutex> LockGuard;
	tthread::condition_variable _condition;
	std::queue<SerialFrame*> _upstreamQueue;
	std::queue<SerialFrame*> _downstreamQueue;
	IUpstreamSerialFrameSink* _upstreamFrameSink;
	IDownstreamSerialFrameSink* _downstreamFrameSink;
	uint8_t _nextUpstreamSequenceCounter;

	UpstreamCharConnection* _upstreamCharConnection;

	typedef std::map<uint32_t, uint8_t> MapFrameIdToSequenceCounter;
	MapFrameIdToSequenceCounter _mapFrameIdToSequenceCounter;
	std::string _subsystemName;
};

