/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LanifCfgProtocol.h"
#include <InternalUtilities.h>
#include <Logger.h>

using namespace std;
using namespace LDU;

LanifCfgProtocol::LanifCfgProtocol(void)
{
}

LanifCfgProtocol::~LanifCfgProtocol(void)
{
}

ProtocolEnum LanifCfgProtocol::getProtocolType() const {
	return PROTOCOL_EQ3LANIFCFG;
}

const std::string LanifCfgProtocol::getProtocolName() const {
	return string("eQ3LanIf");
}

unsigned int LanifCfgProtocol::getProtocolDefaultPort() const {
	return 23272;
}

unsigned int LanifCfgProtocol::getProtocolDefaultReplyPort() const {
	return 23272;
}

string LanifCfgProtocol::assembleFrameHeaderBase(const std::string& devType, const std::string& serial) const {
	string frame(devType);
	frame.resize(8, 0);//ensure the name has exactly 8 chars now.
	frame.append(serial);
	frame.resize(18, 0);//ensure frame has exacctly 18 chars now.
	return frame;
}

string LanifCfgProtocol::getIdentifyFrame(const std::string& devType, const std::string& serial) const {
	string frame = assembleFrameHeaderBase(devType, serial);
	frame.append(1, 'I'); //Identity opcode
	//no userdata
	return frame;
}

vector<LanDevice> LanifCfgProtocol::parseIdentifyResponses( const vector<string>& responses) const {
	vector<LanDevice> devs;
	for(int unsigned i = 0; i < responses.size(); i++) {
		string response = responses.at(i);
		if(response.length() >= 22) {//FIXME check if this is correct size for this protocol!!!
			if((response.at(18) == '>') && (response.at(19) == 'I')) { //valid response?
				LanDevice dev;
				dev.setType(response.substr(0,8));				//Extract device type (signature)
				dev.setSerialNumber(response.substr(8, 10));	//Extract serial number
				/*
					TODO ???:
					Offset 21 ; Länge 3 <- Funkadresse (MAX)
					Offset 24 ; Länge 2 <- Firmware Version (MAX)
				*/
				dev.setProtocolType(PROTOCOL_EQ3LANIFCFG);
				devs.push_back(dev);
			}
		}
	}
	
	return devs;
}

string LanifCfgProtocol::getRuntimeIPConfigRequestFrame(const std::string& devType, const string& serial) const {
	string frame = assembleFrameHeaderBase(devType, serial); //Maybe full qualified devtype string could be used to reduce network traffic
	frame.append(1, 'N');
	return frame;
}

bool LanifCfgProtocol::parseRuntimeIPConfigRequestResponse( const string& response, const std::string& expectedSerial, RuntimeIPConfiguration& ipConfig) const {
	if(response.size() < 40) {
		return false;
	}
	//Extract serial and compare with expected
	string temp = response.substr(8, 10);
	if(expectedSerial.compare(temp) != 0) {
		return false;
	}
	//IP
	temp = assembleIpAddressStringFromByteNumbers( response.at(20), response.at(21), response.at(22), response.at(23) );
	ipConfig.setIPAddress( temp );
	//default gateway
	temp = assembleIpAddressStringFromByteNumbers( response.at(24), response.at(25), response.at(26), response.at(27) );
	ipConfig.setDefaultGateway( temp );
	//netmask
	temp = assembleIpAddressStringFromByteNumbers( response.at(28), response.at(29), response.at(30), response.at(31) );
	ipConfig.setNetmask( temp );
	//primary dns
	temp = assembleIpAddressStringFromByteNumbers( response.at(32), response.at(33), response.at(34), response.at(35) );
	ipConfig.setPrimaryDNS( temp );
	//secondary dns
	temp = assembleIpAddressStringFromByteNumbers( response.at(36), response.at(37), response.at(38), response.at(39) );
	ipConfig.setSecondaryDNS( temp );
	return true;
}

std::string LanifCfgProtocol::getGetNetworkConfigurationFrame(const std::string& devType, const std::string& serial) const
{
	string frame = assembleFrameHeaderBase(devType, serial);
	frame.append(1, 'c');
	return frame;
}

std::string LanifCfgProtocol::getSetNetworkConfigurationFrame(const std::string& devType, const std::string& serial, const IPConfiguration& newIPConfiguration) const 
{
	string frame = assembleFrameHeaderBase(devType, serial);
	//command
	frame.append(1, 'C');//SetConfiguration
	//IP address
	appendAddress(frame, newIPConfiguration.getIPAddress());
	//default gateway
	appendAddress(frame, newIPConfiguration.getDefaultGateway());
	//netmask
	appendAddress(frame, newIPConfiguration.getNetmask());
	//primary dns
	appendAddress(frame, newIPConfiguration.getPrimaryDNS());
	//secondary dns
	appendAddress(frame, newIPConfiguration.getSecondaryDNS());
	//dhcp on off
	char tempChar = (char)(newIPConfiguration.isDHCPEnabled() ? 0x01 : 0x00);
	frame.append(1, tempChar);
	//encryption on/off
	tempChar = (char)(newIPConfiguration.isCryptEnabled() ? 0x01 : 0x00);
	frame.append(1, tempChar);
	return frame;
}

bool LanifCfgProtocol::parseGetNetworkConfigurationFrameResponse(const std::string& response, const std::string& expectedSerial, IPConfiguration& ipConfig ) const {
	if(response.size() < 42) {
		LOG(Logger::LOG_ERROR, "LanifCfgProtocol::parseGetNetworkConfigurationFrameResponse(): Message too small.");
		return false;
	}
	//Extract serial and compare with expected
	string temp = response.substr(8, 10);
	if(expectedSerial.compare(temp) != 0) {
		LOG(Logger::LOG_ERROR, "LanifCfgProtocol::parseGetNetworkConfigurationFrameResponse(): Serials do not match.");
		return false;
	}
	//IP
	temp = assembleIpAddressStringFromByteNumbers( response.at(20), response.at(21), response.at(22), response.at(23) );
	ipConfig.setIPAddress( temp );
	//default gateway
	temp = assembleIpAddressStringFromByteNumbers( response.at(24), response.at(25), response.at(26), response.at(27) );
	ipConfig.setDefaultGateway( temp );
	//netmask
	temp = assembleIpAddressStringFromByteNumbers( response.at(28), response.at(29), response.at(30), response.at(31) );
	ipConfig.setNetmask( temp );
	//primary dns
	temp = assembleIpAddressStringFromByteNumbers( response.at(32), response.at(33), response.at(34), response.at(35) );
	ipConfig.setPrimaryDNS( temp );
	//secondary dns
	temp = assembleIpAddressStringFromByteNumbers( response.at(36), response.at(37), response.at(38), response.at(39) );
	ipConfig.setSecondaryDNS( temp );
	//dhcp on/off
	ipConfig.setDHCPEnabled( (response.at(40) == (char)0x01) ) ;
	//encryption on/off
	ipConfig.setCryptEnabled( (response.at(41) == (char)0x01) );
	return true;
}

bool LanifCfgProtocol::parseAckRespone(const std::string& response, char* requestOpcode, int* returnCode, std::string* /*errorMessage*/) const 
{
	if(response.size() >= 21) {
		*requestOpcode = response.at(19);

		if( response.at(20) == (char)0x01 ) {
			if(returnCode != NULL) {
				*returnCode = 1;
			}
			return true;
		}
		else if(returnCode != NULL) {
			*returnCode = (int)response.at(20);
		}
	}
	return false;
}

bool LanifCfgProtocol::parseTestStatus(const std::string& response, const std::string& expectedSerial, TestStatusConfiguration& testStatusConfiguration) const
{
	return false;
}

void LanifCfgProtocol::appendAddress(std::string& str, const std::string& addr) const
{
	int a, b, c, d;
	extractIPAddressComponents( addr, a, b, c, d );
	str.append(1, (char)a);
	str.append(1, (char)b);
	str.append(1, (char)c);
	str.append(1, (char)d);
}