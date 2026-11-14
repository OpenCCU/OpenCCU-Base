/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LowLevelMacFrameRxModeDefault.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, LowLevelMacFrameRxModeDefault, int> m((SerialFrame::FrameSubsystemType_LowLevelMac<<8)|LowLevelMacFrame::FrameType_RxModeDefault);
/// @endcond



LowLevelMacFrameRxModeDefault::LowLevelMacFrameRxModeDefault(void) : LowLevelMacFrame( FrameType_RxModeDefault )
{
}


LowLevelMacFrameRxModeDefault::~LowLevelMacFrameRxModeDefault(void)
{
}

std::string LowLevelMacFrameRxModeDefault::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " LLMAC UnlockRxMode ";
	return s;
}

