
#include "UDPDatagramSender.h"
#include <Constants.h>

#ifdef WIN32
#	include "winsock2.h"
#	include "ws2tcpip.h"
#else
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#       include <unistd.h>
#	define closesocket(x) close(x)
#endif


#include <sys/timeb.h>
#include <time.h>
#include <string.h>
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>

#include <NetworkInterfaceHelper.h>

using namespace LDU;

const int UDPDatagramSender::UDP_RX_BUFFER_SIZE = 2000;

UDPDatagramSender::UDPDatagramSender(const char* address, const unsigned int port, const unsigned int replyPort, const RoutingSchemeEnum& routingScheme)
: address(address)
, port(port)
, replyPort(replyPort)
, routingScheme(routingScheme)
{
}

UDPDatagramSender::~UDPDatagramSender(void)
{
}

bool UDPDatagramSender::send(const std::string& msg, std::vector<std::string>& responses, unsigned int timeout, unsigned int maxResponseCount) 
{
	pthread_t receiverThread;//thread for receiving and handling responses.
	pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
	pthread_attr_setstacksize(&threadAttr, 512*1024);

	//bool receiverThreadFinished = false;
	ReceiverThreadInfo receiverInfo;
	int sendSocket = -1;
	int receiveSocket = -1;
	bool success = false;
	int retVal = 0;
	int serverPort = 0;
	int osARASize = 0;
	unsigned int interfaceAddressesSize = 1;
	
	//int cnt = 0;
	NetworkInterfaceHelper* pNIH = new NetworkInterfaceHelper();
	std::vector<std::string> interfaceAddresses = pNIH->getIPv4Adresses();
	delete pNIH;

	//Create receiver socket-address
	struct sockaddr_in receiveSocketAddress;

	bool done = createReceiveSocket( replyPort, receiveSocket, receiveSocketAddress, routingScheme, address );
	if(!done) {
		perror("create - receive socket");
		goto cleanup;
	}

	done = bindSocket(receiveSocket, &receiveSocketAddress);
	if(!done) {
		perror("bind - receive socket");
		goto cleanup;
	}
	//Get the server port assigned by OS
	struct sockaddr_in osAssignedReceiverAddr;
	osARASize = sizeof(osAssignedReceiverAddr);
	if(getsockname(receiveSocket, (struct sockaddr*)&osAssignedReceiverAddr, (socklen_t*)&osARASize) == -1) {
		perror("getsockname");
		goto cleanup;
	}
	serverPort = (int)ntohs(osAssignedReceiverAddr.sin_port);

	//Start thread to handle responses
	responses.clear();
	receiverInfo.maxResponseCount = maxResponseCount;
	receiverInfo.pMsg = &msg;
	receiverInfo.pResponses = &responses;
	receiverInfo.receiveSocket = receiveSocket;
	receiverInfo.timeout = timeout;
	receiverInfo.pUDPDatagramSender = this;

	retVal = pthread_create(&receiverThread, NULL, receiverThreadFunction, (void*) &receiverInfo );//attributes???!!! 
	if(retVal != 0) {
		goto cleanup;
	}


	if(routingScheme != ROUTINGSCHEME_UNICAST) {
		interfaceAddressesSize = interfaceAddresses.size();
	}
	for(unsigned int ni = 0 ; ni < interfaceAddressesSize; ni++) {
			//Address of ethernet interface used to send the message.
		struct sockaddr_in ultimateHardCodedForceFeedbackAddress;

		sendSocket = -1;
		struct sockaddr_in sendSocketAddress;
		done = createSendSocket(	port,
									serverPort,
									routingScheme,
									interfaceAddresses.at(ni).c_str(),
									address,
									ultimateHardCodedForceFeedbackAddress,
									sendSocketAddress,
									sendSocket);
		if(!done) {
			perror("create - sendSocket");
			continue;
			//goto cleanup;
		}
		
		done = bindSocket(sendSocket, &ultimateHardCodedForceFeedbackAddress);
		if(!done) {
			perror("bind - sendSocket");
			if(sendSocket >= 0) {
				closesocket(sendSocket);
				sendSocket = -1;
			}
			continue;
			//goto cleanup;
		}

		//SEND MESSAGE
		int retVal = sendto(sendSocket, msg.c_str(), (int)msg.size(), 0, (sockaddr*)&sendSocketAddress, sizeof(struct sockaddr_in)); 
		if(retVal < 0) {
			perror("sendto - sendSocket");
			if(sendSocket >= 0) {
				closesocket(sendSocket);
				sendSocket = -1;
			}
			//goto cleanup;
			continue;
		}
		if(sendSocket >= 0) {
			closesocket(sendSocket);
			sendSocket = -1;
		}

	}

/* //alternative to pthread_join
	uint64_t millisElapsed = 0;
	const uint64_t startTime = time_millis();
	const uint64_t endTime = startTime + timeout;

		while( !receiverThreadFinished && millisElapsed <= endTime ) { 
		Sleep(100);//wait 100 millis
		millisElapsed = time_millis() - startTime;
	}
*/
	pthread_join(receiverThread, NULL);
	pthread_attr_destroy( &threadAttr );

	success = true;
cleanup:
	if(sendSocket >= 0) {
		closesocket(sendSocket);
	}
	if(receiveSocket >= 0) {
		closesocket(receiveSocket);
	}
	return success;

}

void UDPDatagramSender::perror(const char* func)
{
#ifdef WIN32
    int error=WSAGetLastError();
    std::cerr << func << " error: " << error << std::endl;
#else
    ::perror(func);
#endif
}

void* UDPDatagramSender::receiverThreadFunction(void* pStructReceiverThreadInfo) {
	ReceiverThreadInfo* pInfo = (ReceiverThreadInfo*)pStructReceiverThreadInfo;

	uint64_t abs_rx_timeout = 0;
	abs_rx_timeout=time_millis()+pInfo->timeout;
	int cnt = 0;
	while ( pInfo->pResponses->size() < pInfo->maxResponseCount ) {
		//get current time
        uint64_t now=time_millis();
		//check if timelimit is reached and break if so...
		if((int64_t)(now-abs_rx_timeout) >= 0) {
			break;
		}

        struct timeval wait_time;
        wait_time.tv_sec=(abs_rx_timeout-now)/1000;
        wait_time.tv_usec=((abs_rx_timeout-now)%1000)*1000;

	    fd_set inFd;//, outFd, excFd;
	    FD_ZERO(&inFd);
	   // FD_ZERO(&outFd);
	   // FD_ZERO(&excFd);
	
		int maxFd = pInfo->receiveSocket;
//Disbale compiler-warning (Level 4) C4127 caused by FD_SET macro
#ifdef WIN32
#	pragma warning(disable : 4127)
#endif
        FD_SET((unsigned int)pInfo->receiveSocket, &inFd);

        //int n_events=select(maxFd+1, &inFd, &outFd, &excFd, &wait_time);
		int n_events=select(maxFd+1, &inFd, NULL, NULL, &wait_time);
        if(n_events < 0)
        {
            pInfo->pUDPDatagramSender->perror("select");
            //goto cleanup;
			FD_CLR((unsigned int)pInfo->receiveSocket, &inFd);
			pthread_exit(NULL);
        }
        if(n_events>0) {
            char* rx_buffer=new char[UDP_RX_BUFFER_SIZE];

            //socklen_t len_r;
            //len_r = sizeof(struct sockaddr_in);
            cnt = recv(pInfo->receiveSocket, rx_buffer, UDP_RX_BUFFER_SIZE, 0);
            if (cnt < 0) {
				pInfo->pUDPDatagramSender->perror("recvfrom");
                delete[] rx_buffer;
                //goto cleanup;
				return NULL;
            }
            std::string response(rx_buffer, cnt);
            if((response.size() != pInfo->pMsg->size()) || (response.compare(*(pInfo->pMsg))!=0)){
				    pInfo->pResponses->push_back(response);
				}
			delete[] rx_buffer;
        }
		FD_CLR((unsigned int)pInfo->receiveSocket, &inFd);
    }
	pthread_exit(NULL);
	//return NULL;
}

bool UDPDatagramSender::createReceiveSocket(const unsigned int port, 
											int& receiveSocket, 
											sockaddr_in& sockaddr, 
											const RoutingSchemeEnum& routingScheme,
											const std::string& destinationAddress) 
{
	receiveSocket = 0;
	memset(&sockaddr, 0, sizeof(struct sockaddr_in));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons((unsigned short)port);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//Set sockopt re-use 
	//Create receiver socket & and set reuseaddress option.
	const int yes = 1;
	receiveSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP/*0*/);
	int retVal = setsockopt( receiveSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes) );//set reuse addres socket option
	if(retVal < 0) {
		perror("setsockopt - SO_REUSEADDR");
		if(receiveSocket != 0) {
			closesocket(receiveSocket);
		}
		return false;
	}

	//multicast membership for receiver socket
	if(routingScheme == ROUTINGSCHEME_MULTICAST) {
		struct ip_mreq imr;
		imr.imr_multiaddr.s_addr = inet_addr(destinationAddress.c_str());
		imr.imr_interface.s_addr = htonl(INADDR_ANY);
		retVal = setsockopt(receiveSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&imr, sizeof(imr));
		if(retVal < 0) {
			perror("setsockopt - IP_ADD_MEMBERSHIP (receiveSocket)");
			if(receiveSocket != 0) {
				closesocket(receiveSocket);
			} 
			return false;
		}
	}
	return true;
}


bool UDPDatagramSender::createSendSocket(const unsigned int port,
					  const unsigned int sourcePort,
					  const RoutingSchemeEnum& routingScheme, 
					  const std::string& sourceAddress,
					  const std::string& destinationAddress, 
					  sockaddr_in& sockaddrSource,
					  sockaddr_in& sockaddrDestination, 
					  int& sendSocket) 
{
	sendSocket = 0;
	memset(&sockaddrSource, 0, sizeof(struct sockaddr_in));
	memset(&sockaddrDestination, 0, sizeof(struct sockaddr_in));
	sockaddrSource.sin_family = AF_INET;
	sockaddrSource.sin_port = htons((unsigned short)sourcePort);
	sockaddrSource.sin_addr.s_addr = inet_addr( sourceAddress.c_str() );
	sockaddrDestination.sin_family = AF_INET;
	sockaddrDestination.sin_port = htons((unsigned short)port);
	
	switch(routingScheme) {
		case ROUTINGSCHEME_UNICAST:
			sockaddrDestination.sin_addr.s_addr = inet_addr( destinationAddress.c_str() );
			sockaddrSource.sin_addr.s_addr = 0;
			break;
		case ROUTINGSCHEME_MULTICAST:
			sockaddrDestination.sin_addr.s_addr = inet_addr( destinationAddress.c_str() );
			break;
		case ROUTINGSCHEME_BROADCAST:
			sockaddrDestination.sin_addr.s_addr = htonl(INADDR_BROADCAST);
			break;
	}
	//Create sender socket
	sendSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//Set reuse sockopt
	const int yes = 1;
	int retVal = setsockopt(sendSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));//Set reuse address socket option
	if(retVal < 0) {
		perror("setsockopt - SO_REUSEADDR (sendSocket)");
		if(sendSocket != 0) {
			closesocket(sendSocket);
			sendSocket = 0;
		}
		return false;
	}

	switch(routingScheme) {
		case ROUTINGSCHEME_UNICAST:
			break;
		case ROUTINGSCHEME_MULTICAST:
			if(address.compare(Constants::MULTICAST_ADDRESS) == 0) {
				//Multicast broadcast address -> set broadcast sockopt
				retVal = setsockopt(sendSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&yes, sizeof(yes));
				if(retVal < 0) {
					perror("setsockopt - SO_BROADCAST (sendSocket)");
					if(sendSocket != 0) {
						closesocket(sendSocket);
						sendSocket = 0;
					}
				}
			}
			break;
		case ROUTINGSCHEME_BROADCAST:
			retVal = setsockopt(sendSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&yes, sizeof(yes));
			if(retVal < 0) {
				perror("setsockopt - SO_BROADCAST (sendSocket)");
				if(sendSocket != 0) {
					closesocket(sendSocket);
					sendSocket = 0;
				}
			}
			break;
	}
	return true;
}

bool UDPDatagramSender::bindSocket(int sock, sockaddr_in* pSocketAddress) {
	int r = bind(sock, (sockaddr*)pSocketAddress, sizeof(struct sockaddr_in));
	if( r == 0 ) {
		return true;
	}
	else {
		if(sock != 0) {
			closesocket(sock);
			sock = 0;
		}
		return false;
	}
}
