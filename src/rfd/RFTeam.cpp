/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFTeam.h"
#include <type_registry.h>
#include "RFManager.h"
#include <Logger.h>

static hsscomm::type_registry::factory<RFTeam> RFTeamFactory;

RFTeam::RFTeam(void)
{
}

RFTeam::~RFTeam(void)
{
}

RFTeamChannel::RFTeamChannel(void)
{
}

RFTeamChannel::~RFTeamChannel(void)
{
}

void RFTeamChannel::AddTeamChannel(RFChannel* ch)
{
	set_team_channels.insert(ch->GetSerial());
}

void RFTeamChannel::RemoveTeamChannel(RFChannel* ch)
{
	set_team_channels.erase(ch->GetSerial());
	if(set_team_channels.empty()){
		RFTeam* dev=dynamic_cast<RFTeam*>(GetDevice());
		if(dev)RFManager::GetSingleton()->DeleteDevice(dev, 0);
	}
}

bool RFTeamChannel::Describe(XmlRpc::XmlRpcValue* descr)
{
	if(!RFChannel::Describe(descr))return false;
	int i=0;
	for(t_set_team_channels::iterator it=set_team_channels.begin();it!=set_team_channels.end();it++){
		(*descr)["TEAM_CHANNELS"][i++]=*it;
	}
	return true;
}

bool RFTeam::CheckCreationTag(const char *tag)
{
    if(strcmp("device_class_team", tag)==0)return true;
    return false;
}

bool RFTeamChannel::CheckCreationTag(const char *tag)
{
    if(strcmp("channel_class_team", tag)==0)return true;
    return false;
}
bool RFTeamChannel::ReplaceTeamChannel(std::string oldSerial, std::string newSerial)
{
	set_team_channels.erase(oldSerial);
	set_team_channels.insert(newSerial);
	return true;
}
bool RFTeam::CommitPendingConfig()
{
	config_data_dirty=false;
	return true;
}

bool RFTeam::Delete()
{
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++)
	{
		RFTeamChannel* ch=dynamic_cast<RFTeamChannel*>(it->second);
		if(!ch)continue;
		if(ch->GetTeamChannelCount())return false;
	}
	LOG(Logger::LOG_DEBUG, "Deleting team device \"%s\"", GetSerial().c_str());
	return true;
}

bool RFTeam::replaceDevice(RFDevice *oldDevice)
{
    RFTeam *oldTeam = dynamic_cast<RFTeam*>(oldDevice);
    if(!oldTeam)
    {
        LOG(Logger::LOG_ERROR,"RFTeam::replaceDevice(); oldDevice ist kein RFTeam");
        return false;
    }
    channels_t::iterator it;
    RFTeamChannel *newChannel = NULL;
    RFTeamChannel *oldChannel = NULL;
    for (it = oldTeam->channels.begin(); it != oldTeam->channels.end(); ++it)
    {
        newChannel = dynamic_cast<RFTeamChannel*>(this->GetInstance(it->second->GetIndex()));
        oldChannel = dynamic_cast<RFTeamChannel*>(it->second);
        if(newChannel != NULL && oldChannel != NULL)
        {
            break;
        }
    }
    if(newChannel != NULL && oldChannel != NULL)
    {
        newChannel->replaceChannel(oldChannel);
        newChannel->CommitPendingConfig();
    }
    return true;
}

bool RFTeam::IsTeamDeviceInstance() {
	return true;
}

bool RFTeamChannel::SetLinkInfo(const std::string& peer, const std::string& name, const std::string& description)
{
	// Team devices do not support this, since they contain no links
	return true;
}
bool RFTeamChannel::replaceChannel(RFChannel * oldChannel)
{
    t_set_team_channels temp_set_team_channels;
    RFTeamChannel *oldTeamChannel = dynamic_cast<RFTeamChannel *>(oldChannel);
    if(!oldTeamChannel)
    {
        LOG(Logger::LOG_ERROR,"RFTeamChannel::replaceChannel(); der übergebende Kanal ist kein Teamchannel");
        return false;
    }
    this->replaceRFConfigData(oldChannel);
    t_set_team_channels::iterator it;
    for(it = oldTeamChannel->set_team_channels.begin();it != oldTeamChannel->set_team_channels.end();++it)
    {
        temp_set_team_channels.insert(*it);
    }
    for(it = temp_set_team_channels.begin();it != temp_set_team_channels.end();++it)
    {
        RFLogicalInstance *inst = RFManager::GetSingleton()->GetInstance(*it);
        LOG(Logger::LOG_DEBUG,"RFTeamChannel::replaceChannel(); Teamchannel %s",(*it).c_str());
        if (inst)
        {
            RFChannel *ch = dynamic_cast<RFChannel*>(inst);
            if (ch)
            {
                ch->SetTeam(this);
            }
        }
    }
    this->link_peers_valid = true;
    this->link_peers_dirty = true;
    this->config_data_dirty = true;
    return true;
}
