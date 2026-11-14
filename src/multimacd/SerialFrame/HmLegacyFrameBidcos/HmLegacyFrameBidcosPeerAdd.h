/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameBidcos.h"
#include "../../BidcosAesKey.h"
class HmLegacyFrameBidcosPeerAdd :
	public HmLegacyFrameBidcos
{
public:
	HmLegacyFrameBidcosPeerAdd(void);
	virtual ~HmLegacyFrameBidcosPeerAdd(void);
	uint32_t GetRfAddress()const;
	void SetRfAddress(uint32_t rfAddress);
	uint8_t GetKeyIndex()const;
	void SetKeyIndex( uint8_t keyIndex );
	bool GetNeedsWakeupFlag()const;
	void SetNeedsWakeupFlag( bool needsWakeup );
	bool GetLazyConfigFlag()const;
	void SetLazyConfigFlag( bool lazyConfig );
	virtual std::string ToString()const;
};

