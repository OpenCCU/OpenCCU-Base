/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_LANDEVICEUTILS_H_
#define _LIBLANDEVICEUTILS_LANDEVICEUTILS_H_

#include <vector>
#include <LanDevice.h>
#include <LanDeviceUtilsTypes.h>

#include <DLLImportExport.h>

namespace LDU {

class Protocol;
class LanDevice;
class IPConfiguration;
class RuntimeIPConfiguration;

class LanDeviceUtils
{
public:

	LIBLANDEVICEUTILS_API LanDeviceUtils(void);
	LIBLANDEVICEUTILS_API ~LanDeviceUtils(void);

	 LIBLANDEVICEUTILS_API bool searchDevicesByType(const std::vector<std::string>& devTypeFilters, const int protocols, const int routingSchemes, const std::string& firstAddress, const std::string& secondAddress, std::vector<LanDevice>& devices);

	 LIBLANDEVICEUTILS_API bool searchDevicesByType(const std::vector<std::string>& devTypeFilters, std::vector<LanDevice>& devices);

	 /** \brief Search devices by its serial number.
	* \param serialNr Serial number of the device to search.
	* \param lanDevice Result: Lan device. Only valid if true is returned.
	* \return True, if the device was found, otherwise False.
	*/
	LIBLANDEVICEUTILS_API bool searchDeviceBySerial(const std::string& serialNr, LanDevice& lanDevice);

	LIBLANDEVICEUTILS_API bool searchDeviceByTypeAndSerial(const std::string devTypeFilter, const std::string& serialNr, LanDevice& lanDevice);

	LIBLANDEVICEUTILS_API bool readRuntimeNetworkConfiguration(LanDevice& dev);

	LIBLANDEVICEUTILS_API bool readNetworkConfiguration(LanDevice& dev);//must be renamed

	LIBLANDEVICEUTILS_API bool readTestStatus(LanDevice& dev);

	LIBLANDEVICEUTILS_API bool writeTestStatus(LanDevice& dev, const TestStatusConfiguration& testStatusConfiguration);
	
	LIBLANDEVICEUTILS_API bool rebootDevice(LanDevice& dev);

	LIBLANDEVICEUTILS_API bool resetDevice(LanDevice& dev);

	LIBLANDEVICEUTILS_API bool sendUserData(LanDevice& dev, unsigned char userData[], const unsigned char AMOUNT);

	LIBLANDEVICEUTILS_API bool enterBootloader(LanDevice& dev);

	LIBLANDEVICEUTILS_API bool enterApplication(LanDevice& dev);

	LIBLANDEVICEUTILS_API bool keyExchange(LanDevice& dev, unsigned char newKey[], const unsigned char AES_KEY_LENGTH);

	/** \brief Writes network configuration to device.
	* \param dev LAN device with original network configuration (Won't be changed on failure).
	* \param newIPConfiguration Network configuration to write.
	* \return True if network configuration changed, otherwise False.
	*/
	LIBLANDEVICEUTILS_API  bool writeNetworkConfiguration(LanDevice& dev, const IPConfiguration& newIPConfiguration) const;

	LIBLANDEVICEUTILS_API bool doFirmwareUpdate(LanDevice& dev, std::string file);

	LIBLANDEVICEUTILS_API bool getLastResponse(int* length, unsigned char* response);

};
}//namespace

#endif
