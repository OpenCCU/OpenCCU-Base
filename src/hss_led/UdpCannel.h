/*
 * UdpCannel.h
 *
 *  Created on: 17.01.2013
 */

#ifndef UDPCANNEL_H_
#define UDPCANNEL_H_
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "defines.h"



class UdpCannel {
public:
	UdpCannel();
	virtual ~UdpCannel();
	bool RefreshConnection();
	int OpenSocket();
	int BindServer();
	void CloseSocket();

	ssize_t ReceiveMessage(std::string &recv_buf);
protected:
	int sockfd;
};

#endif /* UDPCANNEL_H_ */
