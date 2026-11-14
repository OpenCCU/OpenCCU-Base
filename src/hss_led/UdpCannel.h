/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
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
