/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485LogicalInstance.h"
#include <Logger.h>

using namespace XmlRpc;

HS485LogicalInstance::HS485LogicalInstance(void)
{
}

HS485LogicalInstance::~HS485LogicalInstance(void)
{
}

bool HS485LogicalInstance::SetStoredValue(const std::string& id, const std::string& peer, XmlRpc::XmlRpcValue& param, int flags/*=0*/)
{
	std::string full_id=id+"["+peer+"]";
	return ValueStore::SetStoredValue(full_id, param, flags);
}

bool HS485LogicalInstance::GetStoredValue(const std::string& id, const std::string& peer, XmlRpc::XmlRpcValue* param)
{
	std::string full_id=id+"["+peer+"]";
	return ValueStore::GetStoredValue(full_id, param);
}

bool HS485LogicalInstance::DeleteStoredValues(const std::string& peer)
{
	std::string postfix=std::string("[")+peer+"]";
	std::vector<std::string> values_to_erase;
	for(stored_values_t::iterator it=stored_values.begin();it!=stored_values.end();it++){
		const std::string& id=it->first;
		if(id.size()<postfix.size())continue;
		if(id.substr(id.size()-postfix.size())==postfix)values_to_erase.push_back(id);
	}
	for(std::vector<std::string>::iterator it=values_to_erase.begin();it!=values_to_erase.end();it++){
		stored_values.erase(*it);
	}
	return true;
}

