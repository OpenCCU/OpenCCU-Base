/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// UnixSerialPortWrapper.h: Schnittstelle für die Klasse UnixSerialPortWrapper.
#ifndef _UNIXSERIALPORTWRAPPER_H_
#define _UNIXSERIALPORTWRAPPER_H_

#include "dllexport.h"
#include "PortWrapper.h"
#include <string>
#include "typedefs.h"
//! Implementiert PortWrapper zum Zugriff auf serielle Geräte unter Unix
class DLLEXPORT UnixSerialPortWrapper : public PortWrapper  
{
public:
	//! Konstruktor
	UnixSerialPortWrapper();
	//! Destruktor
	virtual ~UnixSerialPortWrapper();
	//! Öffnen des Ports
	/*!
	 *  \param dev Pfad zum zu öffnenden Device
	 *  \return Dateideskriptor des geöffneten Devices, -1 im Fehlerfall
	 */
	int Open(std::string dev);
	//! Schließen des Devices
	int Close();
	//! Implementierung von PortWrapper::ReadData()
	int ReadData(std::string* data);
	//! Implementierung von PortWrapper::SendData()
	int SendData(const std::string& data);
	//! Implementierung von PortWrapper::WaitForData()
	int WaitForData(int msTime);
protected:
	//! Dateideskriptor
	int fd;
};
#endif // _UNIXSERIALPORTWRAPPER_H_
