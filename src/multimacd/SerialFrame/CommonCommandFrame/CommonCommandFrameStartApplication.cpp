/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CommonCommandFrameStartApplication.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, CommonCommandFrameStartApplication, int> m((SerialFrame::FrameSubsystemType_Common<<8)|CommonCommandFrame::FrameType_StartApplication);
/// @endcond


CommonCommandFrameStartApplication::CommonCommandFrameStartApplication(void) : CommonCommandFrame( FrameType_StartApplication )
{
}


CommonCommandFrameStartApplication::~CommonCommandFrameStartApplication(void)
{
}

std::string CommonCommandFrameStartApplication::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " COMMON StartApp";
	return s;
}

