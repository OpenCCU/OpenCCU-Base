/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _COPROCESSORTEST_H_
#define _COPROCESSORTEST_H_

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
	
class CoprocessorTest : public Command
{
public:
	CoprocessorTest();

	virtual int execute();
	virtual std::string help();
	
private:
	
	enum CoproType {
		CCU2,
		HM_MOD_UART
	};

	std::string accessFile;
	std::string resetFile;

	CoproType coproType;

	bool startBootloader(HM2Mod::CCU2CommControllerMod* pPortWrapper);


	//bool sendSystemCommand(HM2::CCU2CommController* pPortWrapper, const HM2::SystemCommand systemCommand, const std::string& cmdData, std::string* pResponseValue);
	void printUsage();
	void charArrayFromHexString(const std::string& hexStr, unsigned char** charArray, int& charArrayLength);//users must delete charArray pointer
	static unsigned char ConvertHexStringToByte(char high, char low);
	static unsigned char ConvertHexCharToByte(const char value);
};

#endif /* _COPROCESSORTEST_H_ */
