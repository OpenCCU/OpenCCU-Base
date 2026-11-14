/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <UnifiedLanProtocolTypeConverter.h>

#include <sstream>
#include <iostream>
#include <iomanip>

int hexStringToInt(const std::string& hexStr)
{
	unsigned int i = 0;
	std::stringstream ss;
	ss << std::hex << hexStr;
	ss >> i;
	return (int)i;
}

unsigned int hexStringToUInt(const std::string& hexStr)
{
	unsigned int i = 0;
	std::stringstream ss;
	ss << std::hex << hexStr;
	ss >> i;
	return i;
}

unsigned char hexStringToUChar(const std::string& hexStr)
{
	unsigned int i = (char)0x00;
	std::stringstream ss;
	ss << std::hex << hexStr;
	ss >> i;
	return (unsigned char)(i & 0xFF);
}

std::string binaryDataToHexStr(const std::string& binaryData)
{
	std::string out;
	std::stringstream ss;
	ss << std::hex << binaryData;
	ss >> out;
	return out;
}

std::string intToHexStr(const int i)
{
	std::string out;
	std::stringstream ss;
	ss << std::hex << (unsigned int)i;
	ss >> out;
	return out;
}

std::string ucharToHexStr(const unsigned char uc)
{
	std::string out;
	std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)uc;
	ss >> out;
	return out;
}
