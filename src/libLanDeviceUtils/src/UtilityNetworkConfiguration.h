/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_UTILITYNETWORKCONFIGURATION_H_
#define _LIBLANDEVICEUTILS_UTILITYNETWORKCONFIGURATION_H_

#include <string>
#include "InternalUtilities.h"

namespace LDU {

class LanDevice;
class IPConfiguration;

class UtilityNetworkConfiguration
{
public:
	UtilityNetworkConfiguration();
	virtual ~UtilityNetworkConfiguration();

	static bool loadRuntimeNetworkConfiguration(LanDevice& dev);
	static bool loadNetworkConfiguration(LanDevice& dev);
	static bool changeNetworkConfiguration(LanDevice& dev, const IPConfiguration& newIPConfiguration);

private:
	static void appendAdress(unsigned char* data, int indexData, std::string adress);
	static void getNetworkConfigData(unsigned char* data, int* indexData, const IPConfiguration& newIPConfiguration);
};
}

#endif
