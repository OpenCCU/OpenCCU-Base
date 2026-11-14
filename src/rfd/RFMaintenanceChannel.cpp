/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFMaintenanceChannel.h"
#include "RFDevice.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<RFMaintenanceChannel> RFMaintenanceChannelFactory;


RFMaintenanceChannel::RFMaintenanceChannel(void)
{
}

RFMaintenanceChannel::~RFMaintenanceChannel(void)
{
}

bool RFMaintenanceChannel::SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event/*=false*/)
{
//	LOG(Logger::LOG_DEBUG, "RFMaintenanceChannel::SetInternalValue(%s, %s)", name.c_str(), val.toText().c_str());
	if(GetDevice()->SetInternalValue(name, val, fire_event))return true;
	return false;
};

bool RFMaintenanceChannel::GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
//	LOG(Logger::LOG_DEBUG, "RFMaintenanceChannel::GetInternalValue(%s)", name.c_str());
	if(GetDevice()->GetInternalValue(name, val))return true;
	return false;
};

void RFMaintenanceChannel::OnEvent(LogicalInstance* inst, const std::string& id, XmlRpc::XmlRpcValue& val)
{
//	LOG(Logger::LOG_DEBUG, "RFMaintenanceChannel::OnEvent(%s, %s)", id.c_str(), val.toText().c_str());
//	ReportEvent(id, val);
	SendInternalValueEvent(id, val);
}

bool RFMaintenanceChannel::RegisterInternalValueEvent(const std::string& id, LogicalInstance::EventReceiver* rec)
{
//	LOG(Logger::LOG_DEBUG, "RFMaintenanceChannel::RegisterInternalValueEvent(%s.%s)", serial.c_str(), id.c_str());
	if(!GetDevice()->RegisterInternalValueEvent(id, this))return false;
	return LogicalInstance::RegisterInternalValueEvent(id, rec);
}
bool RFMaintenanceChannel::PushDefaultConfig()
{
	bool retval = true;

		config_data_t::iterator confIt;
		for(confIt=config_data.begin();confIt!=config_data.end();confIt++)
		{
			if(!confIt->second.CommitToDevice(this))retval=false;
		}
		return retval;
}
bool RFMaintenanceChannel::CheckCreationTag(const char *tag)
{
    if(strcmp("channel_class_maintenance", tag)==0)return true;
    return false;
}

bool RFMaintenanceChannel::SaveToXml(XMLNode* node)
{
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d", GetIndex());
	node->addAttributeConst("index", buffer);
	node->addAttributeConst("type", GetDescription()->GetType().c_str());
	return true;
}
