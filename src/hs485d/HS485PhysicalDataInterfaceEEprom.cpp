/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485PhysicalDataInterfaceEEprom.h"

#include <Logger.h>
#include <dynamic.h>
#include "HS485LogicalInstance.h"
#include "HS485Device.h"
#include "HS485DeviceDescription.h"
#include "HS485EEpromCache.h"

using namespace XmlRpc;

static dynamic::factory<HS485PhysicalDataInterfaceEEProm> HS485PhysicalDataInterfaceEEPromFactory;

HS485PhysicalDataInterfaceEEProm::HS485PhysicalDataInterfaceEEProm(void)
{
	by_size=1;
	bi_size=0;
	little_endian=false;
	read_size=0;
}

HS485PhysicalDataInterfaceEEProm::~HS485PhysicalDataInterfaceEEProm(void)
{
}

bool HS485PhysicalDataInterfaceEEProm::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    int i=0;
    XMLNode address_node=node.getChildNode("address", &i);
    while(!address_node.isEmpty()){
        HS485PhysicalAddress addr;
        if(!addr.InitFromXml(address_node, root_node))return false;
        addresses.push_back(addr);
        address_node=node.getChildNode("address", &i);
    }
    const char* temp=node.getAttribute("read_size");
	if(temp){
		read_size=strtoul(temp, NULL, 0);
	}

    temp=node.getAttribute("size");
	if(temp){
		std::string s=temp;
		std::string::size_type pos=s.find('.');
		if(pos)by_size=strtol(s.substr(0, pos).c_str(), NULL, 0);
		if(pos!=std::string::npos)bi_size=strtol(s.substr(pos+1).c_str(), NULL, 0);
	}
    temp=node.getAttribute("endian");
	if(temp)little_endian=temp[0]=='l' || temp[0]=='L';
	return true;
}

bool HS485PhysicalDataInterfaceEEProm::CalculateAddress(unsigned int index, unsigned int* byte, unsigned int* bit)
{
    for(unsigned int i=0;i<addresses.size();i++){
		if(addresses[i].CalculateAddress(index, byte, bit))return true;
    }
	return false;
}

bool HS485PhysicalDataInterfaceEEProm::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
//    LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceEEProm::GetData()");
	unsigned int by_pos;
	unsigned int bi_pos;
	HS485LogicalInstance* hs_inst=(HS485LogicalInstance*)inst;
	if(!CalculateAddress(hs_inst->GetCurParamsetIndex(), &by_pos, &bi_pos))return false;

	unsigned int count=read_size;
	if(!count){
		count=by_size+(bi_size+bi_pos+7)/8;
	}
	HS485EEPromCache::data_t data;

	if(!hs_inst->GetDevice()->GetEEPromCache()->GetData(by_pos, count, &data)){
		return false;
	}

	uint32_t val=0;
	for(unsigned int i=0;i<count;i++){
		val <<= 8;
		val |= data[little_endian?count-1-i:i];
	}
	val>>=bi_pos;
	val &= (0xffffffff>>(32-(by_size*8+bi_size)));

	try{
		(int&)*param=(int)val;
	}catch(XmlRpcException e){
		return false;
	}
//    LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceEEProm::GetData()returns %s", param->toText().c_str());
	return true;
}

bool HS485PhysicalDataInterfaceEEProm::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
	unsigned int by_pos;
	unsigned int bi_pos;
	HS485LogicalInstance* hs_inst=(HS485LogicalInstance*)inst;
	if(!CalculateAddress(hs_inst->GetCurParamsetIndex(), &by_pos, &bi_pos))return false;

	unsigned int count=read_size;
	if(!count){
		count=by_size+(bi_size+bi_pos+7)/8;
	}
	HS485EEPromCache::data_t data;

	if(!hs_inst->GetDevice()->GetEEPromCache()->GetShadowData(by_pos, count, &data))return false;

	char buffer[256];
	*buffer=0;
	for(unsigned int i=0;i<count;i++)sprintf(buffer+3*i, "%02X ", (int)data[i]);
	try{
		uint32_t val=(int&)param;
		val <<= bi_pos;
		uint32_t mask=(0xffffffff>>(32-(by_size*8+bi_size)))<<bi_pos;

		for(unsigned int i=0;i<count;i++){
			int index=little_endian?i:count-1-i;
			unsigned char c=data[index];
			c &= ~(mask & 0xff);
			c |= val & 0xff;
			data[index]=c;
			val>>=8;
			mask>>=8;
		}
		for(unsigned int i=0;i<count;i++)sprintf(buffer+3*i, "%02X ", (int)data[i]);
		if(!hs_inst->GetDevice()->GetEEPromCache()->PutData(by_pos, data))return false;

	}catch(XmlRpcException e){
		return false;
	}
	return true;
}

bool HS485PhysicalDataInterfaceEEProm::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_eeprom", tag)==0)return true;
    return false;
}
