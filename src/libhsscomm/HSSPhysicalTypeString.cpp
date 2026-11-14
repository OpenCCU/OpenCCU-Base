/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalTypeString.cpp: Implementierung der Klasse HSSPhysicalTypeString.
//
//////////////////////////////////////////////////////////////////////

#include "HSSPhysicalTypeString.h"
#include <Logger.h>
#include "type_registry.h"

using namespace XmlRpc;

static hsscomm::type_registry::factory<HSSPhysicalTypeString> HSSPhysicalTypeStringFactory;


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSPhysicalTypeString::HSSPhysicalTypeString()
{
}

HSSPhysicalTypeString::~HSSPhysicalTypeString()
{
//    LOG(Logger::LOG_DEBUG, "HSSPhysicalTypeString::~HSSPhysicalTypeString() this=%p", this);
}

bool HSSPhysicalTypeString::CheckCreationTag(const char *tag)
{
    if(strcmp("physical_type_string", tag)==0)return true;
    return false;
}
