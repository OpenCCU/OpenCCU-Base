/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "LowLevelMacFrame.h"
class LowLevelMacFrameTx :
	public LowLevelMacFrame
{
public:
	LowLevelMacFrameTx();
	virtual ~LowLevelMacFrameTx(void);
	void SetFrameStartTimeAbsolute( uint16_t frameStartTime );
	void SetFrameStartTimeRelative( uint16_t frameStartTime );
	void SetFrameStartTime( uint16_t frameStartTime );
	uint16_t GetFrameStartTime()const;
	void SetOptions( int options );
	int GetOptions()const;
	BinaryData GetPayload()const;
	void SetPayload( const BinaryData& payload );
	virtual std::string ToString()const;
};

