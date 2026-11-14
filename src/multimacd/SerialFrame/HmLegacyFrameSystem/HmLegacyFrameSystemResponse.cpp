/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemResponse.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemResponse, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_Response);
/// @endcond


HmLegacyFrameSystemResponse::HmLegacyFrameSystemResponse(void) : HmLegacyFrameSystem( FrameType_Response )
{
}


HmLegacyFrameSystemResponse::~HmLegacyFrameSystemResponse(void)
{
}

HmLegacyFrameSystemResponse::ResponseCode HmLegacyFrameSystemResponse::GetResponseCode()const
{
	return (ResponseCode)Data().GetUInt8Value(0);
}

void HmLegacyFrameSystemResponse::SetResponseCode( ResponseCode responseCode )
{
	Data().SetUInt8Value( 0, (uint8_t)responseCode );
}

uint32_t HmLegacyFrameSystemResponse::GetBootloaderVersion()const
{
	return Data().GetUInt24Value( 1 );
}

void HmLegacyFrameSystemResponse::SetBootloaderVersion( uint32_t bootloaderVersion )
{
	Data().SetUInt24Value( 1, bootloaderVersion );
}

uint32_t HmLegacyFrameSystemResponse::GetApplicationVersion()const
{
	return Data().GetUInt24Value( 4 );
}

void HmLegacyFrameSystemResponse::SetApplicationVersion( uint32_t applicationVersion )
{
	Data().SetUInt24Value( 4, applicationVersion );
}

std::string HmLegacyFrameSystemResponse::GetIdentification()const
{
	return Data().GetStringValue( 1 );
}

void HmLegacyFrameSystemResponse::SetIdentification(const std::string& identification)
{
	Data().SetStringValue( 1, identification );
}

std::string HmLegacyFrameSystemResponse::GetSerialNumber()const
{
	return Data().GetStringValue( 1 );
}

void HmLegacyFrameSystemResponse::SetSerialNumber(const std::string& serialNumber)
{
	Data().SetStringValue( 1, serialNumber );
}

uint8_t HmLegacyFrameSystemResponse::GetDutyCycle()const
{
	return Data().GetUInt8Value( 1 );
}

void HmLegacyFrameSystemResponse::SetDutyCycle( uint8_t dutyCycle )
{
	Data().SetUInt8Value( 1, dutyCycle );
}


std::string HmLegacyFrameSystemResponse::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmSystem Response ";
	s += Data().ToString();
	return s;
}
