/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosTxTelegram.h"
#include <stdio.h>
#include <cinttypes>

#include "../../BidcosTelegram.h"

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosTxTelegram, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_TxTelegram);
/// @endcond


HmLegacyFrameBidcosTxTelegram::HmLegacyFrameBidcosTxTelegram(void) : HmLegacyFrameBidcos( FrameType_TxTelegram )
{
}


HmLegacyFrameBidcosTxTelegram::~HmLegacyFrameBidcosTxTelegram(void)
{
}

void HmLegacyFrameBidcosTxTelegram::SetSendDelay( uint16_t sendDelay )
{
	Data().SetUInt16Value( 0, sendDelay );
}

uint16_t HmLegacyFrameBidcosTxTelegram::GetSendDelay()const
{
	return Data().GetUInt16Value( 0 );
}

void HmLegacyFrameBidcosTxTelegram::SetBurstMode( BurstMode burstMode )
{
	Data().SetUInt8Value( 2, (uint8_t)burstMode );
}

HmLegacyFrameBidcosTxTelegram::BurstMode HmLegacyFrameBidcosTxTelegram::GetBurstMode()const
{
	return (BurstMode)Data().GetUInt8Value( 2 );
}

BinaryData HmLegacyFrameBidcosTxTelegram::GetPayload()const
{
	if( Data().size() > 3 )
	{
		return Data().GetRange( 3, Data().size() - 3 );
	}
	return BinaryData();
}

void HmLegacyFrameBidcosTxTelegram::SetPayload( const BinaryData& payload )
{
	Data().SetRange( 3, payload );
}

std::string HmLegacyFrameBidcosTxTelegram::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos TxTelegram [";
	if( GetSendDelay() )
	{
		s += "after ";
		snprintf( buffer, sizeof(buffer), "%" PRIu16 "ms, ", GetSendDelay() );
		s += buffer;
	}
	switch( GetBurstMode() )
	{
	case BurstMode_None:
		break;
	case BurstMode_Burst:
		s += "burst";
		break;
	case BurstMode_Triple:
		s += "3burst";
		break;
	}
	s += "] ";
	s += BidcosTelegram(GetPayload()).ToString();
	return s;
}

