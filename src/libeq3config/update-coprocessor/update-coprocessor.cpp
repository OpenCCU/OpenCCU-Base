/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "update-coprocessor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

#include <LanDeviceUtils.h>


#include <CCU2CoprocessorCommandMod.h>
#include <CCU2LGWCommControllerMod.h>
#include <CCU2SerialPortCommControllerMod.h>
#include <CCU2CommControllerMod.h>
//#include <Logger.h>
#include <SyslogLogger.h>
#include <ConsoleLogger.h>
#include <md5.h>
#include <PropertyMap.h>
//#include <fstream>
//#include <iostream>

#include <HM2UtilsMod.h>
#ifndef WIN32
 #include <unistd.h>
#endif

using namespace std;
using namespace HM2Mod;

const unsigned char SEND_UPDATEFRAME_COMMAND = 0x05;

CoprocessorUpdate::CoprocessorUpdate() : Command("update-coprocessor")
, accessFile("/dev/null") //default
, resetFile("/dev/ccu2-ic200") //default
, firmwareDir("/firmware/") //default
, coproType(CCU2)//default
{
}

std::string CoprocessorUpdate::help()
{
	std::string usage("update-coprocessor\n");
	usage.append("Usage:\n");
	usage.append("update-coprocessor (<-p port> | <-s serial> [-k Aes-Key]) (-u | -v | -av | -bl | -app | -se | -sg) [-c] [-l LOGLEVEL] [-d FIRMWARE-DIRECTORY] [-t CoprocessorType]\n");
	usage.append("or\n");
	usage.append("update-coprocessor -lgw -u ( <-rfdconf rfd.conf> | <-s serial> <-k Aes-Key> [-c] [-l LOGLEVEL]) [-d FIRMWARE-DIRECTORY]\n");
	usage.append("\t-p: Serial port of CCU2/HM-MOD-UART coprocessor (Alternatively use -s)\n");
	usage.append("\t-s: Serial number of lan gateway. (Alternatively use -p)\n");
	usage.append("\t-k: Aes-Key (passphrase) of lan gateway; Needed if encryption is enabled\n");
	usage.append("\t-u: Do Firmwareupdate if necessary\n");
	usage.append("\t-f: Force update and ignore version\n");
	usage.append("\t-v: Get Version\n");
	usage.append("\t-av: Get avaiable Version\n");
	usage.append("\t-bl: Start Bootloader\n");
	usage.append("\t-app: Start Application\n");
	usage.append("\t-se: Get coprocessor serial number.\n");
	usage.append("\t-sg: Get coprocessor SGTIN if applicable.\n");
	usage.append("\t-lgw: Update target is a LAN gateway.\n");
	usage.append("\t-rfdconf: Path to rfd.conf file. (Used when -lgw is supplied.)\n");
	usage.append("\t-c: Log to console instead of syslog.\n");
	usage.append("\t-l: Loglevel.\n");
	usage.append("\t-d: Path to firmware directory which contains firware files and fwmap file. Default is /firmware.\n");
	usage.append("\t-t: Coprocessor type: CCU2 or HM-MOD-UART. Default ist CCU2.");
	return usage;
}

int CoprocessorUpdate::execute()
{
	HM2Mod::CCU2CommControllerMod* pPortWrapper = NULL;
	std::string port;
	std::string serial;
	std::string keyStr;
	bool doupdate = false;
	UpdateCommand selectedCommand = CommandNone;
	bool updateTargetLGW = false;
	std::string rfdconfPath;
	bool logToConsole = false;
	int logLevel = 2;
	bool skipStartApp = false;

	const int paramsSize = static_cast<int>(params.size());
	for(int i = 0; i < paramsSize ; i++) 
	{
		if((params.at(i).compare("-p") == 0) && ((i+1)<paramsSize) ) {
			i++;
			port.assign(params.at(i));
		}
		else if((params.at(i).compare("-s") == 0) && ((i+1)<paramsSize) ) {
			i++;
			serial.assign(params.at(i));
			coproType = CCU2_RFLGW;
		}
		else if((params.at(i).compare("-k") == 0) && ((i+1)<paramsSize) ) {
			i++;
			keyStr = params.at(i);
		}
		else if((params.at(i).compare("-u") == 0)) {
			selectedCommand = CommandUpdate;
		}
		else if((params.at(i).compare("-f") == 0)) {
			doupdate = true;
			skipStartApp = true;
		}
		else if((params.at(i).compare("-v") == 0)) {
			selectedCommand = CommandGetVersion;
		}
		else if((params.at(i).compare("-av") == 0)) {
			selectedCommand = CommandGetAvaiableVersion;
		}
		else if((params.at(i).compare("-bl") == 0)) {
			selectedCommand = CommandStartBl;
		}
		else if((params.at(i).compare("-app") == 0)) {
			selectedCommand = CommandStartApp;
		}
		else if((params.at(i).compare("-lgw") == 0)) {
			updateTargetLGW = true;
			coproType = CCU2_RFLGW;
		}
		else if((params.at(i).compare("-rfdconf") == 0) && ((i+1)<paramsSize)) {
			i++;
			rfdconfPath = params.at(i);
		}
		else if((params.at(i).compare("-c") == 0)) {
			logToConsole = true;
		}
		else if((params.at(i).compare("-l") == 0) && ((i+1)<paramsSize)) {
			i++;
			sscanf(params.at(i).c_str(), "%d", &logLevel);
		}
		else if((params.at(i).compare("-d") == 0) && ((i+1)<paramsSize)) {
			i++;
			firmwareDir = params.at(i);
			if( (firmwareDir.size() > 0) && (firmwareDir.at(firmwareDir.size()-1) != '/')) {
				firmwareDir.append(1, '/');
			}
		}
		else if((params.at(i).compare("-se") == 0)) {
			selectedCommand = CommandGetCoproSerialNumber;
		}
		else if((params.at(i).compare("-sg") == 0)) {
			selectedCommand = CommandGetCoproSGTIN;
		}
		else if((params.at(i).compare("-t") == 0)) {
			if((i+1) < paramsSize) {
				i++;
				std::string typeStr = params.at(i);
				if(typeStr.compare("CCU2")) {
					coproType = CCU2;
				}
				else if(typeStr.compare("HM-MOD-UART")) {
					coproType = HM_MOD_UART;
				}
				else {
					printUsage();
					return 1;
				}
			}
			else {
				printUsage();
				return 1;
			}
		}

	}

	//Parse args
	if(params.size() < 3 || selectedCommand == CommandNone) {
		printUsage();
		return 1;
	}

	if(logToConsole) {
		logger = new ConsoleLogger();		
	}
	else {
		logger = new SyslogLogger("update-coprocessor");
	}
	logger->SetLevel((Logger::LogLevel)logLevel);
	
	std::string fwmapFilePath(firmwareDir);
	fwmapFilePath.append("fwmap");


	char firmwareFileName[1024];
	char availableFirmwareVersion[64];

	memset(&firmwareFileName, 0, 1024);
	memset(&availableFirmwareVersion, 0 ,64);

	if(this->readFilenameMap(fwmapFilePath, coproType, (char*)&firmwareFileName, (char*)&availableFirmwareVersion) == 0)
	{	
		LOG(Logger::LOG_ERROR, "Error reading firmware map.\n");
		if((selectedCommand == CommandGetAvaiableVersion) || (selectedCommand == CommandUpdate)) { 
			return 2;
		}
		else {
			firmwareFileName[0] = '\0';
			availableFirmwareVersion[0] = '\0';
		}
	}
	else {
		LOG(Logger::LOG_DEBUG, "firmware filename is: %s\n", firmwareFileName);
	}
	
	if (selectedCommand == CommandGetAvaiableVersion) {
		LOG(Logger::LOG_INFO, "%s\n", availableFirmwareVersion);
		return 0;
	}
	
	if(updateTargetLGW) {
		if(!rfdconfPath.empty()) {
			performGatewayCoproUpdate(rfdconfPath, firmwareFileName, availableFirmwareVersion, skipStartApp);
		}
		else if(serial.empty()) {
			LOG(Logger::LOG_ERROR, "Wether -rfdconf path, nor -s supplied\n");
			printUsage();
			return 1;
		}
	}
	else {	
		//keyStr = calculateMD5(keyStr);
	
		if(!serial.empty())
		{
			// Lan Gateway Coprocessor
			// search for device by serial
			std::string ipAddress;
			int tcpPort;
			// Determine IP and start application if necessary
			int retVal = determineIPAddress(serial, ipAddress, tcpPort, keyStr);
			if(retVal != 0) {
				return retVal;
			}		
		
			pPortWrapper = new HM2Mod::CCU2LGWCommControllerMod();
			bool connected = ((HM2Mod::CCU2LGWCommControllerMod*)pPortWrapper)->init(ipAddress, tcpPort, keyStr, serial, skipStartApp);
			//bool connected = ((HM2Mod::LGWPortWrapper*)pPortWrapper)->connect(ipAddress, tcpPort, keyStr, serial);
			if(!connected)
			{
				if(!pPortWrapper->isDeviceOpen()) {//It could be that the connection was established but the copro application could not be started...
					delete pPortWrapper;
					LOG(Logger::LOG_ERROR, "Could not connect to Lan Gateway ip: %s.\n", ipAddress.c_str());
					return 2;
				}
			}		
		}
		else if(!port.empty())
		{
			// CCU2 Coprocessor
			pPortWrapper = new HM2Mod::CCU2SerialPortCommControllerMod();
			//Open connection to coprocessor via serial comport.
			((HM2Mod::CCU2SerialPortCommControllerMod*)pPortWrapper)->init(port, true);
			//removed initialization check -> we must flash copro app if we cannot start it!
		}
		else
		{
			LOG(Logger::LOG_ERROR, "Serial Port nor Lan Gateway serial number is given.\n");
			printUsage();
			return 4;
		}	
	
//		LOG(Logger::LOG_DEBUG, "State 1: %d", (int)pPortWrapper->getCoprocessorState());
//		this->startApplication(pPortWrapper);
//		LOG(Logger::LOG_DEBUG, "State 1: %d", (int)pPortWrapper->getCoprocessorState());
		//initCoprocessorState(pPortWrapper);
	
		std::string version;
		std::vector<FirmwareFrame> frameList;
		std::string data;
		std::string resetFileData;
		std::string coproSerialNumber;
		std::string firmwareFilePath(firmwareDir);
		firmwareFilePath.append(firmwareFileName);
		switch (selectedCommand)
		{
			case CommandUpdate:
				// read firmware file
				if (!readFirmwareFile(firmwareFilePath.c_str(), &frameList))
				{
					LOG(Logger::LOG_ERROR, "Error reading firmware.\n");
					if(pPortWrapper != NULL) {
						delete pPortWrapper;
					}
					return 2;
				}

				if(!doupdate)
				{
					if(this->getApplicationVersion(pPortWrapper, &version))
					{
						if(version.compare(availableFirmwareVersion) != 0) {
							LOG(Logger::LOG_INFO, "Update necessary, installed: %s, avaiable %s\n", version.c_str(), availableFirmwareVersion);
							doupdate = true;
						}
					}
					else {
						LOG(Logger::LOG_ERROR, "Error reading firmware version. Enforce update.\n");
						doupdate = true;
					}
				}
			
				if(doupdate)
				{
					//Proceed with update process
					if(skipStartApp) {
						//don't do anything right now. maybe one day send identify						
					}
					else if(!this->startBootloader(pPortWrapper))
					{
						if(pPortWrapper != NULL) {
							delete pPortWrapper;
						}
						return 5;
					}
					LOG(Logger::LOG_DEBUG," deliver firmware...");
					// Do update
					for(int i = 0; i < (int)frameList.size(); i++)
					{
						data.clear();
						data.append((char*)frameList.at(i).data, frameList.at(i).length - 2); // -2 because of original crc
					
						if(!pPortWrapper->sendSystemCommand((HM2Mod::SystemCommand)SEND_UPDATEFRAME_COMMAND, data, NULL))
						{
							if(skipStartApp) {
								this->startBootloader(pPortWrapper);
								usleep(500000);
								i = -1;
								skipStartApp = false;
								continue;
							}
							LOG(Logger::LOG_DEBUG, "Firmwareupdate not successfull, at frame %d of %d\n", (i + 1), frameList.size());
							if(pPortWrapper != NULL) {
								delete pPortWrapper;
							}
							return 8;
						}
					}
				
					LOG(Logger::LOG_INFO, "Firmwareupdate successfull\n");
				}
				else
				{
					LOG(Logger::LOG_INFO, "No update necessary\n");				
				}
			
				break;
			case CommandGetVersion:
				if(!this->getApplicationVersion(pPortWrapper, &version))
				{
					LOG(Logger::LOG_ERROR, "Error reading version.\n");
					if(pPortWrapper != NULL) {
						delete pPortWrapper;
					}
					return 7;
				}
			
				break;
			case CommandStartBl:
				if(!this->startBootloader(pPortWrapper))
				{
					if(pPortWrapper != NULL) {
						delete pPortWrapper;
					}
					return 5;
				}
			
				break;
			case CommandStartApp:
				if(!this->startApplication(pPortWrapper))
				{
					if(pPortWrapper != NULL) {
						delete pPortWrapper;
					}
					return 6;
				}
			
				break;
			case CommandGetCoproSerialNumber:
				coproSerialNumber = this->getCoproSerialNumber(pPortWrapper);
				if(coproSerialNumber.empty()) {
					return 9;
				}
				else {
					LOG(Logger::LOG_INFO, "SerialNumber: %s", coproSerialNumber.c_str());
				}
				break;
			case CommandGetCoproSGTIN:
				coproSerialNumber = this->getCoproSGTIN(pPortWrapper);
				if(coproSerialNumber.empty()) {
					return 10;
				}
				else {
					LOG(Logger::LOG_INFO, "SGTIN: %s", toHexString(coproSerialNumber).c_str());
				}
				break;
			default:
				printUsage();
				break;
		}
		if(pPortWrapper != NULL) {
			delete pPortWrapper;
		}
	}//else 
	// Cleanup & return
	return 0;
}

void CoprocessorUpdate::printUsage()
{
	LOG(Logger::LOG_ERROR, "%s", help().c_str());
	bool logToConsole = (dynamic_cast<ConsoleLogger*>(Logger::s_logger) != NULL);
	if(!logToConsole) {
		printf("%s\n", help().c_str());
	}
}

std::string CoprocessorUpdate::getCoproSerialNumber(HM2Mod::CCU2CommControllerMod* pPortWrapper)
{
	if(this->startApplication(pPortWrapper)) {
		usleep(50000);
		std::string response;
		if(pPortWrapper->isDualCoprocessor()) {
			pPortWrapper->sendLowLevelMacCommand(LOWLEVELMAC_COMMAND_GET_SERIAL_NUMBER, &response);
		}
		else {
			pPortWrapper->sendSystemCommand(SYSTEMCMD_GET_SERIALNR, "", &response);
		}
		if(response.size() == 0) {
			LOG(Logger::LOG_ERROR, "Error retrieving serial number from coprocessor.");
			return std::string("");
		}
		else {
			return response;
		}
	}
	return std::string("");
}


std::string CoprocessorUpdate::getCoproSGTIN(CCU2CommControllerMod* pPortWrapper)
{
	if(this->startApplication(pPortWrapper)) {
		usleep(50000);
		std::string response;
		if(pPortWrapper->isDualCoprocessor()) {
			pPortWrapper->sendHmipCommonCommand(HMIP_COMMON_GET_SGTIN, std::string(""), &response);
			if(response.empty()) {
				LOG(Logger::LOG_ERROR, "Error retrieving SGTIN from coprocessor.");
			}
			return response;
		}
		else {
			LOG(Logger::LOG_INFO, "Coprocessor does not have a SGTIN.");
			return std::string("");
		}
	}
	return std::string("");
}

bool CoprocessorUpdate::performGatewayCoproUpdate(const std::string& rfdconfPath, const char* firmwareFileName, const char* availableFirmwareVersion, const bool skipStartApp) {
//Read rfd config file
	std::vector<GWInfo> gwInfos;
	bool done = readRFDConfigFile(rfdconfPath, gwInfos);
	if(!done) {
		LOG(Logger::LOG_DEBUG, "No updatable gateways found in rfd.conf file.");
		return false;
	}
//for each configured RF Lan Gateway (Second Version) -> perform the coprocessor update
	for(unsigned int i = 0; i < gwInfos.size(); i++) {
		LOG(Logger::LOG_INFO, "performGatewayCoproUpdate(): updating %s\n", gwInfos.at(i).serial.c_str());
		GWInfo gwInfo = gwInfos.at(i);
		//Find device by ip or serial number
		int tcpPort;
		determineIPAddress(gwInfo.serial, gwInfo.ip, tcpPort, gwInfo.key);
		//HM2Mod::LGWPortWrapper* portWrapper = new HM2Mod::LGWPortWrapper();
		//std::string key = calculateMD5(gwInfo.key);
		CCU2LGWCommControllerMod* commController = new CCU2LGWCommControllerMod();
		bool connected = commController->init(gwInfo.ip, tcpPort, gwInfo.key, gwInfo.serial, true);

		//bool connected = portWrapper->connect(gwInfo.ip, tcpPort, key, gwInfo.serial);
		if(!connected)
		{
			LOG(Logger::LOG_ERROR, "Could not connect to Lan Gateway ip: %s.\n", gwInfo.ip.c_str());
			//delete portWrapper;
			continue;
		}
	//perform the update
		// read firmware file
		std::vector<FirmwareFrame> frameList;
		std::string data;
		std::string firmwareFilePath(firmwareDir);
		firmwareFilePath.append(firmwareFileName);
		if (!readFirmwareFile(firmwareFilePath.c_str(), &frameList)) {
			LOG(Logger::LOG_ERROR, "Error reading firmware.\n");
			//delete portWrapper;
			continue;
		}

		//initCoprocessorState(commController);
		//check if update is necessary
		std::string version;
		if(!skipStartApp) {
			if(this->getApplicationVersion(commController, &version)) {

				if(version.compare(availableFirmwareVersion) != 0) {
					LOG(Logger::LOG_INFO, "Update necessary, installed: %s, avaiable %s\n", version.c_str(), availableFirmwareVersion);
				}
				else {
					LOG(Logger::LOG_INFO, "Coprocessor firmware not necessary.\n");
					//delete portWrapper;
					continue;
				}
			}
			else {
				LOG(Logger::LOG_ERROR, "Error reading version. Enforce update.\n");
			}
			//Start bootloader and do the update
			if(!this->startBootloader(commController))
			{
				//delete portWrapper;
				continue;
			}
		}

		LOG(Logger::LOG_DEBUG, "Starting update...");
		std::string r;
		bool _skipStartApp = skipStartApp;
		bool updateSuccess = true;
		for(unsigned int i = 0; i < frameList.size(); i++)
		{
			data.clear();
			data.append((char*)frameList.at(i).data, frameList.at(i).length - 2); // -2 because of original crc
		
			if(!commController->sendSystemCommand((HM2Mod::SystemCommand)SEND_UPDATEFRAME_COMMAND, data, &r))
			{
				if(_skipStartApp) {
					this->startBootloader(commController);
					usleep(500000);
					i = -1;
					_skipStartApp = false;
					continue;
				}
				LOG(Logger::LOG_ERROR, "Firmwareupdate not successfull, at frame %d of %d\n", (i + 1), frameList.size());
				//delete portWrapper;
				updateSuccess = false;
				break;
			}
		}
		if(updateSuccess) {
			LOG(Logger::LOG_INFO, "Firmwareupdate successfull\n");
		}
	}
	return true;
}

bool CoprocessorUpdate::readRFDConfigFile(const std::string& confFilePath, std::vector<GWInfo>& gwInfos)
{
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
            if(type.compare("HMLGW2") != 0) {
            	continue;//next section
            }
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
        }
    }
    return true;
}

bool CoprocessorUpdate::startBootloader(HM2Mod::CCU2CommControllerMod* pPortWrapper)
{
	LOG(Logger::LOG_ALL, "CoprocessorUpdate::startBootloader()");
	std::string data;
	std::string response;

	if(pPortWrapper->startCoprocessorBootloader())
	{
		LOG(Logger::LOG_DEBUG, "CoprocessorUpdate::startBootloader():Coprocessor entered bootloader.");
	}
	else
	{
		LOG(Logger::LOG_ERROR, "CoprocessorUpdate::startBootloader():Could not start Coprocessor bootloader.\n");
			return false;
	}
	return true;
}

bool CoprocessorUpdate::startApplication(HM2Mod::CCU2CommControllerMod* pPortWrapper)
{
	std::string data;
	std::string response;
	LOG(Logger::LOG_ALL, "CoprocessorUpdate::startApplication()");
	//LOG(Logger::LOG_ALL, "startApplication(): Send identify");
	if(pPortWrapper->startCoprocessorApp(true))
	{
		LOG(Logger::LOG_DEBUG, "CoprocessorUpdate::startApplication():Coprocessor entered application.");
	}
	else
	{
		LOG(Logger::LOG_ERROR, "CoprocessorUpdate::startApplication():Could not start Coprocessor application.\n");
			return false;
	}
	return true;
}

bool CoprocessorUpdate::getApplicationVersion(HM2Mod::CCU2CommControllerMod* pPortWrapper, std::string* pVersion)
{
	LOG(Logger::LOG_ALL, "getApplicationVersion()");
	if(this->startApplication(pPortWrapper))
	{
		usleep(500000);
		std::string data;
		std::string response;
		int start = 0;
		int end = 0;
		if(pPortWrapper->isDualCoprocessor())
		{
			pPortWrapper->sendHmipTrxAdapterCommand(HMIP_TRXADAPTER_Get_VERSION,data,&response);
			start = 0;
			end = 3;
		}
		else
		{
			pPortWrapper->sendSystemCommand(SYSTEMCMD_GETVERSION, data, &response);
			start = 3;
			end = 6;
		}
		pVersion->clear();
		//check for size
		if(response.size() < (unsigned int)end) {
			LOG(Logger::LOG_ERROR, "Error retrieving application version from coprocessor.");
			LOG(Logger::LOG_ALL, "Received %d bytes", response.size());
			return false;
		}
		char* versionBuffer = new char[response.size()+1];
		for(int i = start; i < end; i++)
		{
			snprintf(versionBuffer, response.size()+1, "%d", (int)response.at(i));
			pVersion->append(versionBuffer);
			if(i < end-1)
			{
				pVersion->append( 1, '.');			
			}
		}
		
		LOG(Logger::LOG_INFO, "Version: %s\n", pVersion->c_str());
		delete[] versionBuffer;
		return true;		
	}
	else
	{
		pVersion->append("0.0.0");
		LOG(Logger::LOG_ERROR, "Could not start Application, maybe no application on device, do update with dummy Version: %s\n", pVersion->c_str());
		return true;
	}
}

int CoprocessorUpdate::determineIPAddress(const std::string& serial, std::string& ipAddress, int& tcpPort, std::string& keyStr)
{
	LDU::LanDeviceUtils ldUtils;
	LDU::LanDevice lgw;
	unsigned char* key = NULL;
	int keyLength = 0;
	if(serial.empty()) 
	{
		LOG(Logger::LOG_ERROR, "Please provide a serial number.\n");
		printUsage();
		return 0x12;
	}
	if(serial.empty()) {
		LOG(Logger::LOG_ERROR, "Please provide IP-Address or serial number.\n");
		LOG(Logger::LOG_ERROR, "%s", help().c_str());
		return 2;
	}
	if(!ipAddress.empty()) {//try unicast and if that fails, fallback to search by serial
		std::vector<std::string> devTypeFilters;
		devTypeFilters.push_back(std::string("*"));
		std::vector<LDU::LanDevice> devices;
		ldUtils.searchDevicesByType(devTypeFilters, LDU::PROTOCOL_EQ3CONFIG, LDU::ROUTINGSCHEME_UNICAST, ipAddress, std::string(""), devices);
		bool done = true;
		if(devices.size() > 0) {
			lgw = devices.at(0);
			LDU::RuntimeIPConfiguration runtimeConfig;
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
	
	// Encryption enabled?
	done = ldUtils.readNetworkConfiguration(lgw);
	if(!done) 
	{
		LOG(Logger::LOG_ERROR, "Error reading network configuration.\n");
		return 0x15;
	}

	if(lgw.getIPConfiguration().isCryptEnabled()) 
	{
		if(keyStr.empty()) {
			LOG(Logger::LOG_ERROR, "Please provide aes key.\n");
			if(key != NULL) { delete[] key; }
			return 0x16;
		}
	
		//charArrayFromHexString(keyStr, &key, keyLength);
		lgw.setAesKey(key, keyLength);
	}
	
	// Check if bootloader is started or application
	if(lgw.getType().find("Bl") != std::string::npos)
	{
		done = ldUtils.enterApplication(lgw);
		if(!done) {
			LOG(Logger::LOG_ERROR, "Could not enter application.\n");
			if(key != NULL) { delete[] key; }
			return  0x17;
		}
		
		done = ldUtils.searchDeviceBySerial(serial, lgw);
		if(!done) 
		{
			LOG(Logger::LOG_ERROR, "Could not find Lan Gateway using serial number %s after application start\n",serial.c_str());
			if(key != NULL) { delete[] key; }
			return  0x13;
		}
		
		done = ldUtils.readRuntimeNetworkConfiguration(lgw);
		if(!done) 
		{
			LOG(Logger::LOG_ERROR, "Could not determine IP Address of Lan Gateway with serial number %s after application start\n", serial.c_str());
			if(key != NULL) { delete[] key; }
			return  0x14;
		}
	}
	
	if(key != NULL) { delete[] key; }
	ipAddress = lgw.getRuntimeIPConfiguration().getIPAddress();
	
	std::vector<LDU::LanDevice::ServiceProtocol> sps = lgw.getServiceProtocols();
	for(unsigned int j = 0; j < sps.size(); j++)
	{
		if(sps.at(j).id == 2)
		{
			tcpPort = sps.at(j).port;
		}
	}	
	
	return 0;
}

void CoprocessorUpdate::charArrayFromHexString(const std::string& hexStr, unsigned char** charArray, int& charArrayLength)
{
	unsigned int temp;
	int c = 0;
	if((hexStr.size() % 2) != 0) 
	{
		charArrayLength = 0;
		charArray = NULL;
		return;
	}
	
	charArrayLength = hexStr.size() / 2;
	*charArray = new unsigned char[charArrayLength];
	for(unsigned int i = 0; i < hexStr.size(); i+=2) 
	{
		temp = 0;
		std::stringstream ss;
		ss << std::hex << hexStr.substr(i, 2);
		ss >> temp;
		(*charArray)[c] = (unsigned char)(temp & 0xFF);
		c++;
	}
}

int CoprocessorUpdate::readFilenameMap(const std::string& fwmapFilePath, const CoproType desiredCoproType,  char* firmwareFileName, char* availableFirmwareVersion)
{
	FILE* f;
	char buffer[256];
	f = fopen(fwmapFilePath.c_str(), "r");

	if (!f) 
	{
		LOG(Logger::LOG_ERROR, "unable to open file %s", fwmapFilePath.c_str());
		return 0;
	}
	std::string desiredTypeStr;
	switch(desiredCoproType) {
		case CCU2:
			desiredTypeStr = "CCU2";
			break;
		case HM_MOD_UART:
			desiredTypeStr = "HM-MOD-UART";
			break;
		case CCU2_RFLGW:
			desiredTypeStr = "CCU2-RFLGW";
			break;
		default:
			desiredTypeStr = "CCU2";
			break;
	}
	
	while (fgets(buffer, 256, f)) {
		char* type = strtok(buffer, " \t\n\r");
		//skip comments and empty lines
		if (!type || *type == '#' || *type != 'C')
		{
			continue;
		}
		
		char* filename = strtok(NULL, " \t\n\r#");
		if (!filename || *filename < ' ')
		{
			continue;
		}
		
		char* version = strtok(NULL, " \t\n\r#");
		if (!version || *version < ' ')
		{
			continue;
		}
		
		if (strstr(filename, ".eq3") == NULL )
		{
			continue;
		}
		
		if (strcmp(desiredTypeStr.c_str(), type) != 0)
		{
			continue;
		}
		
		strcat(firmwareFileName, filename);
		strcpy(availableFirmwareVersion, version);
		fclose(f);
		return 1;
	}
	
	fclose(f);
	return 0;
}

bool CoprocessorUpdate::readFirmwareFile(const char* file, std::vector<FirmwareFrame>* frameList)
{
	int size = 0;
	// check if firmware file exists
	std::ifstream fileStream;
	fileStream.open(file, std::ifstream::in); 
	if (!fileStream.good())
	{
		LOG(Logger::LOG_ERROR, "error: firmware file not found.\n");
		return false;
	}

	// read file content
	char* temp = new char[ 2 ];
	fileStream.seekg(0, std::ios::end);
	size = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	int index = 0;
	int counter = 0;
	while (counter < size)
	{
		FirmwareFrame tempFrame;
		tempFrame.index = index++;
		tempFrame.length = 0;
		fileStream.read( temp, 2 );
		tempFrame.length = ConvertHexStringToByte(temp[0], temp[1]) * 256;
		fileStream.read( temp, 2 );
		tempFrame.length += ConvertHexStringToByte(temp[0], temp[1]);
		tempFrame.data = new unsigned char[tempFrame.length];
		counter += (tempFrame.length + 2) * 2; // datalength + 2 length
		for(int i = 0; i < tempFrame.length; i++)
		{
			fileStream.read( temp, 2 );
			tempFrame.data[i] = ConvertHexStringToByte(temp[0], temp[1]);
		}
		
		frameList->push_back(tempFrame);
	}

	return true;
}

unsigned char CoprocessorUpdate::ConvertHexStringToByte(char high, char low)
{
	unsigned char highvalue;
	unsigned char lowvalue;
	unsigned char result;
	
	highvalue = ConvertHexCharToByte(high);
	lowvalue = ConvertHexCharToByte(low);	
	
	result = highvalue * 16 + lowvalue;
	return result;
}

unsigned char CoprocessorUpdate::ConvertHexCharToByte(const char value)
{
	unsigned char byte = value;
	if(value >= 'a')
	{
		byte = value - 'a' + 10;
	}
	else if(value >= 'A')
	{
		byte = value - 'A' + 10;
	}
	else if(value != ':')
	{	
		byte = value - '0';
	}
	
	return byte;
}

std::string CoprocessorUpdate::calculateMD5(const std::string& s)
{
	md5 md5_calculator;
	unsigned char* buffer = new unsigned char[s.length()];
	memcpy(buffer, s.c_str(), s.length());
	md5_calculator.Update(buffer, s.length());
	md5_calculator.Finalize();
	delete[] buffer;
	std::string digest;
	digest.append((const char*) md5_calculator.Digest(), std::string::size_type(16));
	return digest;
}
