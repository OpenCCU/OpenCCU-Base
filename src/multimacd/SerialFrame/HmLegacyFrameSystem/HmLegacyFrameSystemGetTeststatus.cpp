/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemGetTeststatus.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemGetTeststatus, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_GetTeststatus);
/// @endcond


HmLegacyFrameSystemGetTeststatus::HmLegacyFrameSystemGetTeststatus(void) : HmLegacyFrameSystem( FrameType_GetTeststatus )
{
}


HmLegacyFrameSystemGetTeststatus::~HmLegacyFrameSystemGetTeststatus(void)
{
}

std::string HmLegacyFrameSystemGetTeststatus::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmSystem GetTeststatus ";
	s += Data().ToString();
	return s;
}
