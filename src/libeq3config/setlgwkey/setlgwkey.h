/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * setlgwkey.h
 *
 *  Created on: Mar 6, 2013
 *      Author: user
 */

#ifndef _SETLGWKEY_H_
#define _SETLGWKEY_H_

#include <Command.h>

namespace LDU {
	class LanDevice;
	class LanDeviceUtils;
};

class SetLGWKey : public Command
{
public:
	SetLGWKey();

	virtual int execute();
	virtual std::string help();
	
private:
	int determineIPAddress(LDU::LanDeviceUtils& ldUtils, LDU::LanDevice& lgw, const std::string& serial, std::string& ipAddress);
	void charArrayFromHexString(const std::string& hexStr, unsigned char** charArray, int& charArrayLength);//users must delete charArray pointer
	bool updateConfigFile(const std::string& confFilePath, const std::string& serial, const std::string& newKeyStr);
	std::string calculateMD5(const std::string& s);	
};

#endif /* _SETLGWKEY_H_ */
