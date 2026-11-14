/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosGetPeers.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosGetPeers, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_GetPeers);
/// @endcond


HmLegacyFrameBidcosGetPeers::HmLegacyFrameBidcosGetPeers(void) : HmLegacyFrameBidcos( FrameType_GetPeers )
{
}


HmLegacyFrameBidcosGetPeers::~HmLegacyFrameBidcosGetPeers(void)
{
}

std::string HmLegacyFrameBidcosGetPeers::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos GetPeers ";
	s += Data().ToString();
	return s;
}

