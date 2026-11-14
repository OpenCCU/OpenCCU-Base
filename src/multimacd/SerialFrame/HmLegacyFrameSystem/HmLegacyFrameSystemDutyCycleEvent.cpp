/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemDutyCycleEvent.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemDutyCycleEvent, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_DutyCycleEvent);
/// @endcond


HmLegacyFrameSystemDutyCycleEvent::HmLegacyFrameSystemDutyCycleEvent(void) : HmLegacyFrameSystem( FrameType_DutyCycleEvent )
{
}


HmLegacyFrameSystemDutyCycleEvent::~HmLegacyFrameSystemDutyCycleEvent(void)
{
}

uint8_t HmLegacyFrameSystemDutyCycleEvent::GetDutyCycle()const
{
	return Data().GetUInt8Value(0);
}

void HmLegacyFrameSystemDutyCycleEvent::SetDutyCycle( uint8_t dutyCycle )
{
	Data().SetUInt8Value( 0, dutyCycle );
}


std::string HmLegacyFrameSystemDutyCycleEvent::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmSystem DutyCycleEvent ";
	s += Data().ToString();
	return s;
}
