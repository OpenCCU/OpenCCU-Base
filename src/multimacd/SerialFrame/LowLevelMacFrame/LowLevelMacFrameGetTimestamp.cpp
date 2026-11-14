/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LowLevelMacFrameGetTimestamp.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, LowLevelMacFrameGetTimestamp, int> m((SerialFrame::FrameSubsystemType_LowLevelMac<<8)|LowLevelMacFrame::FrameType_GetTimestamp);
/// @endcond



LowLevelMacFrameGetTimestamp::LowLevelMacFrameGetTimestamp(void) : LowLevelMacFrame( FrameType_GetTimestamp )
{
}


LowLevelMacFrameGetTimestamp::~LowLevelMacFrameGetTimestamp(void)
{
}

std::string LowLevelMacFrameGetTimestamp::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " LLMAC GetTimestamp";
	return s;
}

