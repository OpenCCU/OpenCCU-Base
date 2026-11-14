/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFParamset.cpp: Implementierung der Klasse RFParamset.
//
//////////////////////////////////////////////////////////////////////

#include "RFParamset.h"
#include "HSSParameter.h"
#include "RFDeviceDescription.h"
#include "RFDevice.h"
#include "RFController.h"
#include "RFCentral.h"
#include "RFChannel.h"
#include <Logger.h>
#include <typeinfo>
#include <HSSLogicalType.h>

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

RFParamset::RFParamset()
{
	is_linkset=false;
	vec_default_values.resize(3);
}

RFParamset::~RFParamset()
{
}
bool RFParamset::SetDefaultConfig(RFLogicalInstance* inst)
{
	return HSSParamset::SetDefaultConfig(inst);
}
bool RFParamset::InitFromXml(XMLNode &node, XMLNode& root_node)
{
    if(!HSSParamset::InitFromXml(node, root_node))return false;

	is_linkset=(type=="LINK");
    const char* temp;

	int i=0;
	XMLNode default_values_node=node.getChildNode("default_values", &i);
	while(!default_values_node.isEmpty()){
		temp=default_values_node.getAttribute("function");
		int function_mask=0;
		if(!temp){
			function_mask=7;
		}else{
			std::string s=temp;
			for(std::string::size_type left=0;left<s.size();){
				left=s.find_first_of("AB", left);
				if(left==std::string::npos)break;
				std::string::size_type right=s.find_first_not_of("AB", left);
				if(s.substr(left, right-left)=="AB")function_mask|=(1<<RFChannelDescription::FUNCTION_AB);
				else if(s.substr(left, right-left)=="A")function_mask|=(1<<RFChannelDescription::FUNCTION_A);
				else if(s.substr(left, right-left)=="B")function_mask|=(1<<RFChannelDescription::FUNCTION_B);
				left=right;
			}
		}
		if(!function_mask){
			LOG(Logger::LOG_WARNING, "<default_values_node> invalid function specification \"%s\"", temp);
			return false;
		}

		int j=0;
	    XMLNode value_node=default_values_node.getChildNode("value", &j);
		while(!value_node.isEmpty()){
			temp=value_node.getAttribute("id");
			if(!temp){
				LOG(Logger::LOG_WARNING, "<value> attribute \"id\" not found");
				return false;
			}
			HSSParameter* param=GetParameter(temp);
			if(!param){
				LOG(Logger::LOG_WARNING, "default value: parameter \"%s\" does not exist");
				return false;
			}

			temp=value_node.getAttribute("value");
			if(!temp){
				LOG(Logger::LOG_WARNING, "<value> attribute \"value\" not found");
				return false;
			}
			XmlRpcValue value;
			if(temp[0]=='$'){
				(std::string&)value=temp;
			}else{
				std::string s_value=temp;
				if(!value.fromText(s_value)){
					(std::string&)value=s_value;
				}
				if(!param->CheckType(value)){
					LOG(Logger::LOG_ERROR, "Default value type check failed: %s=\"%s\"", param->GetId().c_str(), value.toText().c_str());
					return false;
				}
			}
			for(unsigned int function=0;function<vec_default_values.size();function++){
				if(function_mask&(1<<function))vec_default_values[function][param->GetId()]=value;
			}
//			LOG(Logger::LOG_DEBUG, "Default value %s[%d]=%s", param->GetId().c_str(), function_mask, value.toText().c_str());
	        value_node=default_values_node.getChildNode("value", &j);
		}
		default_values_node=node.getChildNode("default_values", &i);
    }
    return true;
}

bool RFParamset::Get(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue *set, int mode)
{
//	LOG(Logger::LOG_DEBUG, "RFParamset::Get");
	if(is_linkset)dynamic_cast<RFLogicalInstance*>(inst)->SetCurParamsetPeer(peer);
	else dynamic_cast<RFLogicalInstance*>(inst)->SetCurParamsetPeer(0, 0);
    
    return HSSParamset::Get(inst, peer, set,mode);
}

bool RFParamset::Put(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue& set)
{
	if(is_linkset)dynamic_cast<RFLogicalInstance*>(inst)->SetCurParamsetPeer(peer);
	else dynamic_cast<RFLogicalInstance*>(inst)->SetCurParamsetPeer(0, 0);

    return HSSParamset::Put(inst, peer, set);
}

bool RFParamset::SetEnforcedValues(RFLogicalInstance* inst, RFChannel* peer/*=NULL*/)
{
	uint32_t curParamsetPeer = inst->GetCurParamsetPeer();

//	LOG(Logger::LOG_DEBUG, "SetEnforcedValues()");
	if(!peer){
		if(is_linkset)return true;
		inst->SetCurParamsetPeer(0, 0);
	}else{
		if(!is_linkset)return true;
		inst->SetCurParamsetPeer(peer->GetDevice()->GetAddress(), peer->GetIndex());
	}
	//Set our own enforced values
	enforced_values_t::iterator it;
	for(it=enforced_values.begin();it!=enforced_values.end();it++)
	{
        HSSParameter* param=GetParameter(it->first);
		if(!param){
			LOG(Logger::LOG_WARNING, "Enforced value %s not in parameter set", it->first.c_str());
			continue;
		}
		XmlRpcValue value=ResolveConfigValue(it->second, inst, peer);
		if(value.getType()==XmlRpcValue::TypeInvalid)continue;
		LOG(Logger::LOG_DEBUG, "Enforce %s=%s", it->first.c_str(), value.toText().c_str());
		if(!param->SetValue(inst, value)){
			LOG(Logger::LOG_WARNING, "SetEnforcedValues() could not set %s=%s", it->first.c_str(), value.toText().c_str());
		}
	}
	if(peer){
		//Set the enforced values given by the peer
		for(it=peer->GetDescription()->GetLinkEnforcements().begin();it!=peer->GetDescription()->GetLinkEnforcements().end();it++)
		{
			HSSParameter* param=GetParameter(it->first);
			if(!param){
				LOG(Logger::LOG_WARNING, "Peer-enforced value %s not in parameter set", it->first.c_str());
				continue;
			}
			XmlRpcValue value=ResolveConfigValue(it->second, peer, inst);
			if(value.getType()==XmlRpcValue::TypeInvalid){
				LOG(Logger::LOG_WARNING, "Peer-enforced value %s could not be resolved", it->first.c_str());
				continue;
			}
			if(!param->SetValue(inst, value)){
				LOG(Logger::LOG_WARNING, "SetEnforcedValues() could not set %s=%s", it->first.c_str(), value.toText().c_str());
			}
		}
	}
	
	inst->SetCurParamsetPeer(curParamsetPeer);	
	return true;
}

bool RFParamset::SetDefaultValues(RFLogicalInstance* inst, RFChannel* peer)
{
	LOG(Logger::LOG_DEBUG, "SetDefaultValues(%s, %s)", inst->GetSerial().c_str(), peer->GetSerial().c_str());
	if(!is_linkset)return false;
	inst->SetCurParamsetPeer(peer->GetDevice()->GetAddress(), peer->GetIndex());
	unsigned int function=peer->GetFunction();
	if(function>=vec_default_values.size()){
		LOG(Logger::LOG_WARNING, "%s: no default values for function %d", inst->GetSerial().c_str(), function);
		return false;
	}
	enforced_values_t& default_values=vec_default_values[function];

	params_t::iterator it;
	for(it=params.begin();it!=params.end();it++){
		HSSParameter* param=it->second;
		enforced_values_t::iterator def_it=default_values.find(param->GetId());
		XmlRpcValue v;
		if(def_it!=default_values.end()){
			v=ResolveConfigValue(def_it->second, inst, peer);
		}else{
			v=param->GetLogicalType()->GetDefault();
		}
		try{
			param->SetValue(inst, v);
		}catch(XmlRpcException& e){
			LOG(Logger::LOG_WARNING, "Error setting default value %s=%s:%s", param->GetId().c_str(), v.toText().c_str(), e.getMessage().c_str());
		}
	}
	return true;
}

XmlRpcValue RFParamset::ResolveConfigValue(XmlRpc::XmlRpcValue& config_value, RFLogicalInstance* inst, RFLogicalInstance* peer)
{
	XmlRpcValue value;
	if(config_value.getType()==XmlRpcValue::TypeString && ((std::string&)config_value).size() && ((std::string&)config_value)[0]=='$'){
		std::string& s_value=config_value;
		std::string::size_type dotpos=s_value.find('.');
		std::string scope;
		std::string name;
		if(dotpos!=std::string::npos){
			scope=s_value.substr(1, dotpos-1);
			name=s_value.substr(dotpos+1);
		}else{
			name=s_value;
		}

		if(scope=="CHANNEL" || scope==""){
			inst->GetInternalValue(name, &value);
		}else if(scope=="PEER" && peer){
			peer->GetInternalValue(name, &value);
		}else if(scope=="PEER_DEVICE" && peer){
			peer->GetDevice()->GetInternalValue(name, &value);
		}else if(scope=="DEVICE"){
			inst->GetDevice()->GetInternalValue(name, &value);
		}else if(scope=="CENTRAL" && RFCentral::GetSingleton()){
			RFCentral::GetSingleton()->GetInternalValue(name, &value);
		}else{
			LOG(Logger::LOG_DEBUG, "SetEnforcedValues() could not resolve %s", s_value.c_str());
		}
	}else{
		value=config_value;
	}
	return value;
}

bool RFParamset::Determine(RFLogicalInstance* inst, const std::string& peer, const std::string& value_id)
{
	if(is_linkset)inst->SetCurParamsetPeer(peer);
	else inst->SetCurParamsetPeer(0, 0);

	params_t::iterator it=params.find(value_id);
	if(it==params.end()){
		throw XmlRpcException("Unknown Parameter", -5);
	}

	if(!it->second->IsDeterminable()){
		throw XmlRpcException("Operation not supported", -6);
	}

	return it->second->DetermineValue(inst);
}

bool RFParamset::IsCompatible(RFParamset *newParamset)
{
	if(!newParamset)
	{
		return false;
	}
	params_t::iterator it_oldParams;
	params_t::iterator it_newParams;
	for(it_oldParams = this->params.begin();it_oldParams != this->params.end(); ++it_oldParams)
	{
		
		if(newParamset->params.find(it_oldParams->first) == newParamset->params.end())
		{
			return false;
		}
	}
	return true;
}
