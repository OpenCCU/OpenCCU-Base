/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "../SerialFrame.h"
class CommonCommandFrame :
	public SerialFrame
{
public:

	enum FrameType
	{
		FrameType_Invalid = 0xff,
		FrameType_Identify = 0,
		FrameType_IdentifyRequest,
		FrameType_StartBootloader,
		FrameType_StartApplication,
		FrameType_GetSGTIN,
		FrameType_Response,
	};

	CommonCommandFrame(FrameType frameType = FrameType_Invalid);
	virtual ~CommonCommandFrame(void);
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
};

