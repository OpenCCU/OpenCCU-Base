/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LowLevelMacFrameTxWithRxModeSet.h"
#include <stdio.h>
#include <cinttypes>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, LowLevelMacFrameTxWithRxModeSet, int> m((SerialFrame::FrameSubsystemType_LowLevelMac<<8)|LowLevelMacFrame::FrameType_TxWitRxModeSet);
/// @endcond



LowLevelMacFrameTxWithRxModeSet::LowLevelMacFrameTxWithRxModeSet(void) : LowLevelMacFrame( FrameType_TxWitRxModeSet )
{
}


LowLevelMacFrameTxWithRxModeSet::~LowLevelMacFrameTxWithRxModeSet(void)
{
}

void LowLevelMacFrameTxWithRxModeSet::SetRxEndTimeAbsolute( uint16_t lockEndTime )
{
	Data().SetUInt16Value( 0, lockEndTime & ~Timestamp_Relative );
}

void LowLevelMacFrameTxWithRxModeSet::SetRxEndTimeRelative( uint16_t lockEndTime )
{
	Data().SetUInt16Value( 0, lockEndTime | Timestamp_Relative );
}

void LowLevelMacFrameTxWithRxModeSet::SetRxEndTime( uint16_t lockEndTime )
{
	Data().SetUInt16Value( 0, lockEndTime );
}

uint16_t LowLevelMacFrameTxWithRxModeSet::GetRxEndTime()const
{
	return Data().GetUInt16Value( 0 );
}

void LowLevelMacFrameTxWithRxModeSet::SetOptions( int options )
{
	Data().SetUInt8Value(2, options );
}

int LowLevelMacFrameTxWithRxModeSet::GetOptions()const
{
	return Data().GetUInt8Value( 2 );
}

BinaryData LowLevelMacFrameTxWithRxModeSet::GetPayload()const
{
	if( Data().size() > 3 )
	{
		return Data().GetRange( 3, Data().size() - 3 );
	}
	return BinaryData();
}

void LowLevelMacFrameTxWithRxModeSet::SetPayload( const BinaryData& payload )
{
	Data().SetRange( 3, payload );
}

std::string LowLevelMacFrameTxWithRxModeSet::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " LLMAC TxWithRxModeSet [";

	int options = GetOptions();
	switch( options & 0x03 )
	{
	case Option_Band_868_10k:
		s += "10k";
		break;
	case Option_Band_868_100k:
		s += "100k";
		break;
	case Option_Band_869_10k:
		s += "869,10k";
		break;
	case Option_Band_869_50k:
		s += "869,10k";
		break;
	}
	if( options & Option_Aes )
	{
		s += ",AES";
	}
	if( options & Option_Burst )
	{
		s += ",WOR";
	}
	if( options & Option_CcaOff )
	{
		s += ",NoCCA";
	}
	s += "]";

	snprintf( buffer, sizeof(buffer), "->%5" PRIu16 "ms ", GetRxEndTime() );
	s += buffer;

	s += GetPayload().ToString().c_str();

	return s;
}

