/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemSetTeststatus.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemSetTeststatus, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_SetTeststatus);
/// @endcond


HmLegacyFrameSystemSetTeststatus::HmLegacyFrameSystemSetTeststatus(void) : HmLegacyFrameSystem( FrameType_SetTeststatus )
{
}


HmLegacyFrameSystemSetTeststatus::~HmLegacyFrameSystemSetTeststatus(void)
{
}

std::string HmLegacyFrameSystemSetTeststatus::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmSystem SetTeststatus ";
	s += Data().ToString();
	return s;
}
