/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFManager.h"
#include "RFController.h"
#include "BidcosFrame.h"
#include "BidcosFrameEnterConfig.h"
#include "BidcosFrameDecoder.h"
#include "RFCentral.h"
#include "RFTeam.h"
#include "RFFirmwareManager.h"
#include <utils.h>
#include <OSCompat.h>

#include <Logger.h>
#include <fstream>
#include <sys/stat.h>
#include <errno.h>
#include <PropertyMap.h>
#include <string.h>

#include "md5.h"

using namespace XmlRpc;

RFManager* RFManager::singleton;

RFManager::RFManager(void) {
	if (!singleton) {
		singleton = this;
	}
	install_mode = INSTALL_OFF;
	aes_key_index_current = 0;
	aes_key_index_previous = 0;
	aes_key_index_temp = 0;
	persist_aes_keys = false;
	dev_cache.dev = NULL;
	task_id = "RFD";
	bidcos_address = 0;
	has_virtual_remote = true;
	destructing = false;
	fireNACKErrorEvents = true;
	callUpdateDeviceOnOTAUDeviceRebuild = false;
	install_mode_expires = 0;
}

RFManager::~RFManager(void) {
	destructing = true;
	t_dev_instances::iterator it;
	for (it = dev_instances.begin(); it != dev_instances.end(); it++) {
		delete it->second;
	}
	dev_instances.clear();
	for (it = team_instances.begin(); it != team_instances.end(); it++) {
		delete it->second;
	}
	team_instances.clear();
}

bool RFManager::GetParamsetValues(const std::string address,
		const std::string& key,int mode, XmlRpc::XmlRpcValue* set) {
//	LOG(Logger::LOG_DEBUG, "GetParamsetValues");
	RFLogicalInstance* inst = GetInstance(address);
	if(mode > 1)
	{
		throw XmlRpcException("wrong mode",-1);
	}
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	if((key.compare("VALUES") == 0))
	{
		return inst->GetParamsetValues(key,mode,set);
	}
	else
	{
		if(mode == 1)
		{
			throw XmlRpcException("UndefinedValues invalid for this Paramset",-1);
		}
		return inst->GetParamsetValues(key,mode, set);
	}
}

bool RFManager::PutParamsetValues(const std::string address,
		const std::string& key, XmlRpc::XmlRpcValue& set, const std::string& rxmode) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}

	if(rxmode.empty()) {
		return inst->PutParamsetValues(key, set);
	}
	else {
		RFDevice* dev = inst->GetDevice();
		if(inst->GetDevice() != NULL) {
			const int iRxModeOff = 0;
			int iRxMode;
			if(rxmode.compare("WAKEUP") == 0) {
				iRxMode = RFDeviceDescription::RX_WAKEUP;
			}
			else if(rxmode.compare("BURST") == 0) {
				iRxMode = RFDeviceDescription::RX_BURST;
			}
			else {
				throw XmlRpcException("Value not supported at parameter 4. Supported values are WAKEUP and BURST", -5);
			}
			dev->SetTemporaryRxMode(iRxMode);
			bool succ = inst->PutParamsetValues(key, set);
			dev->SetTemporaryRxMode(iRxModeOff);
			return succ;
		}
		else {
			throw XmlRpcException("Cannot find device instance for given address parameter");
		}
	}
}

bool RFManager::DetermineParameter(const std::string address,
		const std::string& key, const std::string& parameter) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->DetermineParameter(key, parameter);
}

bool RFManager::ActivateLinkParamset(const std::string address,
		const std::string& peer, bool longpress) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->ActivateLinkParamset(peer, longpress);
}

bool RFManager::GetParamsetDescription(const std::string address,
		const std::string& key, XmlRpc::XmlRpcValue* set) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->GetParamsetDescription(key, set);
}

bool RFManager::GetValue(const std::string address, const std::string& name,int mode,
		XmlRpc::XmlRpcValue* val) {
	if(mode > 1)
	{
		throw XmlRpcException("wrong mode",-1);
	}
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->ReadValue(name,mode, val);
}

bool RFManager::SetValue(const std::string address, const std::string& name,
		XmlRpc::XmlRpcValue& val, const std::string& rxmode) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}

	if(rxmode.empty()) { //default

		return inst->SetValue(name, val);
	}
	else {//extended version for dtag dynamic rx mode feature
		RFDevice* dev = inst->GetDevice();
		if(inst->GetDevice() != NULL) {
			const int iRxModeOff = 0;
			int iRxMode;
			if(rxmode.compare("WAKEUP") == 0) {
				iRxMode = RFDeviceDescription::RX_WAKEUP;
			}
			else if(rxmode.compare("BURST") == 0) {
				iRxMode = RFDeviceDescription::RX_BURST;
			}
			else {
				throw XmlRpcException("Value not supported at parameter 4. Supported values are WAKEUP and BURST", -5);
			}
			dev->SetTemporaryRxMode(iRxMode);
			bool succ = inst->SetValue(name, val);
			dev->SetTemporaryRxMode(iRxModeOff);
			return succ;
		}
		else {
			throw XmlRpcException("Cannot find device instance for given address parameter");
		}
	}
}

bool RFManager::Init(const char* config_filename) {
	if (!HSSManager::Init(config_filename))
		return false;
	config_file.SetCurrentSection("");
	firmwareManager.SetFirmwarePaths(config_file.GetStringValue("Firmware Dir"), config_file.GetStringValue("User Firmware Dir"));
	persist_aes_keys = config_file.GetIntValue("Persist Keys", 0) != 0;
	has_virtual_remote = config_file.GetIntValue("Virtual RC", 1) != 0;
	if (!GetInterfaceConcentrator()->CreateInterfacesFromFile(config_file)) {
		LOG(Logger::LOG_WARNING, "Error initializing interfaces");
		return false;
	}

	config_file.SetCurrentSection("");
	if(config_file.GetStringValue("Fire NACK Error Events", "true").compare("false") == 0) {
		fireNACKErrorEvents = false;
	}

	config_file.SetCurrentSection("");
	if(config_file.GetStringValue("Call UpdateDevice On OTAU Device Rebuild", "false").compare("true") == 0) {
		callUpdateDeviceOnOTAUDeviceRebuild = true;
	}


	config_file.SetCurrentSection("");

	if (!GetBidcosAddress()) {
		if (GetInterfaceConcentrator()->BegInterfacesForAddress(
				&bidcos_address)) {
			LOG(Logger::LOG_INFO, "Got BidCoS-Address donation: 0x%06X",
					bidcos_address);
			PersistBidcosAddress();
		} else {
			LOG(Logger::LOG_ERROR,
					"No BidCoS-Address set and no Interface to beg it from");
			return false;
		}
	}
	GetInterfaceConcentrator()->SetBidcosAddress(GetBidcosAddress());
	if (persist_aes_keys) {
		ReadAESKeys();
		LOG(Logger::LOG_INFO, "Current AES key=%d, previous AES key=%d",
				aes_key_index_current, aes_key_index_previous);

		if (!GetInterfaceConcentrator()->SetAesKeyUser(aes_key_index_current,
				map_aes_keys[aes_key_index_current], aes_key_index_previous,
				map_aes_keys[aes_key_index_previous])) {
			LOG(Logger::LOG_ERROR, "Error setting AES keys");
		}
	}
	if (!GetInterfaceConcentrator()->StartInterfaces()) {
		LOG(Logger::LOG_ERROR, "Error starting interfaces");
	}

	if (!persist_aes_keys) {
		if (GetInterfaceConcentrator()->BegInterfacesForKeys(
				&aes_key_index_current, &aes_key_index_previous)) {
			LOG(Logger::LOG_INFO, "Current AES key=%d, previous AES key=%d",
					aes_key_index_current, aes_key_index_previous);
		} else {
			LOG(Logger::LOG_ERROR, "Error getting AES keys from ARM7");
			return false;
		}
	}
	if (!system_description.ReadFiles(
			OSCompat::FixPath(
					config_file.GetStringValue("Device Description Dir")).c_str())) {
		LOG(Logger::LOG_WARNING, "Error reading descriptions");
		return false;
	}

	LoadDeviceList();

	//create the central device if it doesn't exist
	if (!RFCentral::GetSingleton()) {
		RFDeviceDescription* central_desc = system_description.GetDeviceByType(
				"CENTRAL");
		if (central_desc) {
			RFCentral* dev = new RFCentral();
			dev_instances[dev->GetSerial()] = dev;
			dev->SetAddress(GetBidcosAddress());
			dev->SetDeviceDescription(central_desc);
			dev->CreateChannels();
			dev_address_map[dev->GetAddress()] = dev;
			//load the old peer list if it exists
			dev->LoadPeerList();
		} else {
			LOG(Logger::LOG_WARNING,
					"Device description for central device not found");
		}
	} else {
		RFCentral::GetSingleton()->SetAddress(GetBidcosAddress());
	}
	initReplaceHistory();
	LoadXmlRpcHandlers();
	return true;
}

bool RFManager::ParseAddress(const std::string& address,
		std::string * dev_address, int * channel) {
	std::string::size_type colon = address.find(':');
	if (colon >= address.size() - 1)
		*channel = -1;
	else {
		*channel = atoi(address.substr(colon + 1).c_str());
	}
	*dev_address = address.substr(0, colon).c_str();
//	LOG(Logger::LOG_DEBUG, "RFManager::ParseAddress(%s):%s, %d", address.c_str(), dev_address->c_str(), *channel);
	return true;
}

bool RFManager::ParseAddress(const std::string& address, int * dev_address,
		int * channel) {
	std::string serial;
	if (address.size() && address[0] == '@') {
		std::string::size_type colon = address.find(':');
		if (colon >= address.size() - 1)
			*channel = -1;
		else {
			*channel = atoi(address.substr(colon + 1).c_str());
		}
		*dev_address = strtoul(address.substr(1).c_str(), NULL, 16);
	} else {
		if (!ParseAddress(address, &serial, channel))
			return false;
		t_dev_instances::iterator it = dev_instances.find(serial);
		if (it == dev_instances.end()) {
			it = team_instances.find(serial);
			if (it == team_instances.end()) {
				return false;
			}
		}
		*dev_address = it->second->GetAddress();
	}
	return true;
}

RFDevice* RFManager::GetRFDevice(int address) {
	t_address_map::iterator it = dev_address_map.find(address);

	if (it != dev_address_map.end()) {
		return it->second;
	} else {
		return NULL;
	}
}

RFLogicalInstance* RFManager::GetInstance(const std::string& address) {
	std::string dev_address;
	int channel;
	if (!ParseAddress(address, &dev_address, &channel))
		return NULL;

	if (dev_address == dev_cache.address && dev_cache.dev != NULL)
		return dev_cache.dev->GetInstance(channel);

	t_dev_instances::iterator it = dev_instances.find(dev_address);
	if (it == dev_instances.end()) {
		it = team_instances.find(dev_address);
		if (it == team_instances.end()) {
			return NULL;
		}
	}
	dev_cache.address = dev_address;
	dev_cache.dev = it->second;
	return it->second->GetInstance(channel);
}

RFLogicalInstance* RFManager::GetInstance(int address, int channel) {
	t_address_map::iterator it = dev_address_map.find(address);
	if (it == dev_address_map.end())
		return NULL;
	return it->second->GetInstance(channel);
}

RFLogicalInstance* RFManager::GetTeamInstance(int address, int channel) {
	t_address_map::iterator it = team_address_map.find(address);
	if (it == team_address_map.end())
		return NULL;
	return it->second->GetInstance(channel);
}

bool RFManager::ListDevices(XmlRpc::XmlRpcValue* devs) {
//	LOG(Logger::LOG_DEBUG, "RFManager::ListDevices()");
	devs->assertArray(0);
	t_dev_instances::iterator it;
	for (it = dev_instances.begin(); it != dev_instances.end(); it++) {
		RFDevice* dev_instance = it->second;
		if (!has_virtual_remote
				&& (dev_instance->GetAddress() == (int) GetBidcosAddress()))
			continue;
		dev_instance->Describe(devs);
	}
	for (it = team_instances.begin(); it != team_instances.end(); it++) {
		RFDevice* team_instance = it->second;
		team_instance->Describe(devs);
	}
	return true;
}

bool RFManager::ListTeams(XmlRpc::XmlRpcValue* devs) {
	devs->assertArray(0);
	t_dev_instances::iterator it;
	for (it = team_instances.begin(); it != team_instances.end(); it++) {
		RFDevice* team_instance = it->second;
		team_instance->Describe(devs);
	}
	return true;
}

bool RFManager::GetDeviceDescription(const std::string& address,
		XmlRpc::XmlRpcValue* descr) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->Describe(descr);
}

std::string RFManager::BuildStringAddress(const std::string& address,
		int channel/*=-1*/) {
	char buffer[5];
	if (channel >= 0 && channel <= 255)
		snprintf(buffer, sizeof(buffer), ":%d", channel);
	else
		*buffer = 0;
	return address + buffer;
}

std::string RFManager::BuildStringAddress(int address, int channel/*=-1*/) {
	t_address_map::iterator it = dev_address_map.find(address);
	if (it == dev_address_map.end()) {
		char buffer[8];
		snprintf(buffer, sizeof(buffer), "@%06X", address);
		return BuildStringAddress(buffer, channel);
	}
	return BuildStringAddress(it->second->GetSerial(), channel);
}

void RFManager::ProcessIncomingFrame(BidcosFrame& frame) {
	t_address_map::iterator it = dev_address_map.find(frame.GetSenderAddress());

	if (install_mode && frame.MatchType(BidcosFrame::FT_SYSINFO)
		&& it == dev_address_map.end()) {
		//Sysinfo from an unknown device and install mode active
		std::string serial_number = frame.GetStringValue(
				BidcosFrame::FIELD_SYSINFO_SERIAL);
		if(install_mode == INSTALL_DEVICE_WHITELIST) {//Bosch's whitelist install mode
			if(installWhiteListDeviceSerial.compare(serial_number) != 0) {
				LOG(Logger::LOG_DEBUG, "Tried to install a device with another serial than specified by SetInstallMode"); 
				return;//INSTALL_DEVICE_WHITELIST means, that only the device with given serial can be installed.
			}
		}
		int channel_a = frame.GetIntValue(BidcosFrame::FIELD_SYSINFO_CH_A);
		int channel_b = frame.GetIntValue(BidcosFrame::FIELD_SYSINFO_CH_B);
		bool has_channel = (channel_a || channel_b) != 0;
		std::string type;
		RFDeviceDescription* descr = system_description.GetDeviceBySysinfo(
				frame, &type);
		if (descr) {
			if (has_channel == descr->PeeringSysinfoExpectChannel()) {
				LOG(Logger::LOG_DEBUG, "%s's type is %s",
						serial_number.c_str(), type.c_str());
				RFDevice* dev = descr->CreateDevice();
				if (!dev) {
					LOG(Logger::LOG_ERROR,
							"Could not instantiate device type %s",
							type.c_str());
					return;
				}
				dev_instances[serial_number] = dev;
				dev->SetAddress(frame.GetSenderAddress());
				dev->SetType(type);
				dev->SetSerial(serial_number);
				dev->SetDeviceDescription(descr);
				dev_address_map[frame.GetSenderAddress()] = dev;
				if (dev->ProcessIncomingFrame(frame)) {
					ReportNewDevice(dev);
					t_map_replace_history::iterator it_replace =
							this->replace_history.find(dev->GetSerial());
					if (it_replace != this->replace_history.end()) {
						it_replace->second->DeleteFromReplaceHistory(
								dev->GetSerial());
						it_replace->second->Save();
						this->initReplaceHistory();
					}
					dev->RequestSave();
					//tell the display process that there is a new device
					SendUDPInfo("NEW_DEVICE", "true");
				} else {
					//peering failed, probably due to an aes key mismatch
					LOG(Logger::LOG_WARNING, "Peering with %s failed",
							serial_number.c_str());
					dev->Delete();
					dev_instances.erase(serial_number);
					dev_address_map.erase(frame.GetSenderAddress());
					delete dev;
					//get rid of any service messages received during peering
					ValidateServiceMessages();
				}
			}
		} else {
			LOG(Logger::LOG_DEBUG, "Could not determine %s's type",
					serial_number.c_str());
		}
		SendUDPInfo("RX", "true", 2000);
	} else {
		// Check for a team message. A team message is characterized as follows:
		// - msg.PossiblyTeamTelegram() returns true and
		// - the seemingly sending device is known as group device
		t_address_map::iterator team_it = team_address_map.find(
				frame.GetSenderAddress());

		if (team_it != team_address_map.end() && frame.PossiblyTeamTelegram()) {
			//this message is a group message. The group address is transmitted as sender and
			//the sending device address is transmitted as receiver.
			//change "it" accordingly
			it = dev_address_map.find(frame.GetReceiverAddress());

			LOG(Logger::LOG_DEBUG, "RX for Team %s:%s",
					team_it->second->GetSerial().c_str(), BidcosFrameDecoder::ToString(&frame).c_str());
			//forward the message to the team device
			team_it->second->ProcessIncomingFrame(frame);
		}
		if (it != dev_address_map.end()) {
			LOG(Logger::LOG_DEBUG, "RX for %s:%s",
					it->second->GetSerial().c_str(), BidcosFrameDecoder::ToString(&frame).c_str());
			//forward the message to the device
			if(install_mode && frame.MatchType(BidcosFrame::FT_SYSINFO)) {//CN: Bugfix [BIDCOS-34] (AES Key not updated if device had been factory-reset)
				it->second->SetKeyIndex(0);//this way RFDevice::ProcessIncomingFrame updates the key
			}
			it->second->ProcessIncomingFrame(frame);
			it = dev_address_map.find(frame.GetSenderAddress());//search again for device, could be deleted in Device->ProcessIncomingFrame()
			if(install_mode  && frame.MatchType(BidcosFrame::FT_SYSINFO) && (it != dev_address_map.end())) {
				ReportReAddedDevice(it->second);
			}
			SendUDPInfo("RX", "true", 2000);
		}
	}
	if (!install_mode && frame.MatchType(BidcosFrame::FT_SYSINFO)) {
		std::string serial_number = frame.GetStringValue(
				BidcosFrame::FIELD_SYSINFO_SERIAL);
		std::string type = "unknown type";
		system_description.GetDeviceBySysinfo(frame, &type);
		LOG(Logger::LOG_DEBUG,
				"Sysinfo received by %s while not in install mode:%s (%s)",
				frame.GetInterfaceId().c_str(), serial_number.c_str(), type.c_str());
	}
}

int RFManager::GetInstallMode() {
	if (!install_mode)
		return 0;
	int64_t time_remain = install_mode_expires - time_millis();
	time_remain += 999;
	if (time_remain <= 1000)
		time_remain = 1000;
	//LOG(Logger::LOG_DEBUG, "GetInstallMode() returns %d", time_remain/1000);
	return time_remain / 1000;
}
int RFManager::GetInstallMode(InstallModes *mode) {
	*mode = install_mode;
	return GetInstallMode();
}

void RFManager::SetInstallMode(InstallModes mode, int seconds, const std::string& devSerial) {
	if(mode == INSTALL_DEVICE_WHITELIST) {
		if(devSerial.empty()) {
			throw XmlRpcException("Device serial number must not be empty.", -1);
		}
		else {
			installWhiteListDeviceSerial = devSerial;
			SetInstallMode(mode, seconds);
		}
	}
	else {
		SetInstallMode(mode, seconds);
	}
}

void RFManager::SetInstallMode(InstallModes mode, int seconds) {
	KillTimer(TIMER_INSTALL_MODE);
	if (seconds > INSTALL_MODE_MAX_TIME)
		seconds = INSTALL_MODE_MAX_TIME;
	if (seconds) {
		install_mode = mode;
		install_mode_expires = time_millis() + seconds * 1000;
		RequestTimer(seconds * 1000, TIMER_INSTALL_MODE);
	} else {
		install_mode = INSTALL_OFF;
		installWhiteListDeviceSerial.clear();
		SetTempAESKey("");
	}
	SendUDPInfo("INSTALL_MODE", install_mode ? "true" : "false");
	if (seconds)
		LOG(Logger::LOG_DEBUG, "Entering install mode for %d s", seconds);
	else
		LOG(Logger::LOG_DEBUG, "Leaving install mode.");
}
void RFManager::SetInstallMode(int seconds) {
	KillTimer(TIMER_INSTALL_MODE);
	if (seconds > INSTALL_MODE_MAX_TIME)
		seconds = INSTALL_MODE_MAX_TIME;
	if (seconds) {
		install_mode = INSTALL_NORMAL;
		install_mode_expires = time_millis() + seconds * 1000;
		RequestTimer(seconds * 1000, TIMER_INSTALL_MODE);
	} else {
		install_mode = INSTALL_OFF;
		installWhiteListDeviceSerial.clear();
		SetTempAESKey("");
	}
	SendUDPInfo("INSTALL_MODE", install_mode ? "true" : "false");
	if (seconds)
		LOG(Logger::LOG_DEBUG, "Entering install mode for %d s", seconds);
	else
		LOG(Logger::LOG_DEBUG, "Leaving install mode.");
}

bool RFManager::GetLinks(const std::string& address, int flags,
		XmlRpc::XmlRpcValue* result) {
	RFLogicalInstance::link_map_t link_map;
	if (address.size()) {
		RFLogicalInstance* inst = GetInstance(address);
		if (!inst) {
			throw XmlRpcException("Unknown instance", -2);
		}
		if (!inst->GetLinks(flags | LogicalInstance::GL_FLAG_CHECK_PEER,
				&link_map))
			return false;
	} else {
		t_dev_instances::iterator it;
		for (it = dev_instances.begin(); it != dev_instances.end(); it++) {
			RFDevice* dev_instance = it->second;
			dev_instance->GetLinks(flags, &link_map);
		}
	}
	result->assertArray(link_map.size());
	RFLogicalInstance::link_map_t::iterator it;
	int i = 0;
	for (it = link_map.begin(); it != link_map.end(); it++) {
		(*result)[i++] = it->second;
	}
	return true;
}

bool RFManager::GetLinkPeers(const std::string& address,
		std::vector<std::string>* peers) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->GetLinkPeers(peers);
}

bool RFManager::AddLink(const std::string& sender_address,
		const std::string& receiver_address) {
	MulticallCollectBegin();
	bool retval = true;
	RFLogicalInstance* sender = GetInstance(sender_address);
	if (sender) {
		retval = sender->AddLinkPeer(receiver_address);
	} else {
		retval = false;
	}
	RFLogicalInstance* receiver = GetInstance(receiver_address);
	if (receiver) {
		retval = receiver->AddLinkPeer(sender_address) && retval;
	} else {
		retval = false;
	}
	MulticallCollectEnd();

	if (!sender || !receiver) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return retval;
}
RFDevice * RFManager::IsTeamMaster(RFDevice *dev)
{
    std::string serial = std::string("*") + dev->GetSerial();
    t_dev_instances::iterator it = team_instances.find(serial);
    if(it == team_instances.end())
    {
        return NULL;
    }
    return it->second;
}

bool RFManager::SetTeam(const std::string& channel_address,
		const std::string& team_address) {
	RFChannel* ch = dynamic_cast<RFChannel*>(GetInstance(channel_address));
	if (!ch) {
		throw XmlRpcException("Unknown instance", -2);
	}
	RFTeamChannel* team = NULL;
	if (team_address.size()) {
		team = dynamic_cast<RFTeamChannel*>(GetInstance(team_address));
		if (!team) {
			throw XmlRpcException("Unknown instance", -2);
		}
	}
	return ch->SetTeam(team);
}

bool RFManager::SetLinkInfo(const std::string& sender_address,
		const std::string& receiver_address, const std::string& name,
		const std::string& description) {
	bool retval = true;
	RFLogicalInstance* sender = GetInstance(sender_address);
	if (sender) {
		retval = sender->SetLinkInfo(receiver_address, name, description);
	} else {
		retval = false;
	}
	RFLogicalInstance* receiver = GetInstance(receiver_address);
	if (receiver) {
		retval = receiver->SetLinkInfo(sender_address, name, description)
				&& retval;
	} else {
		retval = false;
	}
	if (!sender || !receiver) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return retval;
}

bool RFManager::GetLinkInfo(const std::string& sender_address,
		const std::string& receiver_address, std::string* name,
		std::string* description) {
	RFLogicalInstance* inst = GetInstance(sender_address);
	if (inst) {
		if (inst->GetLinkInfo(receiver_address, name, description))
			return true;
	}
	inst = GetInstance(receiver_address);
	if (inst) {
		return inst->GetLinkInfo(sender_address, name, description);
	}
	return false;
}

bool RFManager::RemoveLink(const std::string& sender_address,
		const std::string& receiver_address) {
	MulticallCollectBegin();
	bool retval = true;
	RFLogicalInstance* inst = GetInstance(sender_address);
	if (inst) {
		retval = inst->RemoveLinkPeer(receiver_address);
	}
	inst = GetInstance(receiver_address);
	if (inst) {
		retval = inst->RemoveLinkPeer(sender_address) && retval;
	}
	MulticallCollectEnd();

	return retval;
}

bool RFManager::ClearConfigCache(const std::string& address) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	RFDevice* dev = inst->GetDevice();
	if (!dev)
		return false;
	return dev->ClearConfigCache();
}

bool RFManager::RestoreConfigToDevice(const std::string& address) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	RFDevice* dev = inst->GetDevice();
	if (!dev)
		return false;
	return dev->RestoreConfigToDevice();
}

void RFManager::ReportNewDevice(RFDevice* dev) {
	XmlRpcValue params;
	params[1].assertArray(0);
	dev->Describe(&params[1]);
	XmlRpcCallAsync("newDevices", params);
}
void RFManager::ReportReplaceDevcie(RFDevice *newDev) {
	XmlRpcValue params;
	params[1] = newDev->GetReplaceHistory()[newDev->GetReplaceHistory().size()
			- 1];
	params[2] = newDev->GetSerial();
	XmlRpcCallAsync("replaceDevice", params);
}

void RFManager::ReportDeletedDevice(RFDevice* dev) {
	XmlRpcValue params;
	int i = 0;
	params[1][i++] = dev->GetSerial();
	std::vector<int> channel_ids = dev->ListChannels();
	for (std::vector<int>::iterator it = channel_ids.begin();
			it != channel_ids.end(); it++) {
		params[1][i++] = BuildStringAddress(dev->GetSerial(), *it);
	}
	XmlRpcCallAsync("deleteDevices", params);
}

void RFManager::ReportReAddedDevice(RFDevice* dev)
{
	if(dev != NULL) {
		XmlRpcValue params;
		params[1][0] = dev->GetSerial();
		std::vector<int> channel_ids = dev->ListChannels();
		for(unsigned int i = 0; i < channel_ids.size(); i++) {
			params[1][i+1] = BuildStringAddress(dev->GetSerial(), channel_ids.at(i));
		}
		XmlRpcCallAsync("readdedDevice", params);
		LOG(Logger::LOG_INFO, "readdedDevice: %s",dev->GetSerial().c_str());
	}
	else {
		LOG(Logger::LOG_ERROR, "RFManager::ReportReAddedDevice(): Cannot fire event. (dev is null)");
	}
}

bool RFManager::LoadDeviceList() {
//	LOG(Logger::LOG_DEBUG, "LoadDeviceList()");
	std::string device_files_dir = GetDeviceFilesDir();
	OSCompat::MakeDirectory(device_files_dir.c_str());
	OSCompat::DirectoryLister dl(device_files_dir.c_str());
	std::string entry;
	while ((entry = dl.NextEntry()).size()) {
		std::string filename = device_files_dir + OSCompat::PATH_SEPARATOR
				+ entry;
		std::string::size_type dotpos = filename.rfind('.');
		if (dotpos != std::string::npos && filename.substr(dotpos) == ".dev") {
			XMLResults xmlResult;
			XMLNode rootNode = XMLNode::parseFile(filename.c_str(), "RF",
					&xmlResult);
			if (!xmlResult.error) {
				RFDevice* dev_instance = NULL;
				RFDeviceDescription* descr = NULL;

				const char* temp = rootNode.getAttribute("sysinfo");
				if (temp) {
					if (strlen(temp) > 52) {
						temp += 10;
					};
					BidcosFrame sysinfo_frame;
					sysinfo_frame.FromString(temp);
					descr = system_description.GetDeviceBySysinfo(
							sysinfo_frame);
					if (!descr)
						LOG(Logger::LOG_WARNING, "Matching \"sysinfo\" attribute from file %s failed. Trying \"type\" instead.", entry.c_str());
				} else {
					temp = "";
				}
				if (!descr) {
					temp = rootNode.getAttribute("type");
					if (!temp) {
						LOG(Logger::LOG_ERROR,
								"Missing \"type\" attribute in file %s",
								entry.c_str());
						continue;
					}
					descr = system_description.GetDeviceByType(temp);
					if (!descr) {
						LOG(Logger::LOG_ERROR, "Unknown device type %s", temp);
						continue;
					}
				}
				dev_instance = descr->CreateDevice();
				if (!dev_instance) {
					LOG(Logger::LOG_ERROR,
							"Could not instantiate device type %s", temp);
					continue;
				}
				if (!dev_instance->LoadFromXml(rootNode)) {
					LOG(Logger::LOG_ERROR, "Error loading device file %s",
							entry.c_str());
					delete dev_instance;
				} else {
					dev_instances[dev_instance->GetSerial()] = dev_instance;
					dev_address_map[dev_instance->GetAddress()] = dev_instance;
					LOG(Logger::LOG_DEBUG, "Device created: %s",
							dev_instance->GetSerial().c_str());
				}
			}
		}
	}
	return true;
}

void RFManager::OnTimer(uint32_t cookie) {
	switch (cookie) {
	case TIMER_INSTALL_MODE:
		SetInstallMode(INSTALL_OFF, 0);
		break;
	}
}

bool RFManager::DeleteDevice(const std::string& address, int flags) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	RFDevice* dev = dynamic_cast<RFDevice*>(inst);
	if (!dev) {
		throw XmlRpcException("Device expected", -4);
	}
	return DeleteDevice(dev, flags);
}

bool RFManager::DeleteDevice(RFDevice* dev, int flags) {
	if (destructing) {
		//DeleteDevice() is called while destructing RFManager. We better return.
		return true;
	}
	bool success;
	if (flags & DELETE_FLAG_RESET) {
		success = dev->FactoryReset();
	} else {
		success = dev->UnpeerCentral();
	}
	if (!success) {
		if (flags & DELETE_FLAG_DEFER) {
			dev->ScheduleDelete(flags & ~DELETE_FLAG_DEFER);
			return true;
		}
		if (!(flags & DELETE_FLAG_FORCE))
			return false;
	}
	if (!dev->Delete())
		return false;
	MetadataDelete((dev->GetSerial() + "*").c_str());
	ReportDeletedDevice(dev);
	if (dev_instances.find(dev->GetSerial()) != dev_instances.end()) {
		dev_instances.erase(dev->GetSerial());
		dev_address_map.erase(dev->GetAddress());
	}
	if (team_instances.find(dev->GetSerial()) != team_instances.end()) {
		team_instances.erase(dev->GetSerial());
		team_address_map.erase(dev->GetAddress());
	}
	delete dev;
	dev_cache.dev = NULL;
	dev_cache.address = "";
	this->initReplaceHistory();
	ValidateServiceMessages();
	return true;
}

bool RFManager::AbortDeleteDevice(const std::string& address) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	RFDevice* dev = dynamic_cast<RFDevice*>(inst);
	if (!dev) {
		throw XmlRpcException("Device expected", -4);
	}
	return dev->AbortDelete();
}

bool RFManager::GetParamsetId(const std::string address,
		const std::string& type, std::string* id) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->GetParamsetId(type, id);
}

bool RFManager::AddDevice(InstallModes mode, const std::string& serial_number,
		XmlRpc::XmlRpcValue* descr) {
	BidcosFrameEnterConfig frame;
	frame.SetType(BidcosFrame::FT_CONFIG_ENTER);
	frame.SetCtrl(BidcosFrame::CTRL_BCAST | BidcosFrame::CTRL_RPT_ENABLE);
	frame.SetSenderAddress(GetBidcosAddress());
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, 1);
	frame.SetStringValue(BidcosFrame::FIELD_CONFIG_SERIAL, serial_number);
	frame.SetReceiverAddress(0);
	//frame.SetResponseTimeout(1000);

	//try up to 4 times
	int i = 0;
	while (true) {
		if (RFManager::GetSingleton()->GetInterfaceConcentrator()->SendFrame(
				&frame))
			break;
		if (++i >= 4)
			return false;
		//do tries 3 and 4 in burst mode
		if (i >= 2)
			frame.SetCtrl(
					BidcosFrame::CTRL_BCAST | BidcosFrame::CTRL_RPT_ENABLE
							| BidcosFrame::CTRL_BURST);
	}

	BidcosFrame* response = frame.GetResponse();
	if (response) {
		//LOG(Logger::LOG_DEBUG, "Sysinfo: %s", RFCommMessageDecoder::ToString(response).c_str());
		InstallModes install_mode_save = install_mode;
		install_mode = mode;
		ProcessIncomingFrame(*response);
		install_mode = install_mode_save;
		try {
			if (GetDeviceDescription(serial_number, descr))
				return true;
		} catch (XmlRpcException) {
			throw XmlRpcException("Peering failed. Possibly AES key mismatch.",
					-7);
		}
	}
	return false;
}

std::string RFManager::CalculateMD5(const std::string& s) {
	md5 md5_calculator;
	unsigned char* buffer = new unsigned char[s.length()];
	memcpy(buffer, s.c_str(), s.length());
	md5_calculator.Update(buffer, s.length());
	md5_calculator.Finalize();
	delete[] buffer;

	std::string digest;

	digest.append((const char*) md5_calculator.Digest(),
			std::string::size_type(16));
	return digest;
}

bool RFManager::ChangeAESKey(const std::string& passphrase) {
	//check if we may change the key. All devices need to be running on the current key.
	t_dev_instances::iterator it;
	for (it = dev_instances.begin(); it != dev_instances.end(); it++) {
		RFDevice* dev_instance = it->second;
		if (dev_instance->NeedsAESKeyChange()) {
			LOG(Logger::LOG_WARNING,
					"Not changing AES key because %s is still running on an old key",
					dev_instance->GetSerial().c_str());
			return false;
		}
	}

	//first change the key inside the comm coprocessor
	int new_aes_key_index = (aes_key_index_current + 1) & 0x7f;
	if (!new_aes_key_index)
		new_aes_key_index++;

	aes_key_index_previous = aes_key_index_current;
	aes_key_index_current = new_aes_key_index;
	map_aes_keys[aes_key_index_current] = CalculateMD5(passphrase);

	WriteAESKeys();

	if (!GetInterfaceConcentrator()->SetAesKeyUser(aes_key_index_current,
			map_aes_keys[aes_key_index_current], aes_key_index_previous,
			map_aes_keys[aes_key_index_previous]))
		return false;

	//now change the key for every AES-capable device
	for (it = dev_instances.begin(); it != dev_instances.end(); it++) {
		RFDevice* dev_instance = it->second;
		if (dev_instance->GetDeviceDescription()->SupportsAES()) {
			dev_instance->CheckConfigPendingEvent();
			dev_instance->ScheduleConfig(false);
		}
	}
	return true;
}

bool RFManager::SetTempAESKey(const std::string& passphrase) {
	aes_key_index_temp = aes_key_index_previous - 1;
	if (!aes_key_index_temp)
		aes_key_index_temp--;
	aes_key_index_temp &= 0x7f;
	if (!passphrase.size()) {
		aes_key_index_temp = 0;
	}

	std::string md = CalculateMD5(passphrase);

	if (!GetInterfaceConcentrator()->SetAesKeyTemp(aes_key_index_temp, md)) {
		aes_key_index_temp = 0;
		return false;
	}
	return true;
}

bool RFManager::ReadAESKeys() {
	PropertyMap key_db;
	if (key_db.ReadFromFile(
			OSCompat::FixPath(config_file.GetStringValue("Key File"))) <= 0)
		return false;
	aes_key_index_current = key_db.GetIntValue("Current Index");
	aes_key_index_previous = key_db.GetIntValue("Last Index");
	PropertyMap::Section keys_section = key_db.GetSection("");
	for (PropertyMap::Section::iterator it = keys_section.begin();
			it != keys_section.end(); it++) {
		if (it->first.substr(0, 4) == "Key ") {
			int index = strtol(it->first.substr(4).c_str(), NULL, 0);
			std::string key = key_db.GetBinaryValue(it->first);
			if (key.size())
				map_aes_keys[index] = key;
		}
	}
	return true;
}

bool RFManager::WriteAESKeys() {
	if (!persist_aes_keys)
		return true;
	PropertyMap key_db;
	key_db.SetIntValue("Current Index", aes_key_index_current);
	key_db.SetIntValue("Last Index", aes_key_index_previous);
	for (t_map_aes_keys::iterator it = map_aes_keys.begin();
			it != map_aes_keys.end(); it++) {
		char key_name[8];
		snprintf(key_name, sizeof(key_name), "Key %d", it->first);
		key_db.SetBinaryValue(key_name, it->second);
	}
	std::string key_file = OSCompat::FixPath(
			config_file.GetStringValue("Key File"));
	std::string::size_type pos = key_file.rfind(OSCompat::PATH_SEPARATOR);
	if (pos != std::string::npos) {
		OSCompat::MakeDirectory(key_file.substr(0, pos).c_str());
	}

	return key_db.WriteToFile(key_file) > 0;
}

bool RFManager::ReportValueUsage(const std::string& address,
		const std::string& value, int count) {
	RFLogicalInstance* inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->ReportValueUsage(value, count);
}

std::string RFManager::GetKeyMismatchDevice(bool reset) {
	std::string result = key_mismatch_device;
	if (reset)
		key_mismatch_device.clear();
	return result;
}

void RFManager::GetRSSIInfo(XmlRpc::XmlRpcValue* info) {
	t_map_rssi::iterator it;
	for (it = map_rssi.begin(); it != map_rssi.end(); it++) {
		std::string::size_type pos = it->first.find('/');
		if (pos == std::string::npos)
			continue;
		std::string sender = it->first.substr(0, pos);
		std::string receiver = it->first.substr(pos + 1);

		if (sender.empty()) {
			sender = "UNKNOWN";
		}
		if (receiver.empty()) {
			receiver = "UNKNOWN";
		}

		(*info)[receiver][sender][0] = it->second;
		if ((*info)[receiver][sender][1].getType() == XmlRpcValue::TypeInvalid)
			(*info)[receiver][sender][1] = 65536;
		(*info)[sender][receiver][1] = it->second;
		if ((*info)[sender][receiver][0].getType() == XmlRpcValue::TypeInvalid)
			(*info)[sender][receiver][0] = 65536;
	}
}

void RFManager::ReportUpdate(const std::string& address, int hint) {
	XmlRpcValue params;
	(int&) params[2] = hint;
	params[1] = address;
	XmlRpcCallAsync("updateDevice", params);
}

RFDevice* RFManager::CreateTeamInstance(RFDeviceDescription* descr, int address,
		RFDevice* master_candidate) {
	RFDevice* team = NULL;
	t_address_map::iterator it = team_address_map.find(address);
	if (it == team_address_map.end()) {
		team = descr->CreateDevice();
		if (!team) {
			LOG(Logger::LOG_ERROR, "Could not instantiate team 0x%06X",
					address);
			return NULL;
		}
		team->SetAddress(address);
		if (team->GetType().empty())
			team->SetType("Team");
		team->SetDeviceDescription(descr);
		team->CreateChannels();
		team_address_map[address] = team;
		LOG(Logger::LOG_DEBUG, "Team created: 0x%06X", address);
	} else {
		team = it->second;
	}
	if (team->GetSerial().empty()) {
		RFDevice* master_dev = dynamic_cast<RFDevice*>(this->GetInstance(
				address, 0));
		if (!master_dev && master_candidate
				&& master_candidate->GetAddress() == address)
			master_dev = master_candidate;
		if (master_dev) {
			std::string serial = std::string("*") + master_dev->GetSerial();
			team_instances[serial] = team;
			team->SetSerial(serial);
			ReportNewDevice(team);
			LOG(Logger::LOG_DEBUG, "Serial Number %s assigned to team 0x%06X",
					serial.c_str(), address);
		}
	}
	return team;
}

unsigned int RFManager::GetBidcosAddress() {
	if (bidcos_address)
		return bidcos_address;

	config_file.SetCurrentSection("");
	PropertyMap propMap;
	propMap.ReadFromFile(
			OSCompat::FixPath(config_file.GetStringValue("Address File")));
	bidcos_address = propMap.GetIntValue("BidCoS-Address", 0);
	if (bidcos_address)
		srand(bidcos_address);

	return bidcos_address;
}

bool RFManager::PersistBidcosAddress() {
	PropertyMap propMap;
	std::string address_file = OSCompat::FixPath(
			config_file.GetStringValue("Address File"));
	if (propMap.ReadFromFile(address_file) <= 0) {
		//Just in case: create the directory
		std::string::size_type pos = address_file.rfind(
				OSCompat::PATH_SEPARATOR);
		if (pos != std::string::npos) {
			OSCompat::MakeDirectory(address_file.substr(0, pos).c_str());
		}
	}
	propMap.SetIntValue("BidCoS-Address", bidcos_address);
	return propMap.WriteToFile(address_file) != 0;
}

BidcosInterfaceConcentrator* RFManager::GetInterfaceConcentrator() {
	return &interface_concentrator;
}

bool RFManager::ListBidcosInterfaces(XmlRpc::XmlRpcValue* result) {
	std::vector<BidcosInterface*> vec_interfaces;
	BidcosInterface* default_interface;
	if (!GetInterfaceConcentrator()->ListInterfaces(&vec_interfaces,
			&default_interface))
		return false;
	for (unsigned int i = 0; i < vec_interfaces.size(); i++) {
		BidcosInterface* iface = vec_interfaces[i];
		XmlRpcValue& descr = (*result)[i];
		descr["ADDRESS"] = iface->GetSerialNumber();
		descr["DESCRIPTION"] = iface->GetDescription();
		descr["TYPE"] = iface->GetInterfaceType();
		descr["DUTY_CYCLE"] = iface->GetDutyCycle();
		descr["FIRMWARE_VERSION"] = iface->GetFirmwareVersion();
		(bool&) descr["DEFAULT"] = (iface == default_interface);
		(bool&) descr["CONNECTED"] = (iface->IsConnected());
	}
	return true;
}

bool RFManager::SetBidcosInterface(const std::string& device_address,
		const std::string& interface_id, bool roaming) {
	RFLogicalInstance* inst = GetInstance(device_address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	RFDevice* dev = inst->GetDevice();
	return dev->SetBidcosInterface(interface_id, roaming);
}

std::string RFManager::GetDeviceFilesDir() {
	return OSCompat::FixPath(config_file.GetStringValue("Device Files Dir"));
}

void RFManager::UpdateRssiInfo(const std::string& sender,
		const std::string& receiver, int rssi) {
	map_rssi[sender + "/" + receiver] = rssi;
}

void RFManager::UpdateRssiInfo(int sender_address,
		const std::string& receiver_serial, int rssi) {
	std::string sender_serial = BuildStringAddress(sender_address);
	if (sender_serial[0] != '@')
		UpdateRssiInfo(sender_serial, receiver_serial, rssi);
}

bool RFManager::SetInterfaceClock(const unsigned int utcSeconds,
		const int offsetMinutes) {
	std::vector<BidcosInterface*> vec_interfaces;
	BidcosInterface* default_interface;
	bool done = true;
	if (!GetInterfaceConcentrator()->ListInterfaces(&vec_interfaces,
			&default_interface))
		return false;
	for (unsigned int i = 0; i < vec_interfaces.size(); i++) {
		BidcosInterface* iface = vec_interfaces[i];
		if (iface != NULL) {
			done = iface->SetInterfaceClock(utcSeconds, offsetMinutes) & done;
		}
	}
	return done;
}
RFFirmwareManager * RFManager::GetFirmwareManager() {
	return &firmwareManager;
}

bool RFManager::UpdateFirmware(const std::string& address) {
	bool retVal = false;
	RFLogicalInstance *inst = GetInstance(address);
	if (!inst) {
		throw XmlRpcException("Unknown instance", -2);
	}
	try {
		retVal = inst->GetDevice()->UpdateFirmware();
	} catch (XmlRpcException &e) {
		RFDevice *pDev = dynamic_cast<RFDevice *>(inst);
		if (pDev) {
			pDev->SetUpdatePennding(true);
		}
		throw e;
	}
	return retVal;
}

bool RFManager::ReplaceDevice(const std::string& old_address,
		const std::string& new_address) {

	RFDevice *instOld = dynamic_cast<RFDevice*>(GetInstance(old_address));
	RFDevice *instNew = dynamic_cast<RFDevice*>(GetInstance(new_address));
	if (!instOld || !instNew) {
		throw XmlRpcException("Unknown instance", -2);
	}

	int old_rfAddress = instOld->GetAddress();
	RFTeam *old_team = dynamic_cast<RFTeam *>(IsTeamMaster(instOld));
	if(old_team)
	{

	    RFTeam *newTeam = dynamic_cast<RFTeam *>(IsTeamMaster(instNew));
	    if(!newTeam)
	    {
	        newTeam = dynamic_cast<RFTeam*>( CreateTeamInstance(instNew->GetDeviceDescription()->GetTeamDescription(),instNew->GetAddress(),instNew));
	    }
	    if(!newTeam)
	    {
	        LOG(Logger::LOG_ERROR, "RFManager::ReplaceDevice(); Fehler beim erstellen des neuen Team");
	        return false;
	    }
	    newTeam->replaceDevice(old_team);
	}
	if (instNew->replaceDevice(instOld)) {
		LOG(Logger::LOG_DEBUG, "RFManager::ReplaceDevice(); Ger�t %s gegen %s getauscht",
				old_address.c_str(), new_address.c_str());



		if(dev_instances.find(old_address) != dev_instances.end())
		{
		    dev_instances.erase(old_address);
		    dev_address_map.erase(old_rfAddress);
		}
		if(team_instances.find(old_address) != team_instances.end())
		{
		    team_instances.erase(old_address);
		    team_address_map.erase(old_rfAddress);
		}

		instOld->Delete();

		delete instOld;
		dev_cache.dev = NULL;
		dev_cache.address = "";



		instNew->CommitPendingConfig();
		//instNew->ScheduleConfig(false);
		instNew->CheckConfigPendingEvent(true);

		ReportReplaceDevcie(instNew);

	} else {
		LOG(Logger::LOG_ERROR, "RFManager:: Ger�tetausch fehlgeschlagen");
	}

	return true;
	//Verkn�pfungen anpassen

}
bool RFManager::ListReplaceableDevices(std::string addressNewDeviceToReplace, XmlRpc::XmlRpcValue* outDevs,DeviceReplaceLevel_t replaceLevel)
{
	//	LOG(Logger::LOG_DEBUG, "RFManager::ListReplaceableDevices()");
	outDevs->assertArray(0);
	RFDevice *newInst = dynamic_cast<RFDevice*>(RFManager::GetSingleton()->GetInstance(addressNewDeviceToReplace));
	if(newInst == NULL)
	{
		throw XmlRpcException("Unknown instance", -2);
	}
	t_dev_instances::iterator it;
	for (it = dev_instances.begin(); it != dev_instances.end(); it++) {
		RFDevice* dev_instance = it->second;
		if (!has_virtual_remote
				&& (dev_instance->GetAddress() == (int) GetBidcosAddress()))
			continue;
		if(dev_instance == newInst)
		{
			continue;
		}
		if(CheckReplaceCompatibility(dev_instance,newInst,replaceLevel))
		{
			dev_instance->Describe(outDevs);
		}
	}
	return true;
}

bool RFManager::SetRFLGWInfoLED(const unsigned int state)
{
	std::vector<BidcosInterface*> vec_interfaces;
	BidcosInterface* default_interface;
	if (!GetInterfaceConcentrator()->ListInterfaces(&vec_interfaces,
			&default_interface)) {
		return false;
	}
	bool done = false;
	for (unsigned int i = 0; i < vec_interfaces.size(); i++) {
		BidcosInterface* iface = vec_interfaces[i];
		if (iface != NULL) {
			done |= iface->SetRFLGWInfoLED(state);//should return true if one interface supports that
		}
	}
	return done;
}

bool RFManager::CheckReplaceCompatibility(RFDevice *instOld, RFDevice *instNew,
		DeviceReplaceLevel_t replaceLevel)
{
	bool retVal = false;


	if (!instOld || !instNew) {
		throw XmlRpcException("Unknown instance", -2);
	}
	if(instOld->IsReplaceCompatible(instNew))
	{
		switch(replaceLevel)
		{
		case RFManager::ALL_POSSIBLE:
			retVal = true;
		break;
		case RFManager::DEV_TYPE_MATCH:
			if(instNew->GetStoredSysinfo()->GetIntValue(BidcosFrame::FIELD_SYSINFO_TYPE) == instOld->GetStoredSysinfo()->GetIntValue(BidcosFrame::FIELD_SYSINFO_TYPE))
			{
				retVal = true;
			}
			break;
		case RFManager::REPLACE_MAP:
			if((instNew->GetStoredSysinfo()->GetIntValue(BidcosFrame::FIELD_SYSINFO_TYPE) == instOld->GetStoredSysinfo()->GetIntValue(BidcosFrame::FIELD_SYSINFO_TYPE))
			    && (instNew->GetFirmwareVersion() == instOld->GetFirmwareVersion()) )
			{

				retVal = true;
			}
			else
			{
			      retVal = replaceMap.isReplaceble(instOld,instNew);
			}
			break;
		default:
			break;
		}
	}
	return retVal;
}
bool RFManager::AddVirtualDeviceInstance(
		std::vector<unsigned char> rawSysinfo) {
	bool retVal = false;
	BidcosFrame sysInfoFrame;
	for (size_t i = 0; i < rawSysinfo.size(); ++i) {
		sysInfoFrame.SetByteData(i, rawSysinfo[i]);
	}
	t_address_map::iterator it = dev_address_map.find(
			sysInfoFrame.GetSenderAddress());
	if (it == dev_address_map.end()) {
		//Sysinfo from an unknown device and install mode active
		std::string serial_number = sysInfoFrame.GetStringValue(
				BidcosFrame::FIELD_SYSINFO_SERIAL);
		int channel_a = sysInfoFrame.GetIntValue(
				BidcosFrame::FIELD_SYSINFO_CH_A);
		int channel_b = sysInfoFrame.GetIntValue(
				BidcosFrame::FIELD_SYSINFO_CH_B);
		bool has_channel = (channel_a || channel_b) != 0;
		std::string type;
		RFDeviceDescription* descr = system_description.GetDeviceBySysinfo(
				sysInfoFrame, &type);
		if (descr) {
			if (has_channel == descr->PeeringSysinfoExpectChannel()) {
				LOG(Logger::LOG_DEBUG, "%s's type is %s",
						serial_number.c_str(), type.c_str());
				RFDevice* dev = descr->CreateDevice();
				if (!dev) {
					LOG(Logger::LOG_ERROR,
							"Could not instantiate virtual device type %s",
							type.c_str());
					return false;
				}


					dev_instances[serial_number] = dev;
					dev->SetAddress(sysInfoFrame.GetSenderAddress());
					dev->SetType(type);
					dev->SetSerial(serial_number);
					dev->SetDeviceDescription(descr);
					dev_address_map[sysInfoFrame.GetSenderAddress()] = dev;
if(dev->InitVirtualInstance(sysInfoFrame))
				{
					t_map_replace_history::iterator it_replace =
							this->replace_history.find(dev->GetSerial());
					if (it_replace != this->replace_history.end()) {
						it_replace->second->DeleteFromReplaceHistory(
								dev->GetSerial());
						it_replace->second->Save();
						this->initReplaceHistory();
					}
					dev->RequestSave();
					retVal = true;
				}
			}
		}
	}
	return retVal;
}
void RFManager::initReplaceHistory() {
	this->replace_history.clear();
	t_address_map::iterator it_devices;
	for (it_devices = this->dev_address_map.begin();
			it_devices != this->dev_address_map.end(); ++it_devices) {
		std::vector<std::string> &dev_replace_history =
				(std::vector<std::string> &) it_devices->second->GetReplaceHistory();
		std::vector<std::string>::iterator it_history;
		for (it_history = dev_replace_history.begin();
				it_history != dev_replace_history.end(); ++it_history) {
			this->replace_history[*it_history] = it_devices->second;
		}
	}
}
bool RFManager::IsDeviceReplaced(const std::string &oldDevieceAddress,
		std::string &newDeviceAddress) {
	t_map_replace_history::iterator it = this->replace_history.find(
			oldDevieceAddress);
	if (it != this->replace_history.end()) {
		newDeviceAddress = it->second->GetSerial();
	}
	return false;
}

void RFManager::RefreshDeployedDeviceFirmwareList() {
	firmwareManager.RefreshUserFirmwareMap();
}

bool RFManager::FireNACKErrorEventEnabled()
{
	return fireNACKErrorEvents;
}

bool RFManager::CallUpdateDeviceOnOTAUDeviceRebuild()
{
	return callUpdateDeviceOnOTAUDeviceRebuild;
}

