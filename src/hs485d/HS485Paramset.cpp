/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485Paramset.cpp: Implementierung der Klasse HS485Paramset.
//
//////////////////////////////////////////////////////////////////////

#include "HS485Paramset.h"
#include "HSSParameter.h"
#include "HS485DeviceDescription.h"
#include "HS485Device.h"
#include "HS485LogicalInstance.h"
#include "HSSLogicalType.h"
#include <Logger.h>
#include <typeinfo>

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HS485Paramset::HS485Paramset()
{
	count=1;
}

HS485Paramset::~HS485Paramset()
{
}

bool HS485Paramset::InitFromXml(XMLNode &node, XMLNode& root_node)
{
    if(!HSSParamset::InitFromXml(node, root_node))return false;
    const char* temp;

	temp=node.getAttribute("peer_param");
	if(temp){
		peer_param_id=temp;
		temp=node.getAttribute("channel_param");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<paramset> attribute \"channel_param\" not found");
			return false;
		}
		channel_param_id=temp;
		temp=node.getAttribute("count");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<paramset> attribute \"count\" not found");
			return false;
		}
		count=strtol(temp, NULL, 0);
		if(count<=0){
			LOG(Logger::LOG_WARNING, "<paramset> attribute \"count\" invalid value %s", temp);
			return false;
		}
	}
    return true;
}

bool HS485Paramset::SetEnforcedValues(HS485LogicalInstance* inst)
{
	inst->SetCurParamsetIndex(inst->GetEEPromIndex());
	enforced_values_t::iterator it;
	for(it=enforced_values.begin();it!=enforced_values.end();it++)
	{
        HSSParameter* param=GetParameter(it->first);
		if(!param)return false;
		if(!param->SetValue(inst, it->second))return false;
	}
	return true;
}

bool HS485Paramset::SetIndexByPeer(HS485LogicalInstance* inst, const std::string& peer)
{
	if(count==1){
		inst->SetCurParamsetIndex(inst->GetEEPromIndex());
		return true;
	}else{
//		LOG(Logger::LOG_DEBUG, "HS485Paramset::SetIndexByPeer(%s)", peer.c_str());
		params_t::iterator it;
		it=params.find(peer_param_id);
		if(it==params.end())return false;
		HSSParameter* peer_param=it->second;
		it=params.find(channel_param_id);
		if(it==params.end())return false;
		HSSParameter* channel_param=it->second;
		for(int i=0;i<count;i++){
			try{
				inst->SetCurParamsetIndex(i);
				XmlRpcValue val;
				if(!peer_param->GetValue(inst, &val))return false;
				//LOG(Logger::LOG_DEBUG, "peer_param=%s", ((std::string&)val).c_str());
				if(peer == (std::string&)val){
					val.clear();
					if(!channel_param->GetValue(inst, &val))return false;
//					LOG(Logger::LOG_DEBUG, "inst->GetPhysicalIndex()=%d, channel_param=%d", inst->GetPhysicalIndex(), (int&)val);
					if(inst->GetPhysicalIndex() == (int&)val)return true;
				}
			}catch(XmlRpcException){
				return false;
			}
		}
		return false;
	}
}

bool HS485Paramset::Get(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue *set)
{
    
	if(!SetIndexByPeer((HS485LogicalInstance*)inst, peer))return false;
    return HSSParamset::Get(inst, peer, set);
}

bool HS485Paramset::Put(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue& set)
{
	if(!SetIndexByPeer((HS485LogicalInstance*)inst, peer))return false;

    return HSSParamset::Put(inst, peer, set);
}

bool HS485Paramset::ListPeers(HS485LogicalInstance* inst, std::vector<std::string>* peers)
{
	if(count==1)return false;
//	LOG(Logger::LOG_DEBUG, "HS485Paramset::ListPeers() %08lX:%d(%d)", inst->GetDevice()->GetAddress(), inst->GetLogicalIndex(), inst->GetPhysicalIndex());
	params_t::iterator it;
	it=params.find(peer_param_id);
	if(it==params.end())return false;
	HSSParameter* peer_param=it->second;
	it=params.find(channel_param_id);
	if(it==params.end())return false;
	HSSParameter* channel_param=it->second;
	for(int i=0;i<count;i++){
		try{
			inst->SetCurParamsetIndex(i);
			XmlRpcValue val;
			if(!channel_param->GetValue(inst, &val)){
				LOG(Logger::LOG_WARNING, "HS485Paramset::ListPeers() could not get channel (i=%d)", i);
				continue;
			}
			if(inst->GetPhysicalIndex() == (int&)val){
				val.clear();
				if(!peer_param->GetValue(inst, &val)){
					LOG(Logger::LOG_WARNING, "HS485Paramset::ListPeers() could not get peer (i=%d)", i);
					continue;
				}
//				LOG(Logger::LOG_DEBUG, "peer=%s", ((std::string&)val).c_str());
				peers->push_back((std::string&)val);
			}
		}catch(XmlRpcException e){
			LOG(Logger::LOG_ERROR, "HS485Paramset::ListPeers() exception %d (%s)", e.getCode(), e.getMessage().c_str());
			return false;
		}
	}
	return true;
}

bool HS485Paramset::AddPeer(HS485LogicalInstance* inst, const std::string& peer)
{
	if(count==1)return false;
//	LOG(Logger::LOG_DEBUG, "HS485Paramset::AddPeer(%s)", peer.c_str());

	//look up in which parameters peer address and channel are stored
	params_t::iterator it;
	it=params.find(peer_param_id);
	if(it==params.end())return false;
	HSSParameter* peer_param=it->second;

	it=params.find(channel_param_id);
	if(it==params.end())return false;
	HSSParameter* channel_param=it->second;

	//loop over all slots to find a free slot to put our new link into
	try{
		int first_free_slot=-1;
		for(int i=0;i<count;i++){
			inst->SetCurParamsetIndex(i);
			XmlRpcValue val;
			if(!channel_param->GetValue(inst, &val))return false;
			int channel=(int&)val;
			if(channel == 0xff){
				if(first_free_slot<0)first_free_slot=i;
			}else if(channel==inst->GetPhysicalIndex()){
				val.clear();
				if(!peer_param->GetValue(inst, &val))return false;
				//peer is already linked
				if((std::string&)val == peer){
					LOG(Logger::LOG_DEBUG, "HS485Paramset::AddPeer(%s) already linked", peer.c_str());
					return true;
				}
			}
		}
		if(first_free_slot<0)return false;
		inst->SetCurParamsetIndex(first_free_slot);
		XmlRpcValue val;
		(std::string&)val=peer;
		if(!peer_param->SetValue(inst, val))return false;
		val.clear();
		(int&)val=inst->GetPhysicalIndex();
		if(!channel_param->SetValue(inst, val))return false;
		SetDefaultValues(inst);
		return true;
	}catch(XmlRpcException){
		return false;
	}
	return false;
}

bool HS485Paramset::RemovePeer(HS485LogicalInstance* inst, const std::string& peer)
{
	if(count==1)return false;
	params_t::iterator it;
	it=params.find(peer_param_id);
	if(it==params.end())return false;
	HSSParameter* peer_param=it->second;
	it=params.find(channel_param_id);
	if(it==params.end())return false;
	HSSParameter* channel_param=it->second;
	for(int i=0;i<count;i++){
		try{
			inst->SetCurParamsetIndex(i);
			XmlRpcValue val;
			if(!channel_param->GetValue(inst, &val))return false;
			if(inst->GetPhysicalIndex() == (int&)val){
				val.clear();
				if(!peer_param->GetValue(inst, &val))return false;
				if((std::string&)val == peer){
					val.clear();
					(int&)val=0xff;
					if(!channel_param->SetValue(inst, val))return false;
					val.clear();
					(std::string&)val="@ffffffff:255";
					if(!peer_param->SetValue(inst, val))return false;
					return true;
				}
			}
		}catch(XmlRpcException){
			return false;
		}
	}
	return false;
}

bool HS485Paramset::SetDefaultValues(HS485LogicalInstance* inst)
{
	LOG(Logger::LOG_DEBUG, "SetDefaultValues(%s)", inst->GetSerial().c_str());
	if(!IsLinkset())return false;
	params_t::iterator it;

	for(it=params.begin();it!=params.end();it++){
		HSSParameter* param=it->second;
		if(param->GetId()==peer_param_id || param->GetId()==channel_param_id)continue;
		XmlRpcValue v;
		v=param->GetLogicalType()->GetDefault();
		try{
			param->SetValue(inst, v);
		}catch(XmlRpcException& e){
			LOG(Logger::LOG_WARNING, "Error setting default value %s=%s:%s", param->GetId().c_str(), v.toText().c_str(), e.getMessage().c_str());
		}
	}
	return true;
}

