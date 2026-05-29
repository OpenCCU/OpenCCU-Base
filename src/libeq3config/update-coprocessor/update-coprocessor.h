/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _COPROCESSORUPDATE_H_
#define _COPROCESSORUPDATE_H_

#include <Command.h>

#include <CCU2CoprocessorCommandMod.h>
#include <CCU2PortWrapperMod.h>
#include <vector>

namespace LDU {
	class LanDevice;
	class LanDeviceUtils;
};

namespace HM2Mod {
	class CCU2CommControllerMod;
	class CCU2CoprocessorCommandMod;
	class CCU2PortWrapperMod;
	class CCU2SerialFrameMod;
};
	
enum UpdateCommand
{
	CommandUpdate,
	CommandGetVersion,
	CommandGetAvaiableVersion,
	CommandStartBl,
	CommandStartApp,
	CommandGetCoproSerialNumber,
	CommandGetCoproSGTIN,
	CommandNone,
	CommandInvalid
};

class CoprocessorUpdate : public Command
{
public:
	CoprocessorUpdate();

	virtual int execute();
	virtual std::string help();
	
private:
	struct FirmwareFrame
	{
		int index;
		int length;
		unsigned char* data;
	};
	
	//! Holds information about gateway from rfd.conf 
	struct GWInfo {
		//LGWTypes lgwtype;
		std::string serial;
		std::string ip;
		std::string key;
	};
	
	enum CoproType {
		CCU2,
		HM_MOD_UART,
		CCU2_RFLGW
	};

	std::string accessFile;
	std::string resetFile;
	std::string firmwareDir;

	CoproType coproType;

	bool startBootloader(HM2Mod::CCU2CommControllerMod* pPortWrapper);
	bool startApplication(HM2Mod::CCU2CommControllerMod* pPortWrapper);
	bool getApplicationVersion(HM2Mod::CCU2CommControllerMod* pPortWrapper, std::string* version);
	//bool sendSystemCommand(HM2::CCU2CommController* pPortWrapper, const HM2::SystemCommand systemCommand, const std::string& cmdData, std::string* pResponseValue);
	void printUsage();
	void charArrayFromHexString(const std::string& hexStr, unsigned char** charArray, int& charArrayLength);//users must delete charArray pointer
	int determineIPAddress(const std::string& serial, std::string& ipAddress, int& tcpPort, std::string& keyStr);	
	int readFilenameMap(const std::string& fwmapFilePath, const CoproType desiredCoproType, char* firmwareFileName, char* availableFirmwareVersion);
		
	static bool readFirmwareFile(const char* file, std::vector<FirmwareFrame>* frameList);
	static unsigned char ConvertHexStringToByte(char high, char low);
	static unsigned char ConvertHexCharToByte(const char value);
	std::string calculateMD5(const std::string& s);
	bool performGatewayCoproUpdate(const std::string& rfdconfPath, const char* firmwareFileName, const char* availableFirmwareVersion, const bool skipStartApp);
	bool readRFDConfigFile(const std::string& confFilePath, std::vector<GWInfo>& gwInfos);
	std::string getCoproSerialNumber(HM2Mod::CCU2CommControllerMod* pPortWrapper);
	std::string getCoproSGTIN(HM2Mod::CCU2CommControllerMod* pPortWrapper);
};

#endif /* _COPROCESSORUPDATE_H_ */
