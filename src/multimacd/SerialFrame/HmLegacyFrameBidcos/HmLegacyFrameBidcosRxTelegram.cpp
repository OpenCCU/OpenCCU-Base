/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosRxTelegram.h"
#include <stdio.h>
#include "../../BidcosTelegram.h"

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosRxTelegram, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_RxTelegram);
/// @endcond


HmLegacyFrameBidcosRxTelegram::HmLegacyFrameBidcosRxTelegram(void) : HmLegacyFrameBidcos( FrameType_RxTelegram )
{
	SetKeyIndex( 0xff );
}


HmLegacyFrameBidcosRxTelegram::~HmLegacyFrameBidcosRxTelegram(void)
{
}

void HmLegacyFrameBidcosRxTelegram::SetFlags( int flags )
{
	Data().SetUInt8Value(0, flags );
}

int HmLegacyFrameBidcosRxTelegram::GetFlags()const
{
	return Data().GetUInt8Value( 0 );
}

int HmLegacyFrameBidcosRxTelegram::GetRssi()const
{
	return -Data().GetUInt8Value( 2 );
}

void HmLegacyFrameBidcosRxTelegram::SetRssi( int rssi )
{
	Data().SetUInt8Value( 2, (-rssi) & 0xff );
}

uint8_t HmLegacyFrameBidcosRxTelegram::GetKeyIndex()const
{
	return Data().GetUInt8Value( 1 );
}

void HmLegacyFrameBidcosRxTelegram::SetKeyIndex( uint8_t keyIndex )
{
	Data().SetUInt8Value(1, keyIndex );
}

BinaryData HmLegacyFrameBidcosRxTelegram::GetPayload()const
{
	return Data().GetRange( 3, Data().size() - 3 );
}

void HmLegacyFrameBidcosRxTelegram::SetPayload( const BinaryData& payload )
{
	Data().SetRange( 3, payload );
}


std::string HmLegacyFrameBidcosRxTelegram::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos RxTelegram ";
	switch( GetFlags() & 0x0f )
	{
	case Flag_AuthNone:
		s += "AuthNone ";
		break;
	case Flag_AuthNotNecessary:
		s += "AuthNotNecessary ";
		break;
	case Flag_AuthSuccessful:
		s += "Auth";
		snprintf( buffer, sizeof(buffer), "[%d] ", GetKeyIndex() );
		s += buffer;
		break;
	case Flag_AuthFailed:
		s += "AuthFailed ";
		break;
		break;
	case Flag_AuthUnknownKey:
		s += "AuthNoKey";
		snprintf( buffer, sizeof(buffer), "[%d] ", GetKeyIndex() );
		s += buffer;
		break;
	}
	if( GetFlags() & Flag_Wokenup )
	{
		s += "Wup ";
	}
	s += BidcosTelegram(GetPayload()).ToString();
	return s;
}

