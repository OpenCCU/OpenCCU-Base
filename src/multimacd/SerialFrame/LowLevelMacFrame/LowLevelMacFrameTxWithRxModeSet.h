/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "LowLevelMacFrame.h"
class LowLevelMacFrameTxWithRxModeSet :
	public LowLevelMacFrame
{
public:
	LowLevelMacFrameTxWithRxModeSet(void);
	virtual ~LowLevelMacFrameTxWithRxModeSet(void);
	void SetRxEndTimeRelative( uint16_t lockEndTime );
	void SetRxEndTimeAbsolute( uint16_t lockEndTime );
	void SetRxEndTime( uint16_t lockEndTime );
	uint16_t GetRxEndTime()const;
	void SetOptions( int options );
	int GetOptions()const;
	BinaryData GetPayload()const;
	void SetPayload( const BinaryData& payload );
	virtual std::string ToString()const;
};

