/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "InternalFrame.h"

InternalFrame::InternalFrame(FrameType frameType) : SerialFrame( (uint8_t)FrameSubsystemType_Internal, frameType )
{

}


InternalFrame::~InternalFrame(void)
{
}

InternalFrame::FrameType InternalFrame::GetFrameType()const
{
	return (FrameType)SerialFrame::GetCommand();
}

void InternalFrame::SetFrameType( InternalFrame::FrameType frameType )
{
	SerialFrame::SetCommand( (uint8_t)frameType );
}

