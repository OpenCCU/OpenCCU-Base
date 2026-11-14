/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LowLevelMacFrameRxTelegram.h"
#include <stdio.h>
#include <cinttypes>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, LowLevelMacFrameRxTelegram, int> m((SerialFrame::FrameSubsystemType_LowLevelMac<<8)|LowLevelMacFrame::FrameType_RxTelegram);
/// @endcond



LowLevelMacFrameRxTelegram::LowLevelMacFrameRxTelegram(void) : LowLevelMacFrame( FrameType_RxTelegram )
{
}

LowLevelMacFrameRxTelegram::LowLevelMacFrameRxTelegram(FrameType frameType) : LowLevelMacFrame( frameType )
{
}


LowLevelMacFrameRxTelegram::~LowLevelMacFrameRxTelegram(void)
{
}

void LowLevelMacFrameRxTelegram::SetFrameEndTime( uint16_t frameEndTime )
{
	Data().SetUInt16Value( 0, frameEndTime );
}

uint16_t LowLevelMacFrameRxTelegram::GetFrameEndTime()const
{
	return Data().GetUInt16Value( 0 );
}

void LowLevelMacFrameRxTelegram::SetOptions( int options )
{
	Data().SetUInt8Value(2, options );
}

int LowLevelMacFrameRxTelegram::GetOptions()const
{
	return Data().GetUInt8Value( 2 );
}

int LowLevelMacFrameRxTelegram::GetRssi()const
{
	return -Data().GetUInt8Value( 3 );
}

void LowLevelMacFrameRxTelegram::SetRssi( int rssi )
{
	Data().SetUInt8Value( 3, (-rssi) & 0xff );
}

BinaryData LowLevelMacFrameRxTelegram::GetPayload()const
{
	return Data().GetRange( 4, Data().size() - 4 );
}

void LowLevelMacFrameRxTelegram::SetPayload( const BinaryData& payload )
{
	Data().SetRange( 4, payload );
}

std::string LowLevelMacFrameRxTelegram::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " LLMAC RX ";
	snprintf( buffer, sizeof(buffer), "@%5" PRIu16 "ms ", GetFrameEndTime() );
	s += buffer;
	snprintf( buffer, sizeof(buffer), "%ddBm ", GetRssi() );
	s += buffer;
	s += GetPayload().ToString().c_str();
	return s;
}

