/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
*
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "Subsystem.h"
#include "SerialFrame/SerialFrame.h"
#include "tinythread.h"
#include "Logger.h"
#include "IUpstreamSerialFrameSink.h"
#include "IDownstreamSerialFrameSink.h"
#include "MultimacManager.h"
#include "UpstreamCharConnection.h"
#include "TrafficLogger.h"
#ifndef WIN32
#include <sys/syscall.h>
#endif

Subsystem::Subsystem(MacSubsystemType subsystemType, const char* subsystemName)
{
	_subsystemType = subsystemType;
	_subsystemName = subsystemName;
	_upstreamFrameSink = NULL;
	_downstreamFrameSink = NULL;
	_nextUpstreamSequenceCounter = 0;
	_upstreamCharConnection = NULL;
	_exit = false;
}


Subsystem::~Subsystem(void)
{
	if( _upstreamCharConnection )
	{
		delete _upstreamCharConnection;
	}
}

bool Subsystem::UpstreamConnect( IUpstreamSerialFrameSink* frameSink )
{
	{
		LockGuard lock( _mutex );
		if( _upstreamFrameSink != NULL )
		{
			LOG( Logger::LOG_WARNING, "Subsystem %d refusing upstream connect. Already connected.", _subsystemType );
			return false;
		}
		_upstreamFrameSink = frameSink;
		_nextUpstreamSequenceCounter = 0;
	}
	if( !OnUpstreamConnect() )
	{
		LockGuard lock( _mutex );
		_upstreamFrameSink = NULL;
		return false;
	}

	return true;
}

bool Subsystem::UpstreamDisconnect( IUpstreamSerialFrameSink* frameSink )
{
	LockGuard lock( _mutex );
	if( _upstreamFrameSink != frameSink )
	{
		LOG( Logger::LOG_WARNING, "Subsystem %d refusing upstream disconnect. Client mismatch.", _subsystemType );
		return false;
	}
	_upstreamFrameSink = NULL;
	return true;
}

void Subsystem::SetDownstreamFrameSink( IDownstreamSerialFrameSink* frameSink )
{
	LockGuard lock( _mutex );
	_downstreamFrameSink = frameSink;
}


bool Subsystem::Start()
{
	if( _thread.joinable() )
	{
		LOG( Logger::LOG_WARNING, "Subsystem thread already running" ); 
		return false;
	}

	std::string loopMasterDevice = MultimacManager::Instance().GetConfiguration().GetStringValue( "Loop Master Device" );
	if( !loopMasterDevice.empty() )
	{
		std::string loopSlaveDevice = MultimacManager::Instance().GetConfiguration().GetStringValue( std::string("Loop Slave Device ") + _subsystemName );
		if( !loopSlaveDevice.empty() )
		{
			_upstreamCharConnection = new UpstreamCharConnection( *this );
			_upstreamCharConnection->Start( loopMasterDevice.c_str(), loopSlaveDevice.c_str() );
		}
	}

	LockGuard lock( _mutex );
	_exit = false;
	_thread = tthread::thread( &sThreadFunction, this );
	return true;
}

bool Subsystem::Stop()
{
	if( _thread.joinable() )
	{
		{
			LockGuard lock( _mutex );
			_exit = true;
		}
		_condition.notify_all();
		_thread.join();
		if( _upstreamCharConnection )
		{
			_upstreamCharConnection->Stop();
			delete _upstreamCharConnection;
			_upstreamCharConnection = NULL;
		}
		return true;
	}
	return false;
}

MacSubsystemType Subsystem::GetMacSubsystemType()
{
	return _subsystemType;
}

bool Subsystem::OnUpstreamFrame( SerialFrame* frame )
{
	if( (frame->GetResponsibleSubsystem() == this) || CheckIfResponsibleForUpstreamFrame( frame ) )
	{
		LockGuard lock( _mutex );
		_upstreamQueue.push( frame );
		_condition.notify_all();
		return true;
	}
	return false;
}

void Subsystem::OnDownstreamFrame( SerialFrame* frame )
{
	LockGuard lock( _mutex );
	_downstreamQueue.push( frame );
	_condition.notify_all();
}

void Subsystem::ThreadFunction()
{
#ifndef WIN32
	LOG( Logger::LOG_DEBUG, "Subsystem %d ThreadFunction() started. Id=%d", _subsystemType, syscall(SYS_gettid) ); 
#endif
	OnWorkerThreadStarted();
	LockGuard lock(_mutex);
	while( !_exit )
	{
		if( !_upstreamQueue.empty() )
		{
			SerialFrame* frame = _upstreamQueue.front();
			_upstreamQueue.pop();
			MapFrameIdToSequenceCounter::iterator it = _mapFrameIdToSequenceCounter.find( frame->GetId() );
			if( it != _mapFrameIdToSequenceCounter.end() )
			{
				frame->SetSequenceCounter( it->second );
				_mapFrameIdToSequenceCounter.erase( it );
				_mutex.unlock();
			}else{
				_mutex.unlock();
				frame->SetSequenceCounter(GetNextUpstreamSequenceCounter());
			}
			if( !ProcessUpstreamFrame( frame ) )
			{
				delete frame;
			}
			_mutex.lock();
		}
		if( !_downstreamQueue.empty() )
		{
			SerialFrame* frame = _downstreamQueue.front();
			_downstreamQueue.pop();
			_mutex.unlock();
			if( !ProcessDownstreamFrame( frame ) )
			{
				delete frame;
			}
			_mutex.lock();
		}
		_mutex.unlock();
		uint32_t sleepTime = CanSleep();
		_mutex.lock();
		if( sleepTime > 5000 )
		{
			sleepTime = 5000;
		}
		if( sleepTime && (!_exit) && _downstreamQueue.empty() && _upstreamQueue.empty() )
		{
			//LOG( Logger::LOG_DEBUG, "Subsystem %d sleeping for %ums", _subsystemType, sleepTime ); 
			_condition.wait_for( _mutex, tthread::chrono::milliseconds( sleepTime ) );
		}

	}
	LOG( Logger::LOG_DEBUG, "Subsystem %d ThreadFunction() ended", _subsystemType ); 
}

void Subsystem::SendFrameUpstream( SerialFrame* frame )
{
	if( !frame->IsSequenceCounterValid() )
	{
		frame->SetSequenceCounter( GetNextUpstreamSequenceCounter() );
	}
	LockGuard lock(_mutex);
	if( _upstreamFrameSink )
	{
		_upstreamFrameSink->OnUpstreamFrame( frame );
	}else{
		delete frame;
	}
}

void Subsystem::SendFrameDownstream( SerialFrame* frame )
{
	TrafficLogger::Instance().OnDownstreamFrame( *frame );
	LockGuard lock(_mutex);
	frame->SetResponsibleSubsystem( this );
	_mapFrameIdToSequenceCounter[frame->GetId()] = frame->GetSequenceCounter();
	if( _downstreamFrameSink )
	{
		_downstreamFrameSink->OnDownstreamFrame( frame );
	}else{
		delete frame;
	}
}

uint8_t Subsystem::GetNextUpstreamSequenceCounter()
{
	LockGuard lock(_mutex);
	return _nextUpstreamSequenceCounter++;
}

uint32_t Subsystem::CanSleep()
{
	return 5000;
}
