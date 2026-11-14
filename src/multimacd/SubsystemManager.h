/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "tinythread.h"
#include "TelegramQueue.h"
#include "IUpstreamSerialFrameSink.h"
#include "Enums.h"
#include <map>

class Subsystem;
class IDownstreamSerialFrameSink;
class SubsystemManager : public IUpstreamSerialFrameSink
{
public:
	SubsystemManager(void);
	~SubsystemManager(void);
	bool Start();
	bool Stop();
	void SetDownstreamFrameSink( IDownstreamSerialFrameSink& frameSink );
	void OnUpstreamFrame( SerialFrame* frame );
	Subsystem* GetSubsystem( MacSubsystemType subsystemType );
	static std::string GetSubsystemName( MacSubsystemType subsystemType );
private:
	void RegisterSubsystem( Subsystem* subsystem );
	void ThreadFunction();
	static void sThreadFunction(void* arg)
	{
		reinterpret_cast<SubsystemManager*>(arg)->ThreadFunction();
	}
	tthread::thread _thread;
	bool _exit;
	tthread::mutex _mutex;
	typedef tthread::lock_guard<tthread::mutex> LockGuard;
	TelegramQueue<SerialFrame*> _incomingQueue;
	IDownstreamSerialFrameSink* _downstreamFrameSink;
	typedef std::map<MacSubsystemType, Subsystem*> MapSubsystemsByType;
	MapSubsystemsByType _mapSubsystemsByType;



	//typedef std::map<MacSubsystemType, Subsystem*> SubsystemMap;
	//SubsystemMap _subsystems;

};

