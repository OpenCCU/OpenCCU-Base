/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#include "CCU2SerialPortWrapperLinux.h"
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
// Konstruktion/Destruktion

//#define DUMP 1
#include <stdio.h>

using namespace HM2;

#ifdef DUMP
 #include <Logger.h>
 #include <HM2Utils.h>
#endif


CCU2SerialPortWrapperLinux::CCU2SerialPortWrapperLinux()
: CCU2SerialPortWrapper()
{
	fd=-1;
}
CCU2SerialPortWrapperLinux::~CCU2SerialPortWrapperLinux()
{
	Close();
}
int CCU2SerialPortWrapperLinux::SendData(const std::string& data)
{
#ifdef DUMP
	LOG(Logger::LOG_ALL, "CCU2SerialPortWrapperLinux::SendData(): Writing %s", toDebugHexStr(data).c_str());
#endif
	unsigned int count=0;
	int retval;
	tcflush(fd, TCIFLUSH);
	int i=0;
	while(count<data.size()){
		retval=write(fd, data.data()+count, data.size()-count);
		if(retval<0)return retval;
		count+=retval;
		if(i++ == 100){
			return data.size();
		}
	}
#ifdef DUMP
	LOG(Logger::LOG_ALL, "CCU2SerialPortWrapperLinux::SendData(): Wrote %u bytes.", count);
#endif	
	return count;
}


int CCU2SerialPortWrapperLinux::WaitForData(int msTime)
{
	fd_set inFd, outFd, excFd;
	FD_ZERO(&inFd);
	FD_ZERO(&outFd);
	FD_ZERO(&excFd);

	FD_SET(fd, &inFd);
	
	// Check for events
	int nEvents;
	struct timeval tv;
	tv.tv_sec = msTime/1000;
	tv.tv_usec = (msTime%1000)*1000;
	nEvents = select(fd+1, &inFd, &outFd, &excFd, &tv);
	return nEvents;
}


int CCU2SerialPortWrapperLinux::ReadData(std::string* data)
{
	int n = 0;
	do {
		n = WaitForData(1000);
	} while( n == 0);
	
	//data->clear();
	
	char* buf = new char[256];//TODO Optimization: Use class member buffer
	const unsigned int count = read(fd, buf, 256);
	if(count>0){
#ifdef DUMP
		std::string chunk(buf,count);
		LOG(Logger::LOG_ALL, "CCU2SerialPortWrapperLinux::ReadData(): Read chunk: %s", toDebugHexStr(chunk).c_str());
#endif
		data->append( buf, count );
	}
#ifdef DUMP
	LOG(Logger::LOG_ALL, "CCU2SerialPortWrapperLinux::ReadData(): Received %u bytes", count);
	LOG(Logger::LOG_ALL, "CCU2SerialPortWrapperLinux::ReadData(): Read: %s", toDebugHexStr((*data)).c_str());
#endif
	delete[] buf;
	return count;
}
bool CCU2SerialPortWrapperLinux::Open(std::string dev)
{
	struct termios newtio;
	//Open
	fd = open (dev.c_str(), O_RDWR);
	if (fd <= 0)
	{
		//printf("failure %s open\n", dev);
		return -1;
	}
	
	tcgetattr(fd, &newtio);
	cfmakeraw(&newtio);
	newtio.c_cflag &= ~(PARODD | CRTSCTS | CSIZE | CSTOPB | PARENB);
	newtio.c_cflag |= CLOCAL | CS8 | CREAD;
	cfsetospeed(&newtio, B115200);
	cfsetispeed(&newtio, B115200);
	tcsetattr(fd, TCSANOW, &newtio);
	return (fd != -1);
}
void CCU2SerialPortWrapperLinux::Close()
{
	if(fd!=-1) {
		close(fd);
		fd = -1;
	}
}

bool CCU2SerialPortWrapperLinux::IsConnected() 
{
	return (fd != -1);
}


