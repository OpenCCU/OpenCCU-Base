/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosGetPreviousAesKeyHash.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosGetPreviousAesKeyHash, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_GetPreviousAesKeyHash);
/// @endcond


HmLegacyFrameBidcosGetPreviousAesKeyHash::HmLegacyFrameBidcosGetPreviousAesKeyHash(void) : HmLegacyFrameBidcos( FrameType_GetPreviousAesKeyHash )
{
}


HmLegacyFrameBidcosGetPreviousAesKeyHash::~HmLegacyFrameBidcosGetPreviousAesKeyHash(void)
{
}

std::string HmLegacyFrameBidcosGetPreviousAesKeyHash::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos GetPreviousAesKeyHash ";
	s += Data().ToString();
	return s;
}

