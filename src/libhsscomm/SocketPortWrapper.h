/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// UnixSerialPortWrapper.h: Schnittstelle für die Klasse UnixSerialPortWrapper.
#ifndef _SOCKETPORTWRAPPER_H_
#define _SOCKETPORTWRAPPER_H_

#include "dllexport.h"
#include "PortWrapper.h"
#include <string>
#include "typedefs.h"
//! Implementiert PortWrapper für die Kommunikation per TCP
/*! Es wird kein lokaler Port geöffnet, sondern über TCP die Verbindung zu einer Instanz des Programms
 *  portconnect aufgebaut. Damit läßt sich ein Interfaceprozess auf einer Plattform starten, die bessere
 *  Debugmöglichkeiten bietet als die Zentrale
 */
class DLLEXPORT SocketPortWrapper : public PortWrapper  
{
public:
	//! Konstruktor
	SocketPortWrapper();
	//! Destruktor
	virtual ~SocketPortWrapper();
	//! Verbindungsaufbau
	/*!
	 *  \param host Hostname oder IP-Adresse der Gegenstelle
	 *  \param port TCP-Portnummer der Gegenstelle
	 *  \return Dateideskriptor des geöffneten Sockets, -1 im Fehlerfall
	 */
	int Open(const char* host, int port);
	//! Schließen des Sockets
	int Close();
	//! Implementierung von PortWrapper::ReadData()
	int ReadData(std::string* data);
	//! Implementierung von PortWrapper::SendData()
	int SendData(const std::string& data);
	//! Implementierung von PortWrapper::WaitForData()
	int WaitForData(int msTime);
protected:
	//! Socketdescriptor
	int fd;
};
#endif // _SOCKETPORTWRAPPER_H_
