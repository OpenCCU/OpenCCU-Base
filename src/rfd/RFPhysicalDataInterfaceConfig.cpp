/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFPhysicalDataInterfaceConfig.h"
#include "RFDeviceDescription.h"

#include <Logger.h>
#include "type_registry.h"
using namespace XmlRpc;

static hsscomm::type_registry::factory<RFPhysicalDataInterfaceConfig> RFPhysicalDataInterfaceConfigFactory;

RFPhysicalDataInterfaceConfig::RFPhysicalDataInterfaceConfig(void)
{
	by_index=0;
	bi_index=0;
	by_size=1;
	bi_size=0;
	list=0;
	mask=0xffffffff;
}

RFPhysicalDataInterfaceConfig::~RFPhysicalDataInterfaceConfig(void)
{
}

bool RFPhysicalDataInterfaceConfig::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    const char* temp=node.getAttribute("index");
	if(!temp){
		LOG(Logger::LOG_WARNING, "<physical interface=\"config\"> attribute \"index\" not found");
		return false;
	}
	std::string s=temp;
	std::string::size_type pos=s.find('.');
	if(pos)by_index=strtol(s.substr(0, pos).c_str(), NULL, 0);
	if(pos!=std::string::npos)bi_index=strtol(s.substr(pos+1).c_str(), NULL, 0);

    temp=node.getAttribute("size");
	if(temp){
		std::string s=temp;
		std::string::size_type pos=s.find('.');
		if(pos)by_size=strtol(s.substr(0, pos).c_str(), NULL, 0);
		if(pos!=std::string::npos)bi_size=strtol(s.substr(pos+1).c_str(), NULL, 0);
	}
	if((bi_index || bi_size) && by_size){
		LOG(Logger::LOG_WARNING, "<physical interface=\"config\">: Bit index or bit size greater zero used with non zero byte size. Use \"mask\" attribute instead.");
	}
    temp=node.getAttribute("mask");
	if(temp){
		mask=strtoul(temp, NULL, 0);
	}
	temp=node.getAttribute("list");
	if(temp){
		list=strtol(temp, NULL, 0);
	}
    return true;
}

bool RFPhysicalDataInterfaceConfig::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
	RFConfigData* cd=((RFLogicalInstance*)inst)->GetCurConfigData(list);
	if(!cd){
		LOG(Logger::LOG_DEBUG, "RFPhysicalDataInterfaceConfig::GetData() GetCurConfigData() failed");
		return false;
	}
	*param=(int)cd->GetValue(list, by_index, bi_index, by_size, bi_size, mask);
	//LOG(Logger::LOG_DEBUG, "GET param=%s (%d, %d, %d, %d)", param->toText().c_str(),by_index, bi_index, by_size, bi_size);
    return true;
}

bool RFPhysicalDataInterfaceConfig::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
	RFConfigData* cd=((RFLogicalInstance*)inst)->GetCurConfigData(list);
	if(!cd)return false;
	try{
		uint32_t value=(int)param;
		cd->PutValue(list, by_index, bi_index, by_size, bi_size, value, mask);
		//LOG(Logger::LOG_DEBUG, "PUT param=%s (%d, %d, %d, %d)", param.toText().c_str(),by_index, bi_index, by_size, bi_size);
	}catch(XmlRpcException){
		//LOG(Logger::LOG_WARNING, "PUT param=%s (%d, %d, %d, %d) failed.", param.toText().c_str(),by_index, bi_index, by_size, bi_size);
		return false;
	}
	return true;
}
bool RFPhysicalDataInterfaceConfig::SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val)
{
	RFConfigData* cd=((RFLogicalInstance*)inst)->GetCurConfigData(list);
	if(!cd)return false;
	try{
		uint32_t value=(int)val;
		cd->PushData(list, by_index, bi_index, by_size, bi_size, value, mask);
	}catch(XmlRpcException){
		return false;
	}
	return true;
}
bool RFPhysicalDataInterfaceConfig::DetermineValue(LogicalInstance* inst)
{
	RFConfigData* cd=((RFLogicalInstance*)inst)->GetCurConfigData(list);
	if(!cd)return false;
	return cd->DetermineValue((RFLogicalInstance*)inst, list, by_index);
}

bool RFPhysicalDataInterfaceConfig::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_config", tag)==0)return true;
    return false;
}
