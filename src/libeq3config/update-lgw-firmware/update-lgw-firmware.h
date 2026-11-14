/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LGWFIRMWAREUPDATE_H_
#define _LGWFIRMWAREUPDATE_H_

#include <Command.h>
#include <vector>

namespace LDU {
	class LanDevice;
	class LanDeviceUtils;
};

class LGWAllocation;

class LGWFirmwareUpdate : public Command
{
public:
	LGWFirmwareUpdate();
	virtual ~LGWFirmwareUpdate();

	virtual int execute();
	virtual std::string help();
	
private:

	enum LGWTypes {
		UNDEFINED,
		HMWLGW,
		HMLGW2
	};

	//! Holds information from fwmap file
	struct FWMapInfo {
		//LGWTypes lgwtype;
		std::string lgwtype;
		std::string filename;
		std::string version;
	};
	
	//! Holds information about gateway from rfd.conf / hs485d.conf
	struct GWInfo {
		//LGWTypes lgwtype;
		std::string lgwtype;
		std::string serial;
		std::string ip;
		std::string key;
	};
	
	//! Holds together information used for update a gateway
	struct UpdateInfo {
		GWInfo gwInfo;
		FWMapInfo fwMapInfo;
	};
	
	LGWAllocation* pLGWAllocation;

	void printUsage();
	int determineIPAddress(LDU::LanDeviceUtils& ldUtils, LDU::LanDevice& lgw, const std::string& serial, std::string& ipAddress);
//	void charArrayFromHexString(const std::string& hexStr, unsigned char** charArray, int& charArrayLength);//users must delete charArray pointer	
	std::string calculateMD5(const std::string& s);
	bool readFWMapFile(const std::string& fwMapFilePath, std::vector<FWMapInfo>& fwis);
	bool parseFWMapLine(FWMapInfo& info);
	
	//! Reads information from rfd.conf / hs485d.conf
	bool readConfigFile(const std::string& confFilePath, std::vector<GWInfo>& gwInfos);
	void trim(std::string& str);
	int performFirmwareUpdate(const FWMapInfo& fwmapInfo, const GWInfo& gwInfo, const bool manualMode);
	//int compareFirmwareVersion(const std::string& a, const std::string&b);
	void waitForApp(LDU::LanDeviceUtils& ldUtils, LDU::LanDevice& ld, const std::string& serial);
};

#endif /* _LGWFIRMWAREUPDATE_H_ */
