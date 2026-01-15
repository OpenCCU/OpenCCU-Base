/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485SystemDescription.h: Schnittstelle für die Klasse HS485SystemDescription.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HS485SYSTEMDESCRIPTION_H__43FFE4E2_B89E_483A_9B0A_4F859D41D4D9__INCLUDED_)
#define AFX_HS485SYSTEMDESCRIPTION_H__43FFE4E2_B89E_483A_9B0A_4F859D41D4D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "HS485DeviceDescription.h"

//! Verwaltung der im System bekannten Gerätebschreibungen
/*!
 *  Diese Klasse liest beim Starten alle Gerätebeschreibungsdateien ein. Später kann durch Angabe eines Sysinfo-Strings
 *  oder eines Gerätetyps (Kurzbezeichnung) auf die entsprechende Gerätebeschreibung zugegriffen werden.
 */
class HS485SystemDescription  
{
public:
	//! Gibt die am besten passende Gerätebeschreibung für einen übergebenen Sysinfo-String zurück
	/*!
	 *  \param sysinfoFrame Ein von einem Gerät während des Anlernens empfangener Sysinfo-String
	 *  \param type Variable, die den Gerätetyp (=Kurzbezeichnung) zurückgibt
	 *  \return Gerätebeschreibung zum Sysinfo-Frame, \c NULL wenn es keine solche gibt
	 */
    HS485DeviceDescription* GetDeviceBySysinfo(const std::string& sysinfo, std::string* type=NULL);
	//! Gibt die Gerätebeschreibung für einen übergebenen Gerätetyp zurück
	/*!
	 *  \param type Gerätetyp
	 *  \return Gerätebeschreibung zum Gerätetyp, \c NULL wenn es keine solche gibt
	 */
	HS485DeviceDescription* GetDeviceByType(const std::string& type);
	//! Liest alle \c *.xml -Dateien in einem übergebenen Verzeichnis als Gerätebeschreibungsdateien ein
	/*!
	 *  \param path Verzeichnis, in dem sich die Gerätebeschreibungsdateien befinden
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool ReadFiles(const char* path);
	//! Konstruktor
	HS485SystemDescription();
	//! Destruktor
	virtual ~HS485SystemDescription();
protected:
	//! Typedef für Vektor von Gerätebeschreibungen
    typedef std::vector<HS485DeviceDescription*> devices_t;
	//! Vektor von Gerätebeschreibungen
    devices_t devices;
};

#endif // !defined(AFX_HS485SYSTEMDESCRIPTION_H__43FFE4E2_B89E_483A_9B0A_4F859D41D4D9__INCLUDED_)
