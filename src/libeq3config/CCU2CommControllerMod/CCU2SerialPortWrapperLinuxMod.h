/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// UnixSerialPortWrapper.h: Schnittstelle f�r die Klasse UnixSerialPortWrapper.
#ifndef _CCU2SERIALPORTWRAPPERLINUXMOD_H_
#define _CCU2SERIALPORTWRAPPERLINUXMOD_H_

//#include "dllexport.h"
#include "CCU2SerialPortWrapperMod.h"

#include <string>

namespace HM2Mod {

//! Implementiert PortWrapper zum Zugriff auf serielle Ger�te unter Unix
//class DLLEXPORT UnixSerialPortWrapper : public PortWrapper  
class CCU2SerialPortWrapperLinuxMod : public CCU2SerialPortWrapperMod
{
public:
	//! Konstruktor
	CCU2SerialPortWrapperLinuxMod();
	//! Destruktor
	virtual ~CCU2SerialPortWrapperLinuxMod();
	//! �ffnen des Ports
	/*!
	 *  \param dev Pfad zum zu �ffnenden Device
	 *  \return Dateideskriptor des ge�ffneten Devices, -1 im Fehlerfall
	 */
	bool Open(std::string dev);
	//! Schlie�en des Devices
	void Close();
	//! Implementierung von PortWrapper::ReadData()
	int ReadData(std::string* data);
	//! Implementierung von PortWrapper::SendData()
	int SendData(const std::string& data);


	/** \brief Returns current connection-state.
	* \return True, if connected, otherwise false.
	*/
	bool IsConnected();

protected:
	//! Dateideskriptor
	int fd;
	
	//WaitForData()
	int WaitForData(int msTime);
};

}

#endif 
