/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameBidcos.h"
class HmLegacyFrameBidcosRxTelegram :
	public HmLegacyFrameBidcos
{
public:
	enum
	{
		Flag_AuthNone = 0,
		Flag_AuthNotNecessary,
		Flag_AuthSuccessful,
		Flag_AuthFailed,
		Flag_AuthUnknownKey,
		Flag_Wokenup = 16,
	};
	HmLegacyFrameBidcosRxTelegram(void);
	virtual ~HmLegacyFrameBidcosRxTelegram(void);
	void SetFlags( int flags );
	int GetFlags()const;
	int GetRssi()const;
	void SetRssi( int rssi );
	uint8_t GetKeyIndex()const;
	void SetKeyIndex( uint8_t keyIndex );
	BinaryData GetPayload()const;
	void SetPayload( const BinaryData& payload );
	virtual std::string ToString()const;
};

