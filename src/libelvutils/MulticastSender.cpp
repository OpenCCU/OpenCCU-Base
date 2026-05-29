/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "MulticastSender.h"

#ifdef WIN32
#include "winsock2.h"
#include "ws2tcpip.h"
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define closesocket(x) close(x)
#endif
#include <sys/timeb.h>
#include <time.h>
#include <string.h>
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>

MulticastSender::MulticastSender(const char* mcast_group, unsigned int port, bool broadcast /*=false*/)
{
    group=mcast_group;
    this->port=port;
    use_broadcast=broadcast;
}

MulticastSender::~MulticastSender(void)
{
}

void MulticastSender::perror(const char* func)
{
#ifdef WIN32
    int error=WSAGetLastError();
    std::cerr << func << " error: " << error << std::endl;
#else
    ::perror(func);
#endif
}

bool MulticastSender::SendUDPMessage(const std::string& msg, std::vector<std::string>* responses, unsigned int timeout, unsigned int max_response_count)
{
	int mcast_send_socket=-1;
    int mcast_receive_socket=-1;

    bool retval=false;
	int cnt;
	struct sockaddr_in sa_mcast_server;
	struct sockaddr_in sa_mcast_group;
	struct sockaddr_in sa_broadcast;

    uint64_t abs_rx_timeout;

    memset(&sa_mcast_server, 0, sizeof(struct sockaddr_in));
    memset(&sa_mcast_group, 0, sizeof(struct sockaddr_in));
    memset(&sa_broadcast, 0, sizeof(struct sockaddr_in));
        
    sa_mcast_server.sin_family = AF_INET;
    sa_mcast_server.sin_port = htons(port);
    sa_mcast_server.sin_addr.s_addr = htonl(INADDR_ANY);

    sa_mcast_group.sin_family = AF_INET;
    sa_mcast_group.sin_port = htons(port);
    sa_mcast_group.sin_addr.s_addr = inet_addr(group.c_str());
    
    sa_broadcast.sin_family = AF_INET;
    sa_broadcast.sin_port = htons(port);
    sa_broadcast.sin_addr.s_addr = INADDR_BROADCAST;

    mcast_receive_socket = socket(PF_INET, SOCK_DGRAM, 0);

    int yes=1;
    setsockopt(mcast_receive_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));

    if (bind(mcast_receive_socket, (struct sockaddr *)&sa_mcast_server, sizeof(struct sockaddr_in)) < 0)
	{
        perror("bind");
        goto cleanup;
    }

    if( use_broadcast || sa_mcast_group.sin_addr.s_addr == INADDR_BROADCAST ){
	    if (setsockopt(mcast_receive_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&yes, sizeof(yes)) < 0)
	    {
            perror("setsockopt - SO_BROADCAST");
            goto cleanup;
        }
    }

    if(  sa_mcast_group.sin_addr.s_addr != INADDR_BROADCAST ){
        struct ip_mreq imr;
        imr.imr_multiaddr.s_addr = inet_addr(group.c_str());
        imr.imr_interface.s_addr = htonl(INADDR_ANY);
	
	    if (setsockopt(mcast_receive_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *) &imr, sizeof(struct ip_mreq)) < 0)
	    {
            perror("setsockopt - IP_ADD_MEMBERSHIP");
            goto cleanup;
        }
    }

	mcast_send_socket = socket(PF_INET, SOCK_DGRAM, 0);
    setsockopt(mcast_send_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));

    if( use_broadcast )setsockopt(mcast_send_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&yes, sizeof(yes));

    if (bind(mcast_send_socket, (struct sockaddr *)&sa_mcast_server, sizeof(struct sockaddr_in)) < 0)
	{
        perror("bind");
        goto cleanup;
    }

    {
        struct sockaddr* so_addr=(struct sockaddr *)(use_broadcast?&sa_broadcast:&sa_mcast_group);
        int n = sendto(mcast_send_socket, msg.c_str(), msg.size(), 0, so_addr, sizeof(struct sockaddr_in) );
        if (n < 0) {
            perror("sendto");
            goto cleanup;
        }
    }
    
    responses->clear();

    abs_rx_timeout=time_millis()+timeout;

    while ( responses->size() < max_response_count) {
        uint64_t now=time_millis();
        if(static_cast<int64_t>(now-abs_rx_timeout) >= 0)break;

        struct timeval wait_time;
        wait_time.tv_sec=(abs_rx_timeout-now)/1000;
        wait_time.tv_usec=((abs_rx_timeout-now)%1000)*1000;

	    fd_set inFd, outFd, excFd;
	    FD_ZERO(&inFd);
	    FD_ZERO(&outFd);
	    FD_ZERO(&excFd);
	
	    int maxFd = mcast_receive_socket;
        FD_SET(mcast_receive_socket, &inFd);

        int n_events=select(maxFd+1, &inFd, &outFd, &excFd, &wait_time);
        if(n_events < 0)
        {
            perror("select");
            goto cleanup;
        }
        if(n_events>0){
            char* rx_buffer=new char[UDP_RX_BUFFER_SIZE];

    		//socklen_t len_r;
	    	//len_r = sizeof(struct sockaddr_in);
            cnt = recv(mcast_receive_socket, rx_buffer, UDP_RX_BUFFER_SIZE, 0);
            if (cnt < 0) {
                perror("recvfrom");
                delete[] rx_buffer;
                goto cleanup;
            }
            std::string response(rx_buffer, cnt);
            if((response.size() != msg.size()) || (response.compare(msg)!=0)){
                responses->push_back(response);
            }
            delete[] rx_buffer;
        }
    }
    retval=true;
cleanup:
    if(mcast_send_socket>=0)closesocket(mcast_send_socket);
    if(mcast_receive_socket>=0)closesocket(mcast_receive_socket);
    return retval;
}
