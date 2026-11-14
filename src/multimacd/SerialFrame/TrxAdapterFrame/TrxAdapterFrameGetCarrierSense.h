/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "TrxAdapterFrame.h"
class TrxAdapterFrameGetCarrierSense :
	public TrxAdapterFrame
{
public:
	TrxAdapterFrameGetCarrierSense(void);
	virtual ~TrxAdapterFrameGetCarrierSense(void);
	virtual std::string ToString()const;
};

