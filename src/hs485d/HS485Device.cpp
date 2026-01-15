/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485Device.cpp: Implementierung der Klasse HS485Device.
//
//////////////////////////////////////////////////////////////////////

#include "HS485Device.h"
#include "HS485Controller.h"
#include "HS485DeviceDescription.h"
#include "HS485ChannelDescription.h"
#include "HS485Manager.h"
#include <Logger.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fstream>
#include <dynamic.h>
#include <inttypes.h>

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

static const unsigned int MAX_EEPROM_BLOCK_SIZE=64;

HS485Device::HS485Device(unsigned int address)
:address(address), eep_cache(this)
{
	maintenance_flags=0;
	last_config_pending=false;
	firmware_version="0";
	fail_counter=0;
}

HS485Device::HS485Device(const HS485Device & inst)
:address(inst.address), eep_cache(this), sysinfo(inst.sysinfo)
{
	maintenance_flags=0;
	last_config_pending=false;
	firmware_version="0";
	fail_counter=0;
}

HS485Device::~HS485Device()
{
	ClearChannels();
}

void HS485Device::CheckConfigPendingEvent()
{
	bool config_pending=IsConfigPending();
	if(config_pending == last_config_pending)return;
	last_config_pending=config_pending;
	XmlRpcValue val;
	(int&)val=config_pending;
	SendInternalValueEvent("CONFIG_PENDING", val);
}


void HS485Device::ClearChannels()
{
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		delete it->second;
	}
	channels.clear();
}

void HS485Device::ClearConfigCache()
{
	eep_cache.Clear();
	if(GetDeviceDescription()->GetEEPSize())eep_cache.GenerateEmptyChunks(GetDeviceDescription()->GetEEPSize());
	RequestSave();
}

HS485EEPromCache* HS485Device::GetEEPromCache()
{
    return &eep_cache;
}

bool HS485Device::SendMessage(const std::string& msg, std::string* response)
{
	if(fail_counter>=2){
		return false;
	}
	if(!HS485Controller::GetSingleton()->SendMessage(address, msg, response)){
		if(!(maintenance_flags&FLAG_UNREACH)){
			maintenance_flags|=(FLAG_UNREACH);
			SendInternalValueEvent("UNREACH");
		}
		if(!(maintenance_flags&FLAG_STICKY_UNREACH)){
			maintenance_flags|=(FLAG_STICKY_UNREACH);
			SendInternalValueEvent("STICKY_UNREACH");
		}
		fail_counter++;
		RequestTimer(FAIL_COUNTER_RESET_TIME, TIMER_RESET_FAIL_COUNTER);
		return false;
	}
	fail_counter=0;
	if(maintenance_flags&FLAG_UNREACH){
		maintenance_flags&=~(FLAG_UNREACH);
		SendInternalValueEvent("UNREACH");
	}
	return true;
}

bool HS485Device::SendMessage(HS485CommMessage* msg)
{
	if(fail_counter>=2){
		if(!msg->GetDontDelete()){
			delete msg;
		}
		return false;
	}
	msg->SetReceiverAddress(address);
	if(!HS485Controller::GetSingleton()->SendMessage(msg)){
		if(!(maintenance_flags&FLAG_UNREACH)){
			maintenance_flags|=(FLAG_UNREACH);
			SendInternalValueEvent("UNREACH");
		}
		if(!(maintenance_flags&FLAG_STICKY_UNREACH)){
			maintenance_flags|=(FLAG_STICKY_UNREACH);
			SendInternalValueEvent("STICKY_UNREACH");
		}
		fail_counter++;
		RequestTimer(FAIL_COUNTER_RESET_TIME, TIMER_RESET_FAIL_COUNTER);
		return false;
	}
	fail_counter=0;
	if(maintenance_flags&FLAG_UNREACH){
		maintenance_flags&=~(FLAG_UNREACH);
		SendInternalValueEvent("UNREACH");
	}
	return true;
}

bool HS485Device::ReadEEProm(unsigned int address, unsigned int count, data_t *data)
{
//	LOG(Logger::LOG_DEBUG, "HS485Device::ReadEEProm() dev=%08lX, eep_addr=0x%X, count=%d", this->address, address, count);
	if(address+count > (unsigned int)GetDeviceDescription()->GetEEPSize()){
		LOG(Logger::LOG_ERROR, "Reading %u bytes starting at 0x%X exceeds %s eeprom size (%d)", count, address, GetSerial().c_str(), GetDeviceDescription()->GetEEPSize());
		return false;
	}
    data->resize(count);
    unsigned int read_pos=0;
    while(count){
        unsigned int bytes_to_read=count<MAX_EEPROM_BLOCK_SIZE?count:MAX_EEPROM_BLOCK_SIZE;
        std::string msg="R";
        msg.append(1, (char)((address>>8)&0xff));
        msg.append(1, (char)(address&0xff));
        msg.append(1, (char)(bytes_to_read&0xff));
        std::string response;
        if(!SendMessage(msg, &response))return false;
        if(response.size()!=bytes_to_read)return false;
        std::copy(response.begin(), response.end(), data->begin()+read_pos);
        read_pos+=bytes_to_read;
        count-=bytes_to_read;
        address+=bytes_to_read;
    }
	RequestSave(5000);
	return true;
}

bool HS485Device::GetEEPromUsage(unsigned int address, unsigned int block_size, unsigned int count, data_t* used_bits)
{
    HS485Frame frame;
	frame.SetCtrl(HS485Frame::CTRL_IFRAME);
    used_bits->resize(count/8);
    std::string msg="E";
    msg.append(1, (char)((address>>8)&0xff));
    msg.append(1, (char)(address&0xff));
    msg.append(1, (char)block_size);
    msg.append(1, (char)count);
	frame.SetPayload(msg);

	HS485CommMessage* cm = HS485Controller::GetSingleton()->CreateNewMessage();
	cm->setEEPRomUsage(true);
	cm->SetDontDelete(true);
    cm->SetFrame(frame);

    if(!SendMessage(cm))return false;
	std::string response=cm->GetResponse()->ExtractFrame().GetPayload();
    std::copy(response.begin()+4, response.end(), used_bits->begin());
    delete cm;
    return true;
}

bool HS485Device::WriteEEProm(unsigned int address, const data_t& data)
{
//	LOG(Logger::LOG_DEBUG, "HS485Device::ReadEEProm() dev=%08lX, eep_addr=0x%X, count=%d", this->address, address, data.size());
    unsigned int write_pos=0;
    unsigned int count=data.size();
    while(count){
        unsigned int bytes_to_write=count<MAX_EEPROM_BLOCK_SIZE?count:MAX_EEPROM_BLOCK_SIZE;
        std::string msg="W";
        msg.append(1, (char)((address>>8)&0xff));
        msg.append(1, (char)(address&0xff));
        msg.append(1, (char)(bytes_to_write&0xff));
        msg.resize(bytes_to_write+4);
        std::copy(data.begin()+write_pos, data.begin()+write_pos+bytes_to_write, msg.begin()+4);
        std::string response;
        if(!SendMessage(msg, &response))return false;
        write_pos+=bytes_to_write;
        count-=bytes_to_write;
        address+=bytes_to_write;
    }
	RequestSave(5000);
    return true;
}

void HS485Device::RequestSave(int delay/*=0*/)
{
	KillTimer(TIMER_SAVE);
	RequestTimer(delay, TIMER_SAVE);
}

bool HS485Device::GetParamsetValues(const std::string& key, XmlRpc::XmlRpcValue* set)
{
	LOG(Logger::LOG_ALL, "HS485Device::GetParamsetValues(): key: %s", key.c_str());
	HS485Paramset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->Get(this, key, set);
}

bool HS485Device::PutParamsetValues(const std::string& key, XmlRpc::XmlRpcValue& set)
{
	HS485Paramset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	if(!ps->Put(this, key, set))return false;
	if(!eep_cache.Flush())return false;
	std::string msg="C";
	std::string response;
	SendMessage("C", &response);
	return true;
}

bool HS485Device::GetParamsetDescription(const std::string& key, XmlRpc::XmlRpcValue* set)
{
	HS485Paramset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->GetDefinition(set);
}

bool HS485Device::GetParamsetId(const std::string& key, std::string* id)
{
	HS485Paramset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	*id=ps->GetId();
	return true;
}

bool HS485Device::GetValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	return false;
}

bool HS485Device::SetValue(const std::string& name, XmlRpc::XmlRpcValue& val)
{
	return false;
}

bool HS485Device::Describe(XmlRpc::XmlRpcValue* val)
{
	int index=0;
	bool is_array=false;
	if(val->getType()==XmlRpcValue::TypeArray){
		index=val->size();
		is_array=true;
	}

	XmlRpcValue& dev_desc=is_array?(*val)[index]:*val;
	int dev_index=index;

	dev_desc["ADDRESS"]=HS485Manager::BuildStringAddress(serial);
	dev_desc["TYPE"]=GetType();
	dev_desc["PARENT"]="";
	dev_desc["FLAGS"]=GetDeviceDescription()->GetFlags();

	dev_desc["FIRMWARE"]=firmware_version;
	if(GetAvailableFirmware().size()){
		dev_desc["AVAILABLE_FIRMWARE"]=GetAvailableFirmware();
	}
	dev_desc["UPDATABLE"] = true;
	dev_desc["VERSION"]=GetDeviceDescription()->GetVersion();

	dev_desc["PARAMSETS"].assertArray(0);
	description->ListParamsets(&dev_desc["PARAMSETS"]);
	description->GetAdditionalDescription()->Describe(&dev_desc);

	index++;
	int i=0;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		HS485Channel* ch=it->second;
		if(ch->GetDescription()->IsHidden())continue;
		std::string ch_address=HS485Manager::GetSingleton()->BuildStringAddress(serial, ch->GetIndex());

		if(is_array){
			(*val)[dev_index]["CHILDREN"][i]=ch_address;
			ch->Describe(&((*val)[index]));
			index++;
		}else{
			dev_desc["CHILDREN"][i]=ch_address;
		}
		i++;
	}
	return true;
}

bool HS485Device::SetEnforcedParameters()
{
//	LOG(Logger::LOG_DEBUG, "HS485Device::SetEnforcedParameters");
	if(!description->SetEnforcedParameters(this))return false;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(!it->second->SetEnforcedParameters())return false;
	}
	if(!eep_cache.Flush())return false;
	return true;
}

void HS485Device::ProcessIncomingFrame(HS485Frame& frame)
{

	int iterator=0;
	int channel_index;
	HS485Manager::GetSingleton()->MulticallCollectBegin();
	FrameDescription* fd=description->GetFrameDescription(frame, &channel_index, &iterator);
	while(fd){
		if(channel_index==(int)FrameDescription::ALL_CHANNELS){
			for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
				it->second->ProcessIncomingFrame(frame, fd);
			}
		}else{
			for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
				if(it->second->GetPhysicalIndex()==channel_index)it->second->ProcessIncomingFrame(frame, fd);
			}
		}
		fd=description->GetFrameDescription(frame, &channel_index, &iterator);
	}
	HS485Manager::GetSingleton()->MulticallCollectEnd();

	fail_counter=0;
	if(maintenance_flags&FLAG_UNREACH){
		maintenance_flags&=~(FLAG_UNREACH);
		SendInternalValueEvent("UNREACH");
	}
}

void HS485Device::InitNewDevice()
{
	int eep_size=GetDeviceDescription()->GetEEPSize();
	if(eep_size)eep_cache.GenerateEmptyChunks(eep_size);
}

bool HS485Device::AddChannel(int index, const std::string& type)
{
	HS485ChannelDescription* ch_desc=description->GetChannel(type);
	if(!ch_desc){
		LOG(Logger::LOG_ERROR, "Could not get channel description for type %s", type.c_str());
		return false;
	}

	HS485Channel* ch;
	if(!ch_desc->GetCreationTag().empty()){
		std::string creation_tag="channel_class_";
		creation_tag+=ch_desc->GetCreationTag();
		void* obj=dynamic::create(creation_tag.c_str());
		if(!obj){
			LOG(Logger::LOG_ERROR, "Channel class %s not supported", ch_desc->GetCreationTag().c_str());
			return false;
		}
		ch=dynamic_cast<HS485Channel*>((HS485Channel*)obj);
		if(!ch){
			/* TODO: get rid of the allocated memory */
			//delete obj;
			return false;
		}
	}else{
		ch=CreateChannel();
	}
	channels[index]=ch;
	ch->SetParent(this, index, ch_desc);
	return true;
}


void HS485Device::SetDeviceDescription(HS485DeviceDescription* description, bool add_channels/*=true*/)
{
	this->description=description;
	ClearChannels();
	if(add_channels){
		int i=0;
		HS485ChannelDescription* ch_desc=description->GetChannel(i);
		while(ch_desc){
			int start=ch_desc->GetIndex();
			int count=ch_desc->GetCount();
			for(int j=0;j<count;j++){
				HS485Channel* ch=channels[j+start]=CreateChannel();
				ch->SetParent(this, j+start, ch_desc);
				if(ch_desc->HasSubdescriptions())ch->GetBehaviour();
			}
			i++;
			ch_desc=description->GetChannel(i);
		}
	}
}

HS485LogicalInstance* HS485Device::GetInstance(int channel_index)
{
	if(channel_index<0)return this;
	channels_t::iterator it=channels.find(channel_index);
	if(it==channels.end())return NULL;
	return it->second;
}

std::vector<int> HS485Device::ListChannels()
{
	std::vector<int> retval;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(!it->second->GetDescription()->IsHidden())retval.push_back(it->first);
	}
	return retval;
}

bool HS485Device::FactoryReset()
{
	int eep_size=GetDeviceDescription()->GetEEPSize();
	data_t data;
	data.insert(data.begin(), 16, 0xff);
	for(int i=0;i<eep_size;i+=data.size()){
		if(!WriteEEProm(i, data))return false;
	}
	std::string response;
	SendMessage("!!", &response);
	return true;
}

bool HS485Device::UnpeerCentral()
{
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(!it->second->UnpeerCentral())return false;
	}
	return true;
}

bool HS485Device::SaveToXml(XMLNode* node)
{
	char buffer[16];
	node->addAttributeConst("serial", GetSerial().c_str());
	node->addAttributeConst("type", GetType().c_str());
	std::string s;
	*buffer=0;
	for(unsigned int i=0;(i < sysinfo.size()) && ((i+3) < sizeof(buffer));i++){
		sprintf(buffer+2*i, "%02X", (int)sysinfo[i]);
	}
	if(*buffer)node->addAttributeConst("sysinfo", buffer);
	snprintf(buffer, sizeof(buffer), "0x%08" PRIX32, GetAddress());
	node->addAttributeConst("address", buffer);
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++)
	{
		XMLNode channel_node=node->addChildConst("channel");
		if(!it->second->SaveToXml(&channel_node))return false;
	}
	XMLNode eep_node=node->addChildConst("eeprom");
	if(!eep_cache.SaveToXml(&eep_node))return false;

	XMLNode values_node=node->addChildConst("values");
	if(!ValueStore::SaveToXml(&values_node))return false;

	//Replacement history
	if(replacementHistory.size() > 0)
	{
		XMLNode replase_history_node = node->addChildConst("replaceHistory");
		std::vector<string>::iterator it;
		for(it = replacementHistory.begin();it!= replacementHistory.end();++it)
		{
			replase_history_node.addChildConst(it->c_str());
		}
	}

	return true;
}

bool HS485Device::LoadFromXml(XMLNode& node)
{
	const char* temp;


	HS485DeviceDescription* desc=NULL;
	temp=node.getAttribute("sysinfo");
	if(temp){
		sysinfo.clear();
		std::string s=temp;
		for(unsigned int i=0;(i*2+1)<s.size();i++)sysinfo.append(1, (char)strtol(s.substr(2*i, 2).c_str(), NULL, 16));
		desc=HS485Manager::GetSingleton()->GetSystemDescription()->GetDeviceBySysinfo(sysinfo);
		SetSysinfo(sysinfo);
	}
	temp=node.getAttribute("type");
	if(!temp)return false;
	type=temp;
	if(!desc){
		desc=HS485Manager::GetSingleton()->GetSystemDescription()->GetDeviceByType(temp);
		if(!desc){
			LOG(Logger::LOG_ERROR, "Unknown device type %s", temp);
			return false;
		}
	}
	SetDeviceDescription(desc, false);

	temp=node.getAttribute("serial");
	if(!temp)return false;
	SetSerial(temp);

	temp=node.getAttribute("address");
	if(!temp)return false;
	uint32_t address=strtoul(temp, NULL, 0);
	SetAddress(address);

	int i=0;
	XMLNode channel_node=node.getChildNode("channel", &i);
	while(!channel_node.isEmpty()){
		temp=channel_node.getAttribute("index");
		if(!temp)return false;
		int index=strtol(temp, NULL, 0);
		HS485Channel* channel=dynamic_cast<HS485Channel*>(GetInstance(index));
		if(!channel){
			temp=channel_node.getAttribute("type");
			if(!temp)return false;
			AddChannel(index, temp);
			channel=dynamic_cast<HS485Channel*>(GetInstance(index));
			if(!channel){
				LOG(Logger::LOG_ERROR, "Could not add channel %s", temp);
				return false;
			}
		}
		if(!channel->LoadFromXml(channel_node))return false;
		channel_node=node.getChildNode("channel", &i);
	}
	XMLNode eep_node=node.getChildNode("eeprom");
	if(!eep_node.isEmpty()){
		if(!eep_cache.LoadFromXml(eep_node))return false;
	}
	XMLNode values_node=node.getChildNode("values");
	if(!values_node.isEmpty()){
		if(!ValueStore::LoadFromXml(values_node))return false;
	}
	//Device replacement history
	XMLNode replace_history_node = node.getChildNode("replaceHistory");
	if(!replace_history_node.isEmpty())
	{
		int chile_cound = replace_history_node.nChildNode();
		for(int i = 0;i<chile_cound;++i)
		{
			XMLNode replace = replace_history_node.getChildNode(i);
			this->replacementHistory.push_back(replace.getName());
		}
	}

	return true;
}

bool HS485Device::Save()
{
	LOG(Logger::LOG_DEBUG, "Saving device %s", GetSerial().c_str());
	const std::string deviceFilesPath = HS485Manager::GetSingleton()->GetDeviceFilesPath();
	if (mkdir(deviceFilesPath.c_str(), 0777) < 0 && errno != EEXIST) return false;
	std::string filename(deviceFilesPath);
	filename+="/"+GetSerial()+".dev";
	std::string new_filename=filename+".new";
	std::string bak_filename=filename+".bak";
	XMLNode n=XMLNode::createXMLTopNode();
	n.setNameConst("device");
	if(!SaveToXml(&n))return false;
	char* s=n.createXMLString(1);
	if(!s)return false;
	std::ofstream f(new_filename.c_str());
	if(f.good()){
		f<<s;
	}
	free(s);
	if(!f.good())return false;
	f.close();

	unlink(bak_filename.c_str());

	rename(filename.c_str(), bak_filename.c_str());
	if(rename(new_filename.c_str(), filename.c_str()))return false;
	unlink(bak_filename.c_str());
	return true;
}

bool HS485Device::DetermineSerial(bool has_serial)
{
	if(!has_serial){
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "ELV%08" PRIX32, GetAddress());
		SetSerial(buffer);
		return true;
	}
	std::string response;
	if(!SendMessage("n", &response))return false;
	if(!response.size())return false;
	SetSerial(response);
	return true;
}

bool HS485Device::Delete()
{
	LOG(Logger::LOG_DEBUG, "Deleting persistent data for device %s", GetSerial().c_str());
	std::string filename(HS485Manager::GetSingleton()->GetDeviceFilesPath());
	filename+="/"+GetSerial()+".dev";
	return unlink(filename.c_str())==0;
}

bool HS485Device::SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event/*=false*/)
{
	try{
		if(name=="STICKY_UNREACH"){
			if((int&)val)maintenance_flags|=FLAG_STICKY_UNREACH;
			else maintenance_flags&=~FLAG_STICKY_UNREACH;
		}
	}catch(XmlRpcException& e){
		return false;
	}
	if(fire_event)this->SendInternalValueEvent(name, val);
	return true;
};

bool HS485Device::GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	try{
		if(name=="STICKY_UNREACH"){
			(int&)(*val)=(maintenance_flags&FLAG_STICKY_UNREACH)!=0;
		}else if(name=="UNREACH"){
			(int&)(*val)=(maintenance_flags&FLAG_UNREACH)!=0;
		}else if(name=="CONFIG_PENDING"){
			(int&)(*val)=IsConfigPending();
		}else{
			LOG(Logger::LOG_WARNING, "Tried to get unknown internal value %s", name.c_str());
		}
	}catch(XmlRpcException& e){
		LOG(Logger::LOG_WARNING, "GetInternalValue() exception %s", e.getMessage().c_str());
		return false;
	}
	return true;
};

bool HS485Device::IsConfigPending()
{
	return GetEEPromCache()->IsDirty();
}

bool HS485Device::GetLinks(int flags, link_map_t* result)
{
	flags &= ~GL_FLAG_GROUP;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(it->second->GetDescription()->IsHidden())continue;
		if(!it->second->GetLinks(flags, result)){
			LOG(Logger::LOG_ERROR, "Error getting links from %s", it->second->GetSerial().c_str());
		}
	}
	return true;
}

void HS485Device::SetSysinfo(const std::string& si)
{
	sysinfo=si;
	if(si.size()<3)return;
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d.%02d", si[2], si[3]);
	firmware_version=buffer;
}

const std::string& HS485Device::GetAvailableFirmware()
{
	if(available_firmware.size())return available_firmware;
	if(sysinfo.size()>=2){
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "H%dV%d", sysinfo[0], sysinfo[1]);
		available_firmware=HS485Manager::GetSingleton()->GetFirmwareManager()->GetFirmwareVersion(buffer);
	}
	return available_firmware;
}

HS485Channel* HS485Device::GetChannelByPhysicalIndex(int index, int direction)
{
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(it->second->GetPhysicalIndex()==index){
			if(direction && it->second->GetDescription()->GetDirection()!=direction)continue;
			return it->second;
		}
	}
	return NULL;
}

void HS485Device::OnTimer(uint32_t cookie)
{
	switch(cookie){
		case TIMER_RESET_FAIL_COUNTER:
			fail_counter=0;
		break;
		case TIMER_SAVE:
			Save();
		break;
		default:
			HS485LogicalInstance::OnTimer(cookie);
	}
}

bool HS485Device::WriteFlash(const std::vector<unsigned char> &buffer, unsigned int start)
{
	HS485Controller* ctrl=HS485Controller::GetSingleton();

	unsigned int blocksize=0;
	unsigned int pos=0;
	unsigned int end=0;

    std::string response;
	if(!ctrl->SendBootloaderMessage(address, "p", &response))return false;
	if(response.size()!=2)return false;
	unsigned int highByte = ((unsigned int)(response[0])) << 8;
	unsigned int lowByte = (unsigned int)((unsigned char)response[1]);
	unsigned int pagesize = highByte | lowByte;

	end=start+buffer.size();
    if(start%pagesize)start=(start/pagesize)*pagesize;
    if(end%pagesize)end=(end/pagesize+1)*pagesize;
    pos=start;
	blocksize=pagesize;
	if(blocksize>128)blocksize=128;
    while(pos<end){
		std::string cmd;
		unsigned int j=0;
		unsigned int page_start=pos;
		while(j<pagesize){
			if(blocksize<pagesize)cmd="b";
			else cmd="w";
			cmd+=(pos>>8);
			cmd+=(pos&0xff);
			cmd+=blocksize;
			for(unsigned int i=0;i<blocksize;i++)
			{
				cmd+=pos<end?buffer[pos-start]:0;
				pos++;
				j++;
			}
			if(!ctrl->SendBootloaderMessage(address, cmd, &response))return false;
		}
		if(blocksize<pagesize){
			cmd="w";
			cmd+=(page_start>>8);
			cmd+=(page_start&0xff);
			cmd+=pagesize;
			if(!ctrl->SendBootloaderMessage(address, cmd, &response))return false;
		}
    }
    return true;
}

bool HS485Device::VerifyFlash(const std::vector<unsigned char> &buffer, unsigned int start)
{
	HS485Controller* ctrl=HS485Controller::GetSingleton();
	unsigned int blocksize=0;
	unsigned int pos=0;
	unsigned int end=0;
	unsigned int data_end=start+buffer.size();

    std::string response;
	if(!ctrl->SendBootloaderMessage(address, "p", &response))return false;
	if(response.size()!=2)return false;
	unsigned int highByte = ((unsigned int)((unsigned char)response[0])) << 8;
	unsigned int lowByte = (unsigned int)((unsigned char)response[1]);
	unsigned int pagesize = highByte | lowByte;

	end=start+buffer.size();
    if(start%pagesize)start=(start/pagesize)*pagesize;
    if(end%pagesize)end=(end/pagesize+1)*pagesize;
    pos=start;
	blocksize=pagesize;
	if(blocksize>128)blocksize=128;
    while(pos<end){
		std::string cmd;
		cmd="r";
		cmd+=(pos>>8);
		cmd+=(pos&0xff);
		cmd+=blocksize;
		response.erase();
		if(!ctrl->SendBootloaderMessage(address, cmd, &response))return false;
        if(response.size()!=blocksize){
			LOG(Logger::LOG_ERROR, "Verify error @ 0x%04X : expected %u bytes, got %u bytes\n", pos, blocksize, response.size());
            return false;
        }
		for(unsigned int i=0;i<blocksize;i++){
			if(pos==data_end){
				pos=end;
				break;
			}
			if(((unsigned char)response[i])!=buffer[pos-start]){
				LOG(Logger::LOG_ERROR, "Verify error @ 0x%04X : expected 0x%02X, got 0x%02X\n", pos, (int)buffer[pos-start], (int)response[i]);
				return false;
			}
			pos++;
		}
    }
	return true;
}

bool HS485Device::UpdateFirmware()
{
	if(sysinfo.size()<2)return false;

	char buffer[16];
	snprintf(buffer, sizeof(buffer), "H%dV%d", sysinfo[0], sysinfo[1]);
	Hexfile* firmware=HS485Manager::GetSingleton()->GetFirmwareManager()->GetFirmware(buffer);

	if( !firmware )
	{
		LOG(Logger::LOG_ERROR, "No firmware found for updating %s", GetSerial().c_str());
		return false;
	}

	HS485Controller* ctrl=HS485Controller::GetSingleton();
	bool retval=false;

	//enter sleep mode on all devices
	ctrl->BroadcastSleepMode(true);


	//switch the device into update mode
    HS485Frame frame;
	frame.SetCtrl(HS485Frame::CTRL_IFRAME);
    frame.SetPayload("u");

	HS485CommMessage* cmsg = HS485Controller::GetSingleton()->CreateNewMessage();
	cmsg->SetDontDelete(true);
	cmsg->SetTimeout(0);
	cmsg->SetResponseTimeout(0);
    cmsg->SetFrame(frame);

	SendMessage(cmsg);
	delete cmsg;

	usleep(1000000);
	if(ctrl->SendBootloaderMessage(address, "u", NULL)){
		if(WriteFlash(firmware->GetBuffer(), firmware->GetStart())>0){
			if(VerifyFlash(firmware->GetBuffer(), firmware->GetStart())>0){
				retval=true;
			}
		}
		ctrl->SendBootloaderMessage(address, "g", NULL);
	}

	//leave sleep mode on all devices
	ctrl->BroadcastSleepMode(false);

	if(retval)Rebuild();

	return retval;
}

bool HS485Device::Rebuild(const bool enforceConfigRestore /*=false*/, const bool reportNewDevice /*=true*/)
{
	std::string type;
	std::string desc=HS485Controller::GetSingleton()->GetDeviceDescription(address);
	HS485DeviceDescription* dev_description=HS485Manager::GetSingleton()->GetSystemDescription()->GetDeviceBySysinfo(desc, &type);
	std::string old_type;
	HS485Manager::GetSingleton()->GetSystemDescription()->GetDeviceBySysinfo(sysinfo, &old_type);
	if(dev_description){
		LOG(Logger::LOG_DEBUG, "%s is now a %s", GetSerial().c_str(), type.c_str());
		XmlRpcValue config_backup;
		if(enforceConfigRestore || (dev_description->GetVersion() != GetDeviceDescription()->GetVersion())){
			if(!GetConfig(&config_backup)){
				LOG(Logger::LOG_ERROR, "Error converting old device config for %s (retrieve)", GetSerial().c_str());
				config_backup.clear();
			}
		}
		SetSysinfo(desc);
		description=dev_description;
		SetType(type);
		if(type!=old_type)ClearChannels();
		int i=0;
		HS485ChannelDescription* ch_desc=description->GetChannel(i);
		while(ch_desc){
			int start=ch_desc->GetIndex();
			int count=ch_desc->GetCount();
			for(int j=0;j<count;j++){
				if(!channels[j+start]){
					HS485Channel* ch=channels[j+start]=CreateChannel();
					ch->SetParent(this, j+start, ch_desc);
					if(ch_desc->HasSubdescriptions())ch->GetBehaviour();
				}else{
					channels[j+start]->SetDescription(ch_desc);
				}
			}
			i++;
			ch_desc=description->GetChannel(i);
		}
		if(type!=old_type){
			//something important changed
			SetEnforcedParameters();
			InitNewDevice();
		}
		RequestSave();
		if(reportNewDevice) {
			HS485Manager::GetSingleton()->ReportNewDevice(this);
		}
		if(config_backup.getType()!=XmlRpcValue::TypeInvalid){
			if(!RestoreConfig(config_backup, !enforceConfigRestore)){
				LOG(Logger::LOG_ERROR, "Error converting old device config for %s (restore)", GetSerial().c_str());
			}
		}
		return true;
	}
	return false;
}

bool HS485Device::GetConfig(XmlRpc::XmlRpcValue* c)
{
	XmlRpcValue paramsets;
	if(!GetDeviceDescription()->ListParamsets(&paramsets))return false;
	(*c)["PARAMSETS"].assertStruct();
	for(int i=0;i<paramsets.size();i++){
		(*c)["PARAMSETS"][(std::string&)paramsets[i]].assertStruct();
		if(!GetParamsetValues(paramsets[i], &((*c)["PARAMSETS"][(std::string&)paramsets[i]])))return false;
	}
	(*c)["CHANNELS"].assertStruct();
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		(*c)["CHANNELS"][it->second->GetSerial()]["INDEX"]=it->first;
		if(!it->second->GetConfig(&((*c)["CHANNELS"][it->second->GetSerial()])))return false;
	}
	return true;
}

bool HS485Device::RestoreConfig(XmlRpc::XmlRpcValue& c, const bool disableEEPCacheFlushing)
{
	bool retval=false;
	try{
		LOG(Logger::LOG_DEBUG, "RestoreConfig %s:%s", GetSerial().c_str(), c.toText().c_str());
		XmlRpcValue::ValueStruct& channels=c["CHANNELS"];
		XmlRpcValue::ValueStruct::iterator it;

		if(disableEEPCacheFlushing) {
			GetEEPromCache()->SetFlushInhibit(true);
		}
		GetEEPromCache()->Clear();
		XmlRpcValue paramsets;
		if(!GetDeviceDescription()->ListParamsets(&paramsets))goto end;
		for(int i=0;i<paramsets.size();i++){
			//LOG(Logger::LOG_DEBUG, "Restoring paramset %s", ((std::string&)paramsets[i]).c_str());
			if(!PutParamsetValues(paramsets[i], c["PARAMSETS"][(std::string&)paramsets[i]]))goto end;
		}
		for(it=channels.begin();it!=channels.end();it++){
			int index=it->second["INDEX"];
			//LOG(Logger::LOG_DEBUG, "Restoring channel %d", index);
			HS485Channel* ch=dynamic_cast<HS485Channel*>(GetInstance(index));

			if(!ch)continue;
			//LOG(Logger::LOG_DEBUG, "HS485Channel: Serial %s ; Parent Serial %s", ch->GetSerial().c_str(), (ch->parent_dev != NULL ? ch->parent_dev->GetSerial().c_str() : "No Parent"));
			//LOG(Logger::LOG_DEBUG, "HS485Channel: Serial %s ; Parent Address %d(dec)", ch->GetSerial().c_str(), (ch->parent_dev != NULL ? ch->parent_dev->GetAddress() : 1));
			if(!ch->RestoreConfig(it->second))goto end;
		}
		retval=true;
	}catch(XmlRpcException e){
		LOG(Logger::LOG_ERROR, "RestoreConfig error %s", e.getMessage().c_str());
	}
end:
	if(disableEEPCacheFlushing) {
		GetEEPromCache()->SetFlushInhibit(false);
	}
	GetEEPromCache()->Flush();
	std::string response;
	SendMessage("!!", &response);
	return retval;
}

bool HS485Device::IsCompatible(const HS485Device* newDevice)
{
	if(serial.compare(newDevice->serial) == 0) {
		return false;
	}
	return (type.compare(newDevice->type) == 0);//FIXME Maybe check same information as in RFDevice.
}

const std::vector<std::string>& HS485Device::getReplacementHistory() const
{
	return replacementHistory;
}

bool HS485Device::replaceDeviceWith(HS485Device* newDevice)
{
	if(newDevice == NULL) {
		LOG(Logger::LOG_ERROR, "HS485Device::replaceDeviceWith(): New device instance is null.");
		return false;
	}
	if(!IsCompatible(newDevice)) {
		LOG(Logger::LOG_ERROR, "HS485Device::replaceDeviceWith(): Devices are not compatible.");
		return false;
	}
	channels_t::iterator oldDevChannelIter = channels.begin();
	while(oldDevChannelIter != channels.end()) {//For every channel of oldDev
		HS485Channel* pOldDevChannel = oldDevChannelIter->second;//current channel of old dev
		if(pOldDevChannel != NULL) {
			HS485Channel* pNewDevChannel = dynamic_cast<HS485Channel*>(newDevice->GetInstance(pOldDevChannel->GetIndex()));
			if(pNewDevChannel != NULL) {
				LOG(Logger::LOG_ALL, "HS485Device::replaceDeviceWith(): Exchanging channel %s with %s",pOldDevChannel->GetSerial().c_str(), pNewDevChannel->GetSerial().c_str());
				std::string newDevChannelSerial(pNewDevChannel->GetSerial());//Serial of new device channel (with channel number from current oldDevice channel)
				//Get the peers of current oldDevChannel
				std::vector<std::string> oldDevChannelPeers;
				pOldDevChannel->GetLinkPeers(&oldDevChannelPeers);
				for(unsigned int i = 0; i < oldDevChannelPeers.size(); i++) {//for every peer of oldDevChannel
					HS485Channel* pOldDevPeerChannel = dynamic_cast<HS485Channel*>(HS485Manager::GetSingleton()->GetInstance(oldDevChannelPeers.at(i)));
					if(pOldDevPeerChannel != NULL) {

						//Get link parameters from old dev's link peer if it's a receiver
						XmlRpcValue oldDevLinkPeerParamset;
						//LOG(Logger::LOG_ALL, "HS485Device::replaceDeviceWith(): Getting link direction");
						if(pOldDevPeerChannel->GetDescription()->GetDirection() == HS485ChannelDescription::DIRECTION_RECEIVER) {
							//LOG(Logger::LOG_ALL, "HS485Device::replaceDeviceWith(): Trying to get paramset values for LINK");
							pOldDevPeerChannel->GetParamsetValues(pOldDevChannel->GetSerial(), &oldDevLinkPeerParamset);
							//LOG(Logger::LOG_ALL, "HS485Device::replaceDeviceWith() old devs peer link paramset is: ",oldDevLinkPeerParamset.toText().c_str());
						}


						//Remove oldDev from peer
						LOG(Logger::LOG_ALL, "HS485Device::replaceDeviceWith(): Removing link peer %s",pOldDevChannel->GetSerial().c_str());
						bool done = pOldDevPeerChannel->RemoveLinkPeer(pOldDevChannel->GetSerial());
						//Add newDev to peer
						LOG(Logger::LOG_ALL, "HS485Device::replaceDeviceWith(): Adding link peer %s",pNewDevChannel->GetSerial().c_str());
						done = done && pOldDevPeerChannel->AddLinkPeer(newDevChannelSerial);
						//Set link info to peer
						std::string lName, lDesc;
						pOldDevChannel->GetLinkInfo(pOldDevPeerChannel->GetSerial(), &lName, &lDesc);
						pOldDevPeerChannel->SetLinkInfo(newDevChannelSerial, lName, lDesc);

						//Set link paramset to olddevpeerchannel if applicable
						if(oldDevLinkPeerParamset.valid()) {
							//LOG(Logger::LOG_ALL, "HS485Device::replaceDeviceWith(): Setting link paramset to %s", pOldDevPeerChannel->GetSerial().c_str());
							done = pOldDevPeerChannel->PutParamsetValues(pNewDevChannel->GetSerial(), oldDevLinkPeerParamset);
							if(!done) {
								LOG(Logger::LOG_ERROR, "HS485Device::replaceDeviceWith(): Error setting link paramset to %s", pOldDevPeerChannel->GetSerial().c_str());
							}
						}
						if(!done) {
							LOG(Logger::LOG_ERROR, "HS485Device::replaceDeviceWith(): Error replacing link");
							//return false;
						}
					}
				}
			}
			pOldDevChannel->SetSerial(pNewDevChannel->GetSerial());
		}
		++oldDevChannelIter;
	}
	//Set serial and address of newDevice to this (old device) to take identity of newDevice.
	SetSerial(newDevice->serial);
	SetAddress(newDevice->address);
	//SetSysinfo(newDevice->sysinfo);
	//Update replacement history
	replacementHistory.push_back(this->serial);//store old/this device serial in history.

	return true;
}


