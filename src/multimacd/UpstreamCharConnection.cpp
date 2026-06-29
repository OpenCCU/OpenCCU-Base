/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifdef WIN32
#include "winsock2.h"
#endif
#include "UpstreamCharConnection.h"
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
#include <poll.h>
#else
#define O_NONBLOCK 0
#include <io.h>
#endif
#include "Subsystem.h"
#include "SerialFrame/SerialFrame.h"
#include <Logger.h>

#define EQ3LOOP_IOC_MAGIC  'L'

#define EQ3LOOP_IOCSCREATESLAVE _IOW(EQ3LOOP_IOC_MAGIC,  1, uint32_t)
#define EQ3LOOP_IOCGEVENTS _IOR(EQ3LOOP_IOC_MAGIC,  2, uint32_t)


UpstreamCharConnection::UpstreamCharConnection(Subsystem& subsystem) : _subsystem(subsystem)
{
	_fd = -1;
	_exit = false;
}


UpstreamCharConnection::~UpstreamCharConnection(void)
{
}

bool UpstreamCharConnection::Start( const char* masterdev, const char* slavedev )
{
	if( _thread.joinable() )
	{
		LOG( Logger::LOG_WARNING, "UpstreamCharConnection thread already running" ); 
		return false;
	}
	_fd = SetupDevices( masterdev, slavedev );
	if( _fd < 0 )
	{
		return false;
	}
	_slaveDevice = slavedev;
	_exit = false;
	_thread = tthread::thread( &sThreadFunction, this );
	return true;
}

bool UpstreamCharConnection::Stop()
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

int UpstreamCharConnection::SetupDevices( const char* masterdev, const char* slavedev )
{
	int fd = open (masterdev, O_RDWR | O_NONBLOCK);
	if (fd <= 0)
	{
		LOG( Logger::LOG_WARNING, "UpstreamCharConnection could not open master port %s: %s", masterdev, strerror( errno ) ); 
		return -1;
	}

#ifndef WIN32
	//setup the slave device
	if( ioctl( fd, EQ3LOOP_IOCSCREATESLAVE, slavedev ) )
	{
		LOG( Logger::LOG_WARNING, "UpstreamCharConnection could not create slave device %s: %s", slavedev, strerror( errno ) ); 
		close( fd );
		return -1;
	}
#endif

	return fd;
}

void UpstreamCharConnection::OnUpstreamFrame( SerialFrame* frame )
{
	LockGuard lock( _mutex );
	{
		size_t frameLength = frame->GetRawData( _txBuf, sizeof( _txBuf ) );
		size_t written = 0;
		while( written < frameLength )
		{
			int count = write( _fd, _txBuf + written, frameLength - written );
			if( count < 0 )
			{
				LOG( Logger::LOG_WARNING, "UpstreamCharConnection slave device %s write error: %s", _slaveDevice.c_str(), strerror( errno ) ); 
				//Stop(); 
				break;
			}
			if( count == 0 )
			{
				LOG( Logger::LOG_WARNING, "UpstreamCharConnection slave device %s buffer overflow during write %s", _slaveDevice.c_str() ); 
				break;
			}
			written += count;
		}
	}
	LOG( Logger::LOG_DEBUG, "A<: %s", frame->ToString().c_str() );
	delete frame;
}



void UpstreamCharConnection::ThreadFunction()
{
#ifndef WIN32
	LOG( Logger::LOG_DEBUG, "UpstreamCharConnection slave %s ThreadFunction() started Id=%d", _slaveDevice.c_str(), syscall(SYS_gettid) ); 
#endif

	bool slaveOpen = false;
	uint8_t buffer[1024];
	size_t bufLength = 0;

	while( !_exit )
	{
		if( _fd < 0 )
		{
			LOG( Logger::LOG_WARNING, "UpstreamCharConnection slave %s invalid file descriptor", _slaveDevice.c_str() );
			break;
		}

		pollfd pfd;
		pfd.fd = _fd;
		pfd.events = POLLIN | POLLPRI | POLLERR | POLLHUP;
		pfd.revents = 0;

		int nEvents = poll(&pfd, 1, 5000);
		if( nEvents < 0 )
		{
			if( errno == EINTR )
			{
				continue;
			}
			LOG( Logger::LOG_WARNING, "UpstreamCharConnection slave %s error in poll: %s", _slaveDevice.c_str(), strerror( errno ) );
			break;
		}

		if( pfd.revents & (POLLPRI | POLLERR | POLLHUP) )
		{
			uint32_t slaveEvents = 0;
#ifndef WIN32
			if( ioctl( _fd, EQ3LOOP_IOCGEVENTS, &slaveEvents ) )
			{
				LOG( Logger::LOG_WARNING, "UpstreamCharConnection slave %s error getting events: %s", _slaveDevice.c_str(), strerror( errno ) ); 
				break;
			}
#endif
			//LOG( Logger::LOG_INFO, "UpstreamCharConnection slave device %s events: 0x%04X", _slaveDevice.c_str(), slaveEvents ); 

			if( slaveEvents & SlaveEventClose )
			{
				LOG( Logger::LOG_DEBUG, "UpstreamCharConnection slave device %s closed", _slaveDevice.c_str() ); 
				if( !(slaveEvents & SlaveStateOpen) )
				{
					_subsystem.UpstreamDisconnect( this );
					slaveOpen = false;
				}
			}
			if( slaveEvents & SlaveEventOpen )
			{
				LOG( Logger::LOG_DEBUG, "UpstreamCharConnection slave device %s opened", _slaveDevice.c_str() );
				if( slaveEvents & SlaveStateOpen )
				{
					_subsystem.UpstreamConnect( this );
					slaveOpen = true;
				}
			}
		}

		if( slaveOpen )
		{
			if( pfd.revents & POLLIN )
			{
				int count = read( _fd, buffer + bufLength, sizeof(buffer) - bufLength );
				if( count == 0 )
				{
					LOG( Logger::LOG_DEBUG, "UpstreamCharConnection slave device %s closed after read()", _slaveDevice.c_str() ); 
					_subsystem.UpstreamDisconnect( this );
					slaveOpen = false;
				}else if( count < 0 )
				{
					LOG( Logger::LOG_WARNING, "UpstreamCharConnection slave device %s read error: %s", _slaveDevice.c_str(), strerror( errno ) ); 
					break;
				}else{
					//LOG( Logger::LOG_DEBUG, "UpstreamCharConnection slave device %s read()=%d", _slaveDevice.c_str(), count ); 
					bufLength += count;
					while( bufLength )
					{
						size_t consumed = _subsystem.ConsumeDownstreamBytes( buffer, bufLength );
						if( !consumed )
						{
							break;
						}
						bufLength -= consumed;
						if( bufLength )
						{
							memmove( buffer, buffer + consumed, bufLength );
						}
					}
				}
			}
		}
	}
	LOG( Logger::LOG_DEBUG, "UpstreamCharConnection slave device %s ThreadFunction() ended", _slaveDevice.c_str() ); 
}

