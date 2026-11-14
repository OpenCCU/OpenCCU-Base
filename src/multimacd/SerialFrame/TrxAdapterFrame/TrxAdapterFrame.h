/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "../SerialFrame.h"
class TrxAdapterFrame :
	public SerialFrame
{
public:

	enum FrameType
	{
		FrameType_Invalid = 0xff,
		FrameType_GetVersion = 2,
		FrameType_GetDutyCycle,
		FrameType_Response,
		FrameType_SetDataRate,
		FrameType_DutyCycleEvent,
		FrameType_FactoryReset = 8,
		FrameType_GetCarrierSense = 10,
		FrameType_CarrierSenseEvent = 11,
	};

	TrxAdapterFrame(FrameType frameType = FrameType_Invalid);
	virtual ~TrxAdapterFrame(void);
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
};

