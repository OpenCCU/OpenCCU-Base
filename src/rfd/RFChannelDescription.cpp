/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFChannelDescription.cpp: Implementierung der Klasse RFChannelDescription.
//
//////////////////////////////////////////////////////////////////////

#include "RFChannelDescription.h"
#include "RFDeviceDescription.h"
#include "RFLogicalInstance.h"
#include "RFDevice.h"
#include <Logger.h>
#include <typeinfo>
#include <limits.h>
#include <string.h>

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

RFChannelDescription::RFChannelDescription()
{
	index=0;
	count=0;
	sysinfo_count_field=0;
	autoregister_central=false;
	paired=false;
	aes_default=false;
	aes_always=false;
	max_link_peers=INT_MAX;
	direction=0;
	flags=FLG_VISIBLE;
	pair_first_function=pair_second_function=function=FUNCTION_A;
	hidden=false;
	has_team=false;
	aes_cbc = false;
	device = NULL;
	behaviour_param = NULL;
}

RFChannelDescription::~RFChannelDescription()
{
//    LOG(Logger::LOG_DEBUG, "RFChannelDescription::~RFChannelDescription()");

	//Delete subdescriptions if any
	for(unsigned int i = 0; i < vec_subdescriptions.size(); i++) {
		delete vec_subdescriptions[i];
	}
	if(behaviour_param != NULL) {
		delete behaviour_param;
	}
}

bool RFChannelDescription::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "RFChannelDescription::InitFromXml()");
	const char* temp;
	temp=node.getAttribute("type");
	if(!temp){
		if(type!="_subconfig_type_") {
			LOG(Logger::LOG_WARNING, "<channel> attribute \"type\" not found");
			return false;
		}
	}
	else {
		type=temp;
	}

    temp=node.getAttribute("index");
    if(temp)index=strtoul(temp, NULL, 0);

    temp=node.getAttribute("class");
	if(temp)creation_tag=temp;

    temp=node.getAttribute("count");
	if(temp){
		count=strtoul(temp, NULL, 0);
	}else{
		temp=node.getAttribute("count_from_sysinfo");
		if(temp){
			sysinfo_count_field=StructuredFrame::FieldIdFromString(temp);
		}else{
			count=1;
		}
	}

    temp=node.getAttribute("hidden");
	if(temp)hidden=(temp[0]=='t');

    temp=node.getAttribute("team_tag");
	if(temp)team_tag=temp;

    temp=node.getAttribute("ui_flags");
	if(temp){
		if(strstr(temp, "invisible"))flags &= ~FLG_VISIBLE;
		else if(strstr(temp, "visible"))flags|=FLG_VISIBLE;
		if(strstr(temp, "internal"))flags |= FLG_INTERNAL;
	}

    temp=node.getAttribute("aes_default");
    if(temp)aes_default=(temp[0]=='t');

    temp=node.getAttribute("aes_always");
    if(temp)aes_always=(temp[0]=='t');


	temp=node.getAttribute("aes_cbc");
	if(temp){
		aes_cbc = (*temp=='t');
	}

    temp=node.getAttribute("has_team");
    if(temp)has_team=(temp[0]=='t');

	temp=node.getAttribute("max_link_peers");
	if(temp)max_link_peers=strtoul(temp, NULL, 0);

    temp=node.getAttribute("function");
	if(temp){
		function=FunctionFromChar(temp[0]);
	}
    temp=node.getAttribute("pair_function");
	if(temp && temp[0] && temp[1]){
		pair_first_function=FunctionFromChar(temp[0]);
		pair_second_function=FunctionFromChar(temp[1]);
	}else{
		pair_first_function=function;
		pair_second_function=function;
	}

    temp=node.getAttribute("autoregister");
	if(temp){
		autoregister_central=(temp[0]=='t');
	}
    temp=node.getAttribute("paired");
	if(temp){
		paired=temp[0]=='t';
	}
	temp=node.getAttribute("direction");
	if(temp){
		if(strstr(temp, "sender"))direction|=DIRECTION_SENDER;
		if(strstr(temp, "receiver"))direction|=DIRECTION_RECEIVER;
	}
	int i=0;
	XMLNode forwarding_node = node.getChildNode("forward_param_values");
	if(!forwarding_node.isEmpty()) {
		XMLNode forwarding_channel_node = forwarding_node.getChildNode("channel", &i);
		while(!forwarding_channel_node.isEmpty()) {
			temp = forwarding_channel_node.getAttribute("index");
			if(temp) {
				int tempInt = strtol(temp, NULL, 0);
				valueForwardingChannels.push_back(tempInt);
			}
			forwarding_channel_node = forwarding_node.getChildNode("channel", &i);
		}
	}

	i = 0;
    XMLNode ps_node=node.getChildNode("paramset", &i);
	while(!ps_node.isEmpty()){
	    temp=ps_node.getAttribute("type");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<paramset> attribute \"type\" not found");
			return false;
		}
		RFParamset& ps=paramsets[temp];
		if(!ps.InitFromXml(ps_node, root_node))return false;
		ps_node=node.getChildNode("paramset", &i);
	}

    XMLNode description_node=node.getChildNode("description");
	if(!description_node.isEmpty())additional_description.InitFromXml(description_node, root_node);

    XMLNode link_roles_node=node.getChildNode("link_roles");
	if(!link_roles_node.isEmpty()){
		int i=0;
	    XMLNode target_node=link_roles_node.getChildNode("target", &i);
		while(!target_node.isEmpty()){
		    temp=target_node.getAttribute("name");
			if(!temp)return false;
			link_role_t role;
			role.name=temp;
			role.flags=LINK_ROLE_FLAG_RECEIVER;
		    temp=target_node.getAttribute("virtual");
			if( temp && temp[0]=='t' )role.flags|=LINK_ROLE_FLAG_VIRTUAL;
		    temp=target_node.getAttribute("team");
			if( temp && temp[0]=='t' )role.flags|=LINK_ROLE_FLAG_TEAM;
			link_target_roles[role.name]=role;
		    target_node=link_roles_node.getChildNode("target", &i);
		}

		i=0;
	    XMLNode source_node=link_roles_node.getChildNode("source", &i);
		while(!source_node.isEmpty()){
		    temp=source_node.getAttribute("name");
			if(!temp)return false;
			link_role_t role;
			role.name=temp;
			role.flags=LINK_ROLE_FLAG_SENDER;
		    temp=source_node.getAttribute("virtual");
			if( temp && temp[0]=='t' )role.flags|=LINK_ROLE_FLAG_VIRTUAL;
		    temp=source_node.getAttribute("team");
			if( temp && temp[0]=='t' )role.flags|=LINK_ROLE_FLAG_TEAM;
			link_source_roles[role.name]=role;
		    source_node=link_roles_node.getChildNode("source", &i);
		}
	}
	if(!direction){
		if(!link_source_roles.empty())direction|=DIRECTION_SENDER;
		if(!link_target_roles.empty())direction|=DIRECTION_RECEIVER;
	}
	if((direction & (DIRECTION_SENDER|DIRECTION_RECEIVER))==(DIRECTION_SENDER|DIRECTION_RECEIVER)){
		LOG(Logger::LOG_WARNING, "Channel %d specified as both sender and reciever. Add a direction attribute for main direction.", index);
		return false;
	}
	i=0;
	XMLNode enforce_peer_node=node.getChildNode("enforce_link");
    XMLNode value_node=enforce_peer_node.getChildNode("value", &i);
	while(!value_node.isEmpty()){
		temp=value_node.getAttribute("id");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<value> attribute \"id\" not found");
			return false;
		}
		std::string id=temp;

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
		}
		link_enforcements[id]=value;
        value_node=enforce_peer_node.getChildNode("value", &i);
    }


	//Initialize special parameters and subconfigurations
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
		}
		else if(param->GetId()=="CHANNEL_FUNCTION") {//HM-MOD-EM-8
			behaviour_param=param;
		}
		else{
			LOG(Logger::LOG_ERROR, "<channel type=\"%s\"> unknown special parameter %s", type.c_str(), param->GetId().c_str());
			delete param;
		}
		special_param_node=node.getChildNode("special_parameter", &i);
	}

	i=0;
    XMLNode subconfig_node=node.getChildNode("subconfig", &i);
    int subconfigcount = -1;
	while(!subconfig_node.isEmpty()){
		subconfigcount++;
        RFChannelDescription* ch=new RFChannelDescription;
		ch->additional_description=additional_description;
		ch->autoregister_central=autoregister_central;
		ch->device=device;
		ch->index=index;
		//ch->index_offset=index_offset;
		ch->type="_subconfig_type_";
		if(!ch->InitFromXml(subconfig_node, root_node)){
			LOG(Logger::LOG_ERROR, "Error initializing subconfig index %d (index<->0 to n))", subconfigcount);
			delete ch;
			return false;
		}
		if(ch->type=="_subconfig_type_") {//use parents type if not set in subconfig
			ch->type = type;
		}
		//LOG(Logger::LOG_ALL, "Found subdescription in channel %d with type=%s", GetIndex(), ch->type.c_str());
		vec_subdescriptions.push_back(ch);
		subconfig_node=node.getChildNode("subconfig", &i);
	}
    return true;
}

const std::string& RFChannelDescription::GetType()
{
    return type;
}

void RFChannelDescription::SetDevice(RFDeviceDescription *device)
{
    this->device=device;
}

RFParamset* RFChannelDescription::GetParamset(const std::string& key)
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

bool RFChannelDescription::SetEnforcedParameters(RFLogicalInstance* inst)
{
//	LOG(Logger::LOG_DEBUG, "typeid(*inst)=%s", typeid(*inst).name());
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		if(!it->second.SetEnforcedValues(inst))return false;
	}
	for(unsigned int i=0;i<vec_subdescriptions.size();i++){
		if(!vec_subdescriptions[i]->SetEnforcedParameters(inst))return false;
	}
	return true;
}

bool RFChannelDescription::ListParamsets(XmlRpc::XmlRpcValue* list)
{
	int i=0;
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		(*list)[i]=it->first;
		i++;
	}
	return true;
}

void RFChannelDescription::ProcessIncomingFrame(RFLogicalInstance* inst, BidcosFrame& frame, FrameDescription* fd)
{
    paramsets["VALUES"].ProcessIncomingFrame(inst, frame, fd, false);
}

void RFChannelDescription::ProcessForwardedFrame(RFLogicalInstance* inst, BidcosFrame& frame, FrameDescription* fd) {
	paramsets["VALUES"].ProcessIncomingFrame(inst, frame, fd, true);
}


bool RFChannelDescription::SetDefaultConfig(RFLogicalInstance *inst)
{
	bool retVal = false;
	for(paramsets_t::iterator it = paramsets.begin();it != paramsets.end();it++)
	{
		if(it->first == "MASTER")
		{
			retVal = it->second.SetDefaultConfig(inst);
		}
	}
	return retVal;
}
int RFChannelDescription::GetCount(RFDevice* dev)
{
	if(count>0)return count;
	return dev->GetStoredSysinfo()->GetIntValue(sysinfo_count_field);
}

bool RFChannelDescription::SetupInstance(RFLogicalInstance* inst)
{
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		it->second.SetupInstance(inst);
	}
	for(unsigned int i=0;i<vec_subdescriptions.size();i++){
		if(!vec_subdescriptions[i]->SetupInstance(inst))return false;
	}
	return true;
}

int RFChannelDescription::FunctionFromChar(char c)
{
	switch(c){
		case 'A':
		case 'a':
			return FUNCTION_A;
		case 'B':
		case 'b':
			return FUNCTION_B;
		default:
			return FUNCTION_AB;
	}
}

int RFChannelDescription::GetFunction(int index, int other_pair_index){
	if(!paired){
		return function;
	}
	if(!other_pair_index){
		return function;
	}
	return (index<other_pair_index)?pair_first_function:pair_second_function;
}

std::string RFChannelDescription::GetLinkSourceRoles()
{
	std::string result;
	link_roles_t::iterator it;
	for(it=link_source_roles.begin();it!=link_source_roles.end();it++)
	{
		if(it->second.flags & LINK_ROLE_FLAG_TEAM)continue;
		if(result.size())result+=" ";
		result+=it->second.name;
	}
	return result;
}

std::string RFChannelDescription::GetLinkTargetRoles()
{
	std::string result;
	link_roles_t::iterator it;
	for(it=link_target_roles.begin();it!=link_target_roles.end();it++)
	{
		if(it->second.flags & LINK_ROLE_FLAG_TEAM)continue;
		if(result.size())result+=" ";
		result+=it->second.name;
	}
	return result;
}

bool RFChannelDescription::GetLinkRoles(RFChannelDescription* peer, RFChannelDescription::link_role_t* my_role, RFChannelDescription::link_role_t* peer_role)
{
	link_roles_t::iterator my_it;

	for(my_it=link_target_roles.begin();my_it!=link_target_roles.end();my_it++)
	{
		link_roles_t::iterator peer_it=peer->link_source_roles.find(my_it->first);
		if(peer_it != peer->link_source_roles.end()){
			*my_role=my_it->second;
			*peer_role=peer_it->second;
			return true;
		}
	}

	for(my_it=link_source_roles.begin();my_it!=link_source_roles.end();my_it++)
	{
		link_roles_t::iterator peer_it=peer->link_target_roles.find(my_it->first);
		if(peer_it != peer->link_target_roles.end()){
			*my_role=my_it->second;
			*peer_role=peer_it->second;
			return true;
		}
	}

	return false;
}


bool RFChannelDescription::IsAesCbcSupported()
{
	return aes_cbc;
}

RFChannelDescription* RFChannelDescription::GetSubdescription(int index)
{
	if(index<=0)return this;
	index--;
	if((unsigned int)index>=vec_subdescriptions.size())return this;
	return vec_subdescriptions[index];
}

bool RFChannelDescription::HasSubdescriptions()
{
	return !vec_subdescriptions.empty();
}

HSSParameter* RFChannelDescription::GetBehaviourParam()
{
	return behaviour_param;
}


