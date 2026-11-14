/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosPeerSetAesKeyId.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosPeerSetAesKeyId, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_PeerSetAesKeyId);
/// @endcond


HmLegacyFrameBidcosPeerSetAesKeyId::HmLegacyFrameBidcosPeerSetAesKeyId(void) : HmLegacyFrameBidcos( FrameType_PeerSetAesKeyId )
{
}


HmLegacyFrameBidcosPeerSetAesKeyId::~HmLegacyFrameBidcosPeerSetAesKeyId(void)
{
}

uint32_t HmLegacyFrameBidcosPeerSetAesKeyId::GetRfAddress()const
{
	return Data().GetUInt24Value( 0 );
}

void HmLegacyFrameBidcosPeerSetAesKeyId::SetRfAddress(uint32_t rfAddress)
{
	Data().SetUInt24Value( 0, rfAddress );
}

uint8_t HmLegacyFrameBidcosPeerSetAesKeyId::GetKeyIndex()const
{
	return Data().GetUInt8Value( 3 );
}

void HmLegacyFrameBidcosPeerSetAesKeyId::SetKeyIndex( uint8_t keyIndex )
{
	Data().SetUInt8Value( 3, keyIndex );
}


std::string HmLegacyFrameBidcosPeerSetAesKeyId::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos PeerSetAesKeyId ";
	s += Data().ToString();
	return s;
}

