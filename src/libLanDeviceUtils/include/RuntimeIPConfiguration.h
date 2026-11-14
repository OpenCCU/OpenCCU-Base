/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_RUNTIMEIPCONFIGURATION_H_
#define _LIBLANDEVICEUTILS_RUNTIMEIPCONFIGURATION_H_

#include <string>

namespace LDU {

#include <DLLImportExport.h>

class RuntimeIPConfiguration
{
public:
	LIBLANDEVICEUTILS_API RuntimeIPConfiguration(void);
	LIBLANDEVICEUTILS_API virtual ~RuntimeIPConfiguration(void);

//VARIABLES
private:
	std::string ipAddress;
	std::string netmask;
	std::string defaultGateway;
	std::string primaryDNS;
	std::string secondaryDNS;
	//bool usingDHCP;

//METHODS

public:
	LIBLANDEVICEUTILS_API const std::string& getIPAddress() const;
	LIBLANDEVICEUTILS_API void setIPAddress(const std::string ipAddress);

	LIBLANDEVICEUTILS_API const std::string& getNetmask() const;
	LIBLANDEVICEUTILS_API void setNetmask(const std::string& netmask);

	LIBLANDEVICEUTILS_API const std::string& getDefaultGateway() const;
	LIBLANDEVICEUTILS_API void setDefaultGateway(const std::string& defaultGateway);

	LIBLANDEVICEUTILS_API const std::string& getPrimaryDNS() const;
	LIBLANDEVICEUTILS_API void setPrimaryDNS(const std::string& primaryDNS);

	LIBLANDEVICEUTILS_API const std::string& getSecondaryDNS() const;
	LIBLANDEVICEUTILS_API void setSecondaryDNS(const std::string& secondaryDNS);

	//LIBLANDEVICEUTILS_API bool isUsingDHCP() const;
	//LIBLANDEVICEUTILS_API void setUsingDHCP(const bool usingDHCP);


	LIBLANDEVICEUTILS_API std::string toString() const;
public:
};
}
#endif
