/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameBidcos.h"
class HmLegacyFrameBidcosPeerGetAesKeyId :
	public HmLegacyFrameBidcos
{
public:
	HmLegacyFrameBidcosPeerGetAesKeyId(void);
	virtual ~HmLegacyFrameBidcosPeerGetAesKeyId(void);
	uint32_t GetRfAddress()const;
	void SetRfAddress(uint32_t rfAddress);

	virtual std::string ToString()const;
};

