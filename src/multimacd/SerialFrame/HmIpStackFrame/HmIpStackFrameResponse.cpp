/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/



#include "HmIpStackFrameResponse.h"

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, HmIpStackFrameResponse, int> m((SerialFrame::FrameSubsystemType_HmIpStack<<8)|HmIpStackFrame::FrameType_Response);
/// @endcond

HmIpStackFrameResponse::HmIpStackFrameResponse():HmIpStackFrame(HmIpStackFrame::FrameType_Response) {
	// TODO Auto-generated constructor stub

}

HmIpStackFrameResponse::~HmIpStackFrameResponse() {
	// TODO Auto-generated destructor stub
}

bool HmIpStackFrameResponse::isResponseFrame()
{
	return true;
}
