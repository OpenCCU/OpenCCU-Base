/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionStringUnsignedInteger.cpp: Implementierung der Klasse HSSTypeConversionStringUnsignedInteger.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionStringUnsignedInteger.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionStringUnsignedInteger> HSSTypeConversionFloatIntegerFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionStringUnsignedInteger::HSSTypeConversionStringUnsignedInteger()
{
}

HSSTypeConversionStringUnsignedInteger::~HSSTypeConversionStringUnsignedInteger()
{

}

bool HSSTypeConversionStringUnsignedInteger::CheckCreationTag(const char *tag)
{
    if(strcmp("type_conversion_string_unsigned_integer", tag)==0)return true;
    return false;
}

bool HSSTypeConversionStringUnsignedInteger::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionStringUnsignedInteger::InitFromXml()");


    return true;
}

bool HSSTypeConversionStringUnsignedInteger::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
		
    try{
			const char* strValue = ((std::string) in).c_str();
			unsigned int uintValue = 0;
			
			if (strValue != NULL)
			{
				while (strValue[0] != '\0') 
				{
					uintValue *= 10;
					uintValue += strValue[0] - '0';
					strValue++;
				}	
			}
		
			((int&)*out)= (int) uintValue;
    }catch(XmlRpcException){
        return false;
    }
		
//    LOG(Logger::LOG_DEBUG, "--- --- --- HSSTypeConversionStringUnsignedInteger::LogicalToPhysical() %s -> %s", in.toText().c_str(), out->toText().c_str());
    return true;
}

bool HSSTypeConversionStringUnsignedInteger::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    return false;
}

