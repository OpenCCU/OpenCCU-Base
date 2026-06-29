/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <LanConnection.h>
#include <Logger.h>
#include <LanConnectionListener.h>
using namespace ulc;


#ifdef WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	pragma comment(lib, "ws2_32.lib")
#   define SOCKET_SEND_FLAGS 0
#   include <utils.h>
#else
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <poll.h>
#   include <unistd.h>
#	define closesocket(x) close(x)
#   define SOCKET_SEND_FLAGS MSG_NOSIGNAL
#endif
#include <stdio.h>
#include <sys/types.h>
#include <iostream>
#include <Logger.h>
#include <sstream>
#include <iostream>
#include <errno.h>
#include <limits.h>
#include <string.h>

//#define DUMP 1

#ifdef DUMP
#  include <CommonConversion.h>
#endif


#define RECEIVE_BUFFER_SIZE 4096

LanConnection::LanConnection(const std::string& hostIP, const int port)
:sock(-1)
,hostIP(hostIP)
,port(port)
,connectionClosing(false)
,inSelect(false)
{
	#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2,2);
	int error = WSAStartup( wVersionRequested, &wsaData);
	if(error != 0) {
		exit(-1);
	}
	#endif
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init(&mutexAttr);
	pthread_mutex_init(&sockMutex, &mutexAttr);
	pthread_mutexattr_destroy(&mutexAttr);
}

//-----------------------------------------------------------------------------------------------

LanConnection::~LanConnection()
{
	lanConnectionListener.clear(); //delete them not allowed here
	disconnect();
#ifdef WIN32
	WSACleanup();
#endif
	/*if(receiveBuffer != NULL) {
		delete[] receiveBuffer;
		receiveBuffer = NULL;
	}*/
	pthread_mutex_destroy(&sockMutex);
}

//-----------------------------------------------------------------------------------------------

bool LanConnection::connect()
{
	LOG(Logger::LOG_ALL, "LanConnection::connect");
	//Create socket address
	sockaddr_in socketAddress;
	memset(&socketAddress, 0, sizeof(struct sockaddr_in));
	socketAddress.sin_addr.s_addr = inet_addr(hostIP.c_str());
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(this->port);
	//Create socket
	pthread_mutex_lock(&sockMutex);
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1) {
		pthread_mutex_unlock(&sockMutex);
		LOG(Logger::LOG_ERROR, "LanConnection::connect(): Cannot create socket.");
		return false;
	}
	int retIVal = ::connect(sock, (struct sockaddr*)&socketAddress, sizeof(struct sockaddr));//Visual Studio does not compile without ::
	//LOG(Logger::LOG_ALL, "LanConnection::connect(): Opened socket %d", sock);
	if(retIVal != 0) {
		LOG(Logger::LOG_ALL, "LanConnection::connect - failed closing socket");
		closesocket(sock);
		sock = -1;
		pthread_mutex_unlock(&sockMutex);
		return false;
	}
	pthread_mutex_unlock(&sockMutex);
	LOG(Logger::LOG_ALL, "LanConnection::connect done");
	return true;
}

//-----------------------------------------------------------------------------------------------

void LanConnection::disconnect()
{
	LOG(Logger::LOG_ALL, "LanConnection::disconnect");
	pthread_mutex_lock(&sockMutex);
	connectionClosing = true;
	if(sock != -1) {
		int sockToClose = sock;
		sock = -1;
		LOG(Logger::LOG_ALL, "Closing socket %d", sockToClose);
		int closed = closesocket(sockToClose);
		if(closed != 0) {
			LOG(Logger::LOG_ERROR, "Error closing socket %d",sockToClose);
		}
	}
	pthread_mutex_unlock(&sockMutex);
	int i = 0;
	pthread_mutex_lock(&sockMutex);
	if(inSelect) {
		pthread_mutex_unlock(&sockMutex);
		while(connectionClosing) {
			usleep(1000000);
			if(i >= 10) {
				LOG(Logger::LOG_DEBUG, "Wait for disconnect timed out");
			}
			i++;
		}
	}
	else {
		pthread_mutex_unlock(&sockMutex);
	}
	connectionClosing = false;
	LOG(Logger::LOG_ALL, "LanConnection::disconnect done");
}

//-----------------------------------------------------------------------------------------------

bool LanConnection::isConnected() const
{
	return (sock != -1);
}

//-----------------------------------------------------------------------------------------------

bool LanConnection::send(const std::string &data)
{
	unsigned int sent = ::send(sock, data.c_str(), data.size(), SOCKET_SEND_FLAGS);//VS doesn't compile without :: because this method's name is send too
	//FIXME +1 for \n character ??
	#ifdef DUMP
		LOG(Logger::LOG_ALL, "LanConnection::send(): sock %d: %s",sock, toHexString(data).c_str());
	#endif
	return (sent == data.size());
}

//-----------------------------------------------------------------------------------------------

bool LanConnection::receive(std::string& bytesRead)
{
	memset(receiveBuffer, 0, RECEIVE_BUFFER_SIZE);
	//LOG(Logger::LOG_ALL, "LanConnection::receive(): sock %d", sock);
	int read = recv(sock, receiveBuffer, RECEIVE_BUFFER_SIZE, 0);
	if(read <= 0) {
		if(read == -1) {
			perror("recv error");
			//disconnect();
		}
		else if(read == 0) {//remote host shut down connection
			LOG(Logger::LOG_INFO, "LanConnection::receive(): sock %d: Remote host closed connection.", sock);
			//disconnect();
		}
		return false;
	}
	else {
		bytesRead.assign((const char*)receiveBuffer, (unsigned int)read);
#ifdef DUMP
		LOG(Logger::LOG_ALL, "LanConnection::receive(): Received bytes: %d\n%s", read,toHexString(std::string(receiveBuffer, read)).c_str());
#endif
		return true;
	}

}

bool LanConnection::receiveNonBlocking(std::string& bytesRead)
{

	if(isConnected()) {
	//LOG(Logger::LOG_ALL, "LanConnection::receiveNonBlocking() on sock %d", sock);
		memset(receiveBuffer, 0, RECEIVE_BUFFER_SIZE);
		
#ifndef WIN32		
		int read = recv(sock, receiveBuffer, RECEIVE_BUFFER_SIZE, MSG_DONTWAIT);
#else
		u_long pendingBytes = 0;
		ioctlsocket(sock, FIONREAD, &pendingBytes);//check if there is data to read
		int read = 0;
		if(pendingBytes > 0) {
			read = recv(sock, receiveBuffer, RECEIVE_BUFFER_SIZE, 0);
		}
#endif
		if(read <= 0) {
			LOG(Logger::LOG_ALL, "LanConnection::receiveNonBlocking(): Read error");
			return false;
		}
		else {
			bytesRead.assign((const char*)receiveBuffer, (unsigned int)read); //doesn't work !
#ifdef DUMP
			LOG(Logger::LOG_ALL, "LanConnection::receive(): Received bytes: %d\n%s", read,receiveBuffer);
#endif
			return true;
		}
	}
	else {
		return false;
	}
}

//-----------------------------------------------------------------------------------------------

int LanConnection::waitForData(unsigned int msTimeout)
{
	int currentSock = -1;
	pthread_mutex_lock(&sockMutex);
	inSelect = true;
	if(connectionClosing) {
		connectionClosing = false;
		pthread_mutex_unlock(&sockMutex);
		return -1;
	}
	currentSock = sock;
	if(currentSock < 0) {
		inSelect = false;
		pthread_mutex_unlock(&sockMutex);
		return -1;
	}
	pthread_mutex_unlock(&sockMutex);
	//LOG(Logger::LOG_ALL, "LanConnection::waitForData(%d): sock %d", msTimeout, currentSock);
	int nEvents;
#ifdef WIN32
	fd_set inFd, outFd, excFd;
	FD_ZERO(&inFd);
	FD_ZERO(&outFd);
	FD_ZERO(&excFd);

	FD_SET(currentSock, &inFd);

	struct timeval tv;
	tv.tv_sec = msTimeout/1000;
	tv.tv_usec = (msTimeout%1000)*1000;
	nEvents = select(currentSock+1, &inFd, &outFd, &excFd, &tv);
#else
	pollfd pfd;
	pfd.fd = currentSock;
	pfd.events = POLLIN | POLLPRI | POLLERR | POLLHUP;
	pfd.revents = 0;
	int pollTimeout = (msTimeout > (unsigned int)INT_MAX) ? INT_MAX : (int)msTimeout;
	do {
		nEvents = poll(&pfd, 1, pollTimeout);
	} while(nEvents < 0 && errno == EINTR);
#endif
	pthread_mutex_lock(&sockMutex);
	if(connectionClosing) {
		inSelect = false;
		connectionClosing = false;
		pthread_mutex_unlock(&sockMutex);
		return -1;
	}
	inSelect = false;
	pthread_mutex_unlock(&sockMutex);
	return nEvents;
}

//-----------------------------------------------------------------------------------------------

void LanConnection::perror(const char* func) const
{
#ifdef WIN32
    int error=WSAGetLastError();
	std::stringstream ss;
    ss << func << " error: " << error << std::endl;
	std::string s;
	ss >> s;
	LOG(Logger::LOG_ERROR, "%s", s.c_str());
#else
    ::perror(func);
    LOG(Logger::LOG_ERROR, "LanConnection::perror(): %s", func);
#endif
}
//-----------------------------------------------------------------------------------------------

void LanConnection::addLanConnectionListener(LanConnectionListener* listener)
{
//LOG(Logger::LOG_ERROR, " LanConnection::addLanConnectionListener()");
	if(listener == NULL) {
		return;
	}
	for(unsigned int i = 0; i < lanConnectionListener.size(); i++) {
		if(lanConnectionListener.at(i) == listener) {
			return;
		}
	}
//LOG(Logger::LOG_ERROR, " LanConnection::addLanConnectionListener(): Adding listener");
	lanConnectionListener.push_back(listener);
}

//-----------------------------------------------------------------------------------------------

void LanConnection::removeLanConnectionListener(LanConnectionListener* listener)
{
	for(unsigned int i = 0; i < lanConnectionListener.size(); i++) {
		if(lanConnectionListener.at(i) == listener) {
			lanConnectionListener.erase(lanConnectionListener.begin()+i);
//			LOG(Logger::LOG_ERROR, " LanConnection::addLanConnectionListener(): removing listener");
			break;
		}
	}
}

//-----------------------------------------------------------------------------------------------

void LanConnection::notifyOnDisconnect()
{
	for(unsigned int i = 0; i < lanConnectionListener.size(); i++) {
		lanConnectionListener.at(i)->onDisconnect();
	}
}

//-----------------------------------------------------------------------------------------------
