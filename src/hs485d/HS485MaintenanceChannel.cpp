/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485MaintenanceChannel.h"
#include "HS485Device.h"
#include <Logger.h>
#include "dynamic.h"

static dynamic::factory<HS485MaintenanceChannel> HS485MaintenanceChannelFactory;


HS485MaintenanceChannel::HS485MaintenanceChannel(void)
{
}

HS485MaintenanceChannel::~HS485MaintenanceChannel(void)
{
}

bool HS485MaintenanceChannel::SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event/*=false*/)
{
//	LOG(Logger::LOG_DEBUG, "HS485MaintenanceChannel::SetInternalValue(%s, %s)", name.c_str(), val.toText().c_str());
	if(GetDevice()->SetInternalValue(name, val, fire_event))return true;
	return false;
};

bool HS485MaintenanceChannel::GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
//	LOG(Logger::LOG_DEBUG, "HS485MaintenanceChannel::GetInternalValue(%s)", name.c_str());
	if(GetDevice()->GetInternalValue(name, val))return true;
	return false;
};

void HS485MaintenanceChannel::OnEvent(LogicalInstance* inst, const std::string& id, XmlRpc::XmlRpcValue& val)
{
	//LOG(Logger::LOG_DEBUG, "HS485MaintenanceChannel::OnEvent(%s, %s)", id.c_str(), val.toText().c_str());
	ReportEvent(id, val);
	//SendInternalValueEvent(id, val);
}

bool HS485MaintenanceChannel::RegisterInternalValueEvent(const std::string& id, LogicalInstance::EventReceiver* rec)
{
	//LOG(Logger::LOG_DEBUG, "HS485MaintenanceChannel::RegisterInternalValueEvent(%s)this=%p", id.c_str(), this);
	return GetDevice()->RegisterInternalValueEvent(id, this);
}

bool HS485MaintenanceChannel::CheckCreationTag(const char *tag)
{
    if(strcmp("channel_class_maintenance", tag)==0)return true;
    return false;
}

bool HS485MaintenanceChannel::SaveToXml(XMLNode* node)
{
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d", GetIndex());
	node->addAttributeConst("index", buffer);
	node->addAttributeConst("type", GetDescription()->GetType().c_str());
	return true;
}
