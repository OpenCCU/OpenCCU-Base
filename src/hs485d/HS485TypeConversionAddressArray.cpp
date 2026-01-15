/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485TypeConversionAddressArray.cpp: Implementierung der Klasse HS485TypeConversionAddressArray.
//
//////////////////////////////////////////////////////////////////////

#include "HS485TypeConversionAddressArray.h"
#include "HS485Manager.h"
#include <Logger.h>
#include <dynamic.h>

static dynamic::factory<HS485TypeConversionAddressArray> HS485TypeConversionAddressArrayFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HS485TypeConversionAddressArray::HS485TypeConversionAddressArray()
{
}

HS485TypeConversionAddressArray::~HS485TypeConversionAddressArray()
{

}

bool HS485TypeConversionAddressArray::CheckCreationTag(const char *tag)
{
	if(strcmp(tag, "type_conversion_address_array")==0)return true;
    return false;
}

bool HS485TypeConversionAddressArray::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    return true;
}

bool HS485TypeConversionAddressArray::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
		HS485Channel* peer=dynamic_cast<HS485Channel*>(HS485Manager::GetSingleton()->GetInstance((std::string&)in));
		if(peer){
			((int&)(*out)[0])=peer->GetDevice()->GetAddress();
			((int&)(*out)[1])=peer->GetPhysicalIndex();
		}else{
			uint32_t address;
			int channel;
			if(!HS485Manager::GetSingleton()->ParseAddress((std::string&)in, &address, &channel)){
				LOG(Logger::LOG_WARNING, "Unknown channel %s", ((std::string&)in).c_str());
				return false;
			}
	        ((int&)(*out)[0])=address;
			((int&)(*out)[1])=channel;
		}
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

bool HS485TypeConversionAddressArray::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
	HS485Channel* this_channel=dynamic_cast<HS485Channel*>(inst);
	if(!this_channel)return false;
    try{
		HS485Device* peer_device=HS485Manager::GetSingleton()->GetDeviceByAddress((int&)in[0]);
		if(!peer_device){
			LOG(Logger::LOG_WARNING, "Unknown device with address %08X", (int&)in[0]);
			*out=HS485Manager::GetSingleton()->BuildStringAddress((int&)in[0], (int&)in[1]);
			return true;
		}
		int peer_direction=this_channel->GetDescription()->GetDirection()==HS485ChannelDescription::DIRECTION_RECEIVER?HS485ChannelDescription::DIRECTION_SENDER:HS485ChannelDescription::DIRECTION_RECEIVER;
		HS485Channel* peer=peer_device->GetChannelByPhysicalIndex((int&)in[1],peer_direction);
		if(!peer){
			LOG(Logger::LOG_WARNING, "Could not find matching channel %08X:%d", (int&)in[0], (int&)in[1]);
			*out=HS485Manager::GetSingleton()->BuildStringAddress((int&)in[0], (int&)in[1]);
			return true;
		}
		*out=peer->GetSerial();
    }catch(XmlRpcException){
        return false;
    }
    return true;
}
