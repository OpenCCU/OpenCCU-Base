/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HM2UTILSMOD_H_
#define _HM2UTILSMOD_H_

#include <typedefs.h>

#include <stdio.h>
#include <string.h>

#include <sstream>
#include <iomanip>
#include <stdio.h>


namespace HM2Mod {
	std::string convertBidcosAddressToBigEndianString(int bidcosAddress);
	int convertBidEndianStringToBidcosAddress(const std::string& addrStr);
	std::string toBigEndianString(int value);
	unsigned int convertBigEndianStringToUnsignedInt(const std::string& uintStr);
	std::string convertUnsignedIntToBigEndianString(unsigned int ui);
	std::string toDebugHexStr(const std::string& str);
	std::string toDebugHexStr(const unsigned int ui);
	uint64 toUInt64(const std::string& str);
	std::string toHexString(const std::string& str);
}

#endif
