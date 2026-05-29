/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionFloatUInt8String.cpp: Implementierung der Klasse HSSTypeConversionFloatUInt8String.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionFloatUInt8String.h"
#include <Logger.h>
#include "type_registry.h"
#include <XmlRpcValue.h>
#include <string>

static hsscomm::type_registry::factory<HSSTypeConversionFloatUInt8String> HSSTypeConversionFloatUInt8StringFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionFloatUInt8String::HSSTypeConversionFloatUInt8String()
{
    factor=1.0;
	offset=0.0;
	lengthBytes=4;
	preShiftBits = 32;
}

HSSTypeConversionFloatUInt8String::~HSSTypeConversionFloatUInt8String()
{

}

bool HSSTypeConversionFloatUInt8String::CheckCreationTag(const char *tag)
{
	if(strcmp("type_conversion_float_uint8_string", tag)==0)return true;
	if(strcmp("type_conversion_float_uint8_string_scale", tag)==0)return true;
    return false;
}

bool HSSTypeConversionFloatUInt8String::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionFloatUInt8String::InitFromXml()");
    const char* temp=node.getAttribute("factor");
    if(temp)factor=strtod(temp, NULL);

	temp=node.getAttribute("length_bytes");
	if(temp) {
		lengthBytes = strtol(temp, NULL, 10);
		preShiftBits = ((8 - lengthBytes) * 8);
	}
	if(lengthBytes < 0 || lengthBytes > 8) {
		LOG(Logger::LOG_ERROR, "HssTypeConversionFloatUInt8String::InitFromXML(): Unsupported length_bytes parameter value");
	}

    temp=node.getAttribute("offset");
    if(temp)offset=strtod(temp, NULL);

    return true;
}

bool HSSTypeConversionFloatUInt8String::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
		double dval=((double&)in+offset)*factor;
		if(dval>=0)dval+=0.5;
		else dval-=0.5;
		uint8 iVal = (uint8)dval;
		std::string s;
		iVal >>= preShiftBits;
		for(unsigned int i = lengthBytes-1;  i >= 0; i--) {
			s.append(1, (char)(iVal & 0xFF));
			iVal >>= 8;
		}
        *out=s;
    }catch(XmlRpcException&){
        return false;
    }
    //LOG(Logger::LOG_DEBUG, "HSSTypeConversionFloatUInt8String::LogicalToPhysical() %s -> %s", in.toText().c_str(), out->toText().c_str());
    return true;
}

bool HSSTypeConversionFloatUInt8String::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
		uint8 iVal = 0;
		const std::string& s = in;
		if(s.size() >= lengthBytes) {
			iVal <<= preShiftBits;
			for(unsigned int i = 0 ; i < lengthBytes; i++) {
				iVal <<= 8;
				iVal |= s.at(i) & 0xFF;				
			}
		}
		((double&)*out)=(((double)iVal)/factor)-offset;
    }catch(XmlRpcException&){
        return false;
    }
    return true;
}
