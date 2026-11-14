/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosPeerGetAesKeyId.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosPeerGetAesKeyId, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_PeerGetAesKeyId);
/// @endcond


HmLegacyFrameBidcosPeerGetAesKeyId::HmLegacyFrameBidcosPeerGetAesKeyId(void) : HmLegacyFrameBidcos( FrameType_PeerGetAesKeyId )
{
}


HmLegacyFrameBidcosPeerGetAesKeyId::~HmLegacyFrameBidcosPeerGetAesKeyId(void)
{
}

uint32_t HmLegacyFrameBidcosPeerGetAesKeyId::GetRfAddress()const
{
	return Data().GetUInt24Value( 0 );
}

void HmLegacyFrameBidcosPeerGetAesKeyId::SetRfAddress(uint32_t rfAddress)
{
	Data().SetUInt24Value( 0, rfAddress );
}



std::string HmLegacyFrameBidcosPeerGetAesKeyId::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos PeerGetAesKeyId ";
	s += Data().ToString();
	return s;
}

