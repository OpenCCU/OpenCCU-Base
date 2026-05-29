/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include "setlgwkey.h"

#include <string>
#include <string.h>
#include <vector>
#include <iomanip>
#include <sstream>
#include <LanDeviceUtils.h>
#include <PropertyMap.h>
#include <md5.h>
//#include <UnifiedLanComm.h>
#include <SyslogLogger.h>
#include <ConsoleLogger.h>


using namespace LDU;

SetLGWKey::SetLGWKey() : Command("setlgwkey")
{
}

int SetLGWKey::execute()
{
	
	std::string serial;
	std::string ipAddress;
	unsigned char* key = NULL;
	int keyLength = 0;
	std::string keyStr;
	std::string newKeyStr;
	std::string confFilePath;
	bool generateMD5Checksum = true;
	bool logToConsole = false;
	int logLevel = 5;
	//Parse args
	const int paramsSize = static_cast<int>(params.size());
	for(int i = 0; i < paramsSize ; i++) {
		if((params.at(i).compare("-s") == 0) && ((i+1)<paramsSize) ) {
			i++;
			serial.assign(params.at(i));
		}
		else if((params.at(i).compare("-h") == 0) && ((i+1)<paramsSize) ) {
			i++;
			ipAddress.assign(params.at(i));
		}
		else if((params.at(i).compare("-c") == 0) && ((i+1)<paramsSize) ) {
			i++;
			keyStr = params.at(i);
		}
		else if((params.at(i).compare("-n") == 0) && ((i+1)<paramsSize) ) {
			i++;
			newKeyStr = params.at(i);
		}
		else if((params.at(i).compare("-f") == 0) && ((i+1)<paramsSize) ) {
			i++;
			confFilePath = params.at(i);
		}
		else if((params.at(i).compare("-k") == 0)) {
			generateMD5Checksum = false;
		}
		else if((params.at(i).compare("-console") == 0)) {
			logToConsole = true;
		}
		else if((params.at(i).compare("-l") == 0) && ((i+1)<paramsSize)) {
			i++;
			sscanf(params.at(i).c_str(), "%d", &logLevel);
		}
	}
	
	if(logToConsole) {
		logger = new ConsoleLogger();		
	}
	else {
		logger = new SyslogLogger("setlgwkey");
	}
	logger->SetLevel((Logger::LogLevel)logLevel);
	
	LanDeviceUtils ldUtils;
	LanDevice lgw;
	//Check if serial was supplied. We must have that.
	if(serial.empty()) {
		LOG(Logger::LOG_ERROR, "No serial number supplied.\n");
		LOG(Logger::LOG_ERROR, "%s", help().c_str());
		if(key != NULL) { delete[] key; }
		return 1;
	}

	int retVal = determineIPAddress(ldUtils, lgw, serial, ipAddress);
	if(retVal != 0) {
		LOG(Logger::LOG_ERROR, "Cannot determine IP address of gateway with serial %s\n", serial.c_str());
		if(key != NULL) { delete[] key; }
		return retVal;
	}

	//Encryption enabled?
	bool done = ldUtils.readNetworkConfiguration(lgw);
	if(!done) {
		LOG(Logger::LOG_ERROR, "Error reading network configuration.\n");
		if(key != NULL) { delete[] key; }
		return 4;
	}
	if(lgw.getIPConfiguration().isCryptEnabled()) {
		if(keyStr.empty()) {
			LOG(Logger::LOG_ERROR, "Please provide aes key.\n");
			if(key != NULL) { 
				delete[] key; 
				key = NULL;
			}
			return 5;
		}
		if(generateMD5Checksum) {
			std::string keyStrMD5 = calculateMD5(keyStr);
			key = new unsigned char[keyStrMD5.size()];
			memcpy(key, keyStrMD5.c_str(), keyStrMD5.size());
			keyLength = keyStrMD5.size();
		}
		else {
			charArrayFromHexString(keyStr, &key, keyLength);

		}
		lgw.setAesKey(key, keyLength);
	}
	
	//Perform key exchange
	unsigned char* pNewKey = NULL;
	int newKeyLength = 0;
	if(generateMD5Checksum) {
		std::string newKeyStrMD5 = calculateMD5(newKeyStr);
		pNewKey = new unsigned char[newKeyStrMD5.size()];
		memcpy(pNewKey, newKeyStrMD5.c_str(), newKeyStrMD5.size()); 
		newKeyLength = newKeyStrMD5.size();
	}
	else {
		charArrayFromHexString(newKeyStr, &pNewKey, newKeyLength);
	}
	LOG(Logger::LOG_INFO, "Performing key exchange for lan gateway %s\n", serial.c_str());
	done = ldUtils.keyExchange(lgw, pNewKey, newKeyLength);		
	if(!done) {
		LOG(Logger::LOG_ERROR, "Key exchange failed.\n");
		if(key != NULL) { 
			delete[] key; 
			key = NULL;
		}
		if(pNewKey != NULL) {
			delete[] pNewKey;
			pNewKey = NULL;
		}
		return 6;
	}

	//Update RFD / HS485D config file if -f was supplied
	if(!confFilePath.empty()) {
		done = updateConfigFile(confFilePath, serial, newKeyStr);
	}
	if(!done) {
		//TODO Handle failure, maybe rollback keychange....
	}

	//Cleanup		
	if(key != NULL) { 
		delete[] key; 
		key = NULL;
	}
	if(pNewKey != NULL) {
		delete[] pNewKey;
		pNewKey = NULL;
	}
		
	return 0;
}

std::string SetLGWKey::help()
{
	std::string s("Usage:\nsetlgwkey <-s LGWSerial> [-h IP] [-c CurrentKey] <-n NewKey> [-f ConfigFile] [-k]\n");
	s.append("\t-s: Serial number of Lan Gateway\n");
	s.append("\t-h: IP address of Lan Gateway, to override search by serial\n");
	s.append("\t-c: Current security key, if set\n");
	s.append("\t-n: New security key\n");
	s.append("\t-f: RFD/HS485D config file path (Optional); If supplied, corresponding interface entry will be updated.\n");
	s.append("\t-k: If supplied, keys supplied with -c and -n are used directly (without md5 checksum generation).\n");
	s.append("\t-console: Log to console instead of syslog.\n");
	s.append("\t-l: Loglevel.\n");
	return s;
}

int SetLGWKey::determineIPAddress(LanDeviceUtils& ldUtils, LanDevice& lgw, const std::string& serial, std::string& ipAddress)
{
	if(serial.empty()) {
		LOG(Logger::LOG_ERROR, "Please provide IP-Address or serial number.\n");
		LOG(Logger::LOG_ERROR, "%s", help().c_str());
		return 2;
	}
	if(!ipAddress.empty()) {//try unicast and if that fails, fallback to search by serial
		std::vector<std::string> devTypeFilters;
		devTypeFilters.push_back(std::string("*"));
		std::vector<LanDevice> devices;
		ldUtils.searchDevicesByType(devTypeFilters, PROTOCOL_EQ3CONFIG, ROUTINGSCHEME_UNICAST, ipAddress, std::string(""), devices);
		bool done = true;
		if(devices.size() > 0) {
			lgw = devices.at(0);
		}
		else {
			LOG(Logger::LOG_ERROR, "Could not find gateway by unicast with ip %s. Trying search by serial number.\n", ipAddress.c_str());
			done = ldUtils.searchDeviceBySerial(serial, lgw);
			if(!done) {
				LOG(Logger::LOG_ERROR, "Could not find Lan Gateway using ip and serial number\n");
				return 3;
			}
			else {
				ipAddress = lgw.getRuntimeIPConfiguration().getIPAddress();
			}
		}
	}
	else {
		bool done = ldUtils.searchDeviceBySerial(serial, lgw);
		if(!done) {
			LOG(Logger::LOG_ERROR, "Could not find Lan Gateway using serial number %s\n",serial.c_str());
			return 3;
		}
	}
	bool done = ldUtils.readRuntimeNetworkConfiguration(lgw);
	if(!done) {
		LOG(Logger::LOG_ERROR, "Could not determine IP Address of Lan Gateway with serial number %s\n", serial.c_str());
	}
	if(ipAddress.empty()) {
		ipAddress = lgw.getRuntimeIPConfiguration().getIPAddress();
	}
	return 0;
}


void SetLGWKey::charArrayFromHexString(const std::string& hexStr, unsigned char** charArray, int& charArrayLength)
{
	unsigned int temp;
	int c = 0;
	if((hexStr.size() % 2) != 0) {
		charArrayLength = 0;
		charArray = NULL;
		return;
	}
	charArrayLength = hexStr.size() / 2;
	*charArray = new unsigned char[charArrayLength];
	for(unsigned int i = 0; i < hexStr.size(); i+=2) {
		temp = 0;
		std::stringstream ss;
		ss << std::hex << hexStr.substr(i, 2);
		ss >> temp;
		(*charArray)[c] = (unsigned char)(temp & 0xFF);
		c++;
	}
}

bool SetLGWKey::updateConfigFile(const std::string& confFilePath, const std::string& serial, const std::string& newKeyStr)
{
	bool retVal = false;
	PropertyMap configData;
	if( (configData.ReadFromFile(confFilePath) < 0) ) {//try to read file
		return false;
	}
	PropertyMap::StringList sections=configData.ListSections();
    for(PropertyMap::StringList::iterator it=sections.begin();it!=sections.end();it++)
    {
        std::string& section=*it;
        if(section.find("Interface ")==0)
        {
            configData.SetCurrentSection(section);
            std::string currentSectionSerial = configData.GetStringValue("Serial Number", "");
            if(currentSectionSerial.compare(serial) == 0) {
            	//Found desired interface block -> add or replace encryption key
            	configData.SetStringValue("Encryption Key", newKeyStr);
            }
            //Write changes back to file.
            retVal = (configData.WriteToFile() != -1);
        }
    }
	return retVal;
}

std::string SetLGWKey::calculateMD5(const std::string& s)
{
	md5 md5_calculator;
	unsigned char* buffer = new unsigned char[s.length()];
	memcpy(buffer, s.c_str(), s.length());
	md5_calculator.Update(buffer, s.length());
	md5_calculator.Finalize();
	delete buffer;
	std::string digest;
	digest.append((const char*) md5_calculator.Digest(), std::string::size_type(16));
	return digest;
}

