/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * HSSTypeConversionInt4ToSignedIntX.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: user
 */

#include "HSSTypeConversionInt4ToSignedIntX.h"
#include <Logger.h>
#include "type_registry.h"
#include <stdlib.h>

static hsscomm::type_registry::factory<HSSTypeConversionInt4ToSignedIntX> HSSTypeConversionInt4ToSignedIntXFactory;

HSSTypeConversionInt4ToSignedIntX::HSSTypeConversionInt4ToSignedIntX()
: physicalBytes(4)
{
}

HSSTypeConversionInt4ToSignedIntX::~HSSTypeConversionInt4ToSignedIntX()
{
}

bool HSSTypeConversionInt4ToSignedIntX::InitFromXml(XMLNode& node, XMLNode& root_node)
{
    const char* temp = node.getAttribute("physical_bytes");
	if(temp) {
		physicalBytes = strtoul(temp, NULL, 0);
		if(physicalBytes >= 1 && physicalBytes <= 4) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true;
	}
}

bool HSSTypeConversionInt4ToSignedIntX::CheckCreationTag(const char* tag)
{
    if(strcmp("type_conversion_sint4_sintx", tag)==0) {
    	return true;
    }
    else  {
    	return false;
    }
}

bool HSSTypeConversionInt4ToSignedIntX::PhysicalToLogical(LogicalInstance* inst,
		XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event)
{
	int iOut = 0;
	unsigned int uIn = (unsigned int)((int)in);
	unsigned int mask = (1 << ((8*physicalBytes)-1));
	const bool negative = (uIn & mask) == mask;

	//set value
	iOut = uIn;
	//fill up with 0xff if negative
	if(negative) {
		mask = 0;
		for(int i = 4 - physicalBytes; i > 0; i--) {
			mask = mask >> 8;
			mask |= 0xFF000000;
		}
		iOut |= mask;
	}
	*out = iOut;
	return true;
}

bool HSSTypeConversionInt4ToSignedIntX::LogicalToPhysical(LogicalInstance* inst,
		XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out)
{
	int iOut = (int&)in;
	int mask = 0;
	for(unsigned int i = 0; i < physicalBytes; i++) {
		mask |= ((int)0xFF) << (8*i);
	}
	iOut = iOut & mask;
	*out = iOut;
	return true;
}
