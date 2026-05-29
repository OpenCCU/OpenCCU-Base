/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// CCU2SerialPortWrapper.cpp: Implementierung der Klasse CCU2SerialPortWrapper.

#include "CCU2SerialPortWrapperWin32Mod.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <pthread.h>

#include <Windows.h>

/*
#include <Logger.h>
#include <HM2Utils.h>
*/

using namespace HM2;

CCU2SerialPortWrapperWin32Mod::CCU2SerialPortWrapperWin32Mod()
: CCU2SerialPortWrapperMod()
, serialPortHandle(NULL)
{
}
CCU2SerialPortWrapperWin32Mod::~CCU2SerialPortWrapperWin32Mod()
{
	Close();//also deletes the pointer. open creates it.
}
int CCU2SerialPortWrapperWin32Mod::SendData(const std::string& data)
{
	/*
#ifdef DUMP
    printf("send:\n");
    for(int i=0;i<data.size();i++){
        printf("%02X ", (int)data[i]);
    }
    printf("\n");
#endif
	unsigned int count=0;
	int retval;
	tcflush(fd, TCIFLUSH);
	int i=0;
	while(count<data.size()){
		retval=write(fd, data.data()+count, data.size()-count);
		if(retval<0)return retval;
		count+=retval;
		if(i++ == 100){
			return data.size();
		}
	}
	return count;
	*/
    
	if(serialPortHandle == NULL) {
		return 0;
	}

	//write data
	DWORD dwWritten = 0;
	WriteFile(serialPortHandle, data.c_str(), data.size(), &dwWritten, NULL);
	FlushFileBuffers(serialPortHandle);
	return (int)dwWritten;
}

/*
int CCU2SerialPortWrapperWin32::WaitForData(int msTime)
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
	return nEvents;
	
}
*/

int CCU2SerialPortWrapperWin32Mod::ReadData(std::string* data)
{
	if(serialPortHandle == NULL) {
		return 0;
	}
	char* buffer = new char[256];

	DWORD read = 0;
	BOOL done = ReadFile(serialPortHandle, buffer, (DWORD)256, &read, NULL);//synchronous read, for asynchronous use OVERLAPPED structure as last parameter instead of NULL
	if(!done) {
		//DWORD error = GetLastError();
		return 0;
	}
	data->append(buffer, (unsigned int)read);
	//LOG(Logger::LOG_DEBUG, "CCU2SerialPortWrapperWin32::ReadData(): Received:\n%s", toHexStr(data->c_str()) );
	delete[] buffer;
	return (int)read;
}


bool CCU2SerialPortWrapperWin32Mod::Open(std::string dev)
{
	if(serialPortHandle != NULL) {
		Close();
	}
	//Open
	serialPortHandle = CreateFile( dev.c_str(),
									GENERIC_READ | GENERIC_WRITE,
									0,//FILE_SHARE_READ | FILE_SHARE_WRITE,
									0,
									OPEN_EXISTING,
									0,//FILE_FLAG_OVERLAPPED,
									0);
	if (serialPortHandle == INVALID_HANDLE_VALUE) {
		return false;
	}
	//Configure
	DCB dcb;
	dcb.DCBlength = sizeof(dcb);
	FillMemory(&dcb, sizeof(dcb), 0);
	BuildCommDCB( "115200, n, 8, 1", &dcb);//baud 115200, parity off, 8 bits, 1 stopbit
/*	if (!GetCommState(serialPortHandle, &dcb)) {    // get current DCB
      return FALSE;
	}
*/
/*	dcb.BaudRate = CBR_115200;//baudrate 115200
	dcb.fParity = FALSE;//Parity check off
	dcb.fDtrControl = DTR_CONTROL_DISABLE;//flow control off
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.fDsrSensitivity = FALSE;//matches linux CLOCAL???
	//FIXME is there an equivalent to CREAD????
*/
	//dcb.DCBlength = sizeof(dcb);
	dcb.fDtrControl = 0;
	dcb.fRtsControl = 0;
	
	if(!SetCommState(serialPortHandle, &dcb)) {
		return false;
	}

	if(!SetupComm(serialPortHandle, 1024, 1024)) {
		return false;
	}
	
	
    
	COMMTIMEOUTS cmt;
	GetCommTimeouts(serialPortHandle, &cmt);
	
	cmt.ReadTotalTimeoutMultiplier = 0;
	cmt.ReadTotalTimeoutConstant = 0;
	cmt.ReadIntervalTimeout = MAXDWORD;
	cmt.WriteTotalTimeoutConstant = 12000/115200+1;
	cmt.WriteTotalTimeoutMultiplier = 12000/115200+1;
	
	if(!SetCommTimeouts(serialPortHandle, &cmt)) {
		return false;
	}
	return true;
}

void CCU2SerialPortWrapperWin32Mod::Close()
{
	if(serialPortHandle != NULL) {
		CloseHandle(serialPortHandle);
		serialPortHandle = NULL;
	}
}

bool CCU2SerialPortWrapperWin32Mod::IsConnected() {
	return (serialPortHandle != NULL);
}

