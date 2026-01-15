/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485SystemDescription.cpp: Implementierung der Klasse HS485SystemDescription.
//
//////////////////////////////////////////////////////////////////////

#include "HS485SystemDescription.h"
#include "HS485CommMessage.h"
#include "xmlParser.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <Logger.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HS485SystemDescription::HS485SystemDescription()
{

}

HS485SystemDescription::~HS485SystemDescription()
{
    devices_t::iterator it;
    for(it=devices.begin();it!=devices.end();it++){
		delete (*it);
    }
}

bool HS485SystemDescription::ReadFiles(const char *path)
{
//    LOG(Logger::LOG_DEBUG, "HS485SystemDescription::ReadFiles(%s)", path);
    DIR *pDirectory;
    bool retval=false;
    struct dirent *pEntry;
    pDirectory = opendir(path);
    if(!pDirectory)goto exit;

    pEntry = readdir( pDirectory );
    while(pEntry){
        std::string filename=path;
        filename+="/";
        filename+=pEntry->d_name;
		struct stat st;
		if(stat(filename.c_str(), &st)==0 && S_ISREG(st.st_mode)){
	        LOG(Logger::LOG_DEBUG, "Reading file %s", pEntry->d_name);
	        XMLResults xmlResult;
	        XMLNode rootNode = XMLNode::parseFile( filename.c_str(), "RF", &xmlResult );
		    if(!xmlResult.error){
	            HS485DeviceDescription* dev=new HS485DeviceDescription;
				if(dev->InitFromXml(rootNode, rootNode)){
					devices.push_back(dev);
					LOG(Logger::LOG_DEBUG, "Device %d created", devices.size());
				}else{
					LOG(Logger::LOG_WARNING, "%s: error parsing file", pEntry->d_name);
					delete dev;
				}
			}else{
	            LOG(Logger::LOG_WARNING, "%s: error %s at %d:%d", pEntry->d_name, XMLNode::getError(xmlResult.error), xmlResult.nLine, xmlResult.nColumn );
			}
		}
        pEntry = readdir( pDirectory );
    }
    retval=true;
    LOG(Logger::LOG_DEBUG, "%d devices found", devices.size());
exit:
    if(pDirectory)closedir(pDirectory);
    return retval;
}

HS485DeviceDescription* HS485SystemDescription::GetDeviceBySysinfo(const std::string& sysinfo, std::string* type/*=NULL*/)
{
	HS485Frame sysinfoFrame;
    sysinfoFrame.SetStringValue(0, sysinfo.size(), sysinfo);

	int priority=-1;
	HS485DeviceDescription* retval=NULL;
    devices_t::iterator it;
    for(it=devices.begin();it!=devices.end();it++){
		std::string matching_type;
		int p=(*it)->Matches(sysinfoFrame, &matching_type);
		if(p>priority){
			priority=p;
			retval=(*it);
			if(type)*type=matching_type;
        }
    }
    return retval;
}

HS485DeviceDescription* HS485SystemDescription::GetDeviceByType(const std::string& type)
{
    devices_t::iterator it;
    for(it=devices.begin();it!=devices.end();it++){
        if((*it)->SupportsType(type)){
            return (*it);
        }
    }
    return NULL;
}
