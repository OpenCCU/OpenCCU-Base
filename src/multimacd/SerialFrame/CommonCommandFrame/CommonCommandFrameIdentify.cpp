/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CommonCommandFrameIdentify.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, CommonCommandFrameIdentify, int> m((SerialFrame::FrameSubsystemType_Common<<8)|CommonCommandFrame::FrameType_Identify);
/// @endcond

CommonCommandFrameIdentify::CommonCommandFrameIdentify(void) : CommonCommandFrame( FrameType_Identify )
{
}


CommonCommandFrameIdentify::~CommonCommandFrameIdentify(void)
{
}

std::string CommonCommandFrameIdentify::GetIdentification()const
{
	return Data().GetStringValue( 0 );
}
void CommonCommandFrameIdentify::SetIdentification(const std::string& identification)
{
	Data().SetStringValue( 0, identification );
}

std::string CommonCommandFrameIdentify::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " COMMON Identify ";
	s += GetIdentification();
	return s;
}

