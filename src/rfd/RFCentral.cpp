/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFCentral.h"
#include "RFController.h"
#include "RFManager.h"
#include <Logger.h>
#include <fstream>
#include <set>
#include <type_registry.h>

static hsscomm::type_registry::factory<RFCentral::RFCentralChannel> RFCentralChannelFactory;
static hsscomm::type_registry::factory<RFCentral> RFCentralFactory;

RFCentral* RFCentral::singleton=NULL;

using namespace XmlRpc;

RFCentral::RFCentral(void)
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
	type="HM-RCV-50";
	serial="BidCoS-RF";
}

RFCentral::~RFCentral(void)
{
}

RFCentral::RFCentralChannel::RFCentralChannel(void)
{
	link_peers_valid=true;
}

RFCentral::RFCentralChannel::~RFCentralChannel(void)
{
}

RFCentral* RFCentral::RFCentralChannel::GetDevice(void)
{
	return (RFCentral*)RFChannel::GetDevice();
}

bool RFCentral::RFCentralChannel::GetLinkPeers(std::vector<std::string>* peers)
{
	link_peer_map_t::iterator it;
	for(it=link_peers.begin();it!=link_peers.end();it++){
		int peer_address=(it->first)>>8;
		int peer_channel=(it->first)&0xff;
		if(!peer_channel)peer_channel=-1;
		std::string s_address=RFManager::GetSingleton()->BuildStringAddress(peer_address, peer_channel);
		peers->push_back(s_address);
	}
	return true;
}

bool RFCentral::RFCentralChannel::AddLinkPeer(const std::string& peer)
{
	int peer_address;
	int peer_channel;
	RFManager::GetSingleton()->ParseAddress(peer, &peer_address, &peer_channel);
	return LowLevelAddLinkPeer(peer_address, peer_channel)!=ADD_PEER_FAILED;
}

int RFCentral::RFCentralChannel::LowLevelAddLinkPeer(int peer_address, int peer_channel)
{
	if(peer_channel<0)peer_channel=0;
	uint32_t key=(peer_address<<8)|peer_channel;
	link_peer_map_t::iterator it=link_peers.find(key);
	if(it==link_peers.end()){
		link_peers[key];
		GetDevice()->PeerListDirty();
	}
	RequestSave();
	return ADD_PEER_OK;
}

bool RFCentral::RFCentralChannel::RemoveLinkPeer(const std::string& peer)
{
	int peer_address;
	int peer_channel;
	RFManager::GetSingleton()->ParseAddress(peer, &peer_address, &peer_channel);
	return LowLevelRemoveLinkPeer(peer_address, peer_channel);
}

bool RFCentral::RFCentralChannel::LowLevelRemoveLinkPeer(int peer_address, int peer_channel)
{
	if(peer_channel<0)peer_channel=0;
	uint32_t key=(peer_address<<8)|peer_channel;
	link_peer_map_t::iterator it=link_peers.find(key);
	if(it!=link_peers.end()){
		link_peers.erase(key);
		GetDevice()->PeerListDirty();
	}
	RequestSave();
	return true;
}

bool RFCentral::RFCentralChannel::LoadFromXml(XMLNode& node)
{
	if(!RFChannel::LoadFromXml(node))return false;
	link_peers_valid=true;
	link_peers_dirty=false;
	config_data_dirty=false;
	return true;
}

std::string RFCentral::GetFirmwareVersion()
{
	char buffer[32];
	*buffer=0;
	std::ifstream file("/boot/VERSION");
	while(file.good() && !file.eof()){
		file.getline(buffer, sizeof(buffer));
		const char* ver_start=strstr(buffer, "=");
		if(ver_start)return ver_start+1;
	}
	return "?";
}

bool RFCentral::SendFrame(BidcosFrame* frame)
{
	frame->SetCtrl(BidcosFrame::CTRL_BIDI| BidcosFrame::CTRL_RPT_ENABLE);
	return RFManager::GetSingleton()->GetInterfaceConcentrator()->SendFrame(frame);
}

bool RFCentral::RFCentralChannel::SendFrame(BidcosFrame* frame)
{
	return SendToPeers(frame);
}

RFCentral::RFCentralChannel* RFCentral::GetListenerChannel()
{
	if(listener_channel)return listener_channel;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(it->second->GetDescription()->GetType()=="LISTENER"){
			listener_channel=(RFCentralChannel*)it->second;
		}
	}
	return listener_channel;
}

void RFCentral::PeerListDirty()
{
	KillTimer(TIMER_SAVE_PEERS);
	RequestTimer(10000, TIMER_SAVE_PEERS);
}

void RFCentral::OnTimer(uint32_t cookie)
{
	switch(cookie)
	{
	case TIMER_SAVE_PEERS:
		Save();
		break;
	default:
		RFDevice::OnTimer(cookie);
	};
}

bool RFCentral::SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event/*=false*/)
{
	try{
		if(name=="INSTALL_MODE"){
			RFManager::GetSingleton()->SetInstallMode((int&)val?60:0);
			return true;
		}
	}catch(XmlRpcException&){}
	return false;
};

bool RFCentral::GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	try{
		if(name=="INSTALL_MODE"){
			(int&)*val=RFManager::GetSingleton()->GetInstallMode();
			return true;
		}else if(name=="AES_KEY"){
			(int&)(*val)=RFManager::GetSingleton()->GetCurAESKey();
		}else if(name=="ADDRESS"){
			(int&)(*val)=GetAddress();
		}else{
			LOG(Logger::LOG_WARNING, "Tried to get unknown internal value %s", name.c_str());
		}
	}catch(XmlRpcException& e){
		LOG(Logger::LOG_WARNING, "GetInternalValue() exception %s", e.getMessage().c_str());
		return false;
	}
	return true;
}

bool RFCentral::CheckCreationTag(const char *tag)
{
    if(strcmp("device_class_central", tag)==0)return true;
    return false;
}

bool RFCentral::RFCentralChannel::CheckCreationTag(const char *tag)
{
    if(strcmp("channel_class_central", tag)==0)return true;
    return false;
}

bool RFCentral::LoadPeerList()
//this method is used for compatibility only. It loads the old fashioned peer list used in previous versions of rfd
//once there is a CENTRAL.dev file this method will never be called again.
{
	char buffer[4096];
	std::ifstream file("/etc/config/central_peers");
	if(!file.good())return false;
	LOG(Logger::LOG_WARNING, "Converting old peer list file");
	while(file.good() && !file.eof()){
		int channel;
		int peer_address;
		int peer_channel;
		file>>channel;
		file>>peer_address;
		file>>peer_channel;
		file.getline(buffer, sizeof(buffer));
		if(file.good() && !file.eof()){
			channels_t::iterator it=channels.find(channel);
			if(it!=channels.end()){
				RFCentralChannel* ch=(RFCentralChannel*)it->second;
				ch->LowLevelAddLinkPeer(peer_address, peer_channel);
			}
		}
	}
	return file.good();
}

