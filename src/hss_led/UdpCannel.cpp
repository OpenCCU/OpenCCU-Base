/*
 * UdpCannel.cpp
 *
 *  Created on: 17.01.2013
 */

#include <Logger.h>
#include "UdpCannel.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


UdpCannel::UdpCannel():sockfd(0) {
	// TODO Automatisch generierter Konstruktorstub

}

UdpCannel::~UdpCannel() {
	// TODO !CodeTemplates.destructorstub.tododesc!
	CloseSocket();
}

bool UdpCannel::RefreshConnection() {
	if (sockfd > 0)
		CloseSocket();

	OpenSocket();
	int b = BindServer();

	return b >= 0;
}

int UdpCannel::OpenSocket() {
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		LOG(Logger::LOG_ERROR, "UDP_CHANNEL: failed to create UDP-Socket.");
	}

	return sockfd;
}

int UdpCannel::BindServer() {
	int ret;
	struct sockaddr_in servaddr;

	memset(&servaddr, 0, sizeof(servaddr)); //setzt servaddr-Bytes auf 0
	servaddr.sin_family = AF_INET; //IPv4
	//	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Der Kernel soll nur die eigene IP wählen!
	servaddr.sin_port = htons(ERR_PORT); //big oder little Endian; Prozess gibt Port an!

	if ((ret = bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))
			== -1) {
		LOG(Logger::LOG_ERROR, "UDP_CHANNEL: can't bind Port. err: %i", ret);
	}

	return ret;
}

ssize_t UdpCannel::ReceiveMessage(std::string& recv_buf) {
	ssize_t nread;
	char ptr[BUFFER_SIZE];
	struct timeval waitTimeout;
	waitTimeout.tv_sec = 5;
	waitTimeout.tv_usec = 0;
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(sockfd,&readSet);
	int ret = select(sockfd+1,&readSet,NULL,NULL,&waitTimeout);
	if(ret < 1)
	{
		return 0;
	}
	if ((nread = read(sockfd, ptr, BUFFER_SIZE - 1)) < 0) {
		recv_buf = "";

	}
	else
	{
	ptr[nread] = '\0';
	recv_buf = ptr;
	}
	return nread;
}

void UdpCannel::CloseSocket() {
	if (sockfd > 0)
		close(sockfd);
	sockfd = 0;
}
