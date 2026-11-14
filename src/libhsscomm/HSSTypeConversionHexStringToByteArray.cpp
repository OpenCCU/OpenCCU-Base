/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * HSSTypeConversionHexStringToByteArray.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: user
 */

#include "HSSTypeConversionHexStringToByteArray.h"
#include <Logger.h>

#include <string.h>
#include <iomanip>
#include <sstream>
#include "type_registry.h"

using namespace XmlRpc;

static hsscomm::type_registry::factory<HSSTypeConversionHexStringToByteArray> HSSTypeConversionHexStringToByteArrayFactory;

HSSTypeConversionHexStringToByteArray::HSSTypeConversionHexStringToByteArray()
: HSSTypeConversion()
, has0xPrefix(true)
, delimiter(',')
{
}

HSSTypeConversionHexStringToByteArray::~HSSTypeConversionHexStringToByteArray()
{
}

bool HSSTypeConversionHexStringToByteArray::CheckCreationTag(const char *tag)
{
  return (strcmp("type_conversion_hexstring_bytearray", tag) == 0);
}

bool HSSTypeConversionHexStringToByteArray::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp=node.getAttribute("has0XPrefix");
	if(temp) {
		const std::string s = temp;
		if(s.compare("false") == 0) {
			has0xPrefix = false;
		}
	}
	temp=node.getAttribute("delimiter");
	if(temp) {
		const std::string s = temp;
		if(s.size() == 1) {
			delimiter = s.at(0);
		}
	}
	return true;
}

bool HSSTypeConversionHexStringToByteArray::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
	if(in.size() == 0) {
		return false;
	}
	if (in.getType() != XmlRpcValue::TypeString)
	{
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): in is not string");
		return false;
	}

	const std::string inStr = (std::string) in;
	std::string token;
	int offset = 0;
	std::string outStr;
	if(has0xPrefix) {
		outStr.reserve(inStr.size() / 4 + 1);
	}
	else {
		outStr.reserve(inStr.size() / 2 + 1);
	}

	while(getNextToken(inStr, token, offset)) {
		trim(token);
		unsigned char value;
		if(!fromHexValue(token, value)) {
			LOG(Logger::LOG_ERROR, "HSSTypeConversionHexStringToByteArray::LogicalToPhysical(): Conversion failed.");
			*out = std::string("");
			return false;
		}
		outStr.append(1, value);
	}
	*out = outStr;
	return true;
}

bool HSSTypeConversionHexStringToByteArray::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
	const std::string inStr = in;
	if(in.size() == 0) {
		return false;
	}
	if(in.getType() != XmlRpcValue::TypeString)
	{
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::PhysicalToLogical(): in is not string");
		return false;
	}
	const std::string prefix( has0xPrefix ? "0x" : "" );
	std::string outStr;
	for(unsigned int c = 0; c < inStr.size(); c++) {
		std::string tempStr;
		toHexString(inStr.at(c), prefix, tempStr);
		outStr.append(tempStr);
		outStr.append(1, delimiter);
	}
	if(outStr.size() > 0) {//erase last delimiter
		outStr.erase(outStr.size()-1);
	}
	*out=outStr;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen
//////////////////////////////////////////////////////////////////////
bool HSSTypeConversionHexStringToByteArray::getNextToken(const std::string& inStr, std::string& token, int& offset)
{
	token.clear();
	if(offset < 0) {
		return false;
	}
	std::string::size_type delimiterIndex = inStr.find(delimiter, offset);
	if(delimiterIndex == std::string::npos) {//last token?
		if((unsigned int)offset < inStr.size()) {
			token = inStr.substr(offset, inStr.size()-offset);//last token
			offset = -1;
			return true;
		}
		else {//no more tokens
			offset = -1;
			return false;
		}
	}
	else {//next token
		token = inStr.substr(offset, delimiterIndex-offset);//delimiter character exclusive
		offset = delimiterIndex+1;
		return true;
	}
}

bool HSSTypeConversionHexStringToByteArray::fromHexValue(const std::string& hexValue, unsigned char& value)
{
	value = 0;
	if(has0xPrefix) {
		if(hexValue.size() != 4) {
			LOG(Logger::LOG_ERROR, "HSSTypeConversionHexStringToByteArray::fromHexValue(): Cannot parse hex-string value (wrong size).");
			return false;
		}
	}
	else if(hexValue.size() != 2) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionHexStringToByteArray::fromHexValue(): Cannot parse hex-string value (wrong size).");
		return false;
	}
	std::stringstream ss;
	ss << std::hex;
	ss << hexValue;
	int temp = 0;
	ss >> temp;
	value = (unsigned char)temp &  0xFF;
	return true;
}

void HSSTypeConversionHexStringToByteArray::toHexString(const unsigned char& value, const std::string& prefix, std::string& hexStr)
{
	std::stringstream ss;
	ss << prefix;
	ss << std::uppercase << std::hex << std::setfill('0');
	ss << std::setw(2) << (((unsigned int)value) & 0x000000FF);
	ss >> hexStr;
}

void HSSTypeConversionHexStringToByteArray::trim(std::string& str)
{
	while(!str.empty()) {
		if(isspace(str.at(0))) {
			str.erase(0, 1);
		}
		else {
			break;
		}
	}
	while(!str.empty()) {
		if(isspace(str.at(str.size()-1))) {
			str.erase(str.size()-1, 1);
		}
		else {
			break;
		}
	}
}
