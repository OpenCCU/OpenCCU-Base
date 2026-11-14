/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFSystemDescription.cpp: Implementierung der Klasse RFSystemDescription.
//
//////////////////////////////////////////////////////////////////////

#include "RFSystemDescription.h"
#include "xmlParser.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <Logger.h>
#include <utils.h>
#include <OSCompat.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

RFSystemDescription::RFSystemDescription()
{

}

RFSystemDescription::~RFSystemDescription()
{
    devices_t::iterator it;
    for(it=devices.begin();it!=devices.end();it++){
		delete (*it);
    }
}

bool RFSystemDescription::ReadFiles(const char *path)
{
    OSCompat::DirectoryLister dl(path);
    std::string entry;
    while((entry=dl.NextEntry()).size()){
        std::string filename=std::string(path)+OSCompat::PATH_SEPARATOR+entry;
	    XMLResults xmlResult;
	    XMLNode rootNode = XMLNode::parseFile( filename.c_str(), "RF", &xmlResult );
		if(!xmlResult.error){
		    RFDeviceDescription* dev=new RFDeviceDescription;
			if(dev->InitFromXml(rootNode, rootNode)){
				devices.push_back(dev);
                LOG(Logger::LOG_DEBUG, "Device description %s loaded (%d)", entry.c_str(), devices.size());
			}else{
				LOG(Logger::LOG_WARNING, "%s: error parsing file", entry.c_str());
				delete dev;
			}
		}else{
	        LOG(Logger::LOG_WARNING, "%s: error %s at %d:%d", entry.c_str(), XMLNode::getError(xmlResult.error), xmlResult.nLine, xmlResult.nColumn );
		}
	}
	return true;
}

RFDeviceDescription* RFSystemDescription::GetDeviceBySysinfo(BidcosFrame& sysinfoFrame, std::string* type/*=NULL*/)
{
	int priority=-1;
	RFDeviceDescription* retval=NULL;
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

RFDeviceDescription* RFSystemDescription::GetDeviceByType(const std::string& type)
{
    devices_t::iterator it;
    for(it=devices.begin();it!=devices.end();it++){
        if((*it)->SupportsType(type)){
            return (*it);
        }
    }
    return NULL;
}
