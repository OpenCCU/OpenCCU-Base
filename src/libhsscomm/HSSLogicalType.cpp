/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalType.cpp: Implementierung der Klasse HSSLogicalType.
//
//////////////////////////////////////////////////////////////////////

#include "HSSLogicalType.h"
#include <Logger.h>

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSLogicalType::HSSLogicalType()
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalType::HSSLogicalType() this=%p", this);
	use_default_on_failure=false;
}

HSSLogicalType::~HSSLogicalType()
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalType::~HSSLogicalType() this=%p", this);
}

bool HSSLogicalType::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalType::InitFromXml()");
    const char* temp=node.getAttribute("type");
    if(!temp)return false;
    type=temp;

    temp=node.getAttribute("unit");
    if(temp)unit=temp;

    temp=node.getAttribute("use_default_on_failure");
    if(temp)use_default_on_failure=temp[0]=='t';
    return true;
}

const std::string& HSSLogicalType::GetType()
{
    return type;
}

bool HSSLogicalType::GetDescription(XmlRpc::XmlRpcValue* val)
{
    try{
		(*val)["UNIT"]=unit;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}
