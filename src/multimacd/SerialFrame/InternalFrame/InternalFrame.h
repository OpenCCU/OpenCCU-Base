/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "../SerialFrame.h"
class InternalFrame :
	public SerialFrame
{
public:

	enum FrameType
	{
		FrameType_Invalid,
		FrameType_Identify,
		FrameType_DutyCycle,
	};

	InternalFrame(FrameType frameType = FrameType_Invalid);
	virtual ~InternalFrame(void);
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
};

