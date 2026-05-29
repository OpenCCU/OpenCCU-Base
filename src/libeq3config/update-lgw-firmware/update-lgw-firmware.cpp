/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "update-lgw-firmware.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <LanDeviceUtils.h>
#include <md5.h>
#include <PropertyMap.h>
#include <SyslogLogger.h>
#include <ConsoleLogger.h>
#include <utils.h>

#include <lgwallocation.h>

#ifndef WIN32
# include <unistd.h>
#endif




using namespace std;
using namespace LDU;

#define WSCHARS " \f\n\r\t\v"


LGWFirmwareUpdate::LGWFirmwareUpdate() : Command("update-lgw-firmware")
{
	pLGWAllocation = new LGWAllocation();
}

LGWFirmwareUpdate::~LGWFirmwareUpdate()
{
	delete pLGWAllocation;
}

int LGWFirmwareUpdate::execute()
{
	//Parse args
	if(params.size() < 4) {
		printUsage();
		return 1;
	}

	string updateFilePath;
	string serial;
	string ipAddress;

	string keyStr;
	string confFilePath;
	string fwMapFilePath;
	
	bool logToConsole = false;
	int logLevel = (int)Logger::LOG_INFO;
	
	const int paramsSize = static_cast<int>(params.size());
	
	for(int i = 0; i < paramsSize ; i++) {
		if((params.at(i).compare("-u") == 0) && ((i+1)<paramsSize) ) {
			i++;
			updateFilePath.assign(params.at(i));
		}
		else if((params.at(i).compare("-s") == 0) && ((i+1)<paramsSize) ) {
			i++;
			serial.assign(params.at(i));
		}
		else if((params.at(i).compare("-h") == 0) && ((i+1)<paramsSize) ) {
			i++;
			ipAddress.assign(params.at(i));
		}
		else if((params.at(i).compare("-k") == 0) && ((i+1)<paramsSize) ) {
			i++;
			keyStr = params.at(i);
		}
		else if((params.at(i).compare("-c") == 0) && ((i+1)<paramsSize) ) {
			i++;
			confFilePath = params.at(i);
		}
		else if((params.at(i).compare("-m") == 0) && ((i+1)<paramsSize) ) {
			i++;
			fwMapFilePath = params.at(i);
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
		logger = new SyslogLogger("update-lgw-firmware");
	}
	logger->SetLevel((Logger::LogLevel)logLevel);
	
	
	std::vector<GWInfo> gwInfos;
	std::vector<FWMapInfo> fwis;
	bool manualMode = false;
	//check params for plausibility and do some initialization stuff
	if( (!fwMapFilePath.empty()) && (!confFilePath.empty()) ) {
		//using fwMapFile and conf file
		bool done = readFWMapFile(fwMapFilePath, fwis);
		if((!done) || fwis.size() == 0) {
			LOG(Logger::LOG_ERROR, "Problem parsing fwmap file at %s\n", fwMapFilePath.c_str());
			return 2;
		}
		done = readConfigFile(confFilePath, gwInfos);
		if(!done || gwInfos.size() == 0) {
			LOG(Logger::LOG_INFO, "No gateway found in config file %s\n", confFilePath.c_str());
			return 3;
		}
	}
	else if( (!updateFilePath.empty()) && ( (!serial.empty()) || (!ipAddress.empty()) ) ) {
		//using parameterized version --> fill gwInfos and fwInfos
		GWInfo gwInfo;
		gwInfo.lgwtype = UNDEFINED;
		gwInfo.serial = serial;
		gwInfo.ip = ipAddress;
		gwInfo.key = keyStr;
		gwInfos.push_back(gwInfo);
		FWMapInfo fwInfo;
		fwInfo.lgwtype = UNDEFINED;
		fwInfo.filename = updateFilePath;
		fwis.push_back(fwInfo);
		manualMode = true;
	}
	else {
		printUsage();
		return 1;
	}
	
	//Perform update(s)
	for(unsigned int i = 0; i < gwInfos.size(); i++) {
		GWInfo gwInfo = gwInfos.at(i);
		FWMapInfo fwi;
		//-> Find corresponding fwmapinfo
		for(unsigned int k = 0 ; k < fwis.size(); k++) {
			fwi = fwis.at(k);
			if(fwi.lgwtype.compare(gwInfo.lgwtype) == 0) {
				performFirmwareUpdate(fwi, gwInfo, manualMode);
				break;
			}				
		}
	}
	
	return 0;
}

string LGWFirmwareUpdate::help()
{
	string usage("update-lgw-firmware (C) 2013 - eQ-3 Entwicklung GmbH\n");
	usage.append("Usage:\n");
	usage.append("update-lgw-firmware <-u firmwarefile> [-k Aes-Key] (<-h IPAddress> | <-s Serial>) [-f] [-console] [-l LogLevel]\n");
	usage.append("\t-u: Path to firmware file\n");
	usage.append("\t-h: IP Address of lan gateway. (Alternatively use -s Serial)\n");
	usage.append("\t-s: Serial number of lan gateway. (Alternatively use -h)\n");
	usage.append("\t-k: Aes-Key (hexadecimal); Needed if encryption is enabled\n");
	usage.append("or\n");
	usage.append("update-lgw-firmware <-c rfd-/hs485d-configfile> <-m fwmap-file>\n\n");
	usage.append("\t-c Path to rfd.conf/hs485d.conf\n");
	usage.append("\t-m Path to fwmap file\n\n");
	usage.append("\t-console: Log to console instead of syslog.\n");
	usage.append("\t-l: Loglevel.\n");
	return usage;
}


void LGWFirmwareUpdate::printUsage()
{
	LOG(Logger::LOG_ERROR, "%s", help().c_str());
	printf("%s\n", help().c_str());
}

int LGWFirmwareUpdate::determineIPAddress(LanDeviceUtils& ldUtils, LanDevice& lgw, const std::string& serial, std::string& ipAddress)
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
			RuntimeIPConfiguration runtimeConfig;
			runtimeConfig.setIPAddress(ipAddress);
			lgw.setRuntimeIPConfiguration(runtimeConfig);
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

int LGWFirmwareUpdate::performFirmwareUpdate(const FWMapInfo& fwmapInfo, const GWInfo& gwInfo, const bool manualMode)
{

	LOG(Logger::LOG_INFO, "LAN Gateway Firmware Update...\n");
	LOG(Logger::LOG_INFO, "Gateway %s", gwInfo.serial.c_str());
	unsigned char* key = NULL;
	int keyLength = 0;
	string ipAddress = gwInfo.ip;
	string updateFilePath = fwmapInfo.filename;	
	if(!manualMode) {
		updateFilePath.insert(0, "/firmware/");
	}
	
	string keyStr = gwInfo.key;
	if(!keyStr.empty()) {
		keyStr = calculateMD5(keyStr);
	}

	//If we got the ip, try to connect. Otherwise search for device by serial
	LanDeviceUtils ldUtils;
	LanDevice lgw;
	int retVal = determineIPAddress(ldUtils, lgw, gwInfo.serial, ipAddress);
	if(retVal != 0) {
		if(key != NULL) { delete[] key; }
		return 4;
	}

	//Check gateway type
	LOG(Logger::LOG_INFO, "Gateway type is %s", lgw.getType().c_str());
	const std::string lgwBidcosType = pLGWAllocation->deviceTypeToBidcosType(lgw.getType());
	if((!manualMode) && lgwBidcosType.compare(gwInfo.lgwtype) != 0) {
		LOG(Logger::LOG_ERROR, "Wrong type of gateway. Should be %s, but found %s", gwInfo.lgwtype.c_str(), lgwBidcosType.c_str());
		return 10;
	} 

	//Encryption enabled?
	bool done = ldUtils.readNetworkConfiguration(lgw);
	if(!done) {
		LOG(Logger::LOG_ERROR, "Error reading network configuration.\n");
		if(key != NULL) { delete[] key; }
		return 5;
	}
	if(lgw.getIPConfiguration().isCryptEnabled()) {
		if(keyStr.empty()) {
			LOG(Logger::LOG_ERROR, "Please provide aes key.\n");
			if(key != NULL) { delete[] key; }
			return 6;
		}
		//charArrayFromHexString(keyStr, &key, keyLength);
		key = new unsigned char[keyStr.size()];
		memcpy(key, keyStr.c_str(), keyStr.size()); 
		keyLength = keyStr.size();
		lgw.setAesKey(key, keyLength);
	}
	if(!manualMode) {
		std::string lgwFirmwareVersion = lgw.getFirmwareVersion();
		LOG(Logger::LOG_INFO, "Available Firmware Version:   %s\n" , fwmapInfo.version.c_str());
		LOG(Logger::LOG_INFO, "Lan Gateway Firmware Version: %s\n" , lgw.getFirmwareVersion().c_str());
		//We have to be able to downgrade (i.g. when a user downgrades it's ccu2 firmware)... that's why we just do a string comparison.
		//if( compareFirmwareVersion(fwmapInfo.version, lgwFirmwareVersion) <= 0) {
		if(fwmapInfo.version.compare(lgwFirmwareVersion) == 0) {
			LOG(Logger::LOG_INFO, "Firmware is up to date\n");
			if(key != NULL) { delete[] key; }
			return 0;
		}
	}
	//Send device into bootloader.
	done = ldUtils.enterBootloader(lgw);
	if(!done) {
		LOG(Logger::LOG_ERROR, "Could not enter bootloader.\n");
		if(key != NULL) { delete[] key; }
		return 7;
	}

	//Ensure that we have correct ip address - it may has been changed after going into bootloader.
	done = ldUtils.readRuntimeNetworkConfiguration(lgw);
	if(!done) {
		LOG(Logger::LOG_ERROR, "Cannot determine IP Address after starting gateway bootloader.\n");
		//try to start app
		ldUtils.enterApplication(lgw);
		waitForApp(ldUtils, lgw, gwInfo.serial);
		if(key != NULL) { delete[] key; }
		return 8;
	}

	//Do the update
	LOG(Logger::LOG_INFO, "Updating firmware....\n");
	done = ldUtils.doFirmwareUpdate(lgw, updateFilePath);
	if(!done) {
		LOG(Logger::LOG_ERROR, "Error updating firmware.\n");
		ldUtils.enterApplication(lgw);
		waitForApp(ldUtils, lgw, gwInfo.serial);
		if(key != NULL) { delete[] key; }
		return 9;
	}
	else {
		LOG(Logger::LOG_INFO, "Update performed. Waiting for gateway to get ready.");
		waitForApp(ldUtils, lgw, gwInfo.serial);
	}
	if(key != NULL) { delete[] key; }
	return 0;
}

/*
int LGWFirmwareUpdate::compareFirmwareVersion(const std::string& a, const std::string&b)
{
	int retVal = 0;
	vector<int> va;
	vector<int> vb;
	//parse a
	char* temp = new char[a.size()];
	strncpy(temp, a.c_str(), a.size());
	char* pChar = strtok(temp, ".");
	while(pChar != NULL) {
		if(pChar == NULL) {
			break;
		}
		int i;
		std::stringstream ss;
		ss << pChar;
		ss >> i;
		va.push_back(i);
		pChar = strtok(NULL, ".");
	}
	delete[] temp;
	//parse b	
	temp = new char[a.size()];
	strncpy(temp, b.c_str(), b.size());
	pChar = strtok(temp, ".");
	while(pChar != NULL) {
		if(pChar == NULL) {
			break;
		}
		int i;
		std::stringstream ss;
		ss << pChar;
		ss >> i;
		vb.push_back(i);
		pChar = strtok(NULL, ".");
	}
	delete[] temp;
	//compare a with b
	for(unsigned int i = 0; ((i < va.size()) && (i < vb.size())) ; i++) {
		const int a = va.at(i);
		const int b = vb.at(i);
		if(a > b) {//a greater than b
			return 1;
		}
		else if(a < b) {
			return -1;//a smaller than b
		}
		else if(((i+1) < va.size()) && ((i+1) == vb.size()) ) {
			return 1;//all previous are equal, but b has at least one more token
		}
		else if( ((i+1) < vb.size()) && ((i+1) == va.size()) ) {
			return -1;
		}
	}
	return 0;//all tokens are equal
	
}
*/
std::string LGWFirmwareUpdate::calculateMD5(const std::string& s)
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

bool LGWFirmwareUpdate::readConfigFile(const std::string& confFilePath, std::vector<GWInfo>& gwInfos)
{
	std::vector<std::string> bidcosTypes = pLGWAllocation->getBidcosTypes(); 
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
            std::string type = configData.GetStringValue("Type", "");
          	GWInfo gwInfo;
          	for(unsigned int i = 0; i < bidcosTypes.size(); i++) {
          		if(type.compare(bidcosTypes.at(i)) == 0) {
          			gwInfo.lgwtype = bidcosTypes.at(i);
          		}
          		else {
          			continue;
          			}
          	
/*		        if( type.compare("HMWLGW") == 0 ) {
		        	gwInfo.lgwtype = HMWLGW;
		        }
		        else if(type.compare("HMLGW2") == 0) {
		        	gwInfo.lgwtype = HMLGW2;
		        }
		        else {
		        	continue;//next section
		        }*/
		    	std::string serial = configData.GetStringValue("Serial Number", "");
		    	if(serial.empty()) {
		    		continue; 
		    	}
		    	std::string ip = configData.GetStringValue("IP Address", "");
		    	std::string key = configData.GetStringValue("Encryption Key", "");
		    	gwInfo.serial = serial;
		    	gwInfo.key = key;
		    	gwInfo.ip = ip;
		    	gwInfos.push_back(gwInfo);
	    	}//for
        }
    }
    return true;
}

bool LGWFirmwareUpdate::readFWMapFile(const std::string& fwMapFilePath, std::vector<FWMapInfo>& fwis)
{
	ifstream fwMapFile;
	fwMapFile.open(fwMapFilePath.c_str());
	if (!fwMapFile.is_open()) {
		LOG(Logger::LOG_ERROR, "Unable to open file %s",fwMapFilePath.c_str());
		return false;
	}
	//read and parse file
	string line;
	
	std::vector<std::string> fwmapTypes = pLGWAllocation->getFWMapTypes();
	while(fwMapFile.good()) {
		line.clear();
		getline(fwMapFile, line);
		if(!line.empty()) {
			//find one of supported types
			trim(line);
			if(line.empty()) {//Skip empty lines
				continue;
			}
			if(line.at(0) == '#') {//skip comments
				continue;
			}
			char* lineChars = new char[line.size()+1];
			strncpy(lineChars, line.c_str(), line.size());
			lineChars[line.size()] = '\0';
			char* pChar = strtok(lineChars, WSCHARS);
			for(unsigned int i = 0; i < fwmapTypes.size() ; i++) {
				if(strcmp(pChar, fwmapTypes.at(i).c_str()) == 0) {
					FWMapInfo info;
					info.lgwtype = pLGWAllocation->fwmapTypeToBidcosType(fwmapTypes.at(i)) ;//HMWLGW;
					if(parseFWMapLine(info)) {
						fwis.push_back(info);
					}
				}
			}
			//else if ... another gateway					
			delete[] lineChars;
		}				
	}
	fwMapFile.close();
	return true;
}

bool LGWFirmwareUpdate::parseFWMapLine(FWMapInfo& info)
{
	//Get filename
	char* pChar;
	pChar = strtok(NULL, WSCHARS);
	if(pChar == NULL) { return false; }
	string temp(pChar);
	trim(temp);
	if(temp.empty()) { return false; }
	info.filename = temp;
	temp.clear();
	//Read version
	pChar = strtok(NULL, WSCHARS);
	if(pChar == NULL) { return false; }
	temp.clear();
	temp.append(pChar);
	trim(temp);
	info.version = temp;
	return true;
}

void LGWFirmwareUpdate::trim(string& str)
{
	std::string::size_type index = str.find_last_not_of(WSCHARS);
	if((index != string::npos) && ((index+1) < str.size())) {
		str.erase(index+1);
	}
	index = str.find_first_not_of(WSCHARS);
	if(index != string::npos) {
		str.erase(0, index);
	}
}

void LGWFirmwareUpdate::waitForApp(LDU::LanDeviceUtils& ldUtils, LDU::LanDevice& ld, const std::string& serial)
{
	bool ready = false;
	const std::string lh("127.0.0.1");
	for(unsigned int i = 0; i < 2; i++) {
		bool done = ldUtils.searchDeviceBySerial(serial, ld);
		if(done) {
			done = ldUtils.readRuntimeNetworkConfiguration(ld);
			if(done) {
				if(lh.compare(ld.getRuntimeIPConfiguration().getIPAddress()) != 0) {
					ready = true;
					break;// interrupt waiting
				}
			}
		}
		usleep(1000);//wait a sec
	}
	if(!ready) {
		LOG(Logger::LOG_ERROR, "Gateway not ready after firmware update.");		
	}
}

/*
void LGWFirmwareUpdate::charArrayFromHexString(const std::string& hexStr, unsigned char** charArray, int& charArrayLength)
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
//printf("%x ", temp);
		(*charArray)[c] = (unsigned char)(temp & 0xFF);
		c++;
	}
}
*/
