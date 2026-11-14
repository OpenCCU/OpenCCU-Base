/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFLogicalInstance.h"
#include "RFManager.h"

#include <Logger.h>
#include "RFLogicalInstance.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

using namespace XmlRpc;

RFLogicalInstance::RFLogicalInstance(void)
{
	cur_paramset_peer=0;
}

RFLogicalInstance::~RFLogicalInstance(void)
{
}

RFConfigData* RFLogicalInstance::GetCurConfigData(int list)
{
	return GetConfigData(cur_paramset_peer>>8, cur_paramset_peer&0xff, list);
}

RFConfigData* RFLogicalInstance::GetConfigData(int peer_address, int peer_channel, int list)
{
	RFConfigData& cd=config_data[(peer_address<<8)|(peer_channel&0xff)];
	if(list<0)return &cd;
	if(!cd.MustBeRead(list))return &cd;
	cd.SetPeer(peer_address, peer_channel);
	if(cd.ReadFromDevice(this, list)){
		OnConfigReadFromDevice(&cd, list);
	}
	return &cd;
}

bool RFLogicalInstance::RemoveConfigData(int peer_address, int peer_channel)
{
	config_data[(peer_address<<8)|(peer_channel&0xff)]=RFConfigData();
	return true;
}

void RFLogicalInstance::SetCurParamsetPeer(const std::string& peer)
{
	int address=0;
	int channel=0;
	if(!peer.empty())RFManager::GetSingleton()->ParseAddress(peer, &address, &channel);
	SetCurParamsetPeer(address, channel);
}

bool RFLogicalInstance::ProcessAsyncParamInfo(BidcosFrame& frame)
{
	int peer_address=frame.GetIntValue(BidcosFrame::FIELD_INFO_PARAM_PEER_A);
	int peer_channel=frame.GetIntValue(BidcosFrame::FIELD_INFO_PARAM_PEER_CH);
	int list=frame.GetIntValue(BidcosFrame::FIELD_INFO_PARAM_LIST);
	RFConfigData* cd=GetConfigData(peer_address, peer_channel, list);
	if(cd)cd->ProcessAsyncParamInfo(frame);
	return true;
}

bool RFLogicalInstance::SetStoredValue(const std::string& id, uint32_t peer, XmlRpc::XmlRpcValue& param, int flags)
{
	char temp[16];
	snprintf(temp, sizeof(temp), "[%08" PRIX32 "]", peer);
	std::string full_id=id+temp;
	return ValueStore::SetStoredValue(full_id, param, flags);
}

bool RFLogicalInstance::GetStoredValue(const std::string& id, uint32_t peer, XmlRpc::XmlRpcValue* param)
{
	char temp[16];
	snprintf(temp, sizeof(temp), "[%08" PRIX32 "]", peer);
	std::string full_id=id+temp;
	return ValueStore::GetStoredValue(full_id, param);
}

bool RFLogicalInstance::DeleteStoredValues(uint32_t peer)
{
	char temp[16];
	snprintf(temp, sizeof(temp), "[%08" PRIX32 "]", peer);
	size_t temp_size=strlen(temp);
	std::vector<std::string> values_to_erase;
	for(stored_values_t::iterator it=stored_values.begin();it!=stored_values.end();it++){
		const std::string& id=it->first;
		std::string::size_type pos=id.find(temp);
		if(pos!=std::string::npos && (pos==(id.size()-temp_size)))values_to_erase.push_back(id);
	}
	for(std::vector<std::string>::iterator it=values_to_erase.begin();it!=values_to_erase.end();it++){
		LOG(Logger::LOG_DEBUG, "Deleting stored value %s\n", it->c_str());
		stored_values.erase(*it);
	}

	return true;
}
bool RFLogicalInstance::replaceRFConfigData(RFLogicalInstance *oldInstance)
{
    config_data_t::iterator configIt;

    for(configIt = oldInstance->config_data.begin(); configIt != oldInstance->config_data.end();++configIt)
    {
        LOG(Logger::LOG_DEBUG,"RFLogicalInstance::replaceRFConfigData() CopyConfigdata 0x%lX",configIt->first);
        this->config_data[configIt->first] = RFConfigData(configIt->second);
        this->config_data[configIt->first].SetDevDirty();
    }
    return true;
}
void RFLogicalInstance::SetConfigDevDirty()
{
	config_data_t::iterator it;
	for(it=config_data.begin();it!=config_data.end();it++)
	{
		it->second.SetDevDirty();
	}
}
