/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <EQ3ConfigProtocol.h>
#include <LanDevice.h>
#include <InternalUtilities.h>
#include <Logger.h>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
using namespace LDU;

unsigned char EQ3ConfigProtocol::senderid0 = 0x00;
unsigned char EQ3ConfigProtocol::senderid1 = 0x00;
unsigned char EQ3ConfigProtocol::senderid2 = 0x00;
unsigned char EQ3ConfigProtocol::packagecounter = 0x00;

EQ3ConfigProtocol::EQ3ConfigProtocol(void)
{
}

EQ3ConfigProtocol::~EQ3ConfigProtocol(void)
{
}

ProtocolEnum EQ3ConfigProtocol::getProtocolType() const {
	return PROTOCOL_EQ3CONFIG;
}

const std::string EQ3ConfigProtocol::getProtocolName() const {
	return string("eQ3Config");
}

unsigned int EQ3ConfigProtocol::getProtocolDefaultPort() const {
	return 43439;
}

unsigned int EQ3ConfigProtocol::getProtocolDefaultReplyPort() const {
	return 0;
}

string EQ3ConfigProtocol::assembleHeaderFrameBase(const std::string& devType, const std::string& serial, int & length) {
	string frame("");
	frame.append(1, 0x2);//UDP Version
	++length;

	if(senderid0 == 0x00)
	{
		srand ( time(NULL) );
		senderid0 = rand();
		senderid1 = rand();
		senderid2 = rand();
	}

	// AbsenderID und Paket counter
	frame.append(1, senderid0);
	++length;
	frame.append(1, senderid1);
	++length;
	frame.append(1, senderid2);
	++length;
	frame.append(1, ++packagecounter);
	++length;

	frame.append(devType);//Gerätekennung  
	length += strlen(devType.c_str());
	frame.append(1, 0x0);
	++length;
	frame.append(serial);//Seriennummer
	length += strlen(serial.c_str());
	frame.append(1, 0x0);
	++length;
	return frame;
}

std::string EQ3ConfigProtocol::getIdentifyFrame(const std::string& devType, const std::string& serial) const {	
	int length = 0;
	std::string frame(EQ3ConfigProtocol::assembleHeaderFrameBase(devType, serial, length));
	frame.append("I");//OpCode
	return frame;
}

std::vector<LanDevice> EQ3ConfigProtocol::parseIdentifyResponses( const std::vector<std::string>& responses) const {
	vector<LanDevice> devs;
	string zeroStr;
	zeroStr.append(1, '\0');
	for(unsigned int i = 0; i < responses.size(); i++) {
		string response = responses.at(i);
		LanDevice dev;
		std::string fwVersion, devtype, serial;
		int offset = 0;
		bool done = parseHeader(response, offset, &devtype, &serial);
		if(done) {
			dev.setProtocolType(PROTOCOL_EQ3CONFIG);
			dev.setSerialNumber(serial);
			dev.setType(devtype);
			offset += 3; // > + opcode + returncode
			done = extractString(response, zeroStr, offset, fwVersion);
			if(done)
			{	
				dev.setFirmwareVersion( fwVersion );
			}

			vector<LanDevice::ServiceProtocol> serviceProtocols;

			int sp = -1;
			int port = -1;
			while ((sp != 0) && ((int)response.size() > (offset + 1)))
			{
				done = extractUInt16(response, offset, sp);
				if(done && sp != 0)
				{
					done = extractUInt16(response, offset, port);
					if(done)
					{					
						LanDevice::ServiceProtocol serviceprotocol;
						serviceprotocol.id = sp;
						serviceprotocol.port = port;
						serviceProtocols.push_back(serviceprotocol);
					}
				}
			}

			dev.setServiceProtocols(serviceProtocols);

			devs.push_back(dev);
		}
	}
	return devs;
}

string EQ3ConfigProtocol::getRuntimeIPConfigRequestFrame(const std::string& devType, const string& serial) const {
	int length = 0;
	std::string frame(EQ3ConfigProtocol::assembleHeaderFrameBase(devType, serial, length));
	frame.append(1, 'n');
	return frame;
}

bool EQ3ConfigProtocol::parseRuntimeIPConfigRequestResponse(const string& response, const string& expectedSerial, RuntimeIPConfiguration& ipConfig) const 
{
	std::string serial, data;
	char requestOpCode;
	int returnCode;
	bool done = parseResponseCommonData( response, NULL, &serial, &requestOpCode, &returnCode, &data);
	if(!done) {
		//TODO Log error?! or check return code??
		return false;
	}
	if(expectedSerial.compare(serial) != 0) {
		LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseRuntimeIPConfigRequestResponse(): Serials doesn't match.");
		return false;
	}
	if(requestOpCode != 'n') {
		LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseRuntimeIPConfigRequestResponse(): Unexpected opcode. -> Not expected answer");
	}

	//ip address
	int offset = 0;
	string temp;
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(	data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setIPAddress(temp);
	offset += 4;
	//Gateway
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(  data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setDefaultGateway(temp);
	offset += 4;
	//Netmask
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(  data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setNetmask(temp);
	offset += 4;
	//primary dns server
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(  data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setPrimaryDNS(temp);
	offset += 4;
	//secondary dns server
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(  data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setSecondaryDNS(temp);
	return true;
}

std::string EQ3ConfigProtocol::getGetNetworkConfigurationFrame(const std::string& devType, const std::string& serial) const
{
	int length = 0;
	std::string frame(EQ3ConfigProtocol::assembleHeaderFrameBase(devType, serial, length));
	frame.append(1, 'c');//da opcode
	return frame;
}

std::string EQ3ConfigProtocol::getSetNetworkConfigurationFrame(const std::string& devType, const std::string& serial, const IPConfiguration& newIPConfiguration) const
{
	int length = 0;
	std::string frame(EQ3ConfigProtocol::assembleHeaderFrameBase(devType, serial, length));
	frame.append(1, 'C'); //opcode

	//ip address
	appendAddress(frame, newIPConfiguration.getIPAddress());
	//gateway
	appendAddress(frame, newIPConfiguration.getDefaultGateway()); 
	//netmask
	appendAddress(frame, newIPConfiguration.getNetmask());
	//dns1
	appendAddress(frame, newIPConfiguration.getPrimaryDNS());
	//dns2
	appendAddress(frame, newIPConfiguration.getSecondaryDNS());
	//ip-flags
	char tempC = (char) (newIPConfiguration.isDHCPEnabled() ? 0x01 : 0x00);//dhcp
	tempC = tempC | (char) (newIPConfiguration.isAutoIPEnabled() ? 0x02 : 0x00);//autoip
	frame.append(1, tempC); 
	//crypt
	tempC = (char)(newIPConfiguration.isCryptEnabled() ? 0x01 : 0x00);
	frame.append(1, (char)tempC); //encryption on/off
	//dnsname
	frame.append( newIPConfiguration.getDNSName() );
	frame.append( 1, (char)0x00);//FIXME zero termination?!
	return frame;
}

bool EQ3ConfigProtocol::parseGetNetworkConfigurationFrameResponse(const std::string& response, const std::string& expectedSerial, IPConfiguration& ipConfig ) const
{
	std::string devType, serial, data;
	int returnCode;
	char reqOpcode;
	bool done = parseResponseCommonData( response, &devType, &serial, &reqOpcode, &returnCode, &data);
	if(!done) {
		return false;
	}
	if( serial.compare(expectedSerial) != 0) {
		LOG(Logger::LOG_DEBUG, "EQ3ConfigProtocol::parseGetNetworkConfigurationFrameResponse(): Serials do not match.");
		return false;
	}
	if(reqOpcode != 'c') {
		LOG(Logger::LOG_DEBUG, "EQ3ConfigProtocol::parseGetNetworkConfigurationFrameResponse(): Not response of c command.");
	}
	//Parsing data
	int offset = 0;
	string temp;
	//IP address
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(	data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setIPAddress(temp);
	offset += 4;
	//gateway
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(	data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setDefaultGateway(temp);
	offset += 4;
	//netmask
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(	data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setNetmask(temp);
	offset += 4;
	//dns1
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(	data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setPrimaryDNS(temp);
	offset += 4;
	//dns2
	if(data.size()-1 < (unsigned int)offset+3) {
		return false;
	}
	temp = assembleIpAddressStringFromByteNumbers(	data.at(offset), 
													data.at(offset+1), 
													data.at(offset+2), 
													data.at(offset+3));
	ipConfig.setSecondaryDNS(temp);
	offset += 4;
	//ip flags (dhcp, auto ip)
	if(data.size() <= (unsigned int)offset) {
		return false;
	}
	char tempChar = data.at(offset);
	ipConfig.setDHCPEnabled( ((tempChar & (char)0x01) == (char)0x01) );
	ipConfig.setAutoIPEnabled( ((tempChar & (char)0x02) == (char)0x02) );
	offset++;
	//encryption
	if(data.size() <= (unsigned int)offset) {
		return false;
	}
	tempChar = data.at(offset);
	ipConfig.setCryptEnabled( ((tempChar & (char)0x01) == (char)0x01) );
	ipConfig.setDefaultCrypt( ((tempChar & (char)0x02) != (char)0x02) );
	offset++;
	//dns name max length
	if(data.size() <= (unsigned int)offset) {
		return false;
	}
	ipConfig.setMaxAllowedDNSNameLength( (unsigned int)data.at(offset) );
	offset++;
	//dns name
	if(data.size() <= (unsigned int)offset) {
		return false;
	}
	ipConfig.setDNSName( data.substr( offset ) );
	return true;
}

bool EQ3ConfigProtocol::parseTestStatus(const std::string& response, const std::string& expectedSerial, TestStatusConfiguration& testStatusConfiguration) const
{
	const char EXPECTED_OPCODE = 't';

	std::string serial, data;
	char requestOpCode;
	int returnCode;
	bool done = parseResponseCommonData( response, NULL, &serial, &requestOpCode, &returnCode, &data);
	if(!done) {
		return false;
	}
	if(expectedSerial.compare(serial) != 0) {
		LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseTestStatus(): Serials doesn't match.");
		return false;
	}
	if(requestOpCode != EXPECTED_OPCODE) {
		LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseTestStatus(): Unexpected opcode. -> Not expected answer");
	}

	testStatusConfiguration.setTestStatus(data.at(0));
	return true;
}

bool EQ3ConfigProtocol::extractString(const std::string& response, const std::string& findStr, int& offset, std::string& resultStr) const {
	std::string::size_type index = response.find(findStr, offset);
	if(index != string::npos) {
		string temp = response.substr( offset, index-offset );
		offset += temp.size() + findStr.size();
		resultStr.clear();
		resultStr.append(temp);
		return true;
	}
	else {
		return false;
	}
}

bool EQ3ConfigProtocol::extractUInt16(const std::string& response, int& offset, int& resultInt) const
{
	if(((int)response.size()) >= (offset + 1))
	{
		resultInt = (unsigned char)response.at(offset++) << 8;
		resultInt += (unsigned char)response.at(offset++);

		return true;
	}
	else {
		return false;
	}
}

bool EQ3ConfigProtocol::parseAckRespone(const std::string& response, char* requestOpcode, int* returnCode, std::string* returnData) const 
{
	return parseResponseCommonData( response, NULL, NULL, requestOpcode, returnCode, returnData);
}

bool EQ3ConfigProtocol::parseResponseCommonData(const std::string& response, std::string* devType, std::string* serial, char* requestOpcode, int* returnCode, std::string* data) const
{
	bool returnValue = false;
	std::string zeroStr;
	zeroStr.append(1, (char)0x00);
	int offset;
	bool done = parseHeader( response, offset, devType, serial );
	if(!done) {
		return false;
	}
	//opcode
	offset++; // > (skiping that)
	//ursprungscode
	if((requestOpcode != NULL) && (response.size() > (unsigned int)offset)) {
		*requestOpcode = response.at(offset);
	}
	offset++;
	//returncode
	if(response.size() > (unsigned int)offset) {
		const char rc = response.at(offset);
		if(returnCode != NULL) {
			*returnCode = (int) rc;
		}
		if(data != NULL) {
			data->clear();
			data->append(response.substr(offset+1));
		}
		switch(rc) {
			case 0: //Error
			case 3:
				returnValue = false;
				break;
			case 1: // succ
			case 2: //succ... reboot ...
				returnValue = true;
				break;
			default:
				returnValue = false;
				LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseAckRespone(): Unknown return code.");
		};
	}
	return returnValue;
}

bool EQ3ConfigProtocol::parseHeader(const std::string& response, int& offset, std::string* devType, std::string* serial) const
{
	std::string zeroStr;
	std::string temp;
	zeroStr.append(1, (char)0x00);
	offset = 0;
	//Version (1 byte)
	if(response.size() > (unsigned int)offset) {
		char version = response.at(offset);
		offset++;
		if(version != (char)0x02) {
			LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader(): Not supported protocol version %d", (int)version);
			return false;
		}
	}
	else
	{
		LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader():parsing protocol version, response to short");
		return false;
	}
	
	// Absender-ID und packagecounter
	if(response.size() > (unsigned int)offset + 4) {
		char tempchar = response.at(offset);
		// printf("response at %i, value %i", offset, (unsigned char)tempchar);
		if((unsigned char)tempchar != senderid0)
		{
			LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader():parsing sender id, response not for me");
			return false;
		}

		offset++;
		tempchar = response.at(offset);
		// printf("response at %i, value %i", offset, (unsigned char)tempchar);
		if((unsigned char)tempchar != senderid1)
		{
			LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader():parsing sender id, response not for me");
			return false;
		}

		offset++;
		tempchar = response.at(offset);
		// printf("response at %i, value %i", offset, (unsigned char)tempchar);
		if((unsigned char)tempchar != senderid2)
		{
			LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader():parsing sender id, response not for me");
			return false;
		}

		offset++;

		tempchar = response.at(offset);
		// printf("response at %i, value %i", offset, (unsigned char)tempchar);
		offset++;
	}
	else
	{
		LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader():parsing sender id and package counter, response to short");
		return false;
	}

	//Device type (Gerätekennung)
	if(response.size() > (unsigned int)offset) {
		temp.clear();
		bool done = extractString(response, zeroStr, offset, temp);
		if(!done) {
			LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader(): Error extracting device type string.");
			return false;
		}
		if(devType != NULL) {
			devType->clear();
			devType->append(temp);
		}
	}
	else
	{
		LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader(): Error extracting device type string, response to short.");
		return false;
	}

	//serial number (Geräteseriennummer)
	if(response.size() > (unsigned int)offset) {
		temp.clear();
		bool done = extractString(response, zeroStr, offset, temp);
		if(!done) {
			LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader(): Error extracting serial number.");
			return false;
		}
		if(serial != NULL) {
			serial->clear();
			serial->append(temp);
		}
	}
	else
	{
		LOG(Logger::LOG_ERROR, "EQ3ConfigProtocol::parseHeader(): Error extracting serial number, response to short.");
		return false;
	}

	return true;
}

void EQ3ConfigProtocol::appendAddress(std::string& str, const std::string& addr) const
{
	int a, b, c, d;
	extractIPAddressComponents( addr, a, b, c, d );
	str.append(1, (char)a);
	str.append(1, (char)b);
	str.append(1, (char)c);
	str.append(1, (char)d);
}
