/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HMWLGWUTILS_H_
#define _HMWLGWUTILS_H_

#include <stdio.h>
#include <string.h>




//#include <sstream>
//#include <iostream>
//#include <iomanip>

static inline unsigned int convertBigEndianStringToUnsignedInt(const std::string& uintStr)
{
	unsigned int value = 0;
	int end = uintStr.size();
	if(end > 4) {
		end = 4;
	}
	for(int i = 0; i < end; i++)
	{
		value <<= 8;
		value |= (uintStr.at(i) & 0xff);
	}
	return value;
}

static inline std::string convertUnsignedIntToBigEndianString(unsigned int ui)
{

	char result[4];
	for(int i = 3; i >= 0; i--) {
		int value = ui & 0xff;
		result[i] = (char)value;
		ui >>= 8;
	}
	std::string s(result, 4);
	return s;
}

static inline std::string toDebugHexStr(const unsigned int ui)
{
	std::string hexStr;
	char* buffer = new char[9];
	memset(buffer, 0, 9);
	snprintf(buffer, 9, "%02X", ui);
	hexStr.append(buffer);
	hexStr.append(" (hex)");
	delete[] buffer;
	return hexStr;
}

static inline std::string toDebugHexStr(const std::string& str)
{
	std::string hexStr;
	char* buffer = new char[9];
	for(unsigned int i = 0; i < str.size(); i++) {
		memset(buffer, 0, 9);
		snprintf(buffer, 9, "%02X", (((unsigned int)str.at(i)) & 0x000000FF));
		hexStr.append(buffer);
		hexStr.append(" ");
	}
	hexStr.append(" (hex)");
	delete[] buffer;
	return hexStr;
}

#endif // _HMWLGWUTILS_H_
