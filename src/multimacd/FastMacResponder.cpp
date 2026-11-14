/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "FastMacResponder.h"
#include "IUpstreamSerialFrameSink.h"
#include "SerialFrame/SerialFrame.h"
#include <Logger.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef WIN32
#include <sys/ioctl.h>
#include <sys/syscall.h>
#else
#include <io.h>
#endif
#include "Sysutils.h"

/* Use 'e' as magic number */
#define MXS_AUART_IOC_MAGIC  'u'

/*
* S means "Set" through a ptr,
* G means "Get": reply by setting through a pointer
*/
#define MXS_AUART_IOCSPRIORITY _IOW(MXS_AUART_IOC_MAGIC,  1, uint32_t)
#define MXS_AUART_IOCGPRIORITY _IOR(MXS_AUART_IOC_MAGIC,  2, uint32_t)

FastMacResponder::FastMacResponder(void)
{
	_fd = -1;
	_exit = false;
	_frameSink = NULL;
}


FastMacResponder::~FastMacResponder(void)
{
}

bool FastMacResponder::Start( const std::string& device )
{
	if( _thread.joinable() )
	{
		LOG( Logger::LOG_WARNING, "FastMacResponder thread already running" ); 
		return false;
	}
	_fd = OpenPort( device.c_str() );
	if( _fd < 0 )
	{
		return false;
	}
	if( !SetPriority( _fd, 1 ) )
	{
		//return false;
	}
	_exit = false;
	_thread = tthread::thread( &sThreadFunction, this );
	return true;
}

bool FastMacResponder::Stop()
{
	if( _thread.joinable() )
	{
		_exit = true;
#ifndef WIN32
		pthread_kill( _thread.native_handle(), SIGUSR1 );
#endif
		_thread.join();
		return true;
	}
	return false;
}

int FastMacResponder::OpenPort( const char* device )
{
	int fd = open (device, O_RDWR);
	if (fd <= 0)
	{
		LOG( Logger::LOG_WARNING, "FastMacResponder could not open port %s: %s", device, strerror( errno ) ); 
		return -1;
	}
	return fd;
}

bool FastMacResponder::SetPriority( int fd, unsigned int latency )
{
#ifndef WIN32
	if( ioctl( fd, MXS_AUART_IOCSPRIORITY, &latency ) )
	{
		LOG( Logger::LOG_WARNING, "FastMacResponder could not set port priority: %s", strerror( errno ) ); 
		return false;
	}
#endif
	return true;
}

void FastMacResponder::ThreadFunction()
{
#ifndef WIN32
	LOG( Logger::LOG_DEBUG, "FastMacResponder::ThreadFunction() started Id=%d", syscall(SYS_gettid) ); 
#endif

	if( !Sysutils::ThreadSetSchedulingPriority( 0 ) )
	{
		close( _fd );
		return;
	}
	uint8_t buffer[1024];
	size_t bufLength = 0;
	while( !_exit )
	{
		int count = read( _fd, buffer + bufLength, sizeof(buffer) - bufLength );
		if( count <= 0 )
		{
			if(count == 0) {
				if(errno != 0) {
					LOG(Logger::LOG_DEBUG, "FastMacResponder read returned with count 0");
				}
				continue;
			}
			else {
				LOG( Logger::LOG_WARNING, "FastMacResponder read error: %s", strerror( errno ) );
				break;
			}
		}
		bufLength += count;
		//BinaryData binData( (uint8_t*)buffer, bufLength );
		//LOG( Logger::LOG_DEBUG, "FastMacResponder RX binary:%s", binData.ToString().c_str());

		size_t frameStartPos = 0;
		while( frameStartPos < bufLength && buffer[frameStartPos] != SerialFrame::StartChar )
		{
			frameStartPos++;
		}
		if( frameStartPos )
		{
			LOG( Logger::LOG_WARNING, "FastMacResponder skipped %d bytes", frameStartPos ); 
		}
		if( frameStartPos == bufLength )
		{
			bufLength = 0;
			continue;
		}
		if( bufLength - frameStartPos < 6 )
		{
			//LOG( Logger::LOG_DEBUG, "FastMacResponder frameStartPos=%u, bufLength=%u", frameStartPos, bufLength ); 
			if( frameStartPos > 0 )
			{
				bufLength -= frameStartPos;
				memmove( buffer, buffer + frameStartPos, bufLength );
			}
			continue;
		}
		size_t numEaten = 1;
		while( numEaten && bufLength )
		{
			SerialFrame * frame = SerialFrame::Create( buffer + frameStartPos, bufLength - frameStartPos, &numEaten );
			//LOG( Logger::LOG_DEBUG, "FastMacResponder frameStartPos=%u, numEaten=%u, bufLength=%u", frameStartPos, numEaten, bufLength ); 
			if( frameStartPos + numEaten )
			{
				bufLength -= frameStartPos + numEaten;
				memmove( buffer, buffer + frameStartPos + numEaten, bufLength );
			}
			if( frame )
			{
				OnFrameFromCoprocessor( frame );
			}
		}
	}
	LOG( Logger::LOG_DEBUG, "FastMacResponder::ThreadFunction() ended" ); 
}

void FastMacResponder::OnFrameFromCoprocessor( SerialFrame* frame )
{
	LOG( Logger::LOG_DEBUG, "C> @%u: %s", Sysutils::GetMonotonicTime(), frame->ToString().c_str() );
	if( _frameSink )
	{
		_frameSink->OnUpstreamFrame( frame );
	} else {
		delete frame;
	}
}

void FastMacResponder::SetUpstreamFrameSink( IUpstreamSerialFrameSink& frameSink )
{
	_frameSink = &frameSink;
}
