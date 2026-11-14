/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "TrxAdapterFrameResponse.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, TrxAdapterFrameResponse, int> m((SerialFrame::FrameSubsystemType_TrxAdapter<<8)|TrxAdapterFrame::FrameType_Response);
/// @endcond


TrxAdapterFrameResponse::TrxAdapterFrameResponse(void) : TrxAdapterFrame( FrameType_Response )
{
}


TrxAdapterFrameResponse::~TrxAdapterFrameResponse(void)
{
}

TrxAdapterFrameResponse::ResponseCode TrxAdapterFrameResponse::GetResponseCode()const
{
	return (ResponseCode)Data().GetUInt8Value(0);
}

void TrxAdapterFrameResponse::SetResponseCode( ResponseCode responseCode )
{
	Data().SetUInt8Value( 0, (uint8_t)responseCode );
}

bool TrxAdapterFrameResponse::IsAck()const
{
	return (ResponseCode)Data().GetUInt8Value(0) == ResponseCode_Ack;
}

BinaryData TrxAdapterFrameResponse::GetPayload()const
{
	return Data().GetRange(1, Data().size()-1);
}

void TrxAdapterFrameResponse::SetPayload( const BinaryData& payload )
{
	Data().SetRange(1, payload );
}

uint32_t TrxAdapterFrameResponse::GetApplicationVersion()const
{
	return Data().GetUInt24Value( 1 );
}
void TrxAdapterFrameResponse::SetApplicationVersion( uint32_t applicationVersion )
{
	Data().SetUInt24Value( 1, applicationVersion );
}
uint32_t TrxAdapterFrameResponse::GetBootloaderVersion()const
{
	return Data().GetUInt24Value( 4 );
}
void TrxAdapterFrameResponse::SetBootloaderVersion( uint32_t bootloaderVersion )
{
	Data().SetUInt24Value( 4, bootloaderVersion );
}
uint32_t TrxAdapterFrameResponse::GetHmosVersion()const
{
	return Data().GetUInt24Value( 7 );
}
void TrxAdapterFrameResponse::SetHmosVersion( uint32_t hmosVersion )
{
	Data().SetUInt24Value( 7, hmosVersion );
}

uint8_t TrxAdapterFrameResponse::GetDutyCycle()const
{
	return Data().GetUInt8Value( 1 );
}

void TrxAdapterFrameResponse::SetDutyCycle( uint8_t dutyCycle )
{
	Data().SetUInt8Value( 1, dutyCycle );
}
bool TrxAdapterFrameResponse::isResponseFrame()
{
	return true;
}

std::string TrxAdapterFrameResponse::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " TRX Response ";
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

