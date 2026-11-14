/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystem.h"


HmLegacyFrameSystem::HmLegacyFrameSystem(FrameType frameType) : HmLegacyFrame( SubsystemType_System, (uint8_t)frameType )
{
}

HmLegacyFrameSystem::~HmLegacyFrameSystem(void)
{
}

HmLegacyFrameSystem::FrameType HmLegacyFrameSystem::GetFrameType()const
{
	return (FrameType)SerialFrame::GetCommand();
}

void HmLegacyFrameSystem::SetFrameType( HmLegacyFrameSystem::FrameType frameType )
{
	SerialFrame::SetCommand( (uint8_t)frameType );
}

