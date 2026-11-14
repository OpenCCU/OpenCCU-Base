/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "NetworkInterfaceHelper.h"

#ifdef WIN32
#	include <windows.h>
#	include <Iphlpapi.h>
#else
//#	include <stdio.h>      
#	include <sys/types.h>
#	include <ifaddrs.h>
#	include <netinet/in.h> 
//#	include <string.h> 
#	include <arpa/inet.h>
#endif

#include <InternalUtilities.h>

NetworkInterfaceHelper::NetworkInterfaceHelper(void)
{
}

NetworkInterfaceHelper::~NetworkInterfaceHelper(void)
{
}

//----------------------------------------------------------------------------------------------------------------

#ifdef WIN32
std::vector<std::string> NetworkInterfaceHelper::getIPv4Adresses() 
{
	std::vector<std::string> addresses;
	MIB_IPADDRTABLE* pIpAddressTable = NULL;
	DWORD dwSize = 0;
	DWORD dwRetVal;
	pIpAddressTable = (MIB_IPADDRTABLE*) malloc( sizeof(MIB_IPADDRTABLE) );
	
	//As described in msdn: Make a first call of GetIpAddrTable to obtain size needed into dwSize variable
	if (GetIpAddrTable(pIpAddressTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
		free( pIpAddressTable );//free old insufficient memory 
		pIpAddressTable = (MIB_IPADDRTABLE *) malloc ( dwSize );//get fresh sufficient memory
	
		//Second call to retrieve information
		dwRetVal = GetIpAddrTable( pIpAddressTable, &dwSize, 0 );
		if ( dwRetVal == NO_ERROR ) { 
			for(DWORD i = 0; i < pIpAddressTable->dwNumEntries; i++) {
				DWORD addr = pIpAddressTable->table[i].dwAddr;
				unsigned char* pChar = (unsigned char*) &addr;
				unsigned int a = (unsigned int) pChar[0];
				unsigned int b = (unsigned int) pChar[1];
				unsigned int c = (unsigned int) pChar[2];
				unsigned int d = (unsigned int) pChar[3];
				char* buffer = new char[20];
				snprintf(buffer, 20, "%u.%u.%u.%u", a, b, c, d);
				addresses.push_back(std::string(buffer));
				delete[] buffer;
				buffer = NULL;
			}
		}
	}
	
	//Free memory
	if(pIpAddressTable != NULL) {
		free(pIpAddressTable);
	}
	
	return addresses;
}

#else

std::vector<std::string> NetworkInterfaceHelper::getIPv4Adresses() 
{
	std::vector<std::string> addresses;
	struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr != NULL && ifa->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			std::string str(addressBuffer);
			addresses.push_back( str );
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } /*else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } */
    }
	if (ifAddrStruct!=NULL) { 
		freeifaddrs(ifAddrStruct);
	}
    return addresses;
}


#endif

//----------------------------------------------------------------------------------------------------------------
