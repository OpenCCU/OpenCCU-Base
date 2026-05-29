/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <winsock2.h>
#include <stdio.h>
#include "dllexport.h"

DWORD WINAPI pipe_accept_thread_function(LPVOID arg)
{
	// Wait for a client
	SOCKET listeningSocket=(SOCKET)arg;
	SOCKET s;

	int nret = listen(listeningSocket, 10);

	if (nret == SOCKET_ERROR) {
		return INVALID_SOCKET;
	}
	s = accept(listeningSocket, NULL, NULL);

	return s;
}

int DLLEXPORT win_pipe(int modus[2])
{
	int nret;

	SOCKET listeningSocket;

	listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listeningSocket == INVALID_SOCKET) {
		return -1;
	}

	int port;
	for(port=27654;port<28000;port++){
		SOCKADDR_IN serverInfo;

		serverInfo.sin_family = AF_INET;
		serverInfo.sin_addr.s_addr = INADDR_ANY;
		serverInfo.sin_port = htons(port);

		// Bind the socket to our local server address
		nret = bind(listeningSocket, (LPSOCKADDR)&serverInfo, sizeof(struct sockaddr));
		if(nret != SOCKET_ERROR)break;
	}

	if (nret == SOCKET_ERROR) {
		return -1;
	}

	DWORD listeningThreadID;
	HANDLE listeningThread=CreateThread(NULL, 0, pipe_accept_thread_function, (void*)listeningSocket, 0, &listeningThreadID);

	char hostname[256];
	hostname[0]=0;
	gethostname(hostname, sizeof(hostname));

	LPHOSTENT hostEntry;
	hostEntry = gethostbyname(hostname);

	if (!hostEntry) {
		return -1;
	}

	// Create the socket
	SOCKET clientSocket;
	clientSocket = socket(AF_INET, SOCK_STREAM,	IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		return -1;
	}

	// Fill a SOCKADDR_IN struct with address information
	SOCKADDR_IN serverInfo;

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
	serverInfo.sin_port = htons(port);

	// Connect to the server
	for(int i=0;i<20;i++){
		nret = connect(clientSocket,(LPSOCKADDR)&serverInfo,sizeof(struct sockaddr));
		if(nret != SOCKET_ERROR)break;
		Sleep(50);
	}

	if (nret == SOCKET_ERROR) {
		return -1;
	}


	if(WaitForSingleObject(listeningThread, INFINITE)==WAIT_OBJECT_0){
		DWORD exitCode;
		if(GetExitCodeThread(listeningThread, &exitCode)){
			if(exitCode==INVALID_SOCKET)return -1;
			modus[0]=exitCode;
			modus[1]=clientSocket;
			return 0;
		}
	}
	return -1;

}
