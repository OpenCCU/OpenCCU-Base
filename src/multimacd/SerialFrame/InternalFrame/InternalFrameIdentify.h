/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "InternalFrame.h"
class InternalFrameIdentify :
	public InternalFrame
{
public:
	InternalFrameIdentify(void);
	virtual ~InternalFrameIdentify(void);
	BinaryData GetSgtin()const;
	void SetSgtin(const BinaryData& sgtin);
	uint32_t GetBootloaderVersion()const;
	void SetBootloaderVersion( uint32_t bootloaderVersion );
	uint32_t GetApplicationVersion()const;
	void SetApplicationVersion( uint32_t applicationVersion );
	uint32_t GetHmosVersion()const;
	void SetHmosVersion( uint32_t hmosVersion );
	uint32_t GetDefaultRfAddress()const;
	void SetDefaultRfAddress( uint32_t rfAddress );
	std::string GetIdentification()const;
	void SetIdentification(const std::string& identification);
	std::string GetSerialNumber()const;
	void SetSerialNumber(const std::string& serialNumber);
	std::string GetIdentificationHmIP()const;
	std::string GetIdentificationBidcos()const;

	virtual std::string ToString()const;
};

extern std::string const IDENTIFICATION_INTERNAL_APP;
extern std::string const IDENTIFICATION_INTERNAL_BOOTLOADER;
extern std::string const IDENTIFICATION_BIDCOS_APP;
extern std::string const IDENTIFICATION_BIDCOS_BOOTLOADER;
extern std::string const IDENTIFICATION_HMIP_APP;
extern std::string const IDENTIFICATION_HMIP_BOOTLOADER;
extern std::string const IDENTIFICATION_DUAL_APP;
