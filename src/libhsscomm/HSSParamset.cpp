/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSParamset.cpp: Implementierung der Klasse HSSParamset.
//
//////////////////////////////////////////////////////////////////////

#include "HSSParamset.h"
#include <Logger.h>
#include <typeinfo>
#include "FrameDescription.h"
#include "CommMessage.h"

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSParamset::HSSParamset()
{
}

HSSParamset::~HSSParamset()
{
	params_t::iterator it;
	for(it=params.begin();it!=params.end();it++){
		HSSParameter* param=it->second;
		delete param;
	}
}

bool HSSParamset::InitFromXml(XMLNode &node, XMLNode& root_node)
{
    const char* temp;
	if(id.empty()){
		temp=node.getAttribute("id");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<paramset> attribute \"id\" not found");
			return false;
		}
		id=temp;
	}

	if(type.empty()){
		temp=node.getAttribute("type");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<paramset> attribute \"type\" not found");
			return false;
		}
		type=temp;
	}

	int i=0;
	int tab_order=0;
    XMLNode param_node=node.getChildNode("parameter", &i);
    while(!param_node.isEmpty()){
        HSSParameter* param=new HSSParameter;
		param->SetParamset(this);
        if(!param->InitFromXml(param_node, root_node)){
			LOG(Logger::LOG_ERROR, "<paramset id=\"%s\"> could not initialize parameter", id.c_str());
			delete param;
            return false;
        }
		if((param->GetFlags() & (HSSParameter::FLG_INTERNAL|HSSParameter::FLG_VISIBLE))==HSSParameter::FLG_VISIBLE){
			param->SetTabOrder(tab_order++);
		}
		if(params.find(param->GetId()) != params.end()) {
			//if param already exist, object gets overwritten and never deleted afterwards
			//Would be a minor leak
			HSSParameter* pParam = params[param->GetId()];
			if(pParam != NULL) {
				delete pParam;
			}
			params.erase(param->GetId());
		}
        params[param->GetId()]=param;
        param_node=node.getChildNode("parameter", &i);
    }
	params_t::iterator it;
	for(it=params.begin();it!=params.end();it++){
		if(it->second->GetTabOrder()<0)it->second->SetTabOrder(tab_order++);
	}
    i=0;
//	LOG(Logger::LOG_DEBUG, "Parsing subsets");
    XMLNode subset_node=node.getChildNode("subset", &i);
    while(!subset_node.isEmpty()){
		temp=subset_node.getAttribute("ref");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<subset> attribute \"ref\" not found");
			return false;
		}
		std::string ref=temp;
	    XMLNode paramset_defs_node=root_node.getChildNode("paramset_defs");
		if(paramset_defs_node.isEmpty()){
			LOG(Logger::LOG_WARNING, "node <paramset_defs> not found");
			return false;
		}
        int j=0;
        XMLNode paramset_node=paramset_defs_node.getChildNode("paramset", &j);
        while(!paramset_node.isEmpty()){
			temp=paramset_node.getAttribute("id");
			if(!temp){
				LOG(Logger::LOG_WARNING, "<paramset> attribute \"id\" not found");
				return false;
			}
			if(ref==temp){
				if(!InitFromXml(paramset_node, root_node))return false;
				break;
			}
            paramset_node=paramset_defs_node.getChildNode("paramset", &j);
        }
		subset_node=node.getChildNode("subset", &i);
    }

	i=0;
//	LOG(Logger::LOG_DEBUG, "Parsing enforcements");
    XMLNode enforce_node=node.getChildNode("enforce", &i);
    while(!enforce_node.isEmpty()){
		temp=enforce_node.getAttribute("id");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<enforce> attribute \"id\" not found");
			return false;
		}
        HSSParameter* param=GetParameter(temp);
		if(!param){
			LOG(Logger::LOG_WARNING, "<enforce id=\"%s\"> parameter not found", temp);
			return false;
		}

		temp=enforce_node.getAttribute("value");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<enforce> attribute \"value\" not found");
			return false;
		}
		XmlRpcValue value;
		if(temp[0]=='$'){
			(std::string&)value=temp;
		}else{
			std::string s_value=temp;
//			LOG(Logger::LOG_DEBUG, "Enforced value %s=%s", param->GetId().c_str(), temp);
			if(!value.fromText(s_value)){
				LOG(Logger::LOG_ERROR, "Could not parse enforced value");
				return false;
			}
			if(!param->CheckType(value)){
				LOG(Logger::LOG_ERROR, "Enforced value type check failed: %s", value.toText().c_str());
				return false;
			}
		}
		enforced_values[param->GetId()]=value;
        enforce_node=node.getChildNode("enforce", &i);
    }
    return true;
}

bool HSSParamset::Get(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue *set,int mode)
{
	params_t::iterator it;
	for(it=params.begin();it!=params.end();it++){
		HSSParameter* param=it->second;
		if(!param->IsReadable())continue;
		XmlRpcValue entry;
		if(param->GetValue(inst,mode, &entry) && entry.valid()){
			(*set)[param->GetId()]=entry;
		}else{
			return false;
		}
	}
    return true;
}

bool HSSParamset::Put(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue& set)
{
    XmlRpcValue::ValueStruct& value_struct=(XmlRpc::XmlRpcValue::ValueStruct&)set;

    XmlRpcValue::ValueStruct::iterator it;
    for(it=value_struct.begin();it!=value_struct.end();it++){
        const std::string& id=it->first;
        XmlRpcValue& val=it->second;

        params_t::iterator it2=params.find(id);
        if(it2!=params.end()){
            HSSParameter* param=it2->second;
			if(!param->IsWriteable())continue;
            param->SetValue(inst, val);
        }
    }
    return true;
}
bool HSSParamset::SetDefaultConfig(LogicalInstance *inst)
{
	for(params_t::iterator it = params.begin();it != params.end();it++)
	{
		it->second->SetDefaultConfig(inst);
	}
	return true;
}
bool HSSParamset::GetDefinition(XmlRpc::XmlRpcValue *set)
{
//	LOG(Logger::LOG_DEBUG, "HSSParamset::GetDefinition");
    params_t::iterator it;
    for(it=params.begin();it!=params.end();it++){
        HSSParameter* param=it->second;
		if(!param->IsVisible())continue;
        XmlRpcValue& entry=(*set)[param->GetId()];
		param->GetDescription(&entry);
//		LOG(Logger::LOG_DEBUG, entry.toText().c_str());
    }
    return true;
}

HSSParameter* HSSParamset::GetParameter(const std::string& id)
{
    params_t::iterator it=params.find(id);
	if(it!=params.end())return it->second;

	return NULL;
}

const std::string& HSSParamset::GetId()
{
	return id;
}

const std::string& HSSParamset::GetType()
{
	return type;
}

bool HSSParamset::SetupInstance(LogicalInstance* inst)
{
	params_t::iterator it;
	for(it=params.begin();it!=params.end();it++){
		HSSParameter* param=it->second;
		param->SetupInstance(inst);
	}
	return true;
}

void HSSParamset::ProcessIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, bool forwardedFrame)
{
	if(IsLinkset())return;
	t_vec_params& subscribed_params=map_event_subscriptions[fd->GetId()];
    t_vec_params::iterator it;
    for(it=subscribed_params.begin();it!=subscribed_params.end();it++){
    	if(forwardedFrame) {
    		if((*it)->IsForwardingEnabled()) {
    			(*it)->ProcessIncomingFrame(inst, frame, fd);
    		}
    	}
    	else {
    		(*it)->ProcessIncomingFrame(inst, frame, fd);
    	}
    }
}

void HSSParamset::SubscribeToEventFrame(HSSParameter* param, const std::string& frame_id)
{
	map_event_subscriptions[frame_id].push_back(param);
}
