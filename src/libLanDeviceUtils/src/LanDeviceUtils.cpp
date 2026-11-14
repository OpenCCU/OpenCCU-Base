/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LanDeviceUtils.h"

#include <UtilitySearchDevices.h>
#include <UtilityNetworkConfiguration.h>
#include <UtilityDeviceConfiguration.h>
#include <UtilityFirmwareUpdate.h>
#include <UtilitySendCommand.h>

#ifdef WIN32
	#include <Winsock2.h>
	#pragma comment(lib, "ws2_32.lib")
#endif

using namespace std;
using namespace LDU;

LanDeviceUtils::LanDeviceUtils(void)
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2,2);
	int error = WSAStartup( wVersionRequested, &wsaData);
	if(error != 0) {
		exit(-1);
	}
#endif
}

LanDeviceUtils::~LanDeviceUtils(void)
{
#ifdef WIN32
	WSACleanup();
#endif
}

bool LanDeviceUtils::searchDeviceBySerial(const std::string& serialNr, LanDevice& lanDevice) {
	UtilitySearchDevices usd;
	return usd.searchDeviceBySerial(serialNr, lanDevice);
}

LIBLANDEVICEUTILS_API bool LanDeviceUtils::searchDeviceByTypeAndSerial(const std::string devTypeFilter, const std::string& serialNr, LanDevice& lanDevice)
{
	UtilitySearchDevices usd;
	return usd.searchDeviceByTypeAndSerial(devTypeFilter, serialNr, lanDevice);
}

bool LanDeviceUtils::searchDevicesByType(const std::vector<std::string>& devTypeFilters, std::vector<LanDevice>& devices) {
	UtilitySearchDevices usd;
	const int protocols = PROTOCOL_EQ3CONFIG | PROTOCOL_EQ3LANIFCFG ;
	const int routingSchemes = ROUTINGSCHEME_MULTICAST | ROUTINGSCHEME_BROADCAST;
	return usd.searchDevices(devTypeFilters, protocols, routingSchemes, "", "", devices);
}

bool LanDeviceUtils::searchDevicesByType(const std::vector<std::string>& devTypeFilters, const int protocols, const int routingSchemes, const std::string& firstAddress, const std::string& secondAddress, std::vector<LanDevice>& devices) {
	UtilitySearchDevices usd;
	return usd.searchDevices(devTypeFilters, protocols, routingSchemes, firstAddress, secondAddress, devices);
}


bool LanDeviceUtils::readRuntimeNetworkConfiguration(LanDevice& dev)
{
	return UtilityNetworkConfiguration::loadRuntimeNetworkConfiguration(dev);
}

bool LanDeviceUtils::readNetworkConfiguration(LanDevice& dev) 
{
	return UtilityNetworkConfiguration::loadNetworkConfiguration(dev);
}

bool LanDeviceUtils::readTestStatus(LanDevice& dev)
{
	return UtilityDeviceConfiguration::loadTestStatus(dev);
}

bool LanDeviceUtils::writeTestStatus(LanDevice& dev, const TestStatusConfiguration& testStatusConfiguration)
{
	return UtilityDeviceConfiguration::changeTestStatus(dev, testStatusConfiguration);
}

bool LanDeviceUtils::rebootDevice(LanDevice& dev)
{
	return UtilityDeviceConfiguration::rebootDevice(dev);
}

bool LanDeviceUtils::resetDevice(LanDevice& dev)
{
	return UtilityDeviceConfiguration::resetDevice(dev);
}

bool LanDeviceUtils::sendUserData(LanDevice& dev, unsigned char userData[], const unsigned char AMOUNT)
{
	return UtilityDeviceConfiguration::sendUserDataToDevice(dev, userData, AMOUNT);
}

bool LanDeviceUtils::enterBootloader(LDU::LanDevice &dev)
{
	return UtilityDeviceConfiguration::enterBootloader(dev);
}

bool LanDeviceUtils::enterApplication(LDU::LanDevice &dev)
{
	return UtilityDeviceConfiguration::enterApplication(dev);
}

bool LanDeviceUtils::keyExchange(LanDevice& dev, unsigned char newKey[], const unsigned char AES_KEY_LENGTH)
{
	return UtilityDeviceConfiguration::keyExchange(dev, newKey, AES_KEY_LENGTH);
}

bool LanDeviceUtils::writeNetworkConfiguration(LanDevice& dev, const IPConfiguration& newIPConfiguration) const
{
	return UtilityNetworkConfiguration::changeNetworkConfiguration(dev, newIPConfiguration);
}

bool LanDeviceUtils::doFirmwareUpdate(LanDevice& dev, std::string file)
{
	return UtilityFirmwareUpdate::doFirmwareUpdate(dev, file);
}

bool LanDeviceUtils::getLastResponse(int* length, unsigned char* response)
{
	return UtilitySendCommand::getLastResponse(length, response);
}
