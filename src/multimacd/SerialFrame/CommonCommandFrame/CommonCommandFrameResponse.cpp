/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CommonCommandFrameResponse.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, CommonCommandFrameResponse, int> m((SerialFrame::FrameSubsystemType_Common<<8)|CommonCommandFrame::FrameType_Response);
/// @endcond


CommonCommandFrameResponse::CommonCommandFrameResponse(void) : CommonCommandFrame( FrameType_Response )
{
}


CommonCommandFrameResponse::~CommonCommandFrameResponse(void)
{
}

CommonCommandFrameResponse::ResponseCode CommonCommandFrameResponse::GetResponseCode()const
{
	return (ResponseCode)Data().GetUInt8Value(0);
}

void CommonCommandFrameResponse::SetResponseCode( ResponseCode responseCode )
{
	Data().SetUInt8Value( 0, (uint8_t)responseCode );
}

bool CommonCommandFrameResponse::IsAck()const
{
	return (ResponseCode)Data().GetUInt8Value(0) == ResponseCode_Ack;
}

BinaryData CommonCommandFrameResponse::GetPayload()const
{
	return Data().GetRange(1, Data().size()-1);
}

void CommonCommandFrameResponse::SetPayload( const BinaryData& payload )
{
	Data().SetRange(1, payload );
}
bool CommonCommandFrameResponse::isResponseFrame()
{
	return true;
}
std::string CommonCommandFrameResponse::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " COMMON Response ";
	switch( GetResponseCode() )
	{
	case ResponseCode_Ack:
		s += "Ack ";
		break;
	case ResponseCode_Error:
		s += "Error ";
		break;
	default:
		s += Data().GetRange(0, 1).ToString();
		s += " ";
	}
	s += GetPayload().ToString();
	return s;
}

