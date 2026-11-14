/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#include "RouterFrame.h"



RouterFrame::RouterFrame(FrameType frameType) : SerialFrame( (uint8_t)FrameSubsystemType_Router, frameType ){
}

RouterFrame::~RouterFrame() {
	// TODO Auto-generated destructor stub
}

RouterFrame::FrameType RouterFrame::GetFrameType() const {
	return (FrameType)SerialFrame::GetCommand();
}

void RouterFrame::SetFrameType(FrameType frameType) {
	SerialFrame::SetCommand(frameType);
}
