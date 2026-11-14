/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RFFIRMWARE_MANAGER_H_
#define _RFFIRMWARE_MANAGER_H_

#include <UpdateFile.h>
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>

class RFFirmwareManager
{

public:
	//! Konstruktor
	RFFirmwareManager(void);
	//! Destruktor
	virtual ~RFFirmwareManager(void);
	//! setzt den Pfad zu den Firmwaredateine 
	void SetFirmwarePaths(const std::string& path, const std::string& userFirmwarePath);
	//! liefert den Aktuellen Pfad zu den Firmwaredateien
	const std::string& GetFirmwarePath();
	//! In dieser Klasse wird der Pfad zu einer Firmware und die zugeh�rige Firmwareversion abgelegt 
	class FirmwareFile{
	public:
		FirmwareFile()
		{
			version=-1;
		}
		FirmwareFile(const char* absoluteFilePath, const char* version)
		{
			this->absoluteFilePath=absoluteFilePath;
			if(strchr(version, '.')){
				char* dotpos;
				this->version=strtol(version, &dotpos, 10)<<8;
				this->version+=strtol(dotpos+1, NULL, 10);
			}else{
				this->version=strtol(version, NULL, 0);
			}
		};
		int getVersion()
		{
			return version;
		}
		std::string &getAbsoluteFilePath()
		{
			return absoluteFilePath;
		}
	private:
		std::string absoluteFilePath;
		int version;
	};
	//! Leert den Container in dem die Firmwaredateiene gespeichert sind
	void Free();
	//! liefert die Firmwareversion der verfügbaren Firmwaredatei
	// als Schlüssel wird die GeräteTypenNummer übergeben
	std::string GetFirmwareVersion(const int typeNumber);
	std::string GetFirmwareVersion(const int typeNumber, int *outVersion);
	//! liefert die Firmwaredatei 
	// als Schlüssel wird die GeräteTypenNummer übergeben
	UpdateFile GetFirmware(const int typeNumber);

	//! Aktualisiert die User Firmware Dateien und aktualisiert die interne Firmware-Map.
	//(Die mergedFilenameMap wird neu erzeugt und besteht aus der firmwareMap und der User Firmware Dateien;
	// Die fwmap Datei wird NICHT erneut eingelesen)
	void RefreshUserFirmwareMap();

private:
	//! Lädt die list der verfügbaren Firmwaredateien
	void InitFilenameMap();
	//! Lädt die verfügbaren Firmwaredateien im User Bereich ein
	void ReadUserFirmwarePath(std::vector< std::pair<int,FirmwareFile> >& userFirmwareFiles);
	//! Parsed eine info file
	bool ParseInfoFile(const std::string& infoFilePath, int& typeCode, std::string& version);
	//! Trim line
	void TrimLine(std::string& line);
	//! typedef für die Liste der verf�gbaren Firmwaredateien
	typedef std::map<int, FirmwareFile> t_filenameMap;
	//!Liste der verfügbaren Firmwaredateien
	t_filenameMap filenameMap;
	//!Liste der verfügbaren Firmwaredateien + Liste der verfügbaren User-Firmwaredateien
	t_filenameMap mergedFilenameMap;
	//! Pfad zu den Firmware Dateien
	std::string firmwarePath;
	//! Pfad zu den Firmware Dateien im User Bereich
	std::string userFirmwarePath;
};

#endif //_RFFIRMWARE_MANAGER_H_
