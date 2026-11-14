/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "TrxAdapterFrameCarrierSenseEvent.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, TrxAdapterFrameCarrierSenseEvent, int> m((SerialFrame::FrameSubsystemType_TrxAdapter<<8)|TrxAdapterFrame::FrameType_CarrierSenseEvent);
/// @endcond


TrxAdapterFrameCarrierSenseEvent::TrxAdapterFrameCarrierSenseEvent(void) : TrxAdapterFrame( FrameType_CarrierSenseEvent )
{
}


TrxAdapterFrameCarrierSenseEvent::~TrxAdapterFrameCarrierSenseEvent(void)
{
}



std::string TrxAdapterFrameCarrierSenseEvent::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " TRX CarrierSenseEvent ";
	double dcFloat = ((double)Data().GetUInt8Value( 1 )) * (double)0.5;
	snprintf(buffer, sizeof(buffer), "%.1f%% ", dcFloat);
	s += buffer;
	s += Data().GetUInt8Value(2) == 2 ? "(rising)" : "(falling)";
	return s;
}

