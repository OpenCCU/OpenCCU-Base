/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionIntegerTinyfloat.cpp: Implementierung der Klasse HSSTypeConversionIntegerTinyfloat.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionIntegerTinyfloat.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionIntegerTinyfloat> HSSTypeConversionIntegerTinyfloatFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionIntegerTinyfloat::HSSTypeConversionIntegerTinyfloat()
{
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionIntegerTinyfloat::HSSTypeConversionIntegerTinyfloat() this=%p", this);
}

HSSTypeConversionIntegerTinyfloat::~HSSTypeConversionIntegerTinyfloat()
{
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionIntegerTinyfloat::~HSSTypeConversionIntegerTinyfloat() this=%p", this);
}

bool HSSTypeConversionIntegerTinyfloat::CheckCreationTag(const char *tag)
{
    if(strcmp("type_conversion_integer_tinyfloat", tag)==0)return true;
    return false;
}

bool HSSTypeConversionIntegerTinyfloat::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp=node.getAttribute("mantissa_size");
	if(!temp)return false;
	mantissa_size=strtoul(temp, NULL, 0);

	temp=node.getAttribute("exponent_size");
	if(!temp)return false;
	exponent_size=strtoul(temp, NULL, 0);

	temp=node.getAttribute("mantissa_start");
	if(temp){
		mantissa_start=strtoul(temp, NULL, 0);
	}else{
		mantissa_start=0;
	}

	temp=node.getAttribute("exponent_start");
	if(temp){
		exponent_start=strtoul(temp, NULL, 0);
	}else{
		exponent_start=mantissa_start+mantissa_size;
	}
	return true;
}

bool HSSTypeConversionIntegerTinyfloat::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
		uint32_t mantissa=(int&)in;
		uint32_t test_mask=0xffffffff<<mantissa_size;
		int exponent;
		for(exponent=0;exponent<32-mantissa_size;exponent++){
			if(!(mantissa&test_mask))break;
			mantissa>>=1;
		}
		(int&)*out=(exponent<<exponent_start)|(mantissa<<mantissa_start);
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

bool HSSTypeConversionIntegerTinyfloat::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
		int mantissa=1;
		if(mantissa_size)mantissa=(((int&)in)>>mantissa_start)&(0xffff>>(16-mantissa_size));
		int exponent=(((int&)in)>>exponent_start)&(0xffff>>(16-exponent_size));
        (int&)*out=mantissa<<exponent;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}
