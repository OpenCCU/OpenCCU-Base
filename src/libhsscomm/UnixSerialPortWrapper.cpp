/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// UnixSerialPortWrapper.cpp: Implementierung der Klasse UnixSerialPortWrapper.
#ifndef WIN32
#include "UnixSerialPortWrapper.h"
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
// Konstruktion/Destruktion

//#define DUMP

UnixSerialPortWrapper::UnixSerialPortWrapper()
{
	fd=-1;
}
UnixSerialPortWrapper::~UnixSerialPortWrapper()
{
	Close();
}
int UnixSerialPortWrapper::SendData(const std::string& data)
{
#ifdef DUMP
    printf("send:\n");
    for(int i=0;i<data.size();i++){
        printf("%02X ", (int)data[i]);
    }
    printf("\n");
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
	return count;
}
int UnixSerialPortWrapper::WaitForData(int msTime)
{
	// poll() instead of select(): an fd_set only holds descriptors below FD_SETSIZE (1024)
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	return poll(&pfd, 1, msTime);
}
int UnixSerialPortWrapper::ReadData(std::string* data)
{
	unsigned char buf[256];
	int count;
	//data->clear();
	count=read(fd, buf, 256);
	if(count>0){
		data->insert(data->end(), buf, buf+count);
	}
#ifdef DUMP
    printf("received:\n");
    for(int i=0;i<data->size();i++){
        printf("%02X ", (int)(*data)[i]);
    }
    printf("\n");
#endif
	return count;
}
int UnixSerialPortWrapper::Open(std::string dev)
{
	fd = open(dev.c_str(), O_RDWR | O_NOCTTY );
	return fd;
}
int UnixSerialPortWrapper::Close()
{
	if(fd!=-1)close(fd);
	return 1;
}
#endif
