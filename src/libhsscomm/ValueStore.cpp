/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "ValueStore.h"
#include <Logger.h>
#include <utils.h>

using namespace XmlRpc;

ValueStore::ValueStore(bool hold_timestamps)
{
	this->hold_timestamps=hold_timestamps;
	dirty = false;
}

ValueStore::~ValueStore(void)
{
}

bool ValueStore::GetStoredValue(const std::string& name, XmlRpc::XmlRpcValue* val, uint32_t* age/*=NULL*/)
{
	stored_values_t::iterator it=stored_values.find(name);
	if(it==stored_values.end())return false;
	*val=it->second.val;
	if(age){
		int32_t diff = int32_t(time_millis()-it->second.timestamp);
		if(diff < 0)
			*age=0;
		else
			*age=static_cast<uint32_t>(diff);
	}
	//LOG(Logger::LOG_DEBUG, "GetStoredValue() %s=%s", name.c_str(), val->toText().c_str());
	return true;
}

bool ValueStore::SetStoredValue(const std::string& name, XmlRpc::XmlRpcValue& val, int flags/*=0*/)
{
	//LOG(Logger::LOG_DEBUG, "SetStoredValue() %s=%s", name.c_str(), val.toText().c_str());
	entry_t& entry=stored_values[name];
	entry.val=val;
	entry.flags=flags;
	if(hold_timestamps)entry.timestamp=time_millis();
	if( (flags & FLAG_VOLATILE) == 0) {
		dirty = true;
		//LOG(Logger::LOG_ALL, "SetStoredValue() - flag store dirty. Flags=%d", flags);
	} 
	return true;
}

uint32_t ValueStore::GetAge(const std::string& name)
{
	if(!hold_timestamps)return (uint32_t)-1;
	stored_values_t::iterator it=stored_values.find(name);
	if(it==stored_values.end())return (uint32_t)-1;
	int32_t age=int32_t(time_millis()-it->second.timestamp);
	LOG(Logger::LOG_DEBUG, "%s age=%d timestamp=%u", name.c_str(), age, it->second.timestamp);
	if(age<0)return (uint32_t)-1;
	return age;
}

void ValueStore::ClearStoredValues()
{
	stored_values.clear();
	dirty = false;
}

bool ValueStore::SaveToXml(XMLNode* node)
{
	stored_values_t::iterator it;
	for(it=stored_values.begin();it!=stored_values.end();it++){
		if(it->second.flags & FLAG_VOLATILE)continue;
		const std::string& id=it->first;
		XmlRpcValue& val=it->second.val;
		XMLNode value_node=node->addChildConst("value");
		value_node.addAttributeConst("id", id.c_str());
		value_node.addTextConst(val.toText().c_str());
	}
	dirty = false;
	return true;
}

bool ValueStore::LoadFromXml(XMLNode& node)
{
	const char* temp;

//	LOG(Logger::LOG_DEBUG, "ValueStore::LoadFromXml()");
	int i=0;
	XMLNode value_node=node.getChildNode("value", &i);
	while(!value_node.isEmpty()){
		temp=value_node.getAttribute("id");
		if(!temp)return false;
		std::string id=temp;
		temp=value_node.getText();
		if(!temp)return false;
//		LOG(Logger::LOG_DEBUG, "ValueStore::LoadFromXml() id=%s, val=%s", id.c_str(), temp);
		entry_t& entry=stored_values[id];
		if(!entry.val.fromText(temp))return false;
		entry.timestamp=time_millis()-10000000;
		entry.flags=0;
		value_node=node.getChildNode("value", &i);
	}
	return true;
}

bool ValueStore::IsDirty() const {
	//LOG(Logger::LOG_ALL, "IsDirty() returning %s", (dirty ? "True" : "False"));
	return dirty;
}
