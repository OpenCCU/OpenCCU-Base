/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LowLevelMacFrameResponseSerialNumber.h"

LowLevelMacFrameResponseSerialNumber::LowLevelMacFrameResponseSerialNumber(void)
{
}


LowLevelMacFrameResponseSerialNumber::~LowLevelMacFrameResponseSerialNumber(void)
{
}

std::string LowLevelMacFrameResponseSerialNumber::GetSerialNumber()const
{
	return Data().GetStringValue( 1 );
}

