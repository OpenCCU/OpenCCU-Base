/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include <stdint.h>
#include "BidcosAesKey.h"
class BidcosPeerContext
{
public:
	enum
	{
		Flag_Active = 1,
		Flag_NeedsWakeup = 2,
		Flag_LazyConfig = 4,
		Flag_FirstWakeup = 8,
		Flag_WakeMeUp = 16,
		Flag_WakeupSent = 32,
	};
	BidcosPeerContext();
	~BidcosPeerContext(void);
	void Init( uint32_t rfAddress, uint8_t aesKeyIndex, int flags );
	uint32_t GetRfAddress()const;
	void Update( uint8_t aesKeyIndex, int flags );
	void ResetFlags( int flags );
	void SetFlags( int flags );
	bool IsActive()const;
	void Deactivate();
	void SetAesChannelMask( uint64_t aesChannelMask );
	uint64_t GetAesChannelMask()const;
	bool NeedsAesForChannel( uint8_t channelIndex );
	int GetFlags()const;
	uint8_t GetAesKeyIndex()const;
private:
	uint32_t _rfAddress;
	uint8_t _aesKeyIndex;
	int _flags;
	uint64_t _aesChannelMask;
};

