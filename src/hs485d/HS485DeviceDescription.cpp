/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485DeviceDescription.cpp: Implementierung der Klasse HS485DeviceDescription.
//
//////////////////////////////////////////////////////////////////////

#include "HS485DeviceDescription.h"
#include "HS485LogicalInstance.h"
#include <FrameDescription.h>
#include <Logger.h>
#include <typeinfo>
#include <dynamic.h>
#include "HS485Device.h"

using namespace XmlRpc;
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HS485DeviceDescription::HS485DeviceDescription()
{
	eep_size=0;
	has_serial=true;
	channels_have_subdescriptions=false;
	flags=FLG_VISIBLE;
	version=0;
//    LOG(Logger::LOG_DEBUG, "HS485DeviceDescription::HS485DeviceDescription()");
}

HS485DeviceDescription::~HS485DeviceDescription()
{
//    LOG(Logger::LOG_DEBUG, "HS485DeviceDescription::~HS485DeviceDescription()");
	channels_t::iterator it;
	for(it=channels.begin();it!=channels.end();it++)
	{
		delete *it;
	}
}

bool HS485DeviceDescription::Type::InitFromXml(XMLNode& node, XMLNode& root_node)
{
//    LOG(Logger::LOG_DEBUG, "RFDeviceDescription::Type::InitFromXml()");
    const char* temp=node.getAttribute("name");
	if(!temp){
		LOG(Logger::LOG_WARNING, "<type> attribute \"name\" not found");
		return false;
	}
	name=temp;
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


bool HS485DeviceDescription::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    const char* temp=node.getAttribute("eep_size");
    if(temp)eep_size=strtoul(temp, NULL, 0);

    temp=node.getAttribute("has_serial");
	has_serial=!temp || temp[0]=='t';

    temp=node.getAttribute("class");
	if(temp)creation_tag=temp;

    XMLNode types_node=node.getChildNode("supported_types");
    if(!types_node.isEmpty()){
        int i=0;
        XMLNode type_node=types_node.getChildNode("type", &i);
        while(!type_node.isEmpty()){
            Type t;
            if(!t.InitFromXml(type_node, root_node))return false;
            supported_types.push_back(t);
            type_node=types_node.getChildNode("type", &i);
        }
    }

    temp=node.getAttribute("ui_flags");
	if(temp){
		if(strstr(temp, "dontdelete"))flags |= FLG_DONTDELETE;
	}

	temp=node.getAttribute("version");
	if(temp)version=strtol(temp, NULL, 0);

	int i=0;
    XMLNode ps_node=node.getChildNode("paramset", &i);
	while(!ps_node.isEmpty()){
	    temp=ps_node.getAttribute("type");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<paramset> attribute \"type\" not found");
			return false;
		}
		HS485Paramset& ps=paramsets[temp];
		if(!ps.InitFromXml(ps_node, root_node))return false;
		ps_node=node.getChildNode("paramset", &i);
	}

    XMLNode channels_node=node.getChildNode("channels");
    if(!channels_node.isEmpty()){
        int i=0;
        XMLNode channel_node=channels_node.getChildNode("channel", &i);
        while(!channel_node.isEmpty()){
            HS485ChannelDescription* ch=new HS485ChannelDescription;
            ch->SetDevice(this);
			ch->SetIndex(channels.size());
			if(!ch->InitFromXml(channel_node, root_node)){
				delete ch;
				return false;
			}
            channels.push_back(ch);

			if(ch->HasSubdescriptions())channels_have_subdescriptions=true;
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
			framedefs_by_type[fd.GetType()]=framedefs.size()-1;
			frame_node=frames_node.getChildNode("frame", &i);
		}
	}

	XMLNode description_node=node.getChildNode("description");
	if(!description_node.isEmpty())additional_description.InitFromXml(description_node, root_node);

    return true;
}

int HS485DeviceDescription::Matches(HS485Frame& sysinfoFrame, std::string* type_id)
{
    types_t::iterator it;
	int priority=-1;
    for(it=supported_types.begin();it!=supported_types.end();it++){
        if(it->GetType()>=0 && it->MatchFrame(sysinfoFrame)){
			if(it->GetPriority()>priority){
				*type_id=it->GetId();
				priority=it->GetPriority();
			}
		}
    }
    return priority;
}

bool HS485DeviceDescription::SupportsType(const std::string& type)
{
    types_t::iterator it;
    for(it=supported_types.begin();it!=supported_types.end();it++){
        if(it->GetId()==type){
			return true;
		}
    }
    return false;
}

HS485ChannelDescription* HS485DeviceDescription::GetChannel(int i)
{
    if((unsigned int)i>=channels.size())return NULL;
    return channels[i];
}

unsigned int HS485DeviceDescription::GetChannelCount()
{
    return channels.size();
}

bool HS485DeviceDescription::SetEnforcedParameters(HS485LogicalInstance* inst)
{
//	LOG(Logger::LOG_DEBUG, "HS485DeviceDescription::SetEnforcedParameters() inst->GetLogicalIndex()=%d, inst->GetPhysicalIndex()=%d", inst->GetLogicalIndex(), inst->GetPhysicalIndex());
//	LOG(Logger::LOG_DEBUG, "typeid(*inst)=%s", typeid(*inst).name());
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		if(!it->second.SetEnforcedValues(inst))return false;
	}
	return true;
}

bool HS485DeviceDescription::ListParamsets(XmlRpc::XmlRpcValue* list)
{
	int i=0;
	for(paramsets_t::iterator it=paramsets.begin();it!=paramsets.end();it++){
		(*list)[i]=it->first;
		i++;
	}
	return true;
}

HS485ChannelDescription* HS485DeviceDescription::GetChannel(const std::string& type)
{
	for(unsigned int i=0;i<channels.size();i++){
		if(channels[i]->GetType()==type)return channels[i];
	}
	return NULL;
}

FrameDescription* HS485DeviceDescription::GetFrameDescription(HS485Frame& frame, int* channel, int* iterator)
{
	while((unsigned int)*iterator < framedefs.size()){
		FrameDescription& fd=framedefs[(*iterator)++];
		if(fd.MatchFrame(frame, channel))return &fd;
	}
	return NULL;
};

HS485Paramset* HS485DeviceDescription::GetParamset(const std::string& key)
{
	paramsets_t::iterator it=paramsets.find(key);
	if(it==paramsets.end()){
		LOG(Logger::LOG_ERROR, "Parameterset %s not found", key.c_str());
		return NULL;
	}
	return &(it->second);
}

HS485Device* HS485DeviceDescription::CreateDevice()
{
	if(!creation_tag.empty()){
		std::string full_tag="device_class_";
		full_tag+=creation_tag;
		void* obj=dynamic::create(full_tag.c_str());
		if(!obj){
			LOG(Logger::LOG_ERROR, "Device class %s not supported", creation_tag.c_str());
			return NULL;
		}
		HS485Device* dev=dynamic_cast<HS485Device*>((HS485Device*)obj);
		if(!dev){
			/* TODO: get rid of the allocated memory */
			//delete obj;
			return NULL;
		}
		return dev;
	}else{
		return new HS485Device;
	}
}
