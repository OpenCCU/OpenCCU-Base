/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameSystem.h"
class HmLegacyFrameSystemSetTxRate100k :
	public HmLegacyFrameSystem
{
public:
	HmLegacyFrameSystemSetTxRate100k(void);
	virtual ~HmLegacyFrameSystemSetTxRate100k(void);

	uint16_t GetSyncword()const;
	void SetSyncword( uint16_t syncword );

	virtual std::string ToString()const;
};

