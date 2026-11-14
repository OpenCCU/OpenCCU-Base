/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CommonCommandFrameStartBootloader.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, CommonCommandFrameStartBootloader, int> m((SerialFrame::FrameSubsystemType_Common<<8)|CommonCommandFrame::FrameType_StartBootloader);
/// @endcond


CommonCommandFrameStartBootloader::CommonCommandFrameStartBootloader(void) : CommonCommandFrame( FrameType_StartBootloader )
{
}


CommonCommandFrameStartBootloader::~CommonCommandFrameStartBootloader(void)
{
}

std::string CommonCommandFrameStartBootloader::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " COMMON StartBl";
	return s;
}

