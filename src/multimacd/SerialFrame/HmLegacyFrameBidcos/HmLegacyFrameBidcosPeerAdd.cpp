/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosPeerAdd.h"
#include <stdio.h>
#include <cinttypes>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosPeerAdd, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_PeerAdd);
/// @endcond


HmLegacyFrameBidcosPeerAdd::HmLegacyFrameBidcosPeerAdd(void) : HmLegacyFrameBidcos( FrameType_PeerAdd )
{
}


HmLegacyFrameBidcosPeerAdd::~HmLegacyFrameBidcosPeerAdd(void)
{
}

uint32_t HmLegacyFrameBidcosPeerAdd::GetRfAddress()const
{
	return Data().GetUInt24Value( 0 );
}

void HmLegacyFrameBidcosPeerAdd::SetRfAddress(uint32_t rfAddress)
{
	Data().SetUInt24Value( 0, rfAddress );
}

uint8_t HmLegacyFrameBidcosPeerAdd::GetKeyIndex()const
{
	return Data().GetUInt8Value( 3 );
}

void HmLegacyFrameBidcosPeerAdd::SetKeyIndex( uint8_t keyIndex )
{
	Data().SetUInt8Value( 3, keyIndex );
}

bool HmLegacyFrameBidcosPeerAdd::GetNeedsWakeupFlag()const
{
	return Data().GetUInt8Value( 4 ) != 0;
}

void HmLegacyFrameBidcosPeerAdd::SetNeedsWakeupFlag( bool needsWakeup )
{
	Data().SetUInt8Value( 4, needsWakeup ? 1 : 0 );
}

bool HmLegacyFrameBidcosPeerAdd::GetLazyConfigFlag()const
{
	return Data().GetUInt8Value( 5 ) != 0;
}

void HmLegacyFrameBidcosPeerAdd::SetLazyConfigFlag( bool lazyConfig )
{
	Data().SetUInt8Value( 5, lazyConfig ? 1 : 0 );
}

std::string HmLegacyFrameBidcosPeerAdd::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos PeerAdd ";
	snprintf( buffer, sizeof(buffer), "%06" PRIX32 " ", GetRfAddress() );
	s += buffer;
	snprintf( buffer, sizeof(buffer), "key=%d ", GetKeyIndex() );
	s += buffer;
	if( GetNeedsWakeupFlag() )
	{
		s += "Wakeup ";
	}
	if( GetLazyConfigFlag() )
	{
		s += "LazyConfig ";
	}
	return s;
}

