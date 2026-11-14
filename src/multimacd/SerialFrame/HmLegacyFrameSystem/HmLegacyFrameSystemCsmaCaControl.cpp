/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemCsmaCaControl.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemCsmaCaControl, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_CsmaCaControl);
/// @endcond


HmLegacyFrameSystemCsmaCaControl::HmLegacyFrameSystemCsmaCaControl(void) : HmLegacyFrameSystem( FrameType_CsmaCaControl )
{
}


HmLegacyFrameSystemCsmaCaControl::~HmLegacyFrameSystemCsmaCaControl(void)
{
}

bool HmLegacyFrameSystemCsmaCaControl::GetActiveFlag()const
{
	return Data().GetUInt8Value( 0 ) != 0;
}

void HmLegacyFrameSystemCsmaCaControl::SetActiveFlag( bool active )
{
	Data().SetUInt8Value( 0, active ? 1 : 0 );
}


std::string HmLegacyFrameSystemCsmaCaControl::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmSystem CsmaCaControl ";
	s += Data().ToString();
	return s;
}
