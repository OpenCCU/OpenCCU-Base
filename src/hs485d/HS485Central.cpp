/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485Central.h"
#include "HS485Controller.h"
#include "HS485Manager.h"
#include <Logger.h>
#include <fstream>
#include <set>
#include <dynamic.h>

using namespace XmlRpc;

static dynamic::factory<HS485Central::HS485CentralChannel> HS485CentralChannelFactory;
static dynamic::factory<HS485Central> HS485CentralFactory;

HS485Central* HS485Central::singleton=NULL;

HS485Central::HS485Central(void)
{
	if(!singleton)singleton=this;
	listener_channel=NULL;
	char buffer[32];
	*buffer=0;
	std::ifstream file("/boot/VERSION");
	while(file.good() && !file.eof()){
		file.getline(buffer, sizeof(buffer));
		const char* ver_start=strstr(buffer, "=");
		if(ver_start)firmware_version=ver_start+1;
	}
	type="HMW-RCV-50";
	serial="BidCoS-Wir";
}

HS485Central::~HS485Central(void)
{
}

HS485Central::HS485CentralChannel::HS485CentralChannel(void)
{
}

HS485Central::HS485CentralChannel::~HS485CentralChannel(void)
{
}

HS485Central* HS485Central::HS485CentralChannel::GetDevice(void)
{
	return (HS485Central*)HS485Channel::GetDevice();
}

bool HS485Central::HS485CentralChannel::GetLinkPeers(std::vector<std::string>* peers)
{
	t_link_info_map::iterator it;
	for(it=link_info_map.begin();it!=link_info_map.end();it++){
		peers->push_back(it->first);
	}
	return true;
}

bool HS485Central::HS485CentralChannel::SetLinkInfo(const std::string& peer, const std::string& name, const std::string& description)
{
	t_link_info_map::iterator it=link_info_map.find(peer);
	if(it==link_info_map.end())return false;
	it->second.name=name;
	it->second.description=description;
	GetDevice()->PeerListDirty();
	return true;
}


bool HS485Central::HS485CentralChannel::AddLinkPeer(const std::string& peer)
{
	t_link_info_map::iterator it=link_info_map.find(peer);
	if(it==link_info_map.end()){
		link_info_map[peer];
		GetDevice()->PeerListDirty();
	}
	return true;
}

bool HS485Central::HS485CentralChannel::RemoveLinkPeer(const std::string& peer)
{
	t_link_info_map::iterator it=link_info_map.find(peer);
	if(it!=link_info_map.end()){
		link_info_map.erase(peer);
		GetDevice()->PeerListDirty();
	}
	return true;
}

bool HS485Central::HS485CentralChannel::SendMessage(HS485CommMessage* msg)
{
//	LOG(Logger::LOG_DEBUG, "HS485Central::HS485CentralChannel::SendMessage()");
	return GetDevice()->SendMessage(msg);
}

bool HS485Central::GetLinkPeers(std::vector<std::string>* peers)
{
	return false;
}

bool HS485Central::AddLinkPeer(const std::string& peer)
{
	return false;
}

bool HS485Central::RemoveLinkPeer(const std::string& peer)
{
	return false;
}

bool HS485Central::SendMessage(HS485CommMessage* msg)
{
//	LOG(Logger::LOG_DEBUG, "HS485Central::SendMessage()");
	return HS485Controller::GetSingleton()->SendMessage(msg);
}

void HS485Central::PeerListDirty()
{
	RequestSave(10000);
}

bool HS485Central::SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event/*=false*/)
{
	return false;
};

bool HS485Central::GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	try{
		if(name=="ADDRESS"){
			(int&)(*val)=GetAddress();
		}else{
			LOG(Logger::LOG_WARNING, "Tried to get unknown internal value %s", name.c_str());
		}
	}catch(XmlRpcException& e){
		LOG(Logger::LOG_WARNING, "GetInternalValue() exception %s", e.getMessage().c_str());
		return false;
	}
	return true;
};

bool HS485Central::CheckCreationTag(const char *tag)
{
    if(strcmp("device_class_central", tag)==0)return true;
    return false;
}

bool HS485Central::HS485CentralChannel::CheckCreationTag(const char *tag)
{
    if(strcmp("channel_class_central", tag)==0)return true;
    return false;
}

HS485Central::HS485CentralChannel* HS485Central::GetListenerChannel()
{
	if(listener_channel)return listener_channel;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(it->second->GetDescription()->GetType()=="LISTENER"){
			listener_channel=(HS485CentralChannel*)it->second;
		}
	}
	return listener_channel;
}



