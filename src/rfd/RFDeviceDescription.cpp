/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFDeviceDescription.cpp: Implementierung der Klasse RFDeviceDescription.
//
//////////////////////////////////////////////////////////////////////

#include "RFDeviceDescription.h"
#include "RFLogicalInstance.h"
#include <Logger.h>
#include <typeinfo>
#include "BidcosFrame.h"
#include "RFDevice.h"
#include <type_registry.h>
#include <limits.h>

using namespace XmlRpc;
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

RFDeviceDescription::RFDeviceDescription()
{
	rx_modes=RX_ALWAYS;
	peering_sysinfo_expect_channel=true;
	supports_aes=false;
	max_link_peers=INT_MAX;
	cyclic_timeout=0;
	version=0;
	flags=FLG_VISIBLE;
	team_description=NULL;
	unreach_check_aes=false;
	//updatable = false;
	rx_mode_default = RX_WAKEUP;
	burstMode = BURST_MODE_SINGLE;//default
}

RFDeviceDescription::~RFDeviceDescription()
{
	channels_t::iterator it;
	for(it=channels.begin();it!=channels.end();it++)
	{
		delete *it;
	}
}

bool RFDeviceDescription::Type::InitFromXml(XMLNode& node, XMLNode& root_node)
{
//    LOG(Logger::LOG_DEBUG, "RFDeviceDescription::Type::InitFromXml()");
    const char* temp=node.getAttribute("name");
	if(!temp){
		LOG(Logger::LOG_WARNING, "<type> attribute \"name\" not found");
		return false;
	}
	name=temp;


	temp=node.getAttribute("updatable");
	isUpdatable = temp && temp[0] == 't';

	
	type=0x00;
	direction=DIR_FROM_DEVICE;

    temp=node.getAttribute("priority");
	if(temp){
		priority=strtol(temp, NULL, 0);
		if(priority<0){
			LOG(Logger::LOG_WARNING, "<type> attribute \"priority\" invalid value");
			return false;
		}
	}

	if(!FrameDescription::InitFromXml(node, root_node))return false;
	if(!params.size())type=-1;
	return true;
}

bool RFDeviceDescription::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp;
    XMLNode types_node=node.getChildNode("supported_types");
	if(types_node.isEmpty()){
		LOG(Logger::LOG_WARNING, "<supported_types> not found");
		return false;
	}

    temp=node.getAttribute("ui_flags");
	if(temp){
		if(strstr(temp, "dontdelete"))flags |= FLG_DONTDELETE;
	}

	temp=node.getAttribute("peering_sysinfo_expect_channel");
	if(temp)peering_sysinfo_expect_channel=(*temp=='t');

	temp=node.getAttribute("supports_aes");
	if(temp)supports_aes=(*temp=='t');

	temp=node.getAttribute("unreach_check_aes");
	if(temp)unreach_check_aes=(*temp=='t');

    temp=node.getAttribute("class");
	if(temp)creation_tag=temp;

	temp=node.getAttribute("rx_modes");
	if(temp){
		rx_modes=0;
		if(strstr(temp, "ALWAYS"))rx_modes|=RX_ALWAYS;
		if(strstr(temp, "BURST"))rx_modes|=RX_BURST;
		if(strstr(temp, "CONFIG"))rx_modes|=RX_CONFIG;
		if(strstr(temp, "WAKEUP"))rx_modes|=RX_WAKEUP;
		if(strstr(temp, "LAZY_CONFIG"))rx_modes|=RX_LAZY_CONFIG;
		if(strstr(temp, "TRIPLE_BURST")) {
			rx_modes|=RX_BURST;
			burstMode = BURST_MODE_TRIPLE;
		}
	}

	if( ((rx_modes & RX_BURST_AND_WAKEUP) == RX_BURST_AND_WAKEUP) ) { //device supports both, WAKEUP and BURST. Check for rx_default attribute. If not set WAKEUP is set as default.
		temp = node.getAttribute("rx_default");
		if(temp) {
			if(strstr(temp, "WAKEUP")) { rx_mode_default=RX_WAKEUP; }
			else if(strstr(temp, "BURST")) { rx_mode_default=RX_BURST; }
		}
	}

	temp=node.getAttribute("max_link_peers");
	if(temp)max_link_peers=strtoul(temp, NULL, 0);
	
	

	temp=node.getAttribute("version");
	if(temp)version=strtol(temp, NULL, 0);

    temp=node.getAttribute("cyclic_timeout");
	if(temp){
		cyclic_timeout=strtoul(temp, NULL, 0)*1000;
	}

    int i=0;
    XMLNode type_node=types_node.getChildNode("type", &i);
    while(!type_node.isEmpty()){
        Type t;
        if(!t.InitFromXml(type_node, root_node))return false;
        supported_types.push_back(t);
        type_node=types_node.getChildNode("type", &i);
    }

	i=0;
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

    XMLNode channels_node=node.getChildNode("channels");
    if(!channels_node.isEmpty()){
        int i=0;
        XMLNode channel_node=channels_node.getChildNode("channel", &i);
        while(!channel_node.isEmpty()){
            RFChannelDescription* ch=new RFChannelDescription;
            ch->SetDevice(this);
			ch->SetIndex(channels.size());
            if(!ch->InitFromXml(channel_node, root_node))return false;
            channels.push_back(ch);
            channel_node=channels_node.getChildNode("channel", &i);
        }
    }

    XMLNode frames_node=node.getChildNode("frames");
	if(!frames_node.isEmpty()){
		int i=0;
	    XMLNode frame_node=frames_node.getChildNode("frame", &i);
		while(!frame_node.isEmpty()){
			framedefs.push_back(FrameDescription());
			FrameDescription& fd=framedefs.back();
			if(!fd.InitFromXml(frame_node, root_node))return false;
			framedefs_by_id[fd.GetId()]=framedefs.size()-1;
			frame_node=frames_node.getChildNode("frame", &i);
		}
	}

	XMLNode description_node=node.getChildNode("description");
	if(!description_node.isEmpty())additional_description.InitFromXml(description_node, root_node);

	XMLNode team_node=node.getChildNode("team");
	if(!team_node.isEmpty()){
		team_description=new RFDeviceDescription();
		team_description->creation_tag="team";
		team_description->version=version;
		if(!team_description->InitFromXml(team_node, team_node))return false;
	}
    return true;
}

bool RFDeviceDescription::RxNeedsWakeup()
{
	return (rx_modes & RX_WAKEUP)!=0;
}

bool RFDeviceDescription::RxNeedsBurst()
{
	return (rx_modes & RX_BURST)!=0;
}


int RFDeviceDescription::Matches(BidcosFrame& sysinfo_frame, std::string* type_id)
{
    types_t::iterator it;
	int priority=-1;
    for(it=supported_types.begin();it!=supported_types.end();it++){
        if(it->GetType()>=0 && it->MatchFrame(sysinfo_frame)){
			if(it->GetPriority()>priority){
				*type_id=it->GetId();
				priority=it->GetPriority();
			}
		}
    }
    return priority;
}

RFChannelDescription* RFDeviceDescription::GetChannelDescription(int i)
{
	if(i<(int)channels.size())return channels[i];
	return NULL;
}

RFChannelDescription* RFDeviceDescription::GetChannelDescription(const std::string& type)
{
	for(unsigned int i=0;i<channels.size();i++){
		if(channels[i]->GetType()==type)return channels[i];
	}
	return NULL;
}

unsigned int RFDeviceDescription::GetChannelCount()
{
	return channels.size();
}

RFParamset* RFDeviceDescription::GetParamset(const std::string& key)
{
	paramsets_t::iterator it=paramsets.find(key);
	if(it==paramsets.end()){
		LOG(Logger::LOG_ERROR, "Parameterset %s not found", key.c_str());
		return NULL;
	}
	return &(it->second);
}

bool RFDeviceDescription::SetEnforcedParameters(RFLogicalInstance* inst)
{
//	LOG(Logger::LOG_DEBUG, "typeid(*inst)=%s", typeid(*inst).name());
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		if(!it->second.SetEnforcedValues(inst))return false;
	}
	return true;
}
bool RFDeviceDescription::SetDefaultConfig(RFLogicalInstance *inst)
{
	bool retVal = true;
	for(paramsets_t::iterator it = paramsets.begin();it != paramsets.end();it++)
	{
		if(it->first == "MASTER")
		{
			retVal = it->second.SetDefaultConfig(inst);
		}
	}
	return retVal;
}
bool RFDeviceDescription::ListParamsets(XmlRpc::XmlRpcValue* list)
{
	int i=0;
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		(*list)[i]=it->first;
		i++;
	}
	return true;
}

bool RFDeviceDescription::SupportsType(const std::string& type)
{
    types_t::iterator it;
    for(it=supported_types.begin();it!=supported_types.end();it++){
        if(it->GetId()==type){
			return true;
		}
    }
    return false;
}

FrameDescription* RFDeviceDescription::GetFrameDescription(StructuredFrame& frame, int* channel, int* iterator)
{
	while((unsigned int)*iterator < framedefs.size()){
		FrameDescription& fd=framedefs[(*iterator)++];
		if(fd.MatchFrame(frame, channel))return &fd;
	}
	return NULL;
};

RFDevice* RFDeviceDescription::CreateDevice()
{
	RFDevice* dev=NULL;
//	LOG(Logger::LOG_DEBUG, "RFDeviceDescription::CreateDevice() creation_tag=\"%s\"", creation_tag.c_str());
	if(!creation_tag.empty()){
		std::string full_tag="device_class_";
		full_tag+=creation_tag;
		void* obj=hsscomm::type_registry::create(full_tag.c_str());
		if(!obj){
			LOG(Logger::LOG_ERROR, "Device class %s not supported", creation_tag.c_str());
			return NULL;
		}
		dev=dynamic_cast<RFDevice*>((RFDevice*)obj);
		if(!dev){
			/* TODO: get rid of the allocated memory */
			//delete obj;
			return NULL;
		}
	}else{
		dev=new RFDevice;
	}
	if(supported_types.size())dev->SetType(supported_types[0].GetId());
	return dev;
}
bool RFDeviceDescription::IsUpdatable(StructuredFrame &sysinfo)
{
	

	types_t::iterator it;
    for(it=supported_types.begin();it!=supported_types.end();it++){
		if(it->MatchFrame(sysinfo)) {
			return it->IsUpdatable();
		}
	/*	if(it->GetId()==type && it->IsUpdatable()){//type match?
			for(int p = 0; p < it->params.size() ; p++) {
				if(it->params.at(p).index_bytes == 9) {//check fw version
					bool versionMatch = it->params.at(p).FrameCheckValue(sysinfo);
					if(versionMatch) {
						return true;
					}
				}
			}
		}*/
    }
    return false;
}

RFDeviceDescription::BurstMode RFDeviceDescription::GetBurstMode() {
	return burstMode;
}
