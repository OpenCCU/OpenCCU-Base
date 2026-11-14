/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CommonCommandFrame.h"

CommonCommandFrame::CommonCommandFrame(FrameType frameType) : SerialFrame( (uint8_t)FrameSubsystemType_Common, frameType )
{

}


CommonCommandFrame::~CommonCommandFrame(void)
{
}

CommonCommandFrame::FrameType CommonCommandFrame::GetFrameType()const
{
	return (FrameType)SerialFrame::GetCommand();
}

void CommonCommandFrame::SetFrameType( CommonCommandFrame::FrameType frameType )
{
	SerialFrame::SetCommand( (uint8_t)frameType );
}

