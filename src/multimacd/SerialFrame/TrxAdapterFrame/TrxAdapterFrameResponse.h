/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "TrxAdapterFrame.h"
class TrxAdapterFrameResponse :
	public TrxAdapterFrame
{
public:
	enum ResponseCode {
		ResponseCode_Error,
		ResponseCode_Ack,
	};

	TrxAdapterFrameResponse(void);
	virtual ~TrxAdapterFrameResponse(void);
	ResponseCode GetResponseCode()const;
	void SetResponseCode( ResponseCode responseCode );
	bool IsAck()const;
	BinaryData GetPayload()const;

	uint32_t GetBootloaderVersion()const;
	void SetBootloaderVersion( uint32_t bootloaderVersion );
	uint32_t GetApplicationVersion()const;
	void SetApplicationVersion( uint32_t applicationVersion );
	uint32_t GetHmosVersion()const;
	void SetHmosVersion( uint32_t hmosVersion );

	uint8_t GetDutyCycle()const;
	void SetDutyCycle( uint8_t dutyCycle );

	void SetPayload( const BinaryData& payload );
	bool isResponseFrame();

	virtual std::string ToString()const;
};

