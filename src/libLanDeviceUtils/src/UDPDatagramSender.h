/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_UDPDATAGRAMSENDER_H_
#define _LIBLANDEVICEUTILS_UDPDATAGRAMSENDER_H_

#include <vector>
#include <string>
#include <LanDeviceUtilsTypes.h>

struct sockaddr_in;

namespace LDU {

class UDPDatagramSender
{
public:
	/**\brief Constructor.
	* \param address Destination address. In case of Multicast this is the multicast group address. In case of broadcast, address will be ignored.
	* \param port Port.
	* \param routingScheme RoutingSchemeEnum <-> ROUTINGSCHEME_UNICAST,	ROUTINGSCHEME_MULTICAST, ROUTINGSCHEME_BROADCAST
	*/
	UDPDatagramSender(const char* address, const unsigned int port, const unsigned int replyPort, const RoutingSchemeEnum& routingScheme);
	virtual ~UDPDatagramSender(void);

	bool send(const std::string& msg, std::vector<std::string>& responses, unsigned int timeout, unsigned int maxResponseCount);

private:

	struct ReceiverThreadInfo {
		unsigned int timeout; 
		unsigned int maxResponseCount;
		std::vector<std::string>* pResponses;
		int receiveSocket;
		const std::string* pMsg;
		UDPDatagramSender* pUDPDatagramSender;
	//	ReceiverThreadInfo();
	//	ReceiverThreadInfo& operator=(const ReceiverThreadInfo& other);
	};

	std::string address;
	unsigned int port;
	unsigned int replyPort;
	RoutingSchemeEnum routingScheme;
	static const int UDP_RX_BUFFER_SIZE;

	void perror(const char* func);
	bool createReceiveSocket(const unsigned int port, int& receiveSocket, sockaddr_in& sockaddr, const RoutingSchemeEnum& routingScheme, const std::string& destinationAddress);
	bool createSendSocket(const unsigned int port, const unsigned int sourcePort, const RoutingSchemeEnum& routingScheme, const std::string& sourceAddress,const std::string& destinationAddress, sockaddr_in& sockaddrSource, sockaddr_in& sockaddrDestination, int& sendSocket);
	bool bindSocket(int sock, sockaddr_in* pSocketAddress);
	static void* receiverThreadFunction(void* pStructReceiverThreadInfo);
};

}//namespace

#endif