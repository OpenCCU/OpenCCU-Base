/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalTypeBoolean.cpp: Implementierung der Klasse HSSPhysicalTypeBoolean.
//
//////////////////////////////////////////////////////////////////////

#include "HSSPhysicalTypeBoolean.h"
#include <Logger.h>
#include "type_registry.h"

using namespace XmlRpc;

static hsscomm::type_registry::factory<HSSPhysicalTypeBoolean> HSSPhysicalTypeBooleanFactory;


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSPhysicalTypeBoolean::HSSPhysicalTypeBoolean()
{
}

HSSPhysicalTypeBoolean::~HSSPhysicalTypeBoolean()
{
//    LOG(Logger::LOG_DEBUG, "HSSPhysicalTypeBoolean::~HSSPhysicalTypeBoolean() this=%p", this);
}

bool HSSPhysicalTypeBoolean::CheckCreationTag(const char *tag)
{
    if(strcmp("physical_type_boolean", tag)==0)return true;
    return false;
}
