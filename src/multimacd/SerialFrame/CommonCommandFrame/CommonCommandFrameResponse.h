/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "CommonCommandFrame.h"
class CommonCommandFrameResponse :
	public CommonCommandFrame
{
public:
	enum ResponseCode {
		ResponseCode_Error,
		ResponseCode_Ack,
	};

	CommonCommandFrameResponse(void);
	virtual ~CommonCommandFrameResponse(void);
	ResponseCode GetResponseCode()const;
	void SetResponseCode( ResponseCode responseCode );
	bool IsAck()const;
	BinaryData GetPayload()const;
	void SetPayload( const BinaryData& payload );
	bool isResponseFrame();
	virtual std::string ToString()const;
};

