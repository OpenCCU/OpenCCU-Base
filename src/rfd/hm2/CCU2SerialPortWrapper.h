/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2SERIALPORTWRAPPER_H_
#define _CCU2SERIALPORTWRAPPER_H_

//#include <string>
#include <CCU2PortWrapper.h>

namespace HM2 {

class CCU2SerialPortWrapperLinux;
class CCU2SerialPortWrapperWin32;

/**
* \brief 'Abstract' base class of CCU2SerialPortWrapperWin32 and CCU2SerialPortWrapperLinux. 
*/
class CCU2SerialPortWrapper : public CCU2PortWrapper
{
public:
	/** \brief Constructor.*/
	CCU2SerialPortWrapper();
	
	/** Destructor.*/
	virtual ~CCU2SerialPortWrapper();
	
	/** \brief Open serial port.
	 *  \param dev Path to serial device.
	 *  \return True if connection is established, otherwise false.
	 */
	virtual bool Open(std::string dev) = 0;
	
	/** \brief Close device.*/
	virtual void Close() = 0;
	
	/** \brief Reads data from device.
	* \param data Data from device.
	* \return Number of bytes/chars read.
	*/
	virtual int ReadData(std::string* data) = 0;
	
	/** \brief Sends data to device.
	* \param data Data to send to device.
	* \return Number of bytes/characters written to device.
	*/
	virtual int SendData(const std::string& data) = 0;
	
	//! Implementierung von PortWrapper::WaitForData()
	//int WaitForData(int msTime);

	/** \brief Returns current connection-state.
	* \return True, if connected, otherwise false.
	*/
	virtual bool IsConnected() = 0;

	virtual void Disconnect();

	/** \brief Creates the CCU2SerialPortWrapper for Linux or Windows.
	* \return CCU2SerialPortWrapperLinux/Win32
	*/
	static CCU2SerialPortWrapper* createCCU2SerialPortWrapper();

};

}

#endif // _UNIXSERIALPORTWRAPPER_H_
