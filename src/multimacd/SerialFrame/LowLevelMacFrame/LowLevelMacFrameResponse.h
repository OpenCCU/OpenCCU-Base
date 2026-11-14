/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "LowLevelMacFrame.h"
class LowLevelMacFrameResponse :
	public LowLevelMacFrame
{
public:
	enum ResponseCode {
		ResponseCode_Error,
		ResponseCode_Ack,
		ResponseCode_Busy,
		ResponseCode_InvalidInput,
		ResponseCode_CcaFailed,
		ResponseCode_Abort,
		ResponseCode_WrongFreqBitrate,
		ResponseCode_BusyTrx,
		ResponseCode_DutyCycleFull
	};

	LowLevelMacFrameResponse(void);
	virtual ~LowLevelMacFrameResponse(void);
	ResponseCode GetResponseCode()const;
	void SetResponseCode( ResponseCode responseCode );
	bool IsAck()const;
	BinaryData GetPayload()const;
	void SetPayload( const BinaryData& payload );
	bool isResponseFrame();
	virtual std::string ToString()const;
};

