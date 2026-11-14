/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BackboneFrameResponse.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, BackboneFrameResponse, int> m((SerialFrame::FrameSubsystemType_Backbone<<8)|BackboneFrame::FrameType_Response);
/// @endcond

BackboneFrameResponse::BackboneFrameResponse() : BackboneFrame(FrameType_Response)
{
}

BackboneFrameResponse::~BackboneFrameResponse() {
}

bool BackboneFrameResponse::isResponseFrame() {
    return true;
}