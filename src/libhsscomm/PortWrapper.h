/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// PortWrapper.h: Schnittstelle für die Klasse PortWrapper.
#ifndef _PORTWRAPPER_H_
#define _PORTWRAPPER_H_

#include "dllexport.h"
#include <typedefs.h>
#include <string>

//! Abstrakte Bausisklasse für die Abstrahierung eines Kommunikationskanals
class DLLEXPORT PortWrapper  
{
public:
	//! Lesen von vorhandenen Daten. Kehrt sofort zurück, wenn keine Daten vorhanden sind
	/*! 
	 *  \param data Zeiger auf den String, an den die gelesenen Zeichen angehängt werden
	 *  \return Anzahl gelesener Zeichen
	 */
	virtual int ReadData(std::string* data)=0;
	//! Senden von Daten. Sendet alle Zeichen in \c data.
	/*!
	 *  \param data Referenz auf die zu sendenden Daten
	 *  \return Anzahl gesendeter Zeichen
	 */
	virtual int SendData(const std::string& data)=0;
	//! Warten auf Daten zum Lesen
	/*!
	 *  \param msTime Zeit in ms, die maximal gewartet wird
	 *  \return 0 bei Zeitüberschreitung, >0 sonst
	 */
	virtual int WaitForData(int msTime);
	//! Konstruktor
	PortWrapper();
	//! Destruktor
	virtual ~PortWrapper();
};
#endif // _PORTWRAPPER_H_
