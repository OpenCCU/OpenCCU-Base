/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionScript.cpp: Implementierung der Klasse HSSTypeConversionScript.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionCCRTDNParty.h"
#include <cstdlib>
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionCCRTDNParty> HSSTypeConversionCCRTDNPartyFactory;

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Definitionen
//////////////////////////////////////////////////////////////////////

#define BITMASK_LS_7_BIT (0x7F)
#define BITMASK_LS_6_BIT (0x3F)
#define BITMASK_LS_5_BIT (0x1F)
#define BITMASK_LS_4_BIT (0x0F)

#define BITMASK_MS_4_BIT (0xF0)

//////////////////////////////////////////////////////////////////////
// Private Datenstrukturen
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen (Prototypen)
//////////////////////////////////////////////////////////////////////

//! Parst eine Ganzzahl
static unsigned int parseIntStrToUInt(const std::string& intStr);

//! Parst eine Gleitpunktzahl und konvertiert zu einer Ganzzahl)
static double parseDoubleStrToDouble(const std::string& doubleStr);


//Find next token in inStr. Seperator is ','. Returns false if no more tokens. 
//Sets offset to -1 if last token was found.
//Sets offset behind 
static bool getNextToken(const std::string& inStr, std::string& token, int& offset);

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionCCRTDNParty::HSSTypeConversionCCRTDNParty()
{
}

HSSTypeConversionCCRTDNParty::~HSSTypeConversionCCRTDNParty()
{
}

//////////////////////////////////////////////////////////////////////
// Methoden
//////////////////////////////////////////////////////////////////////

bool HSSTypeConversionCCRTDNParty::CheckCreationTag(const char *tag)
{
  return (strcmp("type_conversion_ccrtdn_party", tag) == 0);
}

bool HSSTypeConversionCCRTDNParty::InitFromXml(XMLNode &node, XMLNode &root_node)
{	
	return true;
}

bool HSSTypeConversionCCRTDNParty::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
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
	unsigned char byte = 0;
	unsigned char* outData = new unsigned char[9];
	memset(outData, 0, 9);
	outData[8] = '\0';

	//<parameter type="integer" index="11.0" size="0.6" param="PARTY_TEMPERATURE"/>
	//float integer scale factor 2
	bool gotToken = getNextToken(inStr, token, offset);
	if(!gotToken) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): Too few arguments (0).");		
		delete[] outData;
		return false;
	}
	double d = parseDoubleStrToDouble(token); 
	d = (d * (double)2);
	if(d < (double)0.0) {
		d *= -1;
	}
	unsigned int uintValue = (int)(d + (double)0.5);
	byte = (unsigned char) (uintValue & BITMASK_LS_6_BIT);
	outData[0] |= byte;
	
	//<parameter type="integer" index="12.0" size="0.6" param="PARTY_START_TIME"/>
	//<conversion type="integer_integer_scale" div="30"/>	
	gotToken = getNextToken(inStr, token, offset);
	if(!gotToken) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): Too few arguments (1).");		
		delete[] outData;
		return false;
	}
	uintValue = parseIntStrToUInt(token);
	uintValue = uintValue / 30;
	byte = (unsigned char) (uintValue & BITMASK_LS_6_BIT);
	outData[1] |= byte;
	
	//<parameter type="integer" index="13.0" size="0.5" param="PARTY_START_DAY"/>
	//no conversion
	gotToken = getNextToken(inStr, token, offset);
	if(!gotToken) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): Too few arguments (2).");	
		delete[] outData;
		return false;
	}
	uintValue = parseIntStrToUInt(token);
	byte = (unsigned char) (uintValue & BITMASK_LS_5_BIT);
	outData[2] |= byte;
    
	//<parameter type="integer" index="18.4" size="0.4" param="PARTY_START_MONTH"/>
	//no conversion
	gotToken = getNextToken(inStr, token, offset);
	if(!gotToken) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): Too few arguments (3).");		
		delete[] outData;
		return false;
	}
	uintValue = parseIntStrToUInt(token);
	byte = (unsigned char) ( (uintValue << 4) & BITMASK_MS_4_BIT);
	outData[7] |= byte;

    //<parameter type="integer" index="14.0" size="0.7" param="PARTY_START_YEAR"/>            
	gotToken = getNextToken(inStr, token, offset);
	if(!gotToken) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): Too few arguments (4).");		
		delete[] outData;
		return false;
	}
	uintValue = parseIntStrToUInt(token);
	byte = (unsigned char) (uintValue & BITMASK_LS_7_BIT);
	outData[3] |= byte;

    //<parameter type="integer" index="15.0" size="0.6" param="PARTY_STOP_TIME"/>
    //<conversion type="integer_integer_scale" div="30"/>	
	gotToken = getNextToken(inStr, token, offset);
	if(!gotToken) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): Too few arguments (5).");		
		delete[] outData;
		return false;
	}
	uintValue = parseIntStrToUInt(token);
	uintValue = uintValue / 30;
	byte = (unsigned char) (uintValue & BITMASK_LS_6_BIT);
	outData[4] |= byte;

    //<parameter type="integer" index="16.0" size="0.5" param="PARTY_STOP_DAY"/>
	//no conversion
	gotToken = getNextToken(inStr, token, offset);
	if(!gotToken) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): Too few arguments (6).");		
		delete[] outData;
		return false;
	}
	uintValue = parseIntStrToUInt(token);
	byte = (unsigned char) (uintValue & BITMASK_LS_5_BIT);
	outData[5] |= byte;
	
    //<parameter type="integer" index="18.0" size="0.4" param="PARTY_STOP_MONTH"/>
	//no conversion
	gotToken = getNextToken(inStr, token, offset);
	if(!gotToken) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): Too few arguments (7).");		
		delete[] outData;
		return false;
	}
	uintValue = parseIntStrToUInt(token);
	byte = (unsigned char) (uintValue & BITMASK_LS_4_BIT);
	outData[7] |= byte;

    //<parameter type="integer" index="17.0" size="0.7" param="PARTY_STOP_YEAR"/>
	//no conversion
	gotToken = getNextToken(inStr, token, offset);
	if(!gotToken) {
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCCRTDNParty::LogicalToPhysical(): Too few arguments (8).");		
		delete[] outData;
		return false;
	}
	uintValue = parseIntStrToUInt(token);
	byte = (unsigned char) (uintValue & BITMASK_LS_7_BIT);
	outData[6] |= byte;

	//clean up and return
	*out = std::string((const char*)outData, 9);
	delete[] outData;
	return true;
}

bool HSSTypeConversionCCRTDNParty::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
	*out=in;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen
//////////////////////////////////////////////////////////////////////
static bool getNextToken(const std::string& inStr, std::string& token, int& offset)
{
	token.clear();
	if(offset < 0) {
		return false;
	}
	std::string::size_type separatorIndex = inStr.find(',', offset);
	if(separatorIndex == std::string::npos) {
		if((unsigned int)offset < inStr.size()) {
			token = inStr.substr(offset, inStr.size()-offset);
			offset = -1;
			return true;
		}
		else {
			offset = -1;
			return false;
		}			
	}
	else {
		token = inStr.substr(offset, separatorIndex-offset);//seperator exclusive
		offset = separatorIndex+1;
		return true;
	}
}

static unsigned int parseIntStrToUInt(const std::string& intStr)
{
	return strtol(intStr.c_str(), NULL, 10);
}

static double parseDoubleStrToDouble(const std::string& doubleStr)
{
	return strtod(doubleStr.c_str(), NULL);
}
