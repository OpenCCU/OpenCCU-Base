/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "../SerialFrame.h"
class LowLevelMacFrame :
	public SerialFrame
{
public:

	enum FrameType
	{
		FrameType_Invalid,
		FrameType_Response,
		FrameType_GetTimestamp,
		FrameType_TxWitRxModeSet,
		FrameType_RxModeDefault,
		FrameType_RxTelegram,
		FrameType_Tx,
		FrameType_GetSerialNumber,
		FrameType_GetDefaultRfAddress,
	};

	enum Option
	{
		Option_Band_868_10k = 0,
		Option_Band_868_100k = 1,
		Option_Band_869_10k = 2,
		Option_Band_869_50k = 3,
		Option_Aes = (1<<3),
		Option_Burst = (1<<4),
		Option_CcaOff = (1<<7),
	};

	enum
	{
		Timestamp_Absolute = 0x0000,
		Timestamp_Relative = 0x8000,
	};

	LowLevelMacFrame(FrameType frameType = FrameType_Invalid);
	virtual ~LowLevelMacFrame(void);
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
};

