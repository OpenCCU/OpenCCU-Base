/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485FirmwareManager.h: Schnittstelle für die Klasse HS485FirmwareManager.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIRMWAREMANAGER_H__7B1C9DA8_C709_4CDE_8449_0288E6E5A184__INCLUDED_)
#define AFX_FIRMWAREMANAGER_H__7B1C9DA8_C709_4CDE_8449_0288E6E5A184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include <Hexfile.h>
#include <cstring>
#include <cstdlib>

//! Diese Klasse verwaltet die Firmwaredateien der HS485-Geräte.

class HS485FirmwareManager  
{
public:
	void SetFirmwarePath(const std::string& path);
	const std::string& GetFirmwarePath();
	class FirmwareFile{
	public:
		FirmwareFile()
		{
			version=-1;
			version_is_address=false;
		}
		FirmwareFile(const char* filename, const char* version)
		{
			this->filename=filename;
			if(*version=='@'){
				version++;
				version_is_address=true;
				this->version=strtol(version, NULL, 0);
			}else if(strchr(version, '.')){
				version_is_address=false;
				char* dotpos;
				this->version=strtol(version, &dotpos, 10)<<8;
				this->version+=strtol(dotpos+1, NULL, 10);
			}else{
				version_is_address=false;
				this->version=strtol(version, NULL, 0);
			}
		};
		std::string filename;
		int version;
		bool version_is_address;
	};
	void Free();
	std::string GetFirmwareVersion(const std::string& sysinfo);
	Hexfile* GetFirmware(const std::string& sysinfo);
	HS485FirmwareManager();
	virtual ~HS485FirmwareManager();
protected:
	void InitFilenameMap();
	typedef std::map<std::string, Hexfile> t_firmwareMap;
	t_firmwareMap firmwareMap;
	typedef std::map<std::string, FirmwareFile> t_filenameMap;
	t_filenameMap filenameMap;
	std::string firmwarePath;
};

#endif // !defined(AFX_FIRMWAREMANAGER_H__7B1C9DA8_C709_4CDE_8449_0288E6E5A184__INCLUDED_)
