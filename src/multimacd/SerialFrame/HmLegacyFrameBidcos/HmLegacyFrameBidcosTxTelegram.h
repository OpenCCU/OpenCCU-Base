/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameBidcos.h"
class HmLegacyFrameBidcosTxTelegram :
	public HmLegacyFrameBidcos
{
public:
	enum BurstMode 
	{
		BurstMode_None,
		BurstMode_Burst,
		BurstMode_Triple = 3,
	};
	HmLegacyFrameBidcosTxTelegram(void);
	virtual ~HmLegacyFrameBidcosTxTelegram(void);
	void SetSendDelay( uint16_t sendDelay );
	uint16_t GetSendDelay()const;
	void SetBurstMode( BurstMode burstMode );
	BurstMode GetBurstMode()const;
	BinaryData GetPayload()const;
	void SetPayload( const BinaryData& payload );
	virtual std::string ToString()const;
};

