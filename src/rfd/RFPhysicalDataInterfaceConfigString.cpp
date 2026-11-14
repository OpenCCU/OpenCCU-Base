/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFPhysicalDataInterfaceConfigString.h"
#include "RFDeviceDescription.h"

#include <Logger.h>
#include "type_registry.h"
#include <inttypes.h>

using namespace XmlRpc;

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MASK (0xffffffff)

static hsscomm::type_registry::factory<RFPhysicalDataInterfaceConfigString> RFPhysicalDataInterfaceConfigStringFactory;

RFPhysicalDataInterfaceConfigString::RFPhysicalDataInterfaceConfigString(void)
{
	index=0;
	size=0;
	list=0;
}

RFPhysicalDataInterfaceConfigString::~RFPhysicalDataInterfaceConfigString(void)
{
}

bool RFPhysicalDataInterfaceConfigString::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp;
	std::string s;

	// erforderlicher Parameter "index"
    temp=node.getAttribute("index");
	if(!temp){
		LOG(Logger::LOG_WARNING, "<physical interface=\"config_string\"> attribute \"index\" not found");
		return false;
	}
	s = temp;
	index=strtol(s.c_str(), NULL, 0);

	// erforderlicher Parameter "size"
    temp=node.getAttribute("size");
	if (!temp)
	{
		LOG(Logger::LOG_WARNING, "<physical interface=\"config_string\"> attribute \"size\" not found");
		return false;
	}
	s = temp;
	size = strtol(s.c_str(), NULL, 0);

	// optionaler Parameter "list" (default: list = 0)
	temp=node.getAttribute("list");
	if(temp){
		list=strtol(temp, NULL, 0);
	}
    return true;
}

bool RFPhysicalDataInterfaceConfigString::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
	RFConfigData* cd=((RFLogicalInstance*)inst)->GetCurConfigData(list);
	if(!cd){
		LOG(Logger::LOG_DEBUG, "RFPhysicalDataInterfaceConfigString::GetData() GetCurConfigData() failed");
		return false;
	}
	
	std::string value = std::string();
	for (int i = 0; i < size; i++)
	{
		char character = (char) (cd->GetValue(list, index + i, 0, 1, 0, MASK));
		if (character != '\0')
		{
			value += character;
		}
		else 
		{
			break;
		}
	}
	*param = value;

    return true;
}
bool RFPhysicalDataInterfaceConfigString::SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val)
{
	return true;
}
bool RFPhysicalDataInterfaceConfigString::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
	RFConfigData* cd=((RFLogicalInstance*)inst)->GetCurConfigData(list);
	if(!cd)return false;
	try{
		std::string strValue = (std::string) param;
		int strSize          = strValue.length();

		for (int i = 0; i < size; i++)
		{
			uint32_t value = (i < strSize) ? strValue[i] : '\0';
			LOG(Logger::LOG_DEBUG, "PutValue (index=%d, value=%" PRIu32 ")", index + i, value);
			cd->PutValue(list, index + i, 0, 1, 0, value, MASK);
		}
	}catch(XmlRpcException){
		return false;
	}
	return true;
}

bool RFPhysicalDataInterfaceConfigString::DetermineValue(LogicalInstance* inst)
{
	RFConfigData* cd=((RFLogicalInstance*)inst)->GetCurConfigData(list);
	if(!cd)return false;
	return cd->DetermineValue((RFLogicalInstance*)inst, list, index);
}

bool RFPhysicalDataInterfaceConfigString::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_config_string", tag)==0)return true;
    return false;
}
