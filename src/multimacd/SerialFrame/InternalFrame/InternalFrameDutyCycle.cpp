/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "InternalFrameDutyCycle.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, InternalFrameDutyCycle, int> m((SerialFrame::FrameSubsystemType_Internal<<8)|InternalFrame::FrameType_DutyCycle);
/// @endcond



InternalFrameDutyCycle::InternalFrameDutyCycle(void) : InternalFrame( FrameType_DutyCycle )
{
}


InternalFrameDutyCycle::~InternalFrameDutyCycle(void)
{
}

uint8_t InternalFrameDutyCycle::GetDutyCylce()const
{
	return Data().GetUInt8Value( 0 );
}

void InternalFrameDutyCycle::SetDutyCycle(uint8_t dutyCycle)
{
	Data().SetUInt8Value( 0,  dutyCycle );
}

std::string InternalFrameDutyCycle::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " Internal DC ";

	uint8_t dutyCyle = GetDutyCylce();
	snprintf( buffer, sizeof(buffer), "%d.%d%%", dutyCyle/2, (dutyCyle % 2) * 5 );
	s += buffer;

	return s;
}
