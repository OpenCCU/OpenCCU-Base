/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosPeerActivateAuth.h"
#include <stdio.h>
#include <cinttypes>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosPeerActivateAuth, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_PeerActivateAuth);
/// @endcond


HmLegacyFrameBidcosPeerActivateAuth::HmLegacyFrameBidcosPeerActivateAuth(void) : HmLegacyFrameBidcos( FrameType_PeerActivateAuth )
{
}

HmLegacyFrameBidcosPeerActivateAuth::HmLegacyFrameBidcosPeerActivateAuth(FrameType frameType) : HmLegacyFrameBidcos( frameType )
{
}


HmLegacyFrameBidcosPeerActivateAuth::~HmLegacyFrameBidcosPeerActivateAuth(void)
{
}

uint32_t HmLegacyFrameBidcosPeerActivateAuth::GetRfAddress()const
{
	return Data().GetUInt24Value( 0 );
}

void HmLegacyFrameBidcosPeerActivateAuth::SetRfAddress(uint32_t rfAddress)
{
	Data().SetUInt24Value( 0, rfAddress );
}

uint64_t HmLegacyFrameBidcosPeerActivateAuth::GetChannels()const
{
	uint64_t channels = 0;
	for( size_t i=3; i<Data().size(); i++ )
	{
		uint8_t channelNumber = Data().GetUInt8Value( i );
		channels |= uint64_t(1) << channelNumber;
	}
	return channels;
}

void HmLegacyFrameBidcosPeerActivateAuth::SetChannels( uint64_t channels )
{
	int index = 3;
	uint8_t channelNumber = 0;
	while( channels )
	{
		if( channels & 0x01 )
		{
			Data().SetUInt8Value( index++, channelNumber );
		}
		channelNumber++;
		channels >>= 1;
	}
}


std::string HmLegacyFrameBidcosPeerActivateAuth::ToString()const
{
	char buffer[32];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos PeerActivateAuth ";
	snprintf( buffer, sizeof(buffer), "%06" PRIX32 " ", GetRfAddress() );
	s += buffer;
	uint64_t channels = GetChannels();
	snprintf( buffer, sizeof(buffer), "%08" PRIX32 " %08" PRIX32 " ", uint32_t(channels >> 32), uint32_t(channels & 0xffffffff) );
	s += buffer;
	s += Data().GetRange( 3, Data().size()-3 ).ToString();
	return s;
}

