/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RouterFrameResponse.h"

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, RouterFrameResponse, int> m((SerialFrame::FrameSubsystemType_Router<<8)|RouterFrame::FrameType_Response);
/// @endcond

RouterFrameResponse::RouterFrameResponse():RouterFrame(RouterFrame::FrameType_Response) {
	// TODO Auto-generated constructor stub

}

RouterFrameResponse::~RouterFrameResponse() {
	// TODO Auto-generated destructor stub
}

bool RouterFrameResponse::isResponseFrame()
{
	return true;
}
