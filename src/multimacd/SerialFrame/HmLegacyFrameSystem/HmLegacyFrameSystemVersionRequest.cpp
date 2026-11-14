/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemVersionRequest.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemVersionRequest, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_VersionRequest);
/// @endcond


HmLegacyFrameSystemVersionRequest::HmLegacyFrameSystemVersionRequest(void) : HmLegacyFrameSystem( FrameType_VersionRequest )
{
}


HmLegacyFrameSystemVersionRequest::~HmLegacyFrameSystemVersionRequest(void)
{
}

std::string HmLegacyFrameSystemVersionRequest::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmSystem VersionRequest ";
	s += Data().ToString();
	return s;
}
