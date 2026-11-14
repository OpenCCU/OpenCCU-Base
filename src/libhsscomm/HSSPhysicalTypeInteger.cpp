/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalTypeInteger.cpp: Implementierung der Klasse HSSPhysicalTypeInteger.
//
//////////////////////////////////////////////////////////////////////

#include "HSSPhysicalTypeInteger.h"
#include <Logger.h>
#include "type_registry.h"

using namespace XmlRpc;

static hsscomm::type_registry::factory<HSSPhysicalTypeInteger> HSSPhysicalTypeIntegerFactory;


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSPhysicalTypeInteger::HSSPhysicalTypeInteger()
{
}

HSSPhysicalTypeInteger::~HSSPhysicalTypeInteger()
{
//    LOG(Logger::LOG_DEBUG, "HSSPhysicalTypeInteger::~HSSPhysicalTypeInteger() this=%p", this);
}

bool HSSPhysicalTypeInteger::CheckCreationTag(const char *tag)
{
    if(strcmp("physical_type_integer", tag)==0)return true;
    return false;
}
