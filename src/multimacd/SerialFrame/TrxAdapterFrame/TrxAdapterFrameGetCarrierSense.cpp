/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "TrxAdapterFrameGetCarrierSense.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, TrxAdapterFrameGetCarrierSense, int> m((SerialFrame::FrameSubsystemType_TrxAdapter<<8)|TrxAdapterFrame::FrameType_GetCarrierSense);
/// @endcond


TrxAdapterFrameGetCarrierSense::TrxAdapterFrameGetCarrierSense(void) : TrxAdapterFrame( FrameType_GetCarrierSense )
{
}


TrxAdapterFrameGetCarrierSense::~TrxAdapterFrameGetCarrierSense(void)
{
}

std::string TrxAdapterFrameGetCarrierSense::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " TRX GetCarrierSense";
	return s;
}

