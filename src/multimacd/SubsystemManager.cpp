/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
*
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "SubsystemManager.h"
#include "Subsystem.h"
#include "SubsystemBidcos.h"
#include "SubsystemHmIp.h"
#include "SerialFrame/SerialFrame.h"
#include "TrafficLogger.h"
#include <Logger.h>
#include "Sysutils.h"
#ifndef WIN32
#include <sys/syscall.h>
#endif

SubsystemManager::SubsystemManager(void)
{
	_downstreamFrameSink = NULL;
	RegisterSubsystem( new SubsystemBidcos() );
	RegisterSubsystem( new SubsystemHmIp() );
}


SubsystemManager::~SubsystemManager(void)
{
	LockGuard lock( _mutex );
	for( MapSubsystemsByType::iterator it = _mapSubsystemsByType.begin(); it != _mapSubsystemsByType.end(); it++)
	{
		delete it->second;
	}
	_mapSubsystemsByType.clear();
}

void SubsystemManager::SetDownstreamFrameSink( IDownstreamSerialFrameSink& frameSink )
{
	LockGuard lock( _mutex );
	_downstreamFrameSink = &frameSink;
}

bool SubsystemManager::Start()
{
	if( _thread.joinable() )
	{
		LOG( Logger::LOG_WARNING, "SubsystemManager thread already running" ); 
		return false;
	}
	LockGuard lock( _mutex );
	_exit = false;
	_thread = tthread::thread( &sThreadFunction, this );

	for( MapSubsystemsByType::iterator it = _mapSubsystemsByType.begin(); it != _mapSubsystemsByType.end(); it++)
	{
		it->second->SetDownstreamFrameSink( _downstreamFrameSink );
		if( !it->second->Start() )
		{
			Stop();
			return false;
		}
	}

	return true;
}

bool SubsystemManager::Stop()
{
	if( _thread.joinable() )
	{
		{
			LockGuard lock( _mutex );
			_exit = true;
		}
		_incomingQueue.Signal();
		_thread.join();

		for( MapSubsystemsByType::iterator it = _mapSubsystemsByType.begin(); it != _mapSubsystemsByType.end(); it++)
		{
			it->second->Stop();
		}
		return true;
	}
	return false;
}

void SubsystemManager::OnUpstreamFrame( SerialFrame* frame )
{
	TrafficLogger::Instance().OnUpstreamFrame( *frame );
	_incomingQueue.Add( frame );
}

void SubsystemManager::RegisterSubsystem( Subsystem* subsystem )
{
	LockGuard lock( _mutex );
	_mapSubsystemsByType[subsystem->GetMacSubsystemType()]=subsystem;
}

Subsystem* SubsystemManager::GetSubsystem( MacSubsystemType subsystemType )
{
	LockGuard lock( _mutex );
	MapSubsystemsByType::iterator it = _mapSubsystemsByType.find(subsystemType);
	if( it != _mapSubsystemsByType.end() )
	{
		return it->second;
	}
	return NULL;

}


void SubsystemManager::ThreadFunction()
{
#ifndef WIN32
	LOG( Logger::LOG_DEBUG, "SubsystemManager::ThreadFunction() started. Id=%d", syscall(SYS_gettid) ); 
#endif

	Sysutils::ThreadSetSchedulingPriority( 3 );

	while( true )
	{
		{
			LockGuard lock(_mutex);
			if( _exit )
			{
				break;
			}
		}
		SerialFrame* frame = _incomingQueue.GetNextElement( 5000 );
		if( frame )
		{
			LockGuard lock( _mutex );
			for( MapSubsystemsByType::iterator it = _mapSubsystemsByType.begin(); it != _mapSubsystemsByType.end(); it++)
			{
				if( frame->GetSubsystem() == SerialFrame::FrameSubsystemType_Internal )
				{
					SerialFrame* clone = frame->Clone();
					if( clone )
					{
						if( !it->second->OnUpstreamFrame( clone ) )
						{
							LOG( Logger::LOG_WARNING, "SubsystemManager: Internal upstream frame not handled by subsystem %s: %s", GetSubsystemName(it->second->GetMacSubsystemType()).c_str(), clone->ToString().c_str() );
							delete clone;
						}
					}
				}else if( it->second->OnUpstreamFrame( frame ) )
				{
					frame = NULL;
					break;
				}
			}
			if( frame )
			{
				if( frame->GetSubsystem() != SerialFrame::FrameSubsystemType_Internal )
				{
					LOG( Logger::LOG_WARNING, "SubsystemManager: Upstream frame not handled by any subsystem" );
				}
				delete frame;
			}
		}
	}
	LOG( Logger::LOG_DEBUG, "SubsystemManager::ThreadFunction() ended" ); 
}

/*static*/ std::string SubsystemManager::GetSubsystemName( MacSubsystemType subsystemType )
{
	switch( subsystemType )
	{
	case MacSubsystemType_HmIp:
		return "HmIP";
	case MacSubsystemType_Bidcos:
		return "Bidcos";
	case MacSubsystemType_Bootloader:
		return "Bootloader";
	case MacSubsystemType_Transparent:
		return "Transparent";
	default:
		return "";
	}
}
