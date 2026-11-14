/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameSystem.h"
class HmLegacyFrameSystemDutyCycleEvent :
	public HmLegacyFrameSystem
{
public:
	HmLegacyFrameSystemDutyCycleEvent(void);
	virtual ~HmLegacyFrameSystemDutyCycleEvent(void);

	uint8_t GetDutyCycle()const;
	void SetDutyCycle( uint8_t dutyCycle );

	virtual std::string ToString()const;
};

