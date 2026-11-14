/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "TrxAdapterFrameGetVersion.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, TrxAdapterFrameGetVersion, int> m((SerialFrame::FrameSubsystemType_TrxAdapter<<8)|TrxAdapterFrame::FrameType_GetVersion);
/// @endcond


TrxAdapterFrameGetVersion::TrxAdapterFrameGetVersion(void) : TrxAdapterFrame( FrameType_GetVersion )
{
}


TrxAdapterFrameGetVersion::~TrxAdapterFrameGetVersion(void)
{
}

std::string TrxAdapterFrameGetVersion::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " TRX GetVersion";
	return s;
}

