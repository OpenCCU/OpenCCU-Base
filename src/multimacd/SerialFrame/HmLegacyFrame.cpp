/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrame.h"

HmLegacyFrame::HmLegacyFrame(SubsystemType subsystemType, uint8_t frameType) : SerialFrame( (uint8_t)subsystemType, frameType )
{
}

HmLegacyFrame::HmLegacyFrame()
{
}


HmLegacyFrame::~HmLegacyFrame(void)
{
}

HmLegacyFrame::SubsystemType HmLegacyFrame::GetSubsystemType()const
{
	return (SubsystemType)SerialFrame::GetSubsystem();
}

/*static*/ HmLegacyFrame* HmLegacyFrame::Create( const unsigned char* buffer, size_t length, size_t* pNumEaten )
{
	size_t numEaten = 0;
	while( length && (*buffer != StartChar) )
	{
		numEaten++;
		buffer++;
		length--;
	}
	if( numEaten )
	{
		*pNumEaten = numEaten;
		return NULL;
	}
	uint8_t sequenceCounter;
	uint8_t subsystemType;
	uint8_t command;
	BinaryData data;
	if( !Parse( buffer, length, pNumEaten, &sequenceCounter, &subsystemType, &command, &data ) )
	{
		return NULL;
	}
	HmLegacyFrame* frame = Dynamic::Order<HmLegacyFrame>::Create( (subsystemType << 8) | command );
	if( frame )
	{
		frame->SetSequenceCounter( sequenceCounter );
		frame->Data() = data;
	}
	return frame;

}

