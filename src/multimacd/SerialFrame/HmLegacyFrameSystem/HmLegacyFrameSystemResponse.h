/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameSystem.h"
class HmLegacyFrameSystemResponse :
	public HmLegacyFrameSystem
{
public:
	enum ResponseCode
	{
		ResponseCode_Error,
		ResponseCode_Ok,
		ResponseCode_OkWithData,
		ResponseCode_Busy,
		ResponseCode_InputError,
	};


	HmLegacyFrameSystemResponse(void);
	virtual ~HmLegacyFrameSystemResponse(void);
	ResponseCode GetResponseCode()const;
	void SetResponseCode( ResponseCode responseCode );

	//ResponseCode_OkWithData for FrameType_VersionRequest
	uint32_t GetBootloaderVersion()const;
	void SetBootloaderVersion( uint32_t bootloaderVersion );
	uint32_t GetApplicationVersion()const;
	void SetApplicationVersion( uint32_t applicationVersion );

	//ResponseCode_OkWithData for FrameType_Identify
	std::string GetIdentification()const;
	void SetIdentification(const std::string& identification);

	//ResponseCode_OkWithData for FrameType_GetSerialNumber
	std::string GetSerialNumber()const;
	void SetSerialNumber(const std::string& serialNumber);

	//ResponseCode_OkWithData for FrameType_DutyCycleRequest
	uint8_t GetDutyCycle()const;
	void SetDutyCycle( uint8_t dutyCycle );

	virtual std::string ToString()const;

};

