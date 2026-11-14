/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcos.h"
#include <stdio.h>

HmLegacyFrameBidcos::HmLegacyFrameBidcos(FrameType frameType) : HmLegacyFrame( SubsystemType_Bidcos, (uint8_t)frameType )
{
}

HmLegacyFrameBidcos::~HmLegacyFrameBidcos(void)
{
}

HmLegacyFrameBidcos::FrameType HmLegacyFrameBidcos::GetFrameType()const
{
	return (FrameType)SerialFrame::GetCommand();
}

void HmLegacyFrameBidcos::SetFrameType( HmLegacyFrameBidcos::FrameType frameType )
{
	SerialFrame::SetCommand( (uint8_t)frameType );
}

std::string HmLegacyFrameBidcos::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos ";
	snprintf( buffer, sizeof(buffer), "%d ", GetFrameType() );
	s += buffer;
	s += Data().ToString();
	return s;
}

