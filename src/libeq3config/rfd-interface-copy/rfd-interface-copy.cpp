/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <PropertyMap.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>

#include "rfd-interface-copy.h"


RfdInterfacCopy::RfdInterfacCopy() : Command("rfd-interface-copy")
{
}

RfdInterfacCopy::~RfdInterfacCopy()
{
}

int RfdInterfacCopy::execute()
{
	const int paramsSize = static_cast<int>(params.size());
	if(paramsSize != 2)
	{
		return 1;
	}
	
	std::string source = params.at(0);
	std::string target = params.at(1);
	
	PropertyMap sourceConfigData;
	PropertyMap targetConfigData;
	if(source.empty() || target.empty()) 
	{
		return 1;
	}
	
	if(sourceConfigData.ReadFromFile(source) < 0) 
	{
		//try to read file
		//if it failed -> try to restore .bak file if there is one
		std::string bakPath(source.substr(0, source.length()-3));
		bakPath.append(".xml");
		rename(bakPath.c_str(), source.c_str());
		if(sourceConfigData.ReadFromFile(source) < 0) 
		{
				return 2;
		}
	}
	
	if(targetConfigData.ReadFromFile(target) < 0) 
	{
		//try to read file
		//if it failed -> try to restore .bak file if there is one
		std::string bakPath(target.substr(0, target.length()-3));
		bakPath.append(".xml");
		rename(bakPath.c_str(), target.c_str());
		if(targetConfigData.ReadFromFile(target) < 0) 
		{
				return 2;
		}
	}

	int targetInterfaceNr = 0;
	PropertyMap::StringList targetsections=targetConfigData.ListSections();
	for(PropertyMap::StringList::iterator it=targetsections.begin();it!=targetsections.end();it++)
	{
		std::string& section=*it;
		if(section.find("Interface ")==0)
		{
			targetConfigData.SetCurrentSection(section);
			std::string interfaceNr = targetConfigData.GetCurrentSection();
			interfaceNr = interfaceNr.substr(10, interfaceNr.size() - 10);
			int interfaceNrAsInt = atoi(interfaceNr.c_str());
			if(interfaceNrAsInt > targetInterfaceNr)
			{
				targetInterfaceNr = interfaceNrAsInt;
			}		
		}
	}	
	
	std::ofstream outfile;
	outfile.open(target.c_str(), std::ios_base::out | std::ios_base::app);
	if(!outfile.is_open())
	{
		return 3;
	}	
		
    	PropertyMap::StringList sections=sourceConfigData.ListSections();
	for(PropertyMap::StringList::iterator it=sections.begin();it!=sections.end();it++)
	{
		std::string& section=*it;
		if(section.find("Interface ")==0)
		{
			sourceConfigData.SetCurrentSection(section);
			std::string interfaceNr = sourceConfigData.GetCurrentSection();
			interfaceNr = interfaceNr.substr(10, interfaceNr.size() - 10);
			std::string type = sourceConfigData.GetStringValue("Type", "");
			
			if(type.compare("Lan Interface") == 0)
			{				
				std::string serialNumber  = sourceConfigData.GetStringValue("Serial Number", "");
				std::string encryptionKey  = sourceConfigData.GetStringValue("Encryption Key", "");
				std::string ipAddress  = sourceConfigData.GetStringValue("IP Address", "");
				std::stringstream ss;
				
				ss << std::endl;
				ss << "[Interface " << ++targetInterfaceNr << "]" << std::endl;				
				ss << "Type = " << type << std::endl;			
				ss << "Serial Number =  " << serialNumber << std::endl;			
				ss << "Encryption Key = " << encryptionKey << std::endl;
							
				if(!ipAddress.empty())
				{		
					ss << "IP Address = " << ipAddress << std::endl;
				}
				
				outfile	<< ss.str();		
			}
		}
	}
	
	outfile.close();
	
	return 0;
}

std::string RfdInterfacCopy::help()
{
	std::string s("rfd-interface-copy source target\n");
	s.append("  Copy the Lan Interfaces from one rfd.conf to another\n");
	s.append("  Parameter:\n");
	s.append("    source: source rfd.conf file\n");
	s.append("    target: target rfd.conf file\n");
	return s;
}
