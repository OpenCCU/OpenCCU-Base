/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "TrxAdapterFrameGetDutyCycle.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, TrxAdapterFrameGetDutyCycle, int> m((SerialFrame::FrameSubsystemType_TrxAdapter<<8)|TrxAdapterFrame::FrameType_GetDutyCycle);
/// @endcond


TrxAdapterFrameGetDutyCycle::TrxAdapterFrameGetDutyCycle(void) : TrxAdapterFrame( FrameType_GetDutyCycle )
{
}


TrxAdapterFrameGetDutyCycle::~TrxAdapterFrameGetDutyCycle(void)
{
}

std::string TrxAdapterFrameGetDutyCycle::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " TRX GetDutyCycle";
	return s;
}

