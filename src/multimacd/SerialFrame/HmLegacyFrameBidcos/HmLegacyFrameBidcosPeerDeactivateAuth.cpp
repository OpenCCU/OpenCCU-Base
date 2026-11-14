/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosPeerDeactivateAuth.h"
#include <stdio.h>
#include <cinttypes>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosPeerDeactivateAuth, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_PeerDeactivateAuth);
/// @endcond


HmLegacyFrameBidcosPeerDeactivateAuth::HmLegacyFrameBidcosPeerDeactivateAuth(void) : HmLegacyFrameBidcosPeerActivateAuth( FrameType_PeerDeactivateAuth )
{
}


HmLegacyFrameBidcosPeerDeactivateAuth::~HmLegacyFrameBidcosPeerDeactivateAuth(void)
{
}

std::string HmLegacyFrameBidcosPeerDeactivateAuth::ToString()const
{
	char buffer[32];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos PeerDeactivateAuth ";
	snprintf( buffer, sizeof(buffer), "%06" PRIX32 " ", GetRfAddress() );
	s += buffer;
	uint64_t channels = GetChannels();
	snprintf( buffer, sizeof(buffer), "%08" PRIX32 " %08" PRIX32 " ", uint32_t(channels >> 32), uint32_t(channels & 0xffffffff) );
	s += buffer;
	s += Data().GetRange( 3, Data().size()-3 ).ToString();
	return s;
}

