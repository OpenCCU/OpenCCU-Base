#include "HS485Manager.h"
#include "HS485Controller.h"
#include "HS485Central.h"
#include "HS485CommMessageDecoder.h"

#include <Logger.h>
#include <fstream>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <set>
#include <inttypes.h>

using namespace XmlRpc;

HS485Manager* HS485Manager::singleton;

HS485Manager::HS485Manager(void)
: deviceFilesPath("/etc/config/hs485d")//default value of devices files path on CCU 2
{
    if(!singleton)singleton=this;
	task_id="HS485D";
}

HS485Manager::~HS485Manager(void)
{
	t_dev_instances::iterator it;
	for(it=dev_instances.begin();it!=dev_instances.end();it++)
	{
		delete it->second;
	}
}

bool HS485Manager::GetParamsetValues(const std::string address, const std::string& key, XmlRpc::XmlRpcValue* set)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->GetParamsetValues(key, set);
}

bool HS485Manager::IsDeviceReplaced(const std::string &oldDeviceSerial, std::string &newDeviceSerial)
{
	std::map<std::string, HS485Device*>::iterator newDevIter = replacementHistory.find(oldDeviceSerial);
	if(newDevIter != replacementHistory.end()) {
		newDeviceSerial.assign(newDevIter->second->GetSerial());
		return true;
	}
	return false;
}

bool HS485Manager::ReplaceDevice(const std::string& oldDeviceSerial, const std::string& newDeviceSerial)
{
	//Get devices for serials
	HS485Device* oldDev = NULL;
	HS485Device* newDev = NULL;
	if(dev_instances.find(oldDeviceSerial) != dev_instances.end()) {
		oldDev = dev_instances.at(oldDeviceSerial);
	}
	else {
		LOG(Logger::LOG_ERROR, "HS485Manager::ReplaceDevice(): Device with serial %s unknown.",oldDeviceSerial.c_str());
		throw XmlRpcException("First parameter: Unknown instance", -2);
	}
	if(dev_instances.find(newDeviceSerial) != dev_instances.end()) {
		newDev = dev_instances.at(newDeviceSerial);
	}
	else {
		LOG(Logger::LOG_ERROR, "HS485Manager::ReplaceDevice(): Device with serial %s unknown.",newDeviceSerial.c_str());
		throw XmlRpcException("Second parameter: Unknown instance", -2);
	}
	//Perform replacement
	const uint32_t oldDevAddress = oldDev->GetAddress();
	bool done = oldDev->replaceDeviceWith(newDev);
	if(done) {
		dev_instances.erase(oldDeviceSerial);
		dev_instances[newDeviceSerial] = oldDev;//(oldDev has identity of newDev now, after replacement)
		address_map.erase(oldDevAddress);
		address_map[oldDev->GetAddress()] = oldDev;


		oldDev->Save(); //this should write the xml

		std::string oldDevDeviceFile(deviceFilesPath);
		oldDevDeviceFile+="/"+newDeviceSerial+".dev";
		XMLResults results;
		XMLNode rootNode = XMLNode::parseFile( oldDevDeviceFile.c_str(), "RF", &results );

		oldDev->Rebuild(true, false);

		//delete the xml file of replaced dev
		delete newDev;
		std::string filename(deviceFilesPath);
		filename+="/"+oldDeviceSerial+".dev";
		unlink(filename.c_str());
		//Report replacement
		ReportDeviceReplacement(oldDeviceSerial, newDeviceSerial);

		LOG(Logger::LOG_DEBUG, "HS485Manager::ReplaceDevice(): Device replaced.");
	}
	else {
		LOG(Logger::LOG_ERROR, "HS485Manager::ReplaceDevice(): Device replacement failed.");
		return false;
	}


	return true;
}

bool HS485Manager::ListReplaceableDevices(const std::string& devSerial,
		XmlRpc::XmlRpcValue* devs)
{
	if(devSerial.empty()) {
		LOG(Logger::LOG_ERROR, "HS485Manager::ListReplaceableDevices(): Device serial is an empty string");
		throw XmlRpcException("Unknown instance", -2);
	}
	devs->assertArray(0);
	//Get device instance to check compatibility for
	HS485Device* pDevInst = dynamic_cast<HS485Device*>(GetInstance(devSerial));
	if(pDevInst == NULL) {
		LOG(Logger::LOG_ERROR, "Hs485Manager::ListReplaceableDevices(): Device instance with serial %s not found.", devSerial.c_str());
		throw XmlRpcException("Unknown instance", -2);
	}
	for(t_dev_instances::iterator it = dev_instances.begin(); it != dev_instances.end(); ++it) 	{
		HS485Device* pOtherDevInst = dynamic_cast<HS485Device*>(GetInstance(it->second->GetSerial()));
		if(pOtherDevInst != NULL) {
			if(pDevInst->IsCompatible(pOtherDevInst)) {
				pOtherDevInst->Describe(devs);
			}
		}
	}
	return true;

}

void HS485Manager::initReplacementHistory()
{
	replacementHistory.clear();
	std::map<std::string, HS485Device*>::iterator devIterator = dev_instances.begin();
	while(devIterator != dev_instances.end()) {
		HS485Device* pDev = devIterator->second;
		std::vector<std::string> oldDevSerials = pDev->getReplacementHistory();
		for(unsigned int i = 0; i < oldDevSerials.size(); i++) {
			replacementHistory[oldDevSerials.at(i)] = pDev;
		}
		devIterator++;
	}
}

void HS485Manager::ReportDeviceReplacement(const std::string oldDevSerial, const std::string newDevSerial)
{
	XmlRpcValue params;
	params[1] = oldDevSerial;
	params[2] = newDevSerial;
	XmlRpcCallAsync("replaceDevice", params);
}

bool HS485Manager::GetLGWStatus(std::string* serial, bool* connected)
{
	HS485Controller* pController = HS485Controller::GetSingleton();
	if(pController != NULL) {
		return pController->getLGWStatus(serial, connected);
	}
	return false;
}

bool HS485Manager::PutParamsetValues(const std::string address, const std::string& key, XmlRpc::XmlRpcValue& set)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->PutParamsetValues(key, set);
}

bool HS485Manager::GetParamsetDescription(const std::string address, const std::string& key, XmlRpc::XmlRpcValue* set)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->GetParamsetDescription(key, set);
}

bool HS485Manager::GetParamsetId(const std::string address, const std::string& type, std::string* id)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->GetParamsetId(type, id);
}

bool HS485Manager::GetValue(const std::string address, const std::string& name, XmlRpc::XmlRpcValue* val)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->GetValue(name, val);
}

bool HS485Manager::SetValue(const std::string address, const std::string& name, XmlRpc::XmlRpcValue& val)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->SetValue(name, val);
}

bool HS485Manager::Init(const std::string& configfilePath)
{
	if(!HSSManager::Init(configfilePath.c_str())) {
		LOG(Logger::LOG_FATAL_ERROR, "HS485Manager::Init(): HSSManager::Init() failed");
		return false;
	}

	std::string tmpStr;

	//device descriptions
	config_file.SetCurrentSection("");
	tmpStr = config_file.GetStringValue("Device Description Dir", "/firmware/hs485types");
    if(!system_description.ReadFiles(tmpStr.c_str())){
       LOG(Logger::LOG_WARNING, "Error reading descriptions");
	   return false;
    }
    //additional special device descriptions used by special persons
    //don't remove this
    system_description.ReadFiles("/etc/config/hs485types");

    //device files path
    config_file.SetCurrentSection("");
    tmpStr = config_file.GetStringValue("Device Files Dir", "/etc/config/hs485d");
    deviceFilesPath = tmpStr;

	config_file.SetCurrentSection("");
	tmpStr = config_file.GetStringValue("Firmware Dir", "/firmware");
	fw_mgr.SetFirmwarePath(tmpStr);

	LoadDeviceList();
	//create the central device if it doesn't exist
	if(!HS485Central::GetSingleton()){
		HS485DeviceDescription* central_desc=system_description.GetDeviceByType("CENTRAL");
		if(central_desc){
			HS485Central* dev=new HS485Central();
			dev_instances[dev->GetSerial()]=dev;
			dev->SetAddress(HS485Controller::GetSingleton()->GetAddress());
			dev->SetDeviceDescription(central_desc);
			address_map[dev->GetAddress()]=dev;
		}else{
		    LOG(Logger::LOG_WARNING, "Device description for central device not found");
		}
	}
	initReplacementHistory();
	LoadXmlRpcHandlers();
	return true;
}

bool HS485Manager::ParseAddress(const std::string& address, std::string * dev_address, int * channel)
{
	std::string::size_type colon=address.find(':');
	if(colon>=address.size()-1)*channel=-1;
	else{
		char* endp;
		std::string s=address.substr(colon+1);
		*channel=strtol(s.c_str(), &endp, 0);
		if(*endp != 0)return false;
	}
	*dev_address=address.substr(0, colon).c_str();
	return true;
}

bool HS485Manager::ParseAddress(const std::string& address, uint32_t* dev_address, int * channel)
{
	std::string serial;
	if(address.size() && address[0]=='@'){
		std::string::size_type colon=address.find(':');
		if(colon>=address.size()-1)*channel=-1;
		else{
			*channel=atoi(address.substr(colon+1).c_str());
		}
		*dev_address=strtoul(address.substr(1).c_str(), NULL, 16);
	}else{
		if(!ParseAddress(address, &serial, channel))return false;
		t_dev_instances::iterator it=dev_instances.find(serial);
		if(it==dev_instances.end())return false;
		*dev_address=it->second->GetAddress();
	}
	return true;	
}

/*
bool HS485Manager::ParseAddress(const std::string& address, uint32_t* dev_address, int * channel)
{
	std::string::size_type colon=address.find(':');
	if(colon>=address.size()-1)*channel=-1;
	else{
		*channel=atoi(address.substr(colon+1).c_str());
	}
	std::string::size_type slash=address.find('/');
	if(slash==std::string::npos)slash=0;
	else slash++;
	*dev_address=strtoul(address.substr(slash).c_str(), NULL, 16);
	//LOG(Logger::LOG_DEBUG, "HS485Manager::ParseAddress(%s):%08lX, %d", address.c_str(), *dev_address, *channel);
	return true;
}
*/

HS485LogicalInstance* HS485Manager::GetInstance(const std::string& address)
{
	std::string serial;
	int channel;
	if(!ParseAddress(address, &serial, &channel))return NULL;
	t_dev_instances::iterator it=dev_instances.find(serial);
	if(it==dev_instances.end())return NULL;
	return it->second->GetInstance(channel);
}

int HS485Manager::SearchDevices()
{
	unsigned int oldcount=dev_instances.size();
    std::vector<uint32_t> addresses;
	LOG(Logger::LOG_DEBUG, "Doing discovery");
	HS485Controller::GetSingleton()->Discovery(&addresses);
	MulticallCollectBegin();
	for(unsigned int i=0;i<addresses.size();i++){
		if(address_map.find(addresses[i])!=address_map.end())continue;
		AddNewDevice(addresses[i]);
	}
	MulticallCollectEnd();
	LOG(Logger::LOG_DEBUG, "%u devices found", addresses.size());
	return dev_instances.size()-oldcount;
}

bool HS485Manager::AddNewDevice(unsigned int address, HS485Frame* add_frame/*=NULL*/)
{
	//LOG(Logger::LOG_DEBUG, "AddNewDevice(0x%08X, %s)", address, add_frame?HS485CommMessageDecoder::ToString(add_frame).c_str():"NULL");
	std::string desc;
	if(add_frame && add_frame->GetSize()>=15)desc=add_frame->GetStringValue(HS485Frame::FIELD_ADD_DESCRIPTION);
	else desc=HS485Controller::GetSingleton()->GetDeviceDescription(address);

	if(desc.empty()){
		LOG(Logger::LOG_WARNING, "Error getting type of device %08lX", address);
		return false;
	}else{
		std::string type;
		HS485DeviceDescription* dev=system_description.GetDeviceBySysinfo(desc, &type);
		if(dev){
			LOG(Logger::LOG_DEBUG, "New device %08lX is a %s", address, type.c_str());
			std::string serial;
			if(add_frame && add_frame->GetSize()>=25)serial=add_frame->GetStringValue(HS485Frame::FIELD_ADD_SERIAL);
			HS485Device* dev_instance=dev->CreateDevice();
			dev_instance->SetAddress(address);
			dev_instance->SetType(type);
			dev_instance->SetSysinfo(desc);
			if(serial.size()){
				dev_instance->SetSerial(serial);
			}else{
				if(!dev_instance->DetermineSerial(dev->HasSerial())){
					LOG(Logger::LOG_WARNING, "Could not get serial number for %08lX", address);
					delete dev_instance;
					return false;
				}
			}
			dev_instance->SetDeviceDescription(dev);
			dev_instance->SetEnforcedParameters();
			dev_instances[dev_instance->GetSerial()]=dev_instance;
			address_map[address]=dev_instance;
			dev_instance->InitNewDevice();
			dev_instance->RequestSave();
			//tell the display process that there is a new device
			SendUDPInfo("NEW_DEVICE", "true");
			ReportNewDevice(dev_instance);
			return true;
		}else{
			uint32_t type=0;
			for(size_t j=0;j<desc.size();j++){
				type<<=8;
				type|=(unsigned char)desc[j];
			}
			LOG(Logger::LOG_WARNING, "Unknown type of %08lX:%08lX", address, type);
			return false;
		}
	}
}

bool HS485Manager::ListDevices(XmlRpc::XmlRpcValue* devs)
{
	devs->assertArray(0);
	t_dev_instances::iterator it;
	for(it=dev_instances.begin();it!=dev_instances.end();it++)
	{
		HS485Device* dev_instance=it->second;
		dev_instance->Describe(devs);
	}
//	LOG(Logger::LOG_DEBUG, "devs=%s", devs->toText().c_str());
	return true;
}

std::string HS485Manager::BuildStringAddress(const std::string&  address, int channel/*=-1*/)
{
	char buffer[5];
	if(channel>=0 && channel<=255)snprintf(buffer, sizeof(buffer), ":%d", channel);
	else *buffer=0;
	return address+buffer;
}

std::string HS485Manager::BuildStringAddress(uint32_t address, int channel/*=-1*/)
{
	t_address_map::iterator it=address_map.find(address);
	if(it==address_map.end()){
		char buffer[12];
		snprintf(buffer, sizeof(buffer), "@%08" PRIu32, address);
		return BuildStringAddress(buffer, channel);
	}
	return BuildStringAddress(it->second->GetSerial(), channel);
}

void HS485Manager::ProcessIncomingFrame(HS485Frame& frame)
{
	uint32_t receiver=frame.GetReceiverAddress();
	uint32_t sender=frame.GetSenderAddress();
	t_address_map::iterator it=address_map.find(sender);
	if(it!=address_map.end()){
		it->second->ProcessIncomingFrame(frame);
	}else{
        if(receiver==0xffffffff && frame.MatchType(HS485Frame::FT_ADD)){
		    //this seems to be a new device
		    AddNewDevice(sender, &frame);
		}
	}
}

bool HS485Manager::GetLinkPeers(const std::string& address, std::vector<std::string>* peers)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst)return false;
	return inst->GetLinkPeers(peers);
}

bool HS485Manager::GetLinks(const std::string& address, int flags, XmlRpc::XmlRpcValue* result)
{
	HS485LogicalInstance::link_map_t link_map;
	if(address.size()){
		HS485LogicalInstance* inst=GetInstance(address);
		if(!inst)return false;
		if(!inst->GetLinks(flags|LogicalInstance::GL_FLAG_CHECK_PEER, &link_map))return false;
	}else{
		t_dev_instances::iterator it;
		for(it=dev_instances.begin();it!=dev_instances.end();it++)
		{
			HS485Device* dev_instance=it->second;
			if(!dev_instance->GetLinks(flags, &link_map)){
				LOG(Logger::LOG_WARNING, "Error getting links from %s", dev_instance->GetSerial().c_str());
			}
		}
	}
	result->assertArray(link_map.size());
	HS485LogicalInstance::link_map_t::iterator it;
	int i=0;
	for(it=link_map.begin();it!=link_map.end();it++)
	{
		(*result)[i++]=it->second;
	}
	return true;
}

bool HS485Manager::AddLink(const std::string& sender_address, const std::string& receiver_address)
{
	XmlRpcValue params;
	(int&)params[2]=UPDATE_HINT_LINKS;
	bool retval=true;
	HS485LogicalInstance* inst=GetInstance(sender_address);
	if(inst){
		retval = inst->AddLinkPeer(receiver_address);
		params[1]=receiver_address;
		XmlRpcCallAsync("updateDevice", params);
	}else{
		retval=false;
	}
	inst=GetInstance(receiver_address);
	if(inst){
		retval = inst->AddLinkPeer(sender_address) && retval;
		params[1]=sender_address;
		XmlRpcCallAsync("updateDevice", params);
	}else{
		retval=false;
	}
	return retval;
}

bool HS485Manager::SetLinkInfo(const std::string& sender_address, const std::string& receiver_address, const std::string& name, const std::string& description)
{
	bool retval=true;
	HS485LogicalInstance* inst=GetInstance(sender_address);
	if(inst){
		retval = inst->SetLinkInfo(receiver_address, name, description);
	}else{
		retval=false;
	}
	inst=GetInstance(receiver_address);
	if(inst){
		retval = inst->SetLinkInfo(sender_address, name, description) && retval;
	}else{
		retval=false;
	}
	return retval;
}

bool HS485Manager::GetLinkInfo(const std::string& sender_address, const std::string& receiver_address, std::string* name, std::string* description)
{
	HS485LogicalInstance* inst=GetInstance(sender_address);
	if(inst){
		if(inst->GetLinkInfo(receiver_address, name, description))return true;
	}
	inst=GetInstance(receiver_address);
	if(inst){
		return inst->GetLinkInfo(sender_address, name, description);
	}
	return false;
}

bool HS485Manager::RemoveLink(const std::string& sender_address, const std::string& receiver_address)
{
	XmlRpcValue params;
	(int&)params[2]=UPDATE_HINT_LINKS;
	bool retval=true;
	HS485LogicalInstance* inst=GetInstance(sender_address);
	if(inst){
		retval =inst->RemoveLinkPeer(receiver_address);
		params[1]=receiver_address;
		XmlRpcCallAsync("updateDevice", params);
	}else{
		if(sender_address.find('@')!=0)retval=false;
	}
	inst=GetInstance(receiver_address);
	if(inst){
		retval =inst->RemoveLinkPeer(sender_address);
		params[1]=sender_address;
		XmlRpcCallAsync("updateDevice", params);
	}else{
		if(receiver_address.find('@')!=0)retval=false;
	}
	return retval;
}

void HS485Manager::ReportNewDevice(HS485LogicalInstance* dev)
{
	XmlRpcValue params;
	params[1].assertArray(0);
	dev->Describe(&params[1]);
	XmlRpcCallAsync("newDevices", params);
}

void HS485Manager::ReportDeletedDevice(HS485Device* dev)
{
	XmlRpcValue params;
	int i=0;
	int address=dev->GetAddress();
	params[1][i++]=BuildStringAddress(address);
	std::vector<int> channel_ids=dev->ListChannels();
	for(std::vector<int>::iterator it=channel_ids.begin();it!=channel_ids.end();it++)
	{
		params[1][i++]=BuildStringAddress(address, *it);
	}
	XmlRpcCallAsync("deleteDevices", params);
}

bool HS485Manager::GetDeviceDescription(const std::string& address, XmlRpc::XmlRpcValue* descr)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst)return false;
	return inst->Describe(descr);
}

bool HS485Manager::DeleteDevice(const std::string& address, int flags)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	HS485Device* dev=dynamic_cast<HS485Device*>(inst);
	if(!dev){
		throw XmlRpcException("Device expected", -4);
	}

	bool success;
	if(flags&DELETE_FLAG_RESET){
		success=dev->FactoryReset();
	}else{
		success=dev->UnpeerCentral();
	}
	if(!success){
		if(!(flags&DELETE_FLAG_FORCE))return false;
	}
	ReportDeletedDevice(dev);
	dev->Delete();
	dev_instances.erase(dev->GetSerial());
	address_map.erase(dev->GetAddress());
	delete dev;
	ValidateServiceMessages();
	return true;
}

bool HS485Manager::LoadDeviceList()
{
	LOG(Logger::LOG_DEBUG, "LoadDeviceList()");
    DIR *pDirectory;
    bool retval=false;
    struct dirent *pEntry;
    pDirectory = opendir(deviceFilesPath.c_str());
    if(!pDirectory)goto exit;

    pEntry = readdir( pDirectory );
    while(pEntry){
        XMLResults xmlResult;
        std::string filename(deviceFilesPath);
        filename+="/";
        filename+=pEntry->d_name;
		std::string::size_type dotpos=filename.rfind('.');
		if(dotpos!=std::string::npos && filename.substr(dotpos)==".dev"){
	        XMLNode rootNode = XMLNode::parseFile( filename.c_str(), "RF", &xmlResult );
		    if(!xmlResult.error){
				HS485DeviceDescription* descr=NULL;
				const char* temp=rootNode.getAttribute("sysinfo");
				if(temp){
					std::string s=temp;
					std::string sysinfo;
					for(unsigned int i=0;(i*2+1)<s.size();i++)sysinfo.append(1, (char)strtol(s.substr(2*i, 2).c_str(), NULL, 16));
					descr=system_description.GetDeviceBySysinfo(sysinfo);
					if(!descr)
						LOG(Logger::LOG_WARNING, "Matching \"sysinfo\" attribute from file %s failed. Trying \"type\" instead.", pEntry->d_name);
				}
				if(!descr){
					temp=rootNode.getAttribute("type");
					if(!temp){
						LOG(Logger::LOG_ERROR, "Missing \"type\" attribute in file %s", pEntry->d_name);
						goto next_entry;
					}
					descr=system_description.GetDeviceByType(temp);
					if(!descr){
						LOG(Logger::LOG_ERROR, "Unknown device type %s", temp);
						goto next_entry;
					}
				}

				HS485Device* dev_instance=descr->CreateDevice();
				if(!dev_instance){
					LOG(Logger::LOG_ERROR, "Could not instantiate device type %s", temp);
					goto next_entry;
				}
				if(!dev_instance->LoadFromXml(rootNode)){
					LOG(Logger::LOG_ERROR, "Error loading device file %s", pEntry->d_name);
					delete dev_instance;
				}else{
					dev_instances[dev_instance->GetSerial()]=dev_instance;
					address_map[dev_instance->GetAddress()]=dev_instance;
					LOG(Logger::LOG_DEBUG, "Device created: %s %s", dev_instance->GetType().c_str(), dev_instance->GetSerial().c_str());
				}
		    }
		}
next_entry:
		pEntry = readdir( pDirectory );
    }
    retval=true;
exit:
    if(pDirectory)closedir(pDirectory);
    return retval;
}

bool HS485Manager::ReportValueUsage(const std::string& address, const std::string& value, int count)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->ReportValueUsage(value, count);
}

HS485Device* HS485Manager::GetDeviceByAddress(uint32_t address)
{
	t_address_map::iterator it=address_map.find(address);
	if(it!=address_map.end()){
		return it->second;
	}else{
		return NULL;
	}
}

bool HS485Manager::ClearConfigCache(const std::string& address)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	HS485Device* dev=inst->GetDevice();
	if(!dev)return false;
	dev->ClearConfigCache();
	return true;
}

bool HS485Manager::ActivateLinkParamset(const std::string address, const std::string& peer, bool longpress)
{
	HS485LogicalInstance* inst=GetInstance(peer);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->ActivateLinkParamset(address, longpress);
}

bool HS485Manager::UpdateFirmware(const std::string& address)
{
	HS485LogicalInstance* inst=GetInstance(address);
	if(!inst){
		throw XmlRpcException("Unknown instance", -2);
	}
	return inst->GetDevice()->UpdateFirmware();
}

const std::string& HS485Manager::GetDeviceFilesPath() const
{
	return deviceFilesPath;
}
