/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemIdentify.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemIdentify, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_Identify);
/// @endcond


HmLegacyFrameSystemIdentify::HmLegacyFrameSystemIdentify(void) : HmLegacyFrameSystem( FrameType_Identify )
{
}


HmLegacyFrameSystemIdentify::~HmLegacyFrameSystemIdentify(void)
{
}

std::string HmLegacyFrameSystemIdentify::GetIdentification()const
{
	return Data().GetStringValue( 0 );
}

void HmLegacyFrameSystemIdentify::SetIdentification(const std::string& identification)
{
	Data().SetStringValue( 0, identification );
}

std::string HmLegacyFrameSystemIdentify::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmSystem Identify ";
	s += GetIdentification();
	return s;
}
