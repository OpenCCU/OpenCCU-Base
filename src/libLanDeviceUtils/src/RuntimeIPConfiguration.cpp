/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RuntimeIPConfiguration.h"

using namespace LDU;

RuntimeIPConfiguration::RuntimeIPConfiguration(void)
{
}

RuntimeIPConfiguration::~RuntimeIPConfiguration(void)
{
}

const std::string& RuntimeIPConfiguration::getIPAddress() const {
	return ipAddress;	
}

void RuntimeIPConfiguration::setIPAddress(const std::string ipAddress) {
	this->ipAddress = ipAddress;
}

const std::string& RuntimeIPConfiguration::getNetmask() const {
	return netmask;
}

void RuntimeIPConfiguration::setNetmask(const std::string& netmask) {
	this->netmask = netmask;
}

const std::string& RuntimeIPConfiguration::getDefaultGateway() const {
	return defaultGateway;
}

void RuntimeIPConfiguration::setDefaultGateway(const std::string& defaultGateway) {
	this->defaultGateway = defaultGateway;
}

const std::string& RuntimeIPConfiguration::getPrimaryDNS() const {
	return primaryDNS;
}

void RuntimeIPConfiguration::setPrimaryDNS(const std::string& primaryDNS) {
	this->primaryDNS = primaryDNS;
}

const std::string& RuntimeIPConfiguration::getSecondaryDNS() const {
	return secondaryDNS;
}

void RuntimeIPConfiguration::setSecondaryDNS(const std::string& secondaryDNS) {
	this->secondaryDNS = secondaryDNS;
}
/*
bool RuntimeIPConfiguration::isUsingDHCP() const {
	return usingDHCP;
}

void RuntimeIPConfiguration::setUsingDHCP(const bool usingDHCP) {
	this->usingDHCP = usingDHCP;
}
*/
std::string RuntimeIPConfiguration::toString() const {
	std::string s;
	s.append("----------------\n");
	s.append("Runtime IP Configuration\n");
	s.append("----------------\n");
	s.append("IP Address:      "); s.append(ipAddress); s.append("\n");
	s.append("Netmask:         "); s.append(netmask); s.append("\n");
	s.append("Default Gateway: "); s.append(defaultGateway); s.append("\n");
	s.append("Primary DNS:     "); s.append(primaryDNS); s.append("\n");
	s.append("Secondary DNS:   "); s.append(secondaryDNS); s.append("\n");
	//s.append("Uses DHCP:       "); s.append( usingDHCP ? "True" : "False" ); s.append("\n");
	s.append("----------------\n");
	return s;
}