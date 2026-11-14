/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CommonCommandFrameIdentifyRequest.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, CommonCommandFrameIdentifyRequest, int> m((SerialFrame::FrameSubsystemType_Common<<8)|CommonCommandFrame::FrameType_IdentifyRequest);
/// @endcond

CommonCommandFrameIdentifyRequest::CommonCommandFrameIdentifyRequest(void) : CommonCommandFrame( FrameType_IdentifyRequest )
{
}


CommonCommandFrameIdentifyRequest::~CommonCommandFrameIdentifyRequest(void)
{
}

std::string CommonCommandFrameIdentifyRequest::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " COMMON IdentifyRequest";
	return s;
}

