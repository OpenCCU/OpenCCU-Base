/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <typedefs.h>

#include <stdio.h>
#include <string.h>

#include <sstream>
#include <iomanip>
#include <stdio.h>


namespace HM2 {

	std::string convertBidcosAddressToBigEndianString(int bidcosAddress) {
		std::string addrStr;
		char result[3];
		for(int i = 2; i >= 0; i--) {
			int value = bidcosAddress & 0xff;
			result[i] = (char)value;
			bidcosAddress >>= 8;
		}
		addrStr.append(result, 3);
		return addrStr;
	}

	int convertBidEndianStringToBidcosAddress(const std::string& addrStr) {
		int value = 0;
		for (int i = 0; i < 3; i++) {
			value <<= 8;
			value |= (addrStr.at(i) & 0xff);
		}

		return value;
	}

	std::string toBigEndianString(int value) {
		std::string str;
		char result[4];
		for(int i = 3; i >= 0 ; i--) {
			int v = value & 0xff;
			result[i] = (char)v;
			value >>= 8;
		}
		str.append(result, 4);
		return str;
	}
	
	unsigned int convertBigEndianStringToUnsignedInt(const std::string& uintStr)
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

	std::string convertUnsignedIntToBigEndianString(unsigned int ui)
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

	std::string toDebugHexStr(const std::string& str) 
	{
		std::string hexStr;
		char* buffer = new char[9];
		for(unsigned int i = 0; i < str.size(); i++) {
			memset(buffer, 0, 9);
			snprintf(buffer, 9, "%02x", (((unsigned int)str.at(i)) & 0x000000FF));
			hexStr.append(buffer);
			hexStr.append(" ");
		}
		delete[] buffer;
		return hexStr;
	}
	
	std::string toDebugHexStr(const unsigned int ui)
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

	uint64 toUInt64(const std::string& str) 
	{
		uint64 value = 0;
		if(str.size() == 8) {
			char* pChar = (char*)&value;
			for(int i = 0; i < 8; i++) {
				*(pChar+i) = str.at(7-i);
			}
		}
		return value;
	}

	std::string toHexString(const std::string& str)
	{
		std::string out;
		std::stringstream ss;
		ss << std::hex << std::setfill('0');
		for(unsigned int i = 0; i < str.size(); i++) {
			ss << std::setw(2) << (((unsigned int)str.at(i)) & 0x000000FF);
		}
		ss >> out;
		return out;
	}
}
