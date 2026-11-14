/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosPeerRemove.h"
#include <stdio.h>
#include <cinttypes>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosPeerRemove, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_PeerRemove);
/// @endcond


HmLegacyFrameBidcosPeerRemove::HmLegacyFrameBidcosPeerRemove(void) : HmLegacyFrameBidcos( FrameType_PeerRemove )
{
}


HmLegacyFrameBidcosPeerRemove::~HmLegacyFrameBidcosPeerRemove(void)
{
}

uint32_t HmLegacyFrameBidcosPeerRemove::GetRfAddress()const
{
	return Data().GetUInt24Value( 0 );
}

void HmLegacyFrameBidcosPeerRemove::SetRfAddress(uint32_t rfAddress)
{
	Data().SetUInt24Value( 0, rfAddress );
}


std::string HmLegacyFrameBidcosPeerRemove::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos PeerRemove ";
	snprintf( buffer, sizeof(buffer), "%06" PRIX32 " ", GetRfAddress() );
	s += buffer;
	return s;
}

