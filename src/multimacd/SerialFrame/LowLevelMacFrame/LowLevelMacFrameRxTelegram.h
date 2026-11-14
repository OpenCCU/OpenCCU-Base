/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "LowLevelMacFrame.h"
class LowLevelMacFrameRxTelegram :
	public LowLevelMacFrame
{
public:
	LowLevelMacFrameRxTelegram();
	virtual ~LowLevelMacFrameRxTelegram(void);
	void SetFrameEndTime( uint16_t frameEndTime );
	uint16_t GetFrameEndTime()const;
	void SetOptions( int options );
	int GetOptions()const;
	int GetRssi()const;
	void SetRssi( int rssi );
	BinaryData GetPayload()const;
	void SetPayload( const BinaryData& payload );
	virtual std::string ToString()const;
protected:
	LowLevelMacFrameRxTelegram(FrameType frameType);

};

