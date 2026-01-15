/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485FirmwareManager.cpp: Implementierung der Klasse HS485FirmwareManager.
//
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
#pragma warning(disable:4786)
#endif


#include "HS485FirmwareManager.h"
#include <Logger.h>
#include <stdio.h>

static const char* MAPFILENAME="fwmap";

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HS485FirmwareManager::HS485FirmwareManager()
{
}

HS485FirmwareManager::~HS485FirmwareManager()
{
	Free();
}

Hexfile* HS485FirmwareManager::GetFirmware(const std::string& type)
{
	t_firmwareMap::iterator it=firmwareMap.find(type);
	if(it==firmwareMap.end()){
		Hexfile hf;
		if(hf.Read(firmwarePath+filenameMap[type].filename)>0){
			firmwareMap[type]=hf;
			return &firmwareMap[type];
		}else{
			return 0;
		}
	}else{
		return &it->second;
	}

}

std::string HS485FirmwareManager::GetFirmwareVersion(const std::string& type)
{
	LOG(Logger::LOG_DEBUG, "HS485FirmwareManager::GetFirmwareVersion(%s)", type.c_str());
	int i_version;
	if(filenameMap.find(type)==filenameMap.end())return "";
	FirmwareFile& ff=filenameMap[type];
	if(!ff.version_is_address){
		i_version=ff.version;
	}else{
		Hexfile* hf=GetFirmware(type);
		if(!hf){
			LOG(Logger::LOG_ERROR, "Could not read firmware file for type %s", type.c_str());
			return "";
		}
		unsigned int firmwarePos=ff.version-(hf->GetStart());
		ucVec& buffer=hf->GetBuffer();
		if(buffer.size()<=firmwarePos+1){
			LOG(Logger::LOG_ERROR, "Firmware file for type %s too short", type.c_str());
			return "";
		}
		i_version=buffer[firmwarePos]|(buffer[firmwarePos+1]<<8);
	}
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d.%02d", i_version>>8, i_version&0xff);
	return buffer;
}

void HS485FirmwareManager::Free()
{
	firmwareMap.clear();
}

void HS485FirmwareManager::InitFilenameMap()
{
	filenameMap.clear();
    FILE* f;
	char buffer[256];
    f=fopen((firmwarePath+MAPFILENAME).c_str(), "r");

    if(!f){
		LOG(Logger::LOG_ERROR, "unable to open file %s", (firmwarePath+MAPFILENAME).c_str());
        return;
    }
    while(fgets(buffer, 256, f)){
		char* type=strtok(buffer, " \t\n\r");
		//skip comments and empty lines
		if(!type || *type=='#' || *type<' ')continue;
		char* filename=strtok(NULL, " \t\n\r#");
		if(!filename || *filename<' ')continue;
		char* version=strtok(NULL, " \t\n\r#");
		if(!version || *version<' ')continue;
		if(strstr(filename, ".hex")==NULL)continue;
		filenameMap[type]=FirmwareFile(filename, version);
		LOG(Logger::LOG_DEBUG, "%s=%s(%s)", type, filename, version);
	}
}

void HS485FirmwareManager::SetFirmwarePath(const std::string &path)
{
	firmwarePath=path;
	if(firmwarePath.size() && firmwarePath[firmwarePath.size()-1]!='/')firmwarePath+='/';
	InitFilenameMap();
}

const std::string& HS485FirmwareManager::GetFirmwarePath()
{
	return firmwarePath;
}
