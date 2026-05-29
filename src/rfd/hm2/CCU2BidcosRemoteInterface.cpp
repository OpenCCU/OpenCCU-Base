/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CCU2BidcosRemoteInterface.h"
#include <CCU2LGWCommController.h>
#include <CCU2SerialPortCommController.h>
#include <OSCompat.h>
#include <BidcosInterfaceConcentrator.h>
#include <Logger.h>
#include <HM2Utils.h>
#include <FileIO.h>
#include <TimeZoneInfo.h>
#include <RFManager.h>


using namespace HM2;

CCU2BidcosRemoteInterface::CCU2BidcosRemoteInterface(const InterfaceSubType& ifSubtype)
: BidcosInterface()
, pCommController(NULL)
, dataRate(DATA_RATE_10k)
{
	aesKeys.currentKeyIndex = 0;
	aesKeys.previousKeyIndex = 0;
	switch(ifSubtype) {
		case HMLGW2:
			pCommController = new CCU2LGWCommController(this);
			break;
		case CCU2:
		case HM_MOD_UART:
			pCommController = new CCU2SerialPortCommController(this);
			break;
		default:
			LOG(Logger::LOG_FATAL_ERROR, "CCU2BidcosRemoteInterface: Undefined interface subtype. Using default, which probably won't work!");
			pCommController = new CCU2SerialPortCommController(this);
			break;

	}
}

CCU2BidcosRemoteInterface::~CCU2BidcosRemoteInterface()
{
	if(pCommController != NULL) {
		delete pCommController;
		pCommController = NULL;
	}
}


bool CCU2BidcosRemoteInterface::SetAesKeyTemp(int index, const std::string& data) 
{
	if(data.empty()) {
		return true;
	}
	std::string reqData(data);
	reqData.append(1, (char)index);
	//std::string response;
	bool done = pCommController->sendBidcosRequest( BIDCOSCMD_SET_TEMP_AES_KEY, reqData/*, &response*/);
	//TODO Remove following debug logging
	
	if(!done) {
		LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::SetAesKeyTemp(): Error setting temporary key.");
	}
	return done;//TODO To be tested
}

bool CCU2BidcosRemoteInterface::SetAesKeyUser(int index, const std::string& data, int last_index, const std::string& last_data) 
{
	//Remember keys for republishing
	aesKeys.currentKeyIndex = index;
	aesKeys.previousKeyIndex = last_index;
	aesKeys.currentKey = data;
	aesKeys.previousKey = last_data;

	LOG(Logger::LOG_ALL, "CCU2BidcosRemoteInterface::SetAesKeyUser(): Trying to change user keys...");
	//Prepare data
	std::string curKeyData(data);
	curKeyData.append(1, (char)index );

	std::string prevKeyData(last_data);
	prevKeyData.append(1, (char)last_index);
	//LOG(Logger::LOG_ALL, "CCU2BidcosRemoteInterface::SetAesKeyUser(): CurrentKey : %d %s", index, toDebugHexStr(curKeyData).c_str());
	//LOG(Logger::LOG_ALL, "CCU2BidcosRemoteInterface::SetAesKeyUser(): PreviousKey: %d %s", last_index, toDebugHexStr(prevKeyData).c_str());
	//Set remotely
	bool done = pCommController->sendBidcosRequest( BIDCOSCMD_SET_CURRENT_AES_KEY, curKeyData); 
	if((!done) && (index != 0)) { //We can't set key with index 0. Coprocessor denys it.
		LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::SetAesKeyUser(): Error setting current key with index %d", index);
		return false; 
	}
	if(!last_data.empty()) {//This can be when we exchange keys the first time, cuz we don't have the default key
		done = pCommController->sendBidcosRequest( BIDCOSCMD_SET_PREVIOUS_KEY, prevKeyData); 
		if(!done) {
			LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::SetAesKeyUser(): Error setting previous key with index %d", last_index);
			return false; 
		}
	}
	else {
		LOG(Logger::LOG_ALL, "CCU2BidcosRemoteInterface::SetAesKeyUser(): Previous key empty. Skip setting previous key.");	//Set internally 
	}
	
	return true;
}

bool CCU2BidcosRemoteInterface::StartInterface(int bidcos_address) 
{
	LOG(Logger::LOG_DEBUG, "CCU2BidcosRemoteInterface::StartInterface(): addr=%s", toDebugHexStr( convertBidcosAddressToBigEndianString( bidcos_address ) ).c_str() );
	//Start reveive thread
	pCommController->startReceiver();
	//Remote: Set bidcos address at coprocessor
	std::string addrStr = convertBidcosAddressToBigEndianString(bidcos_address);
	bool done = pCommController->sendBidcosRequest( BIDCOSCMD_SET_RF_ADDRESS, addrStr );
	return done;
}
  
bool CCU2BidcosRemoteInterface::StopInterface() 
{
	int addr = (GetConcentrator() != NULL) ? GetConcentrator()->GetBidcosAddress() : -1;
	LOG(Logger::LOG_DEBUG, "CCU2BidcosRemoteInterface::StopInterface(): addr=%s", toDebugHexStr( convertBidcosAddressToBigEndianString( addr ) ).c_str() );
	//TODO Test me and verify that nothing else needed
	return pCommController->stopReceiver();
}

bool CCU2BidcosRemoteInterface::SendFrame(BidcosFrame* frame) 
{
	return pCommController->sendBidcosTelegram( frame );
}

bool CCU2BidcosRemoteInterface::SendFrameTripleBurst(BidcosFrame* frame) 
{
	return pCommController->sendBidcosTelegramTripleBurst(frame);
}

bool CCU2BidcosRemoteInterface::AddDevice(int address) {
	LOG(Logger::LOG_ALL, "CCU2BidcosRemoteInterface::AddDevice()");
	bool done = BidcosInterface::AddDevice(address); 
	if(done) {
		done = AddDeviceRemote(address);
		if(!done)
		{
			BidcosInterface::RemoveDevice(address);
		}
	}
	return done;
}

bool CCU2BidcosRemoteInterface::AddDeviceRemote(int address, std::string* authChannels) {
	int aes_key;
    uint64 aes_channels;
    if(!GetDeviceAesPolicy(address, &aes_key, &aes_channels))return false;
	//Convert address and send message to coprocessor.
	std::string addrStr = convertBidcosAddressToBigEndianString(address);
	addrStr.append(1, (char) (aes_key & 0xFF)); //key index 0 //FIXME replace with whatever.. maybe ask before or something
	addrStr.append(1, (char) (BidcosInterface::NeedsWakeup(address) ? 0x01 : 0x00));
	if(BidcosInterface::NeedsLazyConfig(address))
	{
		addrStr.append(1, (char)0x01);
	}
	else
	{
		addrStr.append(1, (char)0x00);
	}

	std::string responseData;//number of link peers (2 bytes), channel authentification (uint64)
	bool done = pCommController->sendBidcosRequest( BIDCOSCMD_ADD_LINK_PEER, addrStr, &responseData);
	LOG(Logger::LOG_ALL, "BidcosRemoteInterface::AddDevice(): Added device remotely: %s.", toDebugHexStr(addrStr).c_str() );
	if(done) {
		if( (authChannels != NULL) && (responseData.size() == 10) ) {//number of link peers (2 byte) + auth.-channels (8 byte)
			authChannels->clear();
			authChannels->append( responseData.substr(2) );
		}
	}
	else {
		LOG(Logger::LOG_WARNING, "BidcosRemoteInterface::AddDevice(): Adding device remotely failed.");
		return false;
	}
	return done;
}
  
bool CCU2BidcosRemoteInterface::RemoveDevice(int address) 
{
	std::string response;
	std::string addrStr = convertBidcosAddressToBigEndianString( address );
	bool done = pCommController->sendBidcosRequest( BIDCOSCMD_REMOVE_LINK_PEER, addrStr, &response);
	LOG(Logger::LOG_ALL, "BidcosRemoteInterface::RemoveDevice(): Removed device remotely: %s.", toDebugHexStr(addrStr).c_str() );
	if(done) {
		done = BidcosInterface::RemoveDevice(address);//Remove address from map.
	}
	else {
		LOG(Logger::LOG_ERROR, "BidcosRemoteInterface::RemoveDevice(): Can't remove device remotely: %s.", toDebugHexStr(addrStr).c_str() );
	}
	return done;
}
  
bool CCU2BidcosRemoteInterface::SetDeviceAesPolicy(int address, int aes_key, uint64 aes_channels) 
{
	//set aes channels internally (important to do this first, because AddDeviceRemote calls that)
	bool done = BidcosInterface::SetDeviceAesPolicy(address, aes_key, aes_channels);
	//Get current aes_channels values
	std::string devChannels;
	AddDeviceRemote(address, &devChannels);
	uint64 devAuthChannels = toUInt64(devChannels);
	LOG(Logger::LOG_ALL, "CCU2BidcosRemoteInterface::SetDeviceAesPolicy(): dev aes_channels: %ld", devAuthChannels);
	//Invert bidcos aes_channels value (bidcos 0 is off; device: 1 is off)
	const uint64 bidcosAuthChannels = ~ aes_channels ;
	done = false;
	//Compare with aes_channels value from device
	if(devAuthChannels != bidcosAuthChannels) {
		//not equal -> update device auth channels
		std::vector<int> activeChannels, inactiveChannels;
		setupAuthChannelNrDistribution(bidcosAuthChannels, devAuthChannels, activeChannels, inactiveChannels);		
		//Update inactive channels
		if(inactiveChannels.size() > 0) {
			std::string params = convertBidcosAddressToBigEndianString( address );
	 		for(unsigned int i = 0; i < inactiveChannels.size() ; i++) {
				params.append(1, (char)inactiveChannels.at(i));
				LOG(Logger::LOG_ALL, "Deactivating channel authentication for channel nr %d (address: %s)", inactiveChannels.at(i), toDebugHexStr(convertBidcosAddressToBigEndianString( address )).c_str());
				
			}
			done = pCommController->sendBidcosRequest(BIDCOSCMD_AUTHENTICATION_OFF, params); 
			if(!done) {
				LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::SetDeviceAesPolicy(): Error disabling channel authentication.");
				return false;
			}
		}
		//Update active channels
		if( activeChannels.size() > 0 ) {
			std::string params = convertBidcosAddressToBigEndianString( address );
 			for(unsigned int i = 0; i < activeChannels.size() ; i++) {
				params.append(1, (char)activeChannels.at(i));
				LOG(Logger::LOG_ALL, "Activating channel authentication for channel nr %d (address: %s)", activeChannels.at(i), toDebugHexStr(convertBidcosAddressToBigEndianString( address )).c_str());
			}
			done = pCommController->sendBidcosRequest(BIDCOSCMD_AUTHENTICATION_ON, params); 
			if(!done) {
				LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::SetDeviceAesPolicy(): Error enabling channel authentication.");
				return false;
			}
		}
	}
	return done;
}

bool HM2::CCU2BidcosRemoteInterface::SetRFLGWInfoLED(const unsigned int state)
{
	return pCommController->setRFLGWInfoLED(state);
}

void CCU2BidcosRemoteInterface::setupAuthChannelNrDistribution(const uint64& bidcosAuthChannels, const uint64& deviceAuthChannels, std::vector<int>& activeChannels, std::vector<int>& inactiveChannels) 
{
	activeChannels.clear();
	inactiveChannels.clear();
	for(uint64 b = 1; b < 64; b++) {
		//check if update is necessary
		const uint64 bitmask = (uint64)1 << b;
		const uint64 dev = deviceAuthChannels & bitmask;
		const uint64 bidcos = bidcosAuthChannels & bitmask;
		if(dev != bidcos) {
			//update is necessary -> put into activate or inactive list
			if(bidcos != 0) {//inactive (!= 0 because we negated bidcos auth channels so that 0 means active and 1 means deaktivated)
				inactiveChannels.push_back((int)b);
			}
			else {//active
				activeChannels.push_back((int)b);
			}
		}
	}
	//if aes is active for any channel, it must be active for channel 0 too
	if(!activeChannels.empty()) {
		activeChannels.insert(activeChannels.begin(), 0);
	}
	else {
		inactiveChannels.insert(inactiveChannels.begin(),0);
	}
}
bool CCU2BidcosRemoteInterface::AddDeviceWakeupRequest(int address, bool lazyConfig)
{
	bool done = BidcosInterface::AddDeviceWakeupRequest(address,lazyConfig);
	if(done) 
	{
		done = AddDeviceRemote( address );	
		if(!done) {
			LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::AddDeviceWakeupRequest(): Failed remotely");
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::AddDeviceWakeupRequest(): Failed");
	}
	return done;
}
  
bool CCU2BidcosRemoteInterface::RemoveDeviceWakeupRequest(int address) 
{
	bool done = BidcosInterface::RemoveDeviceWakeupRequest(address);
	if(done) 
	{
		done = AddDeviceRemote( address );
		if(!done) {
			LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::RemoveDeviceWakeupRequest(): Failed remotely");
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::RemoveDeviceWakeupRequest(): Failed");
	}
	return done;
}

bool CCU2BidcosRemoteInterface::Init(std::map<std::string, std::string>& params) 
{
	bool retVal = false;
	PropertyMap map = RFManager::GetSingleton()->GetConfigPropertyMap();
	const std::string curSec = map.GetCurrentSection();
	map.SetCurrentSection("");
	const std::string improvedCoproInit = map.GetStringValue("Improved Coprocessor Initialization");
	map.SetCurrentSection(curSec);
	bool improvedCoproInitEnabled = improvedCoproInit.compare("true") == 0;
	const std::string type = params["Type"];
	if(!type.empty()) {
		if( (type.compare("CCU2") == 0) || (type.compare("HM-MOD-UART") == 0)) {
			retVal = InitCCU2SerialPortCommController(params, improvedCoproInitEnabled);
		}
		else if(type.compare("HMLGW2") == 0) {
			retVal = InitHMLGWPortCommController(params, true);
		}
		else {
			LOG(Logger::LOG_FATAL_ERROR, "CCU2BidcosRemoteInterface::Init():Unsupported interface type: %s", type.c_str());
			return false;
		}
		if(retVal) {
			pCommController->setInterfaceSerial( params["Serial Number"] );
		}
	}
	else {
		LOG(Logger::LOG_FATAL_ERROR, "CCU2BidcosRemoteInterface::Init(): Could not get interface type.");
		return false;
	}
	if(retVal) {
		//Set interface clock
		SetInterfaceClockBySystemTime();
	}
	return retVal;
}

void CCU2BidcosRemoteInterface::SetInterfaceClockBySystemTime() 
{
	TimeZoneInfo tzi(time(NULL));
	const int utcOffsetMinutes = ((int)tzi.GetUTCOffset()/(int)60);
	SetInterfaceClock((unsigned int)time(NULL), utcOffsetMinutes);
}

bool CCU2BidcosRemoteInterface::InitCCU2SerialPortCommController(std::map<std::string, std::string>& params, bool improvedCoproInit)
{
	//Get configuration
	std::string dev = params["ComPortFile"];
	if(dev.empty()) {
		LOG(Logger::LOG_ERROR, "Init failed. Parameter 'ComPortFile' not set in bidcos configuration file.");
		return false;
	}
	std::string access = params["AccessFile"];
	if(!improvedCoproInit && access.empty()) {
		LOG(Logger::LOG_ERROR, "Init failed. Parameter 'AccessFile' not set in bidcos configuration file.");
		return false;
	}
	std::string reset = params["ResetFile"];
	if(!improvedCoproInit && reset.empty()) {
		LOG(Logger::LOG_ERROR, "Init failed. Parameter 'ResetFile' not set in bidcos configuration file.");
		return false;
	}



	//Enable/Disable CSMA/CA
	std::string csmacaEnabledStr = params["CSMACA-Enabled"];
	bool csmacaEnabled = false;
	if(csmacaEnabledStr.compare("true") == 0) {
		csmacaEnabled = true;
	}
	CCU2SerialPortCommController* pSerialCommController = dynamic_cast<CCU2SerialPortCommController*>(pCommController);
	if(pSerialCommController == NULL) {
		LOG(Logger::LOG_FATAL_ERROR, "CCU2BidcosRemoteInterface::Init(): CommController instance is null");
		return false;
	}
	bool done = pSerialCommController->init( dev, OSCompat::FixPath(access), OSCompat::FixPath(reset), csmacaEnabled, improvedCoproInit);
	if(!done) {
		return false;
	}
	if(params["Serial Number"].empty()) {//written into map when the rfd config file had been read
		//serial number was not set/overridden in rfd config file, so we try to read it from config file
		std::string coprocessorSerialNr = pCommController->readSerialNumber();
		params["Serial Number"] = coprocessorSerialNr;
		done = BidcosInterface::Init(params);
		if(!done) {
			LOG(Logger::LOG_ERROR, "Init failed. Serial number could not be determined and is not set/'overridden' in rfd config file.");
			return false;
		}
	}
	else {
		done = BidcosInterface::Init(params);
		if(!done) {
			LOG(Logger::LOG_ERROR,"Init failed. BidCosInterface::Init() returned false.");
			return false;
		}
	}
	return true;
}

bool CCU2BidcosRemoteInterface::InitHMLGWPortCommController(std::map<std::string, std::string>& params, bool improvedCoproInit)
{
	bool done = false;
	//Get information needed for initialization of CCU2LGWCommController
	const std::string serialNr = params["Serial Number"];
	const std::string encKey = params["Encryption Key"];
	const std::string host = params["IP Address"];//optional, skips/overrides search via serial
	const std::string csmaca = params["CSMACA-Enabled"];
	bool csmacaEnabled = false;
	if(csmaca.compare("true") == 0) {
		csmacaEnabled = true;
	}

	//Initialize CCU2LGWCommController
	done = ((CCU2LGWCommController*)pCommController)->init(host, 2000, encKey, serialNr, csmacaEnabled, improvedCoproInit);
	if(!done) {
		return false;
	}
	return BidcosInterface::Init(params);
}

bool CCU2BidcosRemoteInterface::IsConnected() 
{
	return pCommController->isDeviceOpen();
}

bool CCU2BidcosRemoteInterface::DonateAddress(unsigned int* native_address, unsigned int* given_address) 
{
	std::string noParams, response;
	//native address from device
	bool done = pCommController->sendBidcosRequest( BIDCOSCMD_GET_DEFAULT_RF_ADDRESS, noParams, &response);
	if(done) {
		if(native_address != NULL) {
			*native_address = convertBidEndianStringToBidcosAddress( response );
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::DonateAddress(): Error retrieving default address.");
	}
	response.clear();
	//given address from device, maybe set one time...
	bool done2 = pCommController->sendBidcosRequest( BIDCOSCMD_GET_RF_ADDRESS, noParams, &response);
	if(done2) {
		if(given_address != NULL) {
			*given_address = convertBidEndianStringToBidcosAddress( response );
		}
	}
	else {
		*given_address = 0;
		LOG(Logger::LOG_DEBUG, "CCU2BidcosRemoteInterface::DonateAddress(): Could not retrieve rf address. Maybe it wasn't set.");
	}
	return (done || done2);
}

void CCU2BidcosRemoteInterface::handleEvent(BidcosFrame bidcosFrame)
{
	BidcosInterfaceConcentrator* pBIC = GetConcentrator();
	if(pBIC != NULL) {
		pBIC->ProcessReceivedFrame( bidcosFrame );
	}
	else {
		LOG(Logger::LOG_FATAL_ERROR, "CCU2BidcosRemoteInterface::handleEvent(): Unable to handle event because BidcosInterfaceConcetrator is NULL.");
	}
}

void CCU2BidcosRemoteInterface::handleDutyCycleEvent(const double dutyCycleValue) 
{
	const int intValue = (int) (dutyCycleValue + (double)0.5);
	SetDutyCycle(intValue);
}

void CCU2BidcosRemoteInterface::publishFirmwareVersion(const std::string& firmwareVersion)
{
	SetFirmwareVersion(firmwareVersion);
}

bool CCU2BidcosRemoteInterface::SetInterfaceClock(const unsigned int utcSeconds, const int offsetMinutes)
{
	return pCommController->setInterfaceClock(utcSeconds, offsetMinutes);
}
bool CCU2BidcosRemoteInterface::republishAllDevices()
{
	//Set user keys (workaround for Copro 1.0.11 key problem after (re-)starting app)
	SetAesKeyUser(aesKeys.currentKeyIndex, aesKeys.currentKey, aesKeys.previousKeyIndex, aesKeys.previousKey);

	//Republish devices
	std::vector<int> devices;
	ListDevices(&devices);
	bool retVal = true;
	for(unsigned int i = 0; i < devices.size(); i++) {
		bool done = AddDeviceRemote(devices.at(i));
		if(done) {
			LOG(Logger::LOG_DEBUG, "CCU2BidcosRemoteInterface::republishAllDevices(): Re-added device with address %d(dec)",devices.at(i));
		}
		else {
			LOG(Logger::LOG_ERROR, "CCU2BidcosRemoteInterface::republishAllDevices(): Could not republish device with address %d(dec) to coprocessor.",devices.at(i));
			retVal = false;
		}
	}
	return retVal;
}
bool CCU2BidcosRemoteInterface::Set100kMode()
{
	if(pCommController->setDataRate100k())
	{
		dataRate = DATA_RATE_100k;
		return true;
	}
	return false;
}
bool CCU2BidcosRemoteInterface::Set10kMode()
{
	if(pCommController->setDataRate10k())
	{
		dataRate = DATA_RATE_10k;
		return true;
	}
	return false;
}
BidcosInterface::DataRate_t CCU2BidcosRemoteInterface::getDataRate()
{
	return this->dataRate;
}
bool CCU2BidcosRemoteInterface::Updateable()
{
	return true;
}
bool CCU2BidcosRemoteInterface::SupportLazyConfig()
{
    return true;
}
int CCU2BidcosRemoteInterface::GetDutyCycle() const
{
	int dutyCycle = -1;
	pCommController->getDutyCycle(&dutyCycle);
	return dutyCycle;
}

bool CCU2BidcosRemoteInterface::SupportsTripleBurst() {
	return true;
}
