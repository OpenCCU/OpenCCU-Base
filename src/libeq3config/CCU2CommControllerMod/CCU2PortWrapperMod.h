/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2PORTWRAPPERMOD_H_
#define _CCU2PORTWRAPPERMOD_H_

#include <string>

namespace HM2Mod {

/** \brief Abstract base class for CCU2SerialPortWrapper and CCU2LGWPortWrapper */
class CCU2PortWrapperMod {

public:
	enum PortWrapperSubtype {
		SERIAL,
		LGW
	};

	/** \brief Constructor.*/
	CCU2PortWrapperMod();

	/**\brief Destructor.*/
	virtual ~CCU2PortWrapperMod();

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

	virtual void Disconnect() = 0;

};

}//namespace

#endif
