/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "../HmLegacyFrame.h"
class HmLegacyFrameSystem :
	public HmLegacyFrame
{
public:
	enum FrameType 
	{
		FrameType_Identify,
		FrameType_VersionRequest = 2,
		FrameType_StartBootloader,
		FrameType_Response,
		FrameType_DutyCycleEvent,
		FrameType_SetTxRate10k,
		FrameType_SetTxRate100k,
		FrameType_DutyCycleRequest,
		FrameType_DutyCycleControl,
		FrameType_CsmaCaControl,
		FrameType_GetSerialNumber,
		FrameType_SetTeststatus,
		FrameType_GetTeststatus,
		FrameType_SetTime,
	};
	virtual ~HmLegacyFrameSystem(void);
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
protected:
	HmLegacyFrameSystem(FrameType frameType);
};

