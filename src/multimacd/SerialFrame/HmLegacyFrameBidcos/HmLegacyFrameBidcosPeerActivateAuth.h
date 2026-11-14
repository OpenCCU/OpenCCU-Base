/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameBidcos.h"
class HmLegacyFrameBidcosPeerActivateAuth :
	public HmLegacyFrameBidcos
{
public:
	HmLegacyFrameBidcosPeerActivateAuth(void);
	virtual ~HmLegacyFrameBidcosPeerActivateAuth(void);
	uint32_t GetRfAddress()const;
	void SetRfAddress(uint32_t rfAddress);
	uint64_t GetChannels()const;
	void SetChannels( uint64_t channels );
	virtual std::string ToString()const;
protected:
	HmLegacyFrameBidcosPeerActivateAuth(FrameType frameType);
};

