/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BackboneFrame.h"

BackboneFrame::BackboneFrame(FrameType frameType) : SerialFrame( (uint8_t)FrameSubsystemType_Backbone, frameType ) {
}

BackboneFrame::~BackboneFrame() {
}

BackboneFrame::FrameType BackboneFrame::GetFrameType() const {
	return (FrameType)SerialFrame::GetCommand();
}

void BackboneFrame::SetFrameType(FrameType frameType) {
	SerialFrame::SetCommand(frameType);
}