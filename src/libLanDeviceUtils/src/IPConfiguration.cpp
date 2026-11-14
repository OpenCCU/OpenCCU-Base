/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "IPConfiguration.h"
#ifndef WIN32
# include <string.h>
# include <stdio.h>
#else
# define snprintf _snprintf
#endif


using namespace LDU;

IPConfiguration::IPConfiguration(void)
: dhcpEnabled(false)
, autoIPEnabled(false)
, cryptEnabled(false)
, defaultCrypt(false)
, maxAllowedDNSNameLength(0)
{
}

IPConfiguration::~IPConfiguration(void)
{
}

const std::string& IPConfiguration::getIPAddress() const 
{
	return this->ipAddress;	
}

void IPConfiguration::setIPAddress(const std::string ipAddress) 
{
	this->ipAddress = ipAddress;
}

const std::string& IPConfiguration::getNetmask() const 
{
	return this->netmask;
}

void IPConfiguration::setNetmask(const std::string& netmask) 
{
	this->netmask = netmask;
}

const std::string& IPConfiguration::getDefaultGateway() const 
{
	return this->defaultGateway;
}

void IPConfiguration::setDefaultGateway(const std::string& defaultGateway) 
{
	this->defaultGateway = defaultGateway;
}

const std::string& IPConfiguration::getPrimaryDNS() const 
{
	return this->primaryDNS;
}

void IPConfiguration::setPrimaryDNS(const std::string& primaryDNS) 
{
	this->primaryDNS = primaryDNS;
}

const std::string& IPConfiguration::getSecondaryDNS() const 
{
	return secondaryDNS;
}

void IPConfiguration::setSecondaryDNS(const std::string& secondaryDNS) 
{
	this->secondaryDNS = secondaryDNS;
}

bool IPConfiguration::isDHCPEnabled() const 
{
	return dhcpEnabled;
}

void IPConfiguration::setDHCPEnabled(const bool enabled) 
{
	this->dhcpEnabled = enabled;
}

bool IPConfiguration::isAutoIPEnabled() const 
{
	return autoIPEnabled;
}

void IPConfiguration::setAutoIPEnabled(const bool enabled) 
{
	this->autoIPEnabled = enabled;
}

bool IPConfiguration::isCryptEnabled() const
{
	return this->cryptEnabled;
}

void IPConfiguration::setCryptEnabled(const bool enabled)
{
	if(enabled)
	{
		printf("cryptEnabled true");
	}

	this->cryptEnabled = enabled;
}

bool IPConfiguration::isDefaultCrypt() const
{
	return this->defaultCrypt;
}

void IPConfiguration::setDefaultCrypt(const bool isDefault)
{
	if(isDefault)
	{
		// printf("isDefault true");
	}

	this->defaultCrypt = isDefault;
}

void IPConfiguration::setMaxAllowedDNSNameLength(const unsigned int length)
{
	maxAllowedDNSNameLength = (unsigned char)length;
}

unsigned int IPConfiguration::getMaxAllowedDNSNameLength() const
{
	return (int)maxAllowedDNSNameLength;
}

std::string IPConfiguration::getDNSName() const
{
	return dnsName;
}
	
bool IPConfiguration::setDNSName(const std::string& name)
{
	this->dnsName = name;
	return true;
}

std::string IPConfiguration::toString() const 
{
	std::string s;
	s.append("----------------\n");
	s.append("IP Configuration\n");
	s.append("----------------\n");
	s.append("IP Address:          "); s.append(this->ipAddress); s.append("\n");
	s.append("Netmask:             "); s.append(this->netmask); s.append("\n");
	s.append("Default Gateway:     "); s.append(this->defaultGateway); s.append("\n");
	s.append("Primary DNS:         "); s.append(this->primaryDNS); s.append("\n");
	s.append("Secondary DNS:       "); s.append(this->secondaryDNS); s.append("\n");
	s.append("Uses DHCP:           "); s.append(this->dhcpEnabled ? "True" : "False" ); s.append("\n");
	s.append("Uses AutoIP:         "); s.append(this->autoIPEnabled ? "True" : "False" ); s.append("\n");
	s.append("Uses Encryption:     "); s.append(this->cryptEnabled ? "True" : "False" ); s.append("\n");
	s.append("Default Encryption:  "); s.append(this->defaultCrypt ? "True" : "False" ); s.append("\n");
	char* buffer = new char[128];
	memset(buffer, 0, sizeof(char)*128);
	snprintf(buffer, 128, "%d", maxAllowedDNSNameLength);
	s.append("DNS-Name mx. length: "); s.append( buffer ); s.append("\n");
	delete[] buffer;
	s.append("DNS-Name:            "); s.append( dnsName ); s.append("\n");
	s.append("----------------\n");
	return s;
}
