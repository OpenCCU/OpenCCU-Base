/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485ChannelDescription.cpp: Implementierung der Klasse HS485ChannelDescription.
//
//////////////////////////////////////////////////////////////////////

#include "HS485ChannelDescription.h"
#include "HS485DeviceDescription.h"
#include "HS485LogicalInstance.h"
#include <Logger.h>
#include <typeinfo>
#include <cstring>

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HS485ChannelDescription::HS485ChannelDescription()
{
	index_offset=0;
	index=0;
	count=0;
	direction=0;
	virtual_links=false;
	autoregister_central=false;
	behaviour_param=NULL;
	flags=FLG_VISIBLE;
	hidden=false;
}

HS485ChannelDescription::~HS485ChannelDescription()
{
	for(unsigned int i=0;i<vec_subdescriptions.size();i++){
		delete vec_subdescriptions[i];
	}
	if(behaviour_param)delete behaviour_param;
}

bool HS485ChannelDescription::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HS485ChannelDescription::InitFromXml()");
    const char* temp;
	if(type.empty()){
		temp=node.getAttribute("type");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<channel> attribute \"type\" not found");
			return false;
		}
		type=temp;
	}

    temp=node.getAttribute("physical_index_offset");
    if(temp)index_offset=strtol(temp, NULL, 0);

    temp=node.getAttribute("index");
    if(temp)index=strtoul(temp, NULL, 0);

    temp=node.getAttribute("class");
	if(temp)creation_tag=temp;

    temp=node.getAttribute("hidden");
	if(temp){
		hidden=(temp[0]=='t');
	}

    temp=node.getAttribute("count");
	if(temp){
		count=strtoul(temp, NULL, 0);
	}

    temp=node.getAttribute("test_paramset_frame_short");
	if(temp){
		test_paramset_frame_short=temp;
	}

    temp=node.getAttribute("test_paramset_frame_long");
	if(temp){
		test_paramset_frame_long=temp;
	}

	temp=node.getAttribute("ui_flags");
	if(temp){
		if(strstr(temp, "invisible"))flags &= ~FLG_VISIBLE;
		else if(strstr(temp, "visible"))flags|=FLG_VISIBLE;
		if(strstr(temp, "internal"))flags |= FLG_INTERNAL;
	}

    temp=node.getAttribute("autoregister");
	if(temp){
		autoregister_central=(temp[0]=='t');
	}
	temp=node.getAttribute("direction");
	if(temp){
		if(strstr(temp, "sender"))direction|=DIRECTION_SENDER;
		if(strstr(temp, "receiver"))direction|=DIRECTION_RECEIVER;
	}else{
		direction=DIRECTION_NONE;
	}

    temp=node.getAttribute("virtual_links");
	if(temp)virtual_links = *temp=='t';

	int i=0;
    XMLNode ps_node=node.getChildNode("paramset", &i);
	while(!ps_node.isEmpty()){
	    temp=ps_node.getAttribute("type");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<paramset> attribute \"type\" not found");
			return false;
		}
		paramsets.erase(temp);
		HS485Paramset& ps=paramsets[temp];
		if(!ps.InitFromXml(ps_node, root_node))return false;
		ps_node=node.getChildNode("paramset", &i);
	}

    XMLNode link_roles_node=node.getChildNode("link_roles");
	if(!link_roles_node.isEmpty()){
		link_target_roles.clear();
		link_source_roles.clear();
		int i=0;
	    XMLNode target_node=link_roles_node.getChildNode("target", &i);
		while(!target_node.isEmpty()){
		    temp=target_node.getAttribute("name");
			if(!temp){
				LOG(Logger::LOG_WARNING, "<target> attribute \"name\" not found");
				return false;
			}
			if(!link_target_roles.empty())link_target_roles+=" ";
			link_target_roles+=temp;
		    target_node=link_roles_node.getChildNode("target", &i);
		}

		i=0;
	    XMLNode source_node=link_roles_node.getChildNode("source", &i);
		while(!source_node.isEmpty()){
		    temp=source_node.getAttribute("name");
			if(!temp){
				LOG(Logger::LOG_WARNING, "<source> attribute \"name\" not found");
				return false;
			}
			if(!link_source_roles.empty())link_source_roles+=" ";
			link_source_roles+=temp;
		    source_node=link_roles_node.getChildNode("source", &i);
		}
	}
	if(!direction){
		if(!link_source_roles.empty())direction|=DIRECTION_SENDER;
		if(!link_target_roles.empty())direction|=DIRECTION_RECEIVER;
	}

    XMLNode description_node=node.getChildNode("description");
	if(!description_node.isEmpty())additional_description.InitFromXml(description_node, root_node);

	i=0;
    XMLNode special_param_node=node.getChildNode("special_parameter", &i);
	while(!special_param_node.isEmpty()){
        HSSParameter* param=new HSSParameter;
        if(!param->InitFromXml(special_param_node, root_node)){
			LOG(Logger::LOG_ERROR, "<channel type=\"%s\"> could not initialize special parameter", type.c_str());
			delete param;
            return false;
        }
		if(param->GetId()=="BEHAVIOUR"){
			behaviour_param=param;
		}else{
			LOG(Logger::LOG_ERROR, "<channel type=\"%s\"> unknown special parameter %s", type.c_str(), param->GetId().c_str());
			delete param;
		}
		special_param_node=node.getChildNode("special_parameter", &i);
	}

	i=0;
    XMLNode subconfig_node=node.getChildNode("subconfig", &i);
	while(!subconfig_node.isEmpty()){
        HS485ChannelDescription* ch=new HS485ChannelDescription;
		ch->additional_description=additional_description;
		ch->autoregister_central=autoregister_central;
		ch->device=device;
		ch->index=index;
		ch->index_offset=index_offset;
		ch->type=type;
		if(!ch->InitFromXml(subconfig_node, root_node)){
			delete ch;
			return false;
		}
		vec_subdescriptions.push_back(ch);
		subconfig_node=node.getChildNode("subconfig", &i);
	}

    return true;
}

const std::string& HS485ChannelDescription::GetType()
{
    return type;
}

void HS485ChannelDescription::SetDevice(HS485DeviceDescription *device)
{
    this->device=device;
}

bool HS485ChannelDescription::GetLinkPeers(HS485LogicalInstance* inst, std::vector<std::string>* peers)
{
	if(virtual_links){
		return true;
	}else{
		paramsets_t::iterator it=paramsets.find("LINK");
		if(it==paramsets.end())return false;
		return it->second.ListPeers(inst, peers);
	}
}

bool HS485ChannelDescription::RemoveLinkPeer(HS485LogicalInstance* inst, const std::string& peer)
{
	if(virtual_links)return true;
	paramsets_t::iterator it=paramsets.find("LINK");
	if(it==paramsets.end())return false;
	return it->second.RemovePeer(inst, peer);
}

bool HS485ChannelDescription::AddLinkPeer(HS485LogicalInstance* inst, const std::string& peer)
{
	if(virtual_links)return true;
	paramsets_t::iterator it=paramsets.find("LINK");
	if(it==paramsets.end())return false;
	return it->second.AddPeer(inst, peer);
}

bool HS485ChannelDescription::SetEnforcedParameters(HS485LogicalInstance* inst)
{
//	LOG(Logger::LOG_DEBUG, "HS485ChannelDescription::SetEnforcedParameters() inst->GetLogicalIndex()=%d, inst->GetPhysicalIndex()=%d", inst->GetLogicalIndex(), inst->GetPhysicalIndex());
//	LOG(Logger::LOG_DEBUG, "typeid(*inst)=%s", typeid(*inst).name());
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		if(!it->second.SetEnforcedValues(inst))return false;
	}
	for(unsigned int i=0;i<vec_subdescriptions.size();i++){
		if(!vec_subdescriptions[i]->SetEnforcedParameters(inst))return false;
	}
	return true;
}

bool HS485ChannelDescription::ListParamsets(XmlRpc::XmlRpcValue* list)
{
	int i=0;
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		(*list)[i]=it->first;
		i++;
	}
	return true;
}

void HS485ChannelDescription::ProcessIncomingFrame(HS485LogicalInstance* inst, HS485Frame& frame, FrameDescription* fd)
{
	paramsets_t::iterator it=paramsets.find("VALUES");
	if(it==paramsets.end())return;
    it->second.ProcessIncomingFrame(inst, frame, fd);
}

bool HS485ChannelDescription::SetupInstance(HS485LogicalInstance* inst)
{
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		it->second.SetupInstance(inst);
	}
	for(unsigned int i=0;i<vec_subdescriptions.size();i++){
		if(!vec_subdescriptions[i]->SetupInstance(inst))return false;
	}
	return true;
}

HS485Paramset* HS485ChannelDescription::GetParamset(const std::string& key)
{
	paramsets_t::iterator it=paramsets.find(key);
	if(it==paramsets.end()){
		it=paramsets.find("LINK");
	}
	if(it==paramsets.end()){
		LOG(Logger::LOG_ERROR, "Parameterset %s not found", key.c_str());
		return NULL;
	}
	return &(it->second);
}

HS485ChannelDescription* HS485ChannelDescription::GetSubdescription(int index)
{
	if(index<=0)return this;
	index--;
	if((unsigned int)index>=vec_subdescriptions.size())return this;
	return vec_subdescriptions[index];
}
