/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "InternalFrame.h"
class InternalFrameDutyCycle :
	public InternalFrame
{
public:
	InternalFrameDutyCycle(void);
	virtual ~InternalFrameDutyCycle(void);
	uint8_t GetDutyCylce()const;
	void SetDutyCycle(uint8_t dutyCycle);

	virtual std::string ToString()const;
};

