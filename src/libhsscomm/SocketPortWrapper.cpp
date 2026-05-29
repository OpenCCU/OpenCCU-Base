/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// SocketPortWrapper.cpp: Implementierung der Klasse SocketPortWrapper.
#include "SocketPortWrapper.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#define close(x) closesocket(x)
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include "Logger.h"

// Konstruktion/Destruktion
SocketPortWrapper::SocketPortWrapper()
{
	fd=-1;
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD( 2, 2 );
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		LOG(Logger::LOG_ERROR, "Could not initialize winsock2 library");
	}
#endif
}
SocketPortWrapper::~SocketPortWrapper()
{
	Close();
}
int SocketPortWrapper::SendData(const std::string& data)
{
	unsigned int count=0;
	int retval;
	while(count<data.size()){
		retval=send(fd, data.data()+count, data.size()-count, 0);
		if(retval<0)return retval;
		count+=retval;
	}
	return count;
}
int SocketPortWrapper::WaitForData(int msTime)
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
	if(nEvents == -1){
		LOG(Logger::LOG_ERROR, "select error");
	}
	return nEvents;
}
int SocketPortWrapper::ReadData(std::string* data)
{
	char buf[256];
	int count;
	(*data)="";
	count=recv(fd, buf, 256, 0);
	if(count==0){
		fd=-1;
	}else if(count>0){
		data->insert(data->end(), buf, buf+count);
	}
	return count;
}
int SocketPortWrapper::Open(const char* host, int port)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd==-1){
        perror("Could not open socket");
        return -1;
    }
    struct sockaddr_in addr;
    
	addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);
    memset(&(addr.sin_zero), 0, sizeof(addr.sin_zero));

    if(connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr))==-1){
        perror("Could not connect to host");
        close(fd);
        return -1;
    }

	return fd;
}
int SocketPortWrapper::Close()
{
	if(fd!=-1)close(fd);
	return 1;
}
