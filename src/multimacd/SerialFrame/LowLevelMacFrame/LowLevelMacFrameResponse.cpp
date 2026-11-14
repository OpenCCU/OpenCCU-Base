/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LowLevelMacFrameResponse.h"
#include <stdio.h>
#include <cinttypes>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, LowLevelMacFrameResponse, int> m((SerialFrame::FrameSubsystemType_LowLevelMac<<8)|LowLevelMacFrame::FrameType_Response);
/// @endcond



LowLevelMacFrameResponse::LowLevelMacFrameResponse(void) : LowLevelMacFrame( FrameType_Response )
{
}


LowLevelMacFrameResponse::~LowLevelMacFrameResponse(void)
{
}

LowLevelMacFrameResponse::ResponseCode LowLevelMacFrameResponse::GetResponseCode()const
{
	return (ResponseCode)Data().GetUInt8Value(0);
}

void LowLevelMacFrameResponse::SetResponseCode( ResponseCode responseCode )
{
	Data().SetUInt8Value( 0, (uint8_t)responseCode );
}

bool LowLevelMacFrameResponse::IsAck()const
{
	return (ResponseCode)Data().GetUInt8Value(0) == ResponseCode_Ack;
}

BinaryData LowLevelMacFrameResponse::GetPayload()const
{
	return Data().GetRange(1, Data().size()-1);
}

void LowLevelMacFrameResponse::SetPayload( const BinaryData& payload )
{
	Data().SetRange(1, payload );
}
bool LowLevelMacFrameResponse::isResponseFrame()
{
	return true;
}

std::string LowLevelMacFrameResponse::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " LLMAC Response ";
	switch( GetResponseCode() )
	{
	case ResponseCode_Ack:
		s += "ACK";
		if( GetPayload().size() )
		{
			s += " @";
			snprintf( buffer, sizeof(buffer), "%" PRIu16, GetPayload().GetUInt16Value(0) );
			s += buffer;
		}
		break;
	case ResponseCode_Abort:
		s += "Abort";
		break;
	case ResponseCode_Busy:
		s += "Busy";
		break;
	case ResponseCode_BusyTrx:
		s += "BusyTrx";
		break;
	case ResponseCode_CcaFailed:
		s += "CcaFailed";
		break;
	case ResponseCode_DutyCycleFull:
		s += "DutyCycleFull";
		break;
	case ResponseCode_Error:
		s += "Error";
		break;
	case ResponseCode_InvalidInput:
		s += "InvalidInput";
		break;
	case ResponseCode_WrongFreqBitrate:
		s += "WrongFreqBitrate";
		break;
	}
	if( GetPayload().size() )
	{
		s += ": ";
		s += GetPayload().ToString().c_str();
	}
	return s;
}
