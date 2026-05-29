/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFDevice.cpp: Implementierung der Klasse RFDevice.
//
//////////////////////////////////////////////////////////////////////

#include "RFDevice.h"
#include "RFDeviceDescription.h"
#include "RFChannelDescription.h"
#include "BidcosFrameDecoder.h"
#include "BidcosFrame.h"
#include "BidcosFrameStartBootloader.h"
#include "BidcosFrameLinkPeerReq.h"
#include "RFManager.h"
#include "RFCentral.h"
#include "RFFirmwareManager.h"
#include "RFTeam.h"
#include <utils.h>
#include <Logger.h>
#include <type_registry.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fstream>
#include <OSCompat.h>
#include <XmlRpcException.h>
#ifndef WIN32
#include <unistd.h>
#endif

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

RFDevice::RFDevice()
:unreach_reason(BidcosFrame::UNREACH_FALSE)
,deviceInBootloader(false)
,firmwareUpdatePeding(false)
,rx_mode_temporary(0)//0 means, no temporary rx_mode setting
{
	address=0;
	fail_counter=0;
	maintenance_flags=0;
	config_data_dirty=false;
	last_config_pending=false;
	config_schedule=0;
	cur_aes_key=0;
	is_new_device=true;
	burst_active_until=time_millis();
	firmware_version="?";
	delete_deferred_flags=0;
	aes=false;
    roaming=false;
	rssi_device = BidcosFrame::INVALID_RSSI_VALUE;
	rssi_peer = BidcosFrame::INVALID_RSSI_VALUE;
	SetValueAsDefined("NEEDS_BURST"); // ist ein Vorgabe aus der XML Beschreibung -> es ist immer definiert
	SetValueAsDefined("UPDATE_PENDING"); //ist nach dem Start immer False
	scheduledRetriesCount = 0;
	scheduledRetriesExecuted = 0;
}

RFDevice::~RFDevice()
{
	ClearChannels();
}

void RFDevice::ClearChannels()
{
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		delete it->second;
	}
	channels.clear();
}

bool RFDevice::SendFrame(BidcosFrame* frame)
{
	if(fail_counter>=2){
		return false;
	}
	int tries=1;
	frame->SetReceiverAddress(address);
	if(RxNeedsBurst() && ((frame->GetCtrl() & BidcosFrame::CTRL_WAKEUP)==0) ){//checking wakeup flag to override RxNeedsBurst call. (dtag dynamic rx mode feature)
	int64_t time_diff=burst_active_until-time_millis();
		frame->SetCtrl(BidcosFrame::CTRL_BIDI | BidcosFrame::CTRL_RPT_ENABLE |((time_diff<=0 || fail_counter)?BidcosFrame::CTRL_BURST:0));
		if(!(frame->GetCtrl() & BidcosFrame::CTRL_BURST))tries=2;
	}else{
		if( (frame->GetCtrl() & BidcosFrame::CTRL_WAKEUP) != 0 ) {//if wakeup flag set, unset it. (was set in QueueAfterWakeupFrame() to override evaluation of RxNeedsBurst here when a device supports burst and wakeup (dtag dynamic rx mode feature)
			frame->SetCtrl( (frame->GetCtrl() ^ BidcosFrame::CTRL_WAKEUP) );
		}
		frame->SetCtrl(BidcosFrame::CTRL_BIDI | BidcosFrame::CTRL_RPT_ENABLE);
	}
	bool unreachNotBlocked = true;
	while(tries){
		if(RFManager::GetSingleton()->GetInterfaceConcentrator()->SendFrame(frame)){
			fail_counter=0;
			burst_active_until=time_millis()+BURST_ACTIVE_INTERVAL;
			BidcosFrame* response=frame->GetResponse();
			if(response)
			{
				SetValueAsDefined("UNREACH");
				UpdateUnreachFlags(response);
			}
			UpdateCurAESKey(frame);
			return true;
		}
		else {
			if(frame->WasNacked()) {
				LOG(Logger::LOG_ALL, "RFDevice::SendFrame(): Frame was NACKED");
				unreachNotBlocked = false;//Otherwise a NACKED frame would throw an UNREACH
				break;
			}
		}
		UpdateCurAESKey(frame);
		tries--;
		if(tries){
			frame->SetCtrl(BidcosFrame::CTRL_BIDI | BidcosFrame::CTRL_RPT_ENABLE |BidcosFrame::CTRL_BURST);
		}
	}
	this->unreach_reason = frame->GetUnreachReason();
	fail_counter++;
	if(unreachNotBlocked &&  (RxAlways()||RxNeedsBurst())){
		UpdateUnreachFlags(NULL);
	}
    LOG(Logger::LOG_DEBUG, "SendFrame failed %u times: %s", fail_counter, BidcosFrameDecoder::ToString(frame).c_str());
	RequestTimer(FAIL_COUNTER_RESET_TIME, TIMER_RESET_FAIL_COUNTER);
	return false;
}

void RFDevice::setLastBurstTime(uint64_t timeMillis)
{
	burst_active_until = timeMillis+BURST_ACTIVE_INTERVAL;
}

bool RFDevice::SetDefaultConfig(void)
{
	for(channels_t::iterator it = channels.begin();it != channels.end();it++)
	{
		it->second->InitDefaultConfig();
		if(!it->second->SetDefaultConfig())
		{
			LOG(Logger::LOG_WARNING, "Faild to set default configuration to device: %s channel %d",serial.c_str(),it->second->GetIndex());
		}
	}
	return true;
}
bool RFDevice::GetParamsetValues(const std::string& key, XmlRpc::XmlRpcValue* set)
{
	RFParamset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->Get(this, key, set);
}
bool RFDevice::GetParamsetValues(const std::string& key,int mode, XmlRpc::XmlRpcValue* set)
{
	RFParamset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->Get(this, key, set,mode);
}
bool RFDevice::PutParamsetValues(const std::string& key, XmlRpc::XmlRpcValue& set)
{
	RFParamset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	if(!ps->Put(this, key, set))return false;
	RFConfigData& cd=config_data[cur_paramset_peer];
	if(!cd.CommitToDevice(this)){
		config_data_dirty=true;
		CheckConfigPendingEvent();
	}
	if(key != "VALUES" || ValueStore::IsDirty()) {
		RequestSave();	
	} 
	return true;
}

bool RFDevice::GetParamsetDescription(const std::string& key, XmlRpc::XmlRpcValue* set)
{
	RFParamset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->GetDefinition(set);
}

bool RFDevice::GetParamsetId(const std::string& key, std::string* id)
{
	RFParamset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	*id=ps->GetId();
	return true;
}

bool RFDevice::DetermineParameter(const std::string& key, const std::string& parameter)
{
	RFParamset* ps=description->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->Determine(this, key, parameter);
}

bool RFDevice::GetValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	return false;
}
bool RFDevice::ReadValue(const std::string& name,int mode, XmlRpc::XmlRpcValue* val)
{
	return false;
}
bool RFDevice::SetValue(const std::string& name, XmlRpc::XmlRpcValue& val)
{
	return false;
}

bool RFDevice::Describe(XmlRpc::XmlRpcValue* val)
{
	int index=0;
	bool is_array=false;
	if(val->getType()==XmlRpcValue::TypeArray){
		index=val->size();
		is_array=true;
	}

	XmlRpcValue& dev_desc=is_array?(*val)[index]:*val;
	int dev_index=index;

	dev_desc["ADDRESS"]=RFManager::GetSingleton()->BuildStringAddress(serial);
	dev_desc["RF_ADDRESS"]=GetAddress();
	dev_desc["TYPE"]=GetType();
	dev_desc["PARENT"]="";
	dev_desc["FIRMWARE"]=GetFirmwareVersion();
	dev_desc["VERSION"]=GetDeviceDescription()->GetVersion();
    dev_desc["INTERFACE"]=bidcos_interface_id;
    dev_desc["ROAMING"]=roaming;
    dev_desc["RX_MODE"]=GetDeviceDescription()->GetRxMode();
	dev_desc["FLAGS"]=GetDeviceDescription()->GetFlags();
	if(GetAvailableFirmware().size())
	{
		dev_desc["AVAILABLE_FIRMWARE"]=GetAvailableFirmware();
	}
	dev_desc["UPDATABLE"]=GetDeviceDescription()->IsUpdatable(this->sysinfo_frame);
	dev_desc["PARAMSETS"].assertArray(0);
//	LOG(Logger::LOG_DEBUG, "ListParamsets");
	description->ListParamsets(&dev_desc["PARAMSETS"]);
	description->GetAdditionalDescription()->Describe(&dev_desc);

	index++;
	int i=0;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		RFChannel* ch=it->second;
		if(ch->GetDescription()->IsHidden())continue;
		std::string ch_address=RFManager::GetSingleton()->BuildStringAddress(serial, ch->GetIndex());

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

bool RFDevice::SetEnforcedParameters()
{
	config_data_dirty=true;
	if(!description->SetEnforcedParameters(this))return false;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(!it->second->SetEnforcedParameters())return false;
	}
//	CommitPendingConfig();
	return true;
}

bool RFDevice::ProcessIncomingFrame(BidcosFrame& frame)
{
    RFManager* manager=RFManager::GetSingleton();
	/*if(delete_deferred_flags){
			manager->DeleteDevice(this, delete_deferred_flags);
			return true;
	}*/
    bool deviceInBootloader_old = this->deviceInBootloader;
	if(frame.MatchType(BidcosFrame::FT_INFO_SERIAL) && (frame.GetIntValue(BidcosFrame::FIELD_RECEIVER) == 0))
	{
		this->deviceInBootloader = true;
	}
	else
	{
		this->deviceInBootloader = false;
	}
	SetValueAsDefined("DEVICE_IN_BOOTLOADER");
	if(deviceInBootloader_old != this->deviceInBootloader)
	{
		SendInternalValueEvent("DEVICE_IN_BOOTLOADER");
	}
	if( frame.DeviceWokenup() ){
		LOG(Logger::LOG_DEBUG, "Device has been woken up");
		if(delete_deferred_flags){
			manager->DeleteDevice(this, delete_deferred_flags);
			return true;
		}
		SendAfterWakeupFrames();
		CommitPendingConfig();
		ProcessPendingUpdate();
	}else if(config_schedule && (RxAlways() || RxNeedsBurst())){
		ScheduleConfig((config_schedule&SCHEDULE_SET_ENFORCED)!=0);
	}else if(this->firmwareUpdatePeding && (RxAlways() || RxNeedsBurst() || this->deviceInBootloader))
	{
		ProcessPendingUpdate();
	}
	UpdateUnreachFlags(&frame);
	fail_counter=0;

	if(frame.MatchType(BidcosFrame::FT_SYSINFO)){
		InstallModes instMode = INSTALL_OFF;
		if(delete_deferred_flags){
			manager->DeleteDevice(this, delete_deferred_flags);
			return true;
		}
		if(manager->GetInstallMode(&instMode)){
			int version=frame.GetIntValue(BidcosFrame::FIELD_SYSINFO_SWVER);
			char buffer[16];
			snprintf(buffer, sizeof(buffer), "%d.%d", version>>4, version&0x0f);
			firmware_version=buffer;
            SetBidcosInterface(frame.GetInterfaceId(), false);
			sysinfo_frame=frame;
			if(channels.empty())CreateChannels();
			LOG(Logger::LOG_DEBUG, "Device firmware version is %s", firmware_version.c_str());
			int channel_a=frame.GetIntValue(BidcosFrame::FIELD_SYSINFO_CH_A);
			int channel_b=frame.GetIntValue(BidcosFrame::FIELD_SYSINFO_CH_B);
			bool has_channel=(channel_a || channel_b)!=0;
			RFConfigData cd;
			if(has_channel == GetDeviceDescription()->PeeringSysinfoExpectChannel()){
				switch(instMode)
				{
				case INSTALL_NORMAL:
				case INSTALL_DEVICE_WHITELIST:
					
					//Set the central address list 0, index 10
					cd.PutValue(0, 10, 0, 3, 0, manager->GetBidcosAddress());
					//Disable manual peering of the device
					cd.PutValue(0, 2, 0, 0, 1, 1);
					if (HasInternalKeys()) {
						// set internal keys visible
						cd.PutValue(0, 2, 7, 0, 1, 1);
					}
					if(!cd.CommitToDevice(this))
					{
						//check for a key mismatch (device should be running on default key index 0)
						int device_aes_key=GetAESKey();
						if(device_aes_key)
						{
							LOG(Logger::LOG_INFO, "Key mismatch detected. %s is running on key %d", GetSerial().c_str(), device_aes_key);
							manager->SetKeyMismatchDevice(GetSerial());
						}
						return false;
					}

					if(NeedsAESKeyChange() && !ChangeAESKey())
					{
						//remove the central address
						cd.PutValue(0, 10, 0, 3, 0, 0);
						//Enable manual peering of the device
						cd.PutValue(0, 2, 0, 0, 1, 0);
						cd.CommitToDevice(this);
						manager->SetKeyMismatchDevice(GetSerial());
						return false;
					}
					ScheduleConfig(true);
					break;
					//setzt die default Configuration die in der Devicedescription bzw. der Channeldescription hinterlegt sind  
					//auserdem werden alle besehenden Verkn�pfungen gel�scht
				case INSTALL_PUSH_DEFAULT_CONFIG: 
					
					config_data[0].InitDefault();
					//Set the central address list 0, index 10
					config_data[0].PushData(0, 10, 0, 3, 0, manager->GetBidcosAddress());
					//Disable manual peering of the device
					config_data[0].PushData(0, 2, 0, 0, 1, 1);
					//Setzt die Default configurationnen f�r das Ger�t 
					if(!description->SetDefaultConfig(this))
					{
						LOG(Logger::LOG_WARNING, "Faild to set default configuration to device: %s",serial.c_str());
					}

					if(!config_data[0].CommitToDevice(this))
					{
						//check for a key mismatch (device should be running on default key index 0)
						int device_aes_key=GetAESKey();
						if(device_aes_key)
						{
							LOG(Logger::LOG_INFO, "Key mismatch detected. %s is running on key %d", GetSerial().c_str(), device_aes_key);
							manager->SetKeyMismatchDevice(GetSerial());
						}
						return false;
					}

					if(NeedsAESKeyChange() && !ChangeAESKey())
					{
						//remove the central address
						config_data[0].PushData(0, 10, 0, 3, 0, 0);
						//Enable manual peering of the device
						config_data[0].PushData(0, 2, 0, 0, 1, 0);
						config_data[0].CommitToDevice(this);
						manager->SetKeyMismatchDevice(GetSerial());
						return false;
					}
					ScheduleInclusionPushMode();
					break;	
				default:
					break;
				}
				
				
			}else{
				CommitPendingConfig();
			}
		}else{
			CommitPendingConfig();
			ProcessPendingUpdate();
		}
		is_new_device=false;
		return true;
	}else if(frame.MatchType(BidcosFrame::FT_INFO_PARAM_ASYNC_PAIRS) || frame.MatchType(BidcosFrame::FT_INFO_PARAM_ASYNC_SEQ)){
		int channel=frame.GetIntValue(BidcosFrame::FIELD_INFO_PARAM_CH);
		if(channel){
			channels_t::iterator it=channels.find(channel);
			if(it!=channels.end()){
				it->second->ProcessAsyncParamInfo(frame);
			}
		}else{
			ProcessAsyncParamInfo(frame);
		}
		RequestSave();
	}

	UpdateDeviceFlags(frame);

	if(frame.MatchType(BidcosFrame::FT_INFO_STATUS) && frame.GetIntValue(BidcosFrame::FIELD_INFO_STATUS_CH)==0)
	{
		//device was just switched on
		CommitPendingConfig();
		//return true;
	}
	if((RxAlways() || RxNeedsBurst()) && IsConfigPending()){
		if(delete_deferred_flags){
			manager->DeleteDevice(this, delete_deferred_flags);
			return true;
		}
		ScheduleConfig(false);
	}

	int iterator=0;
	int channel_index;
	manager->MulticallCollectBegin();
	FrameDescription* fd=description->GetFrameDescription(frame, &channel_index, &iterator);
	while(fd){
		if(channel_index==(int)FrameDescription::ALL_CHANNELS){
			for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
				it->second->ProcessIncomingFrame(frame, fd);
			}
		}
		else if(fd->isValueFowardingEnabled()) {
			RFChannel* channel=dynamic_cast<RFChannel*>(GetInstance(channel_index));
			if(channel) {
				if(channel->ProcessIncomingFrame(frame, fd)) {
					const std::vector<int>& forwardingChannels = channel->GetDescription()->getValueForwardingChannels();
					for(unsigned int i = 0 ; i < forwardingChannels.size(); i++){
						RFChannel* forwardChannel=dynamic_cast<RFChannel*>(GetInstance(forwardingChannels.at(i)));
						if(forwardChannel != NULL) {
							forwardChannel->ProcessForwardedFrame(frame, fd);
						}
					}
				}
			}
		}
		else{
			RFChannel* channel=dynamic_cast<RFChannel*>(GetInstance(channel_index));
			if(channel)channel->ProcessIncomingFrame(frame, fd);
		}
		fd=description->GetFrameDescription(frame, &channel_index, &iterator);
	}

	if( frame.MatchType(BidcosFrame::FT_ACK_STATUS) || frame.MatchType(BidcosFrame::FT_INFO_STATUS) ){
		if(frame.GetSize()>=13){
			int rssi=-1*frame.GetByteData(13);
			unsigned int receiver=frame.GetReceiverAddress();
            if(receiver && rssi){
				if(receiver != manager->GetBidcosAddress()) {
					manager->UpdateRssiInfo( receiver, GetSerial(), rssi );
				} else {
					manager->UpdateRssiInfo( frame.GetInterfaceId(), GetSerial(), rssi );
					SetDeviceRSSI(rssi);
				}
            }
		}
	}
	manager->MulticallCollectEnd();

	return true;
}
bool RFDevice::InitVirtualInstance(BidcosFrame &sysinfoFrame)
{
	initBasicDeviceParameter(sysinfoFrame);
	return true;
}
void RFDevice::initBasicDeviceParameter(BidcosFrame &sysinfoFrame)
{
	int version=sysinfoFrame.GetIntValue(BidcosFrame::FIELD_SYSINFO_SWVER);
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "%d.%d", version>>4, version&0x0f);
		firmware_version=buffer;
		SetBidcosInterface(sysinfoFrame.GetInterfaceId(), false);
		sysinfo_frame=sysinfoFrame;
		if(channels.empty())CreateChannels();
		LOG(Logger::LOG_DEBUG, "Device firmware version is %s", firmware_version.c_str());
}
void RFDevice::CreateChannels()
{
	ClearChannels();
	int i=0;
	RFChannelDescription* ch_desc=description->GetChannelDescription(i);
	while(ch_desc){
		int start=ch_desc->GetIndex();
		int count=ch_desc->GetCount(this);
		for(int j=0;j<count;j++){
			AddChannel(j+start, ch_desc->GetType());
			RFChannel* ch=dynamic_cast<RFChannel*>(GetInstance(j+start));
			if(ch)ch->SetParent(this, j+start, ch_desc);
		}
		i++;
		ch_desc=description->GetChannelDescription(i);
	}
	if( GetAddress() && (!IsTeamDeviceInstance()) ){//TWIST-533: Das TeamDevice des HM-Sec-SD-2 überschrieb die AES Einstellungen im Coprocessor für das 'echte' Geräte.
        if(bidcos_interface_id.empty())bidcos_interface_id=RFManager::GetSingleton()->GetInterfaceConcentrator()->GetDefaultInterfaceId();
		RFManager::GetSingleton()->GetInterfaceConcentrator()->AddDevice(GetAddress(), bidcos_interface_id, roaming);
		SetAesPolicy();
	}
}

bool RFDevice::IsTeamDeviceInstance() {//TWIST-533
	return false;
}

RFDeviceDescription* RFDevice::GetDeviceDescription()
{
	return description;
}

bool RFDevice::AddChannel(int index, const std::string& type)
{
	RFChannelDescription* ch_desc=description->GetChannelDescription(type);
	if(!ch_desc){
		LOG(Logger::LOG_ERROR, "Could not get channel description for type %s", type.c_str());
		return false;
	}

	RFChannel* ch;
	if(!ch_desc->GetCreationTag().empty()){
		std::string creation_tag="channel_class_";
		creation_tag+=ch_desc->GetCreationTag();
		void* obj=hsscomm::type_registry::create(creation_tag.c_str());
		if(!obj){
			LOG(Logger::LOG_ERROR, "Channel class %s not supported", ch_desc->GetCreationTag().c_str());
			return false;
		}
		ch=dynamic_cast<RFChannel*>((RFChannel*)obj);
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

RFLogicalInstance* RFDevice::GetInstance(int channel_index)
{
	if(channel_index<0)return this;
	channels_t::iterator it=channels.find(channel_index);
	if(it==channels.end())return NULL;
	return it->second;
}

void RFDevice::OnTimer(uint32_t cookie)
{
	switch(cookie){
		case TIMER_RESET_FAIL_COUNTER:
			fail_counter=0;
		break;
		case TIMER_COMMIT_CONFIG:
			if(config_schedule & SCHEDULE_SET_ENFORCED){
				SetEnforcedParameters();
			}
			if(scheduledRetriesCount == (scheduledRetriesExecuted+1)) {
				scheduledRetriesExecuted++;
			}
			CommitPendingConfig();
			config_schedule=0;
		break;
		case TIMER_INCLUDE_PUSH_DEFAULT:
			PushDefaultConfig();
			config_schedule = 0;
			break;
		case TIMER_CYCLIC_TIMEOUT:
			unreach_reason = BidcosFrame::UNREACH_CYCLIC_TIMEOUT;
			UpdateUnreachFlags(NULL);
		break;
		case TIMER_SAVE:
			Save();
		break;
		default:
			RFLogicalInstance::OnTimer(cookie);
	}
}

bool RFDevice::SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event/*=false*/)
{
	//LOG(Logger::LOG_DEBUG, "RFDevice::SetInternalValue(%s, %s)", name.c_str(), val.toText().c_str());

	try{
		if(name=="STICKY_UNREACH"){
			if((int&)val)maintenance_flags|=FLAG_STICKY_UNREACH;
			else maintenance_flags&=~FLAG_STICKY_UNREACH;
		}else if(name=="LOWBAT"){
			SetValueAsDefined("LOWBAT");
			if((int&)val)maintenance_flags|=FLAG_LOWBAT;
			else maintenance_flags&=~FLAG_LOWBAT;
		}else if(name=="DUTYCYCLE"){
			if((int&)val)maintenance_flags|=FLAG_DUTYCYCLE;
			else maintenance_flags&=~FLAG_DUTYCYCLE;
		} else if (name=="RSSI_DEVICE") {
			rssi_device = (int&) val;
			SetValueAsDefined("RSSI_DEVICE");
			fire_event = false;
		} else if (name=="RSSI_PEER") {
			rssi_peer = (int&) val;
			fire_event = false;
			SetValueAsDefined("RSSI_PEER");
		} else if (name=="DEVICE_IN_BOOTLOADER") {
			deviceInBootloader = (int&) val;
		}else if (name=="UPDATE_PENDING") {
			firmwareUpdatePeding = (int&) val;
			SetValueAsDefined("UPDATE_PENDING");
		}
	}catch(XmlRpcException&){
		LOG(Logger::LOG_WARNING, "Tried to set unknown internal value %s", name.c_str());
		return false;
	}
	if(fire_event)this->SendInternalValueEvent(name, val);
	return true;
};

bool RFDevice::GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	try{
		if(name=="STICKY_UNREACH"){
			if(maintenance_flags&FLAG_STICKY_UNREACH)
			{

				(int&)(*val)= unreach_reason == 0 ? 1:unreach_reason;
			}
			else
			{
				(int&)(*val)=0;
			}
		}else if(name=="UNREACH"){
			if(maintenance_flags&FLAG_UNREACH)
			{
				(int&)(*val)=unreach_reason == 0 ? 1:unreach_reason;
			}
			else
			{
				(int&)(*val)=0;
			}
		}else if(name=="LOWBAT"){
			(int&)(*val)=(maintenance_flags&FLAG_LOWBAT)!=0;
		}else if(name=="DUTYCYCLE"){
			(int&)(*val)=(maintenance_flags&FLAG_DUTYCYCLE)!=0;
		}else if(name=="CONFIG_PENDING"){
			(int&)(*val)=IsConfigPending();
		}else if(name=="AES_KEY"){
			(int&)(*val)=cur_aes_key;
		}else if(name=="NEEDS_BURST"){
			(bool&)(*val)=description->RxNeedsBurst();
		} else if (name=="RSSI_DEVICE") {
			(int&)(*val)=rssi_device;
		} else if (name=="RSSI_PEER") {
			(int&)(*val)=rssi_peer;
		}else if(name=="DEVICE_IN_BOOTLOADER"){
			(int&)(*val) = deviceInBootloader;
		}else if(name=="UPDATE_PENDING"){
			(int&)(*val) = firmwareUpdatePeding;
		}else{
			LOG(Logger::LOG_WARNING, "Tried to get unknown internal value %s", name.c_str());
		}
	}catch(XmlRpcException& e){
		LOG(Logger::LOG_WARNING, "GetInternalValue() exception %s", e.getMessage().c_str());
		return false;
	}
	return true;
};
bool RFDevice::PushDefaultConfig()
{
	bool retval=true;
    config_data[0].InitDefault();
	SetDefaultConfig();
	if(NeedsAESKeyChange()){
		ChangeAESKey();
	}
	config_data_t::iterator it;
	for(it=config_data.begin();it!=config_data.end();it++)
	{
		if(!it->second.CommitToDevice(this))retval=false;
	}

	config_data_dirty=!retval;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(!it->second->PushDefaultConfig())retval=false;
	}
	return retval;
}
bool RFDevice::CommitPendingConfig()
{
	bool retval=true;

	if(NeedsAESKeyChange()){
		ChangeAESKey();
	}

	try{
		XmlRpcValue dummy;
		GetParamsetValues("MASTER", &dummy);
	}catch(XmlRpcException){}
	if(config_data_dirty){
		config_data_t::iterator it;
		for(it=config_data.begin();it!=config_data.end();it++){
			if(!it->second.CommitToDevice(this))retval=false;
		}
		config_data_dirty=!retval;
	}
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(!it->second->CommitPendingConfig())retval=false;
	}
	RequestSave();
	CheckConfigPendingEvent();
	return retval;
}

bool RFDevice::IsConfigPending()
{
//	LOG(Logger::LOG_DEBUG, "RFDevice::IsConfigPending() serial=%s, %d", GetSerial().c_str(), (int)config_data_dirty);
	if(GetDeviceDescription()->SupportsAES() && (RFManager::GetSingleton()->GetCurAESKey()!=cur_aes_key))return true;
	SetValueAsDefined("CONFIG_PENDING");
	if(config_data_dirty)return true;
	if(delete_deferred_flags)return true;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(it->second->IsConfigPending())return true;
	}
	return false;
}

bool RFDevice::SaveToXml(XMLNode* node)
{
	char buffer[16];
	node->addAttributeConst("serial", GetSerial().c_str());
	node->addAttributeConst("type", GetType().c_str());
	snprintf(buffer, sizeof(buffer), "0x%06X", GetAddress());
	node->addAttributeConst("address", buffer);
	snprintf(buffer, sizeof(buffer), "%d", cur_aes_key);
	node->addAttributeConst("aes_key_index", buffer);
	node->addAttributeConst("firmware_version", firmware_version.c_str());
    node->addAttributeConst("bidcos_interface", bidcos_interface_id.c_str());
	if(roaming)node->addAttributeConst("roaming", "true");

	if(deviceInBootloader)node->addAttributeConst("deviceInBootloader","true");

	if(config_data_dirty)node->addAttributeConst("config_dirty", "true");
	if(replace_history.size())
	{
		XMLNode replase_history_node = node->addChildConst("replaceHistory");
		replaceHistory_t::iterator it;
		for(it = replace_history.begin();it!= replace_history.end();++it)
		{
			replase_history_node.addChildConst(it->c_str());
		}
	}

	XMLNode config_node=node->addChildConst("config");
	if(!config_data[0].SaveToXml(&config_node))return false;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++)
	{
		XMLNode channel_node=node->addChildConst("channel");
		if(!it->second->SaveToXml(&channel_node))return false;
	}
	if(sysinfo_frame.GetSize()){
		node->addAttributeConst("sysinfo", sysinfo_frame.ToString().c_str());
	}
	XMLNode values_node=node->addChildConst("values");
	if(!ValueStore::SaveToXml(&values_node))return false;
	return true;
}

bool RFDevice::LoadFromXml(XMLNode& node)
{
	is_new_device=false;
	const char* temp;

	bool sysinfo_found=false;
	temp=node.getAttribute("sysinfo");
	if(temp){
		if(strlen(temp)>52){
			temp+=10;
		}
		sysinfo_frame.FromString(temp);
		sysinfo_found=true;
	}

	RFDeviceDescription* desc=RFManager::GetSingleton()->GetSystemDescription()->GetDeviceBySysinfo(sysinfo_frame, &type);
	if(!desc){
		temp=node.getAttribute("type");
		if(!temp)return false;
		desc=RFManager::GetSingleton()->GetSystemDescription()->GetDeviceByType(temp);
		if(!desc){
			LOG(Logger::LOG_ERROR, "Unknown device type %s", temp);
			return false;
		}
		type=temp;
	}
	temp=node.getAttribute("serial");
	if(!temp)return false;
	SetSerial(temp);

	SetDeviceDescription(desc);
	if(sysinfo_found)CreateChannels();

	temp=node.getAttribute("firmware_version");
	if(temp)firmware_version=temp;

	temp=node.getAttribute("address");
	if(!temp)return false;
	int address=strtoul(temp, NULL, 0);
	SetAddress(address);

	temp=node.getAttribute("aes_key_index");
	if(temp){
		cur_aes_key=strtol(temp, NULL, 0);
		SetValueAsDefined("AES_KEY");
		if(cur_aes_key<0)cur_aes_key=0;
	}

	temp=node.getAttribute("bidcos_interface");
	if(temp){
        bidcos_interface_id=temp;
	}

	temp=node.getAttribute("deviceInBootloader");
	deviceInBootloader=temp && temp[0] == 't';

	temp=node.getAttribute("roaming");
	roaming=temp && temp[0]=='t';

	temp=node.getAttribute("config_dirty");

	config_data_dirty=temp && temp[0]=='t';
	
	XMLNode replace_history_node = node.getChildNode("replaceHistory");
	if(!replace_history_node.isEmpty())
	{
		int chile_cound = replace_history_node.nChildNode();
		for(int i = 0;i<chile_cound;++i)
		{
			XMLNode replace = replace_history_node.getChildNode(i);
			this->replace_history.push_back(replace.getName());
		}
	}

	XMLNode config_node=node.getChildNode("config");
	if(config_node.isEmpty())return false;
	if(!config_data[0].LoadFromXml(config_node))return false;

	int i=0;
	XMLNode channel_node=node.getChildNode("channel", &i);
	while(!channel_node.isEmpty()){
		temp=channel_node.getAttribute("index");
		if(!temp)return false;
		int index=strtol(temp, NULL, 0);
		if(sysinfo_found){
			RFChannel* channel=dynamic_cast<RFChannel*>(GetInstance(index));
			if(channel){
				if(!channel->LoadFromXml(channel_node))return false;
			}else{
				LOG(Logger::LOG_WARNING, "Skipping channel %d (%s)", index, temp);
			}
		}else{
			temp=channel_node.getAttribute("type");
			if(!temp)return false;
			AddChannel(index, temp);
			RFChannel* channel=dynamic_cast<RFChannel*>(GetInstance(index));
			if(channel){
				if(!channel->LoadFromXml(channel_node))return false;
			}else{
				LOG(Logger::LOG_ERROR, "Could not add channel %s", temp);
				return false;
			}
		}
		channel_node=node.getChildNode("channel", &i);
	}
	XMLNode values_node=node.getChildNode("values");
	if(!values_node.isEmpty()){
		if(!ValueStore::LoadFromXml(values_node))return false;
	}
    if(bidcos_interface_id.empty())bidcos_interface_id=RFManager::GetSingleton()->GetInterfaceConcentrator()->GetDefaultInterfaceId();
	RFManager::GetSingleton()->GetInterfaceConcentrator()->AddDevice(this, GetAddress(), bidcos_interface_id, roaming);
	this->last_config_pending = !IsConfigPending(); // damit nach dem start der config panding wert neu zum logikprozess �bertragen wird
	CheckConfigPendingEvent();
	if(description->GetCyclicTimeout()){
		RequestTimer(description->GetCyclicTimeout(), TIMER_CYCLIC_TIMEOUT);
	}
	SetAesPolicy();
	return true;
}

bool RFDevice::Save()
{
	LOG(Logger::LOG_DEBUG, "Saving device %s", GetSerial().c_str());
    std::string filename=RFManager::GetSingleton()->GetDeviceFilesDir();
	OSCompat::MakeDirectory(filename.c_str());
    filename+=OSCompat::PATH_SEPARATOR+GetSerial()+".dev";
	std::string new_filename=filename+".new";
	std::string bak_filename=filename+".bak";
	XMLNode n=XMLNode::createXMLTopNode();
	n.setNameConst("device");
	if(!SaveToXml(&n)) {
		return false;
		LOG(Logger::LOG_ERROR, "Error saving device.");
	}
	char* s=n.createXMLString(1);
	if(!s)return false;

	FILE* fd = fopen(new_filename.c_str(), "wb");
	if(fd == NULL) {
		unlink(new_filename.c_str());
		free(s);
		return false;
	}
	fprintf(fd, "%s", s);
	fflush(fd);
#ifndef WIN32
	fsync(fileno(fd));
#endif
	fclose(fd);
	

	unlink(bak_filename.c_str());

	rename(filename.c_str(), bak_filename.c_str());
	if(rename(new_filename.c_str(), filename.c_str())) {
		free(s);
		return false;
	}
	unlink(bak_filename.c_str());
	free(s);
	return true;
}

bool RFDevice::Delete()
{
	RFManager::GetSingleton()->GetInterfaceConcentrator()->RemoveDevice(GetAddress());
	LOG(Logger::LOG_DEBUG, "Deleting persistent data for device %s", GetSerial().c_str());
    std::string filename=RFManager::GetSingleton()->GetDeviceFilesDir();
    filename+=OSCompat::PATH_SEPARATOR+GetSerial()+".dev";
	return unlink(filename.c_str())==0;
}

bool RFDevice::AbortDelete()
{
	delete_deferred_flags=0;
	CheckConfigPendingEvent();
	return true;
}

void RFDevice::CheckWakeup()
{
	if(RxNeedsWakeup()){
		if( this->firmwareUpdatePeding || IsConfigPending() || !after_wakeup_queue.empty() ){
		    if(RxSupportLazyConfig())
		    {
		        RFManager::GetSingleton()->GetInterfaceConcentrator()->AddDeviceWakeupRequest(GetAddress(),true);
		    }
		    else
		    {
		        RFManager::GetSingleton()->GetInterfaceConcentrator()->AddDeviceWakeupRequest(GetAddress());
		    }

		}else{
			RFManager::GetSingleton()->GetInterfaceConcentrator()->RemoveDeviceWakeupRequest(GetAddress());
		}
	}
}

void RFDevice::CheckConfigPendingEvent(bool force)
{
	CheckWakeup();
	bool config_pending=IsConfigPending();
	if(RxAlways()) {
		if(config_pending) {
			ScheduleConfigRetry();
		}
		else {
			scheduledRetriesCount = 0;
			scheduledRetriesExecuted = 0;
		}
	}
	if((config_pending == last_config_pending) && !force)return;
	last_config_pending=config_pending;
	XmlRpcValue val;
	(int&)val=config_pending;
	SendInternalValueEvent("CONFIG_PENDING", val);
}


bool RFDevice::ClearConfigCache()
{
	bool retval=true;
	config_data.clear();
	config_data_dirty=false;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++)
	{
		retval&=it->second->ClearConfigCache();
	}
	RequestSave();
	CheckConfigPendingEvent();
	return retval;
}
void RFDevice::ScheduleInclusionPushMode()
{
	int32_t ms=0;
	KillTimer(TIMER_INCLUDE_PUSH_DEFAULT);
	RequestTimer(ms, TIMER_INCLUDE_PUSH_DEFAULT);
	config_schedule=SCHEDULE_COMMIT;
	
}
void RFDevice::ScheduleConfig(bool set_enforced_parameters, int32_t delay)
{
	KillTimer(TIMER_COMMIT_CONFIG);
	RequestTimer(delay, TIMER_COMMIT_CONFIG);
	config_schedule=SCHEDULE_COMMIT;
	if(set_enforced_parameters)config_schedule |= SCHEDULE_SET_ENFORCED;
}

void RFDevice::ScheduleConfig(bool set_enforced_parameters) {
	ScheduleConfig(set_enforced_parameters, 0);
}

bool RFDevice::ScheduleConfigRetry() {
	if(scheduledRetriesCount != scheduledRetriesExecuted) {
		return false;
	}
	scheduledRetriesCount++;
	if(scheduledRetriesCount > 2) {
		return false;
	}
	const int32_t timeout = (int32_t)scheduledRetriesCount * 5000L;
	LOG(Logger::LOG_INFO, "RFDevice::ScheduleConfigRetry(): Scheduling send retry for device %s in %d seconds.", GetSerial().c_str(), timeout/1000);
	ScheduleConfig(false, timeout);
	return true;
}

std::vector<int> RFDevice::ListChannels()
{
	std::vector<int> retval;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(!it->second->GetDescription()->IsHidden())retval.push_back(it->first);
	}
	return retval;
}

bool RFDevice::ChangeAESKey()
{
	if(RFManager::GetSingleton()->GetCurAESKey()==0){
		LOG(Logger::LOG_WARNING, "Not changing key for device %s back to factory key.", GetSerial().c_str());
		return false;
	}

	int keys_to_try[4];
	keys_to_try[0]=cur_aes_key;
	keys_to_try[3]=RFManager::GetSingleton()->GetTempAESKey();
	if(RFManager::GetSingleton()->GetTempAESKey()>0){
		keys_to_try[0]=RFManager::GetSingleton()->GetTempAESKey();
		keys_to_try[3]=cur_aes_key;
	}
	keys_to_try[1]=RFManager::GetSingleton()->GetCurAESKey();
	keys_to_try[2]=keys_to_try[1]-1;

	int key=0;
	BidcosFrame frame;
	frame.SetType(BidcosFrame::FT_AES_CONTAINER_KEY);
	frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	frame.SetIntValue(BidcosFrame::FIELD_CONTAINER_KEY_IDX, key);
	unsigned int i;
	for(i=0;i<sizeof(keys_to_try)/sizeof(keys_to_try[0]);i++){
		fail_counter=0;
		KillTimer(TIMER_RESET_FAIL_COUNTER);
		key=keys_to_try[i];
		if(key<0)continue;
		LOG(Logger::LOG_DEBUG, "Trying to change key for device %s using key index %d", GetSerial().c_str(), key);

		frame.SetIntValue(BidcosFrame::FIELD_CONTAINER_KEY_IDX, key);
		if(!SendFrame(&frame)){
			LOG(Logger::LOG_DEBUG, "Key exchange for device %s using key index %d failed", GetSerial().c_str(), key);
		}else{
			break;
		}
	}
	if(i>=sizeof(keys_to_try)/sizeof(keys_to_try[0])){
		LOG(Logger::LOG_DEBUG, "Key exchange for device %s error sending first part", GetSerial().c_str());
		return false;
	}
	frame.SetIntValue(BidcosFrame::FIELD_CONTAINER_KEY_PART, 1);
	if(!SendFrame(&frame)){
		LOG(Logger::LOG_DEBUG, "Key exchange for device %s error sending second part", GetSerial().c_str());
		return false;
	}
	cur_aes_key=RFManager::GetSingleton()->GetCurAESKey();
	SetValueAsDefined("AES_KEY");
	RFManager::GetSingleton()->GetInterfaceConcentrator()->SetDeviceAesPolicy(GetAddress(), cur_aes_key, GetChannelAESMask());

	LOG(Logger::LOG_DEBUG, "Key exchange for device %s succeeded", GetSerial().c_str());
	RequestSave();
	return true;
}

void RFDevice::UpdateCurAESKey(BidcosFrame* frame)
{
	if(frame->GetAuthKey() != BidcosFrame::INVALID_AUTH_KEY){
		int key=frame->GetAuthKey();
		//neither accept the default key nor the current AES key
		if(key!=0 && key!=cur_aes_key){
			cur_aes_key=key;
			SetValueAsDefined("AES_KEY");
			if(key>0)RFManager::GetSingleton()->GetInterfaceConcentrator()->SetDeviceAesPolicy(GetAddress(), cur_aes_key, GetChannelAESMask());
		}
	}
}

void RFDevice::SetKeyIndex(int key_index)
{
//	LOG(Logger::LOG_DEBUG, "SetKeyIndex(%d)", key_index);
	if(key_index!=cur_aes_key){
		cur_aes_key=key_index;
		SetValueAsDefined("AES_KEY");
		SetValueAsDefined("AES_KEY");
		RequestSave();
		if(IsConfigPending())ScheduleConfig(false);
	}
}

bool RFDevice::SetAesPolicy()
{
	int key=cur_aes_key;
	if(is_new_device){
		if(RFManager::GetSingleton()->GetTempAESKey()<0)key=0;
		else key=RFManager::GetSingleton()->GetTempAESKey();
	}
	uint64 channel_mask=GetChannelAESMask();
	aes = (channel_mask != 0);
	return RFManager::GetSingleton()->GetInterfaceConcentrator()->SetDeviceAesPolicy(GetAddress(), key, channel_mask);
}

uint64 RFDevice::GetChannelAESMask()
{
	uint64 mask=0;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(it->second->GetAES())mask |= ( uint64(1) << it->second->GetIndex() );
	}
	return mask;
}

bool RFDevice::NeedsAESKeyChange(){
	if(!GetDeviceDescription()->SupportsAES())return false;
	return RFManager::GetSingleton()->GetCurAESKey()!=cur_aes_key;
}

bool RFDevice::FactoryReset()
{
	BidcosFrame frame;
	frame.SetType(BidcosFrame::FT_CENTRAL_RESET);
	frame.SetIntValue(BidcosFrame::FIELD_CENTRAL_CHANNEL, 0);
	if(!SendFrame(&frame))return false;
	return true;
}

bool RFDevice::UnpeerCentral()
{
	RFConfigData cd;
	bool retval=false;

	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		if(!it->second->UnpeerCentral())goto exit;
	}

	//Set the central address list 0, index 10 to 0x000000
	cd.PutValue(0, 10, 0, 3, 0, 0x000000);
	//Enable manual peering of the device
	cd.PutValue(0, 2, 0, 1, 0, 0);
	if(!cd.CommitToDevice(this))goto exit;

	retval=true;

exit:
	if(!retval)CheckConfigPendingEvent();
	RequestSave();
	return retval;
}

bool RFDevice::GetLinks(int flags, link_map_t* result)
{
	flags &= ~GL_FLAG_GROUP;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
LOG(Logger::LOG_ALL, "RFDevice::GetLinks(): Device %s", GetSerial().c_str());
		if(it->second->GetDescription()->IsHidden())continue;
LOG(Logger::LOG_ALL, "RFDevice::GetLinks(): Getting links for Device %s Channel %d", GetSerial().c_str(), it->second->GetIndex());
		if(!it->second->GetLinks(flags, result)){
			LOG(Logger::LOG_ERROR, "%s: GetLinks() for channel %d failed", GetSerial().c_str(), it->second->GetIndex());
			return false;
		}
	}
	return true;
}

void RFDevice::SetConfigDevDirty()
{
	RFLogicalInstance::SetConfigDevDirty();
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		it->second->SetConfigDevDirty();
	}
	config_data_dirty=true;
}

bool RFDevice::RestoreConfigToDevice()
{
	SetConfigDevDirty();
	return CommitPendingConfig();
}

void RFDevice::UpdateDeviceFlags(BidcosFrame& frame)
{
	uint32_t lowbat=maintenance_flags&FLAG_LOWBAT;
	uint32_t duty_cycle=maintenance_flags&FLAG_DUTYCYCLE;
	switch(frame.GetType()){
		case BidcosFrame::FT_ACK_OR_NACK:
			if(!frame.MatchType(BidcosFrame::FT_ACK_STATUS))return;
			frame.GetIntValue(12, 7, 0, 1, &lowbat);
			frame.GetIntValue(12, 0, 0, 1, &duty_cycle);
			break;
		case BidcosFrame::FT_INFO:
			if(!frame.MatchType(BidcosFrame::FT_INFO_STATUS))return;
			frame.GetIntValue(12, 7, 0, 1, &lowbat);
			frame.GetIntValue(12, 0, 0, 1, &duty_cycle);
			break;
		case BidcosFrame::FT_TIME:
			frame.GetIntValue(9, 0, 0, 1, &lowbat);
			break;
		case BidcosFrame::FT_SWITCH:
		case BidcosFrame::FT_CONDITIONAL_SWITCH:
		case BidcosFrame::FT_LEVEL:
		case BidcosFrame::FT_WEATHER:
			frame.GetIntValue(9, 7, 0, 1, &lowbat);
			break;
		default:
			return;
	}
	if( lowbat || (maintenance_flags&FLAG_LOWBAT) ){
		XmlRpcValue v=(int)lowbat;
		SetInternalValue("LOWBAT", v, true);
	}
	if( duty_cycle || (maintenance_flags&FLAG_DUTYCYCLE) ){
		XmlRpcValue v=(int)lowbat;
		SetInternalValue("DUTY_CYCLE", v, true);
	}
}

void RFDevice::ScheduleDelete(int flags)
{
	delete_deferred_flags=flags|0x8000;
	CheckConfigPendingEvent();
}

void RFDevice::QueueAfterWakeupFrame(BidcosFrame& frame)
{
	after_wakeup_queue.push(frame);
	if(after_wakeup_queue.size()>10) {
		LOG(Logger::LOG_WARNING, "QueueAfterWakeupFrame: Queue size exceeded, dropped oldest frame.");
		after_wakeup_queue.pop();
	}

	CheckWakeup();
	LOG(Logger::LOG_DEBUG, "QueueAfterWakeupFrame(%s)", BidcosFrameDecoder::ToString(&frame).c_str());
}

bool RFDevice::IsEnoughSpaceLeftInWakeupFrameQueue(const unsigned int desiredSpace/*=1*/) {
	return ((after_wakeup_queue.size()+desiredSpace) <= 10);
}

void RFDevice::SendAfterWakeupFrames()
{
	while(!after_wakeup_queue.empty()){
		BidcosFrame frame=after_wakeup_queue.front(); //cn
		after_wakeup_queue.pop();
		frame.SetCtrl( (frame.GetCtrl() | BidcosFrame::CTRL_WAKEUP) );//used in SendFrame to override call of NeedsBurst()
		if(SendFrame(&frame)){
			BidcosFrame* response=frame.GetResponse();
			if(response)ProcessIncomingFrame(*response);
		}else{
			LOG(Logger::LOG_WARNING, "SendAfterWakeupFrame %s failed", BidcosFrameDecoder::ToString(&frame).c_str());
			break;
		}
	}
}

void RFDevice::SetSerial(const std::string s)
{
	serial=s;
	for(channels_t::iterator it=channels.begin();it!=channels.end();it++){
		it->second->SetSerial(s);
	}
}

void RFDevice::UpdateUnreachFlags(BidcosFrame* frame)
{
	//if(msg)LOG(Logger::LOG_DEBUG, "UpdateUnreachFlags(%s)", msg->DumpToString().c_str());
	if(frame){


		bool auth_violation=GetAES() && (!frame->WasAuthenticated());
		if(!auth_violation || !description->UnreachCheckAES()){
			if(maintenance_flags&FLAG_UNREACH){
				maintenance_flags&=~(FLAG_UNREACH);
				SetValueAsDefined("UNREACH");
				SendInternalValueEvent("UNREACH");
			}
			if(description->GetCyclicTimeout()){
				SetValueAsDefined("UNREACH");
				KillTimer(TIMER_CYCLIC_TIMEOUT);
				RequestTimer(description->GetCyclicTimeout(), TIMER_CYCLIC_TIMEOUT);
			}
		}
	}else{
		if(!(maintenance_flags&FLAG_UNREACH)){
			maintenance_flags|=(FLAG_UNREACH);
		}
		if(!(maintenance_flags&FLAG_STICKY_UNREACH)){
			maintenance_flags|=(FLAG_STICKY_UNREACH);
		}
		SetValueAsDefined("UNREACH");
		SetValueAsDefined("STICKY_UNREACH");
		SendInternalValueEvent("UNREACH");
		SendInternalValueEvent("STICKY_UNREACH");
	}
}

void RFDevice::RequestSave()
{
	//Next time we are idle, Save() will be called
	KillTimer(TIMER_SAVE);
	RequestTimer(0, TIMER_SAVE);
}

bool RFDevice::SetBidcosInterface(const std::string& interface_id, bool roaming)
{
    bidcos_interface_id=interface_id;
    this->roaming=roaming;
    if(bidcos_interface_id.empty())bidcos_interface_id=RFManager::GetSingleton()->GetInterfaceConcentrator()->GetDefaultInterfaceId();
	RFManager::GetSingleton()->GetInterfaceConcentrator()->AddDevice(GetAddress(), bidcos_interface_id, roaming);
    RequestSave();
    return true;
}

void RFDevice::SetDeviceRSSI(int rssi)
{
	XmlRpcValue value = rssi;
	SetInternalValue("RSSI_DEVICE", value, true);
}

void RFDevice::SetPeerRSSI(int rssi)
{
	XmlRpcValue value = rssi;
	SetInternalValue("RSSI_PEER", value, true);
}

bool RFDevice::HasInternalKeys() {
	std::string paramsetKey = "MASTER";
	std::string parameterKey = "INTERNAL_KEYS_VISIBLE";
	RFParamset* ps = description->GetParamset(paramsetKey);
	HSSParameter* param = ps->GetParameter(parameterKey);

	return (param != NULL);
}

void RFDevice::SetValueAsDefined(const std::string& name) {
	channels_t::iterator it = channels.find(0);
		if(it != channels.end())
			it->second->SetValueAsDefined(name);
}

void RFDevice::SetValueAsUndefined(const std::string& name) {
	channels_t::iterator it = channels.find(0);
	if(it != channels.end())
		it->second->SetValueAsUndefined(name);
}

bool RFDevice::set100kDataRate(void)
{
    for (int i = 0; i < 3; ++i)
    {
        if (!RFManager::GetSingleton()->GetInterfaceConcentrator()->SetInterfaceTo10kMode(address))
        {
            continue;
        }
        BidcosFrame setDataRateFrame;
        setDataRateFrame.SetType(BidcosFrame::FT_WRITE_TRX_REG);
        setDataRateFrame.SetCtrl(0);
        setDataRateFrame.SetReceiverAddress(address);
        setDataRateFrame.SetSenderAddress(RFManager::GetSingleton()->GetBidcosAddress());
        char charPl[] = { (char) 0x10, (char) 0x5B, (char) 0x11, (char) 0xF8, (char) 0x15, (char) 0x47,
                (char)0x0B,(char)0x08,(char)0x1A,(char)0x1C,(char)0x19,(char)0x1D,(char)0x1B,(char)0xC7,
                (char)0x1C,(char)0x00,(char)0x1D,(char)0xB2,(char)0x21,(char)0xB6,(char)0x23,(char)0xEA };
        std::string payload;
        payload.append(charPl,sizeof(charPl));
        setDataRateFrame.SetStringValue(9, sizeof(charPl), payload);
        if (!RFManager::GetSingleton()->GetInterfaceConcentrator()->SendFrame(&setDataRateFrame))
        {
            continue;
        }
        //Datenrate im Interface auf 100kbit/s stellen
        if (!RFManager::GetSingleton()->GetInterfaceConcentrator()->SetInterfaceTo100kMode(address))
        {
            continue;
        }
        setDataRateFrame.SetCtrl(BidcosFrame::CTRL_BIDI);
        if (RFManager::GetSingleton()->GetInterfaceConcentrator()->SendFrame(&setDataRateFrame))
        {
            return true;
        }
    }
    return false;
}
bool RFDevice::UpdateFirmware()
{
	//TODO: vorerst wird der reine Ablauf eines Updates implamentiert. Im anschluss m�ssen noch weiter sachen Implementiert werden:
	//			- eventuell muss persistiert werden das ein Update begonnen wurde
	//			- Wenn immomment kein Update durchgef�hrt werden kann, soll dann sp�ter das update automatisch durchgef�hrt werden?
	try
	{
		int tryCount = 0;
		double UpdateTime = time_millis();
		int typeNumber = sysinfo_frame.GetIntValue(BidcosFrame::FIELD_SYSINFO_TYPE);
		RFManager::GetSingleton()->GetInterfaceConcentrator()->SetInterfaceTo10kMode(this->address);
		if(!RFManager::GetSingleton()->GetInterfaceConcentrator()->IsInterfaceUpdatable(this->address))
		{
			throw XmlRpcException("the interface is not able to carry an update",-7);
		}
		UpdateFile updateFile = RFManager::GetSingleton()->GetFirmwareManager()->GetFirmware(typeNumber);
		if(!updateFile.isInitialized())
		{
			throw XmlRpcException("Unable to load updatefile");
		}
		if(!RFManager::GetSingleton()->GetInterfaceConcentrator()->DutyCycleForUpdate(this->address,updateFile))
		{
			throw XmlRpcException("not enough DutyCycle free",-8);
		}
		int flag = BidcosFrame::CTRL_BIDI;
		if(RxNeedsBurst())
		{
			flag |= BidcosFrame::CTRL_BURST;
		}
		//Bootloader im Gerät starten
		BidcosFrameStartBootloader startBootloaderFrame;
		startBootloaderFrame.SetType(BidcosFrame::FT_CENTRAL_START_BOOTLOADER);
		startBootloaderFrame.SetCtrl(flag);
		startBootloaderFrame.SetReceiverAddress(address);
		startBootloaderFrame.SetSenderAddress(RFManager::GetSingleton()->GetBidcosAddress());
		if(!deviceInBootloader)
		{
			if(!RFManager::GetSingleton()->GetInterfaceConcentrator()->SendFrame(&startBootloaderFrame))
			{
			    BidcosFrame *response = startBootloaderFrame.GetResponse();
			    if(!(response != NULL && !response->IsNack()))
			    {
			        SetUpdatePennding(true);
			        CheckWakeup();
			        throw XmlRpcException("Bootloader in device "+this->serial+" didn't start");
			    }
			}
		}
		//Datenrate im Gerät auf 100kbit/s stellen
		if(!this->set100kDataRate())
		{
		    throw XmlRpcException("Unable to set datarate to 100k");
		}

		int uFrameCount = updateFile.getUpdateFrameCount(); // anzahl der zu senden UpdateRahmen
		//Speichern, dass das Ger�t im Bootloader ist -> Servicemeldung 
		XmlRpcValue devInBl = 1;
		SetInternalValue("DEVICE_IN_BOOTLOADER",devInBl,true);
		Save();
		//Alle updaterahmen �bertragen
		bool firstUpdateFrame = true;
		for (int i = 0; i < uFrameCount ; ++i)
		{
			const std::string &updateFrame = updateFile.getUpdateFrame(i);
			if (!RFManager::GetSingleton()->GetInterfaceConcentrator()->SendUpdataFrame(updateFrame,address))
			{
				tryCount++;
				if (tryCount < 4)
				{
					i--;
					continue;
				}
				else
				{
					LOG(Logger::LOG_ERROR, "Firmwareupdate feur das Ger�t mit der Seriennummer %s ist fehlgeschlagen ", this->serial.c_str());
					if(firstUpdateFrame)
					{   // Der erste Rahmen wure vom Ger�t nach 3 Versuchen nicht best�tigt -> wahrscheinlich bei 100kbit/s nicht mehr in Reichweite 
						throw XmlRpcException("Device is out of range",-9);
					}
					else
					{	//Update wurde begonnen, konnte aber nicht vollst�ndig durchgef�hrt werden
						throw XmlRpcException("Firmware update aborted");
					}
				}
			}
			else
			{
				firstUpdateFrame = false;
				tryCount = 0;
			}
		}
		//Datenrate im Interface wieder auf 10kbit/s setzen
		RFManager::GetSingleton()->GetInterfaceConcentrator()->SetInterfaceTo10kMode(address);
		//Ger�tedaten anpassen
		this->firmwareUpdatePeding = false;
		SetValueAsDefined("UPDATE_PENDING");
		this->Rebuild();
		devInBl = 0;
		SetInternalValue("DEVICE_IN_BOOTLOADER",devInBl,true);
		Save();
		LOG(Logger::LOG_DEBUG, "Firmwareupdate completed after %f ms ", time_millis() - UpdateTime);
	}
	catch(XmlRpcException &e)
	{
		RFManager::GetSingleton()->GetInterfaceConcentrator()->SetInterfaceTo10kMode(address);
		LOG(Logger::LOG_ERROR,"Firmware update faild! %s",e.getMessage().c_str());
		throw e;
	}
	return true;
}
const std::string RFDevice::GetAvailableFirmware()
{
	//TODO: ob ein Firmwareupdate m�glich ist h�ngt auch von der momentan installierten Version ab 
	// z.B. 16LED Statusanzeige ist erst ab Version 1.1 updatef�hig in den entsprechenden 
	// Ger�tebeschreibungensdateien muss dies vermerkt werden
	LOG(Logger::LOG_DEBUG, "GetAvailableFirmware for %s ", this->type.c_str());
	if(description->IsUpdatable(this->sysinfo_frame))
	{
		int typeNumber = sysinfo_frame.GetIntValue(BidcosFrame::FIELD_SYSINFO_TYPE);
		 return RFManager::GetSingleton()->GetFirmwareManager()->GetFirmwareVersion(typeNumber);
	}
	else
	{
		return std::string("");
	}
}

bool RFDevice::Rebuild()
{
	std::string typeNew;

	int versionNew, typeNumber;
	char buffer[16];

	typeNumber = sysinfo_frame.GetIntValue(BidcosFrame::FIELD_SYSINFO_TYPE);
	RFManager::GetSingleton()->GetFirmwareManager()->GetFirmwareVersion(typeNumber,&versionNew);
	BidcosFrame newSysinfo(sysinfo_frame);
	newSysinfo.SetIntValue(BidcosFrame::FIELD_SYSINFO_SWVER,((versionNew >> 4) & 0xf0)|(versionNew & 0x0f));
	RFDeviceDescription *newDescription = RFManager::GetSingleton()->GetSystemDescription()->GetDeviceBySysinfo(newSysinfo,&typeNew);
	sysinfo_frame = newSysinfo;
	description = newDescription;
	
	snprintf(buffer, sizeof(buffer), "%d.%d", versionNew>>8, versionNew&0xff);
	firmware_version=buffer;
	this->channels.clear();
	this->CreateChannels();

	Save();

	if(RFManager::GetSingleton()->CallUpdateDeviceOnOTAUDeviceRebuild()) {
		RFManager::GetSingleton()->ReportUpdate(this->serial,HSSManager::UPDATE_HINT_ALL);
	}
	else {
		RFManager::GetSingleton()->ReportNewDevice(this);
	}

	return true;
}

bool RFDevice::GetConfig(XmlRpc::XmlRpcValue* c)
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
void RFDevice::SetUpdatePennding(bool val)
{
	XmlRpcValue xmlVal;
	if(val)
	{
		xmlVal = 1;
	}
	else
	{
		xmlVal=0;
	}
	SetInternalValue("UPDATE_PENDING",xmlVal,true);
}

bool RFDevice::ProcessPendingUpdate()
{
	if(this->firmwareUpdatePeding)
	{
		try
		{
			UpdateFirmware();
			SetUpdatePennding(false);
			return true;
		}
		catch(XmlRpcException &e)
		{
		    SetUpdatePennding(true);
		    LOG(Logger::LOG_ERROR, "Pendingupdate faild!! %s ",e.getMessage().c_str());
		}
	}
	return false;
}

bool RFDevice::replaceDevice(RFDevice *oldDevice)
{
	std::map<std::string,RFDevice*> linktDevices;
	if(!oldDevice)
	{
		return false;
	}
	//Kompatibilit�t pr�fen
	if(!oldDevice->IsReplaceCompatible(this))
	{
		return false;
	}
	//Konfigurationsdaten vom alten auf das neue Ger�t �nertragen

	//Verkn�pfungen bearbeiten
	channels_t::iterator it;
	for(it = oldDevice->channels.begin(); it != oldDevice->channels.end(); ++it)
	{		
		RFChannel *newChannel = dynamic_cast<RFChannel*>( this->GetInstance(it->second->GetIndex()));
		std::string newPeerSerial = newChannel->GetSerial();
		std::vector<std::string> peers;
		it->second->GetLinkPeers(&peers);
		std::vector<std::string>::iterator it_peers;
		for(it_peers = peers.begin(); it_peers != peers.end();++it_peers)
		{
			RFChannel *ch = dynamic_cast<RFChannel*>(RFManager::GetSingleton()->GetInstance(*it_peers));
			if(!ch) continue;
			RFDevice *deviece = ch->GetDevice();
			if(deviece != oldDevice)
			{
			    ch->replacePeer(it->second->GetSerial(),newPeerSerial);
			    //ch->CommitPendingConfig();
				linktDevices[deviece->GetSerial()] = deviece;
			}
		}
		RFTeamChannel *teamChannel = it->second->GetTeamChannel();
		if(teamChannel)
		{
			teamChannel->ReplaceTeamChannel(it->second->GetSerial(),newPeerSerial);
			teamChannel->CommitPendingConfig();
		}
	}

	for(it = oldDevice->channels.begin(); it != oldDevice->channels.end(); ++it)
    {
        RFChannel *newChannel = dynamic_cast<RFChannel*>(this->GetInstance(it->second->GetIndex()));
        newChannel->replaceChannel(it->second);
        //newChannel->CommitPendingConfig();
    }
	//Konfigurationen der Partner speichenr
	std::map<std::string,RFDevice*>::iterator linkDevIt;
	for(linkDevIt = linktDevices.begin(); linkDevIt != linktDevices.end();++linkDevIt)
	{
	    linkDevIt->second->CommitPendingConfig();
	    //linkDevIt->second->Save();
		//linkDevIt->second->CheckConfigPendingEvent();
	}
	//tauschhistorie hinzuf�gen
	this->replace_history.push_back(oldDevice->serial);
	this->roaming = oldDevice->roaming;
	RFManager::GetSingleton()->GetInterfaceConcentrator()->AddDevice(this->address,this->bidcos_interface_id,this->roaming );

	this->replaceRFConfigData(oldDevice);
	this->last_config_pending = false;
	this->config_data_dirty = true;
	return true;
}
const std::vector<std::string> & RFDevice::GetReplaceHistory()
{
	return this->replace_history; 
}

void RFDevice::DeleteFromReplaceHistory(std::string serial)
{
	replaceHistory_t::iterator it;
	//bool eraseSerial = false;
	for(it = replace_history.begin();it != replace_history.end();++it)
	{
		if(!it->compare(serial))
		{
		    //eraseSerial = true;
			break;
		}
	}
	this->replace_history.erase(it);
}

bool RFDevice::IsReplaceCompatible(RFDevice *newDevice)
{
	if(this == newDevice)
	{
		return false;
	}
	//Kan�le vergleichen 
	channels_t::iterator it_oldChannel;
	channels_t::iterator it_newChannel;
	for(it_oldChannel = this->channels.begin();it_oldChannel != this->channels.end();++it_oldChannel)
	{
		it_newChannel = newDevice->channels.find(it_oldChannel->first);
		if((it_newChannel == newDevice->channels.end())
			|| (it_oldChannel->second->GetDescription()->GetType() != it_newChannel->second->GetDescription()->GetType()) )
		{
			return false;
		}
		else
		{
			RFParamset *oldParamset =NULL;
			std::string paramsetID = "MASTER";
			for(int i= 0;i<2;++i)
			{
				switch(i)
				{
				case 0:
					paramsetID = "MASTER";
					break;
				case 1:
					paramsetID = "VALUES";
					break;
				default:
					paramsetID = "MASTER";
					break;
					
				}
				oldParamset = it_oldChannel->second->GetDescription()->GetParamset(paramsetID);
				if(oldParamset)
				{
					if(!oldParamset->IsCompatible(it_newChannel->second->GetDescription()->GetParamset(paramsetID)))
					{
						return false;
					}
				}
			}
		}
	}
	if(this->description == newDevice->description)
	{
		//die Kan�le sind gleich und die Ger�te verwenden die selbe beschreibungsdatei -> die Parameter sind gleich  
		return true;
	}
	RFParamset *oldParamset = this->description->GetParamset("MASTER");
	if(oldParamset)
	{
		if(!oldParamset->IsCompatible(newDevice->description->GetParamset("MASTER")))
		{
			return false;
		}
	}
	return true;
}

void RFDevice::SetTemporaryRxMode(const int tempRxMode)
{
	rx_mode_temporary = tempRxMode;
}

bool RFDevice::RxAlways()
{
	return description->RxAlways();
}

bool RFDevice::RxAfterConfig()
{
	return description->RxAfterConfig();
}

bool RFDevice::RxNeedsBurst()
{
	const int rx_modes = description->GetRxMode();
	const int rx_mode_default = description->GetRxModeDefault();
	if(TemporaryRxModeActive()) {
		return (rx_mode_temporary & RFDeviceDescription::RX_BURST)!=0;
	}
	else if( ((rx_modes&0xF) & RFDeviceDescription::RX_BURST_AND_WAKEUP) == RFDeviceDescription::RX_BURST_AND_WAKEUP) {
		return ((rx_mode_default & RFDeviceDescription::RX_BURST)!=0);
	}
	else {
		return description->RxNeedsBurst();
	}

}

bool RFDevice::RxNeedsWakeup()
{
	const int rx_modes = description->GetRxMode();
	const int rx_mode_default = description->GetRxModeDefault();
	if(TemporaryRxModeActive()) {
		return (rx_mode_temporary & RFDeviceDescription::RX_WAKEUP)!=0;
	}
	else if(((rx_modes&0xF) & RFDeviceDescription::RX_BURST_AND_WAKEUP) == RFDeviceDescription::RX_BURST_AND_WAKEUP) {
		return ((rx_mode_default & RFDeviceDescription::RX_WAKEUP)!=0); //use default if not specified what to use in setValue/putParamset
	}
	else {
		return description->RxNeedsWakeup();
	}
}

bool RFDevice::RxSupportLazyConfig() {
	return description->RxSupportLazyConfig();
}
