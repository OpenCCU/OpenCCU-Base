/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosSetRfAddress.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosSetRfAddress, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_SetRfAddress);
/// @endcond


HmLegacyFrameBidcosSetRfAddress::HmLegacyFrameBidcosSetRfAddress(void) : HmLegacyFrameBidcos( FrameType_SetRfAddress )
{
}


HmLegacyFrameBidcosSetRfAddress::~HmLegacyFrameBidcosSetRfAddress(void)
{
}

uint32_t HmLegacyFrameBidcosSetRfAddress::GetRfAddress()const
{
	return Data().GetUInt24Value( 0 );
}

void HmLegacyFrameBidcosSetRfAddress::SetRfAddress(uint32_t rfAddress)
{
	Data().SetUInt24Value( 0, rfAddress );
}

std::string HmLegacyFrameBidcosSetRfAddress::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos SetRfAddress ";
	s += Data().ToString();
	return s;
}

