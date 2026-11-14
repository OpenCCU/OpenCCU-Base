/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_IPCONFIGURATION_H_
#define _LIBLANDEVICEUTILS_IPCONFIGURATION_H_

#include <string>

namespace LDU {

#include <DLLImportExport.h>

class IPConfiguration
{
public:
	LIBLANDEVICEUTILS_API IPConfiguration(void);
	LIBLANDEVICEUTILS_API virtual ~IPConfiguration(void);

//VARIABLES
private:
	std::string ipAddress;
	std::string netmask;
	std::string defaultGateway;
	std::string primaryDNS;
	std::string secondaryDNS;
	bool dhcpEnabled;
	bool autoIPEnabled;
	bool cryptEnabled;
	bool defaultCrypt;
	unsigned char maxAllowedDNSNameLength;
	std::string dnsName;

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

	LIBLANDEVICEUTILS_API bool isDHCPEnabled() const;
	LIBLANDEVICEUTILS_API void setDHCPEnabled(const bool enabled);

	LIBLANDEVICEUTILS_API bool isAutoIPEnabled() const;
	LIBLANDEVICEUTILS_API void setAutoIPEnabled(const bool enabled);

	LIBLANDEVICEUTILS_API bool isCryptEnabled() const;
	LIBLANDEVICEUTILS_API void setCryptEnabled(const bool enabled);
	
	LIBLANDEVICEUTILS_API bool isDefaultCrypt() const;
	LIBLANDEVICEUTILS_API void setDefaultCrypt(const bool isDefault);

	LIBLANDEVICEUTILS_API void setMaxAllowedDNSNameLength(const unsigned int length);
	LIBLANDEVICEUTILS_API unsigned int getMaxAllowedDNSNameLength() const;

	LIBLANDEVICEUTILS_API std::string getDNSName() const;
	
	/**\brief Sets DNS name.
	* \param name DNS name to set.
	* \return True if set, false if not set because name's length is greater than maxAllowedDNSNameLength.
	*/
	LIBLANDEVICEUTILS_API bool setDNSName(const std::string& name);

	LIBLANDEVICEUTILS_API std::string toString() const;
public:
};
}
#endif
