/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemSetTime.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemSetTime, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_SetTime);
/// @endcond


HmLegacyFrameSystemSetTime::HmLegacyFrameSystemSetTime(void) : HmLegacyFrameSystem( FrameType_SetTime )
{
}


HmLegacyFrameSystemSetTime::~HmLegacyFrameSystemSetTime(void)
{
}

uint32_t HmLegacyFrameSystemSetTime::GetSeconds()const
{
	return Data().GetUInt32Value( 0 );
}

void HmLegacyFrameSystemSetTime::SetSeconds( uint32_t seconds )
{
	Data().SetUInt32Value( 0, seconds );
}

int8_t HmLegacyFrameSystemSetTime::GetTimezone()const
{
	return (int8_t)Data().GetUInt8Value(4);
}

void HmLegacyFrameSystemSetTime::SetTimezone( int8_t timezone )
{
	Data().SetUInt8Value( 4, (uint8_t)timezone );
}

std::string HmLegacyFrameSystemSetTime::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos SetTime ";
	s += Data().ToString();
	return s;
}
