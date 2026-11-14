/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameSystem.h"
class HmLegacyFrameSystemDutyCycleControl :
	public HmLegacyFrameSystem
{
public:
	HmLegacyFrameSystemDutyCycleControl(void);
	virtual ~HmLegacyFrameSystemDutyCycleControl(void);
	bool GetActiveFlag()const;
	void SetActiveFlag( bool active );
	virtual std::string ToString()const;
};

