/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "InternalFrameIdentify.h"
#include <stdio.h>
#include <cinttypes>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<SerialFrame, InternalFrameIdentify, int> m((SerialFrame::FrameSubsystemType_Internal<<8)|InternalFrame::FrameType_Identify);
/// @endcond

extern std::string const IDENTIFICATION_INTERNAL_APP = "BL";
extern std::string const IDENTIFICATION_INTERNAL_BOOTLOADER = "APP";
extern std::string const IDENTIFICATION_BIDCOS_APP = "Co_CPU_App";
extern std::string const IDENTIFICATION_BIDCOS_BOOTLOADER = "Co_CPU_BL";
extern std::string const IDENTIFICATION_HMIP_APP = "HMIP_TRX_App";
extern std::string const IDENTIFICATION_HMIP_BOOTLOADER = "HMIP_TRX_Bl";
extern std::string const IDENTIFICATION_DUAL_APP = "DualCoPro_App";


InternalFrameIdentify::InternalFrameIdentify(void) : InternalFrame( FrameType_Identify )
{
}


InternalFrameIdentify::~InternalFrameIdentify(void)
{
}

BinaryData InternalFrameIdentify::GetSgtin()const
{
	return Data().GetRange(0, 12 );
}
void InternalFrameIdentify::SetSgtin(const BinaryData& sgtin)
{
	Data().SetRange(0, sgtin.GetRange(0, 12) );
}
uint32_t InternalFrameIdentify::GetBootloaderVersion()const
{
	return Data().GetUInt24Value( 12 );
}
void InternalFrameIdentify::SetBootloaderVersion( uint32_t bootloaderVersion )
{
	Data().SetUInt24Value( 12, bootloaderVersion );
}
uint32_t InternalFrameIdentify::GetApplicationVersion()const
{
	return Data().GetUInt24Value( 15 );
}
void InternalFrameIdentify::SetApplicationVersion( uint32_t applicationVersion )
{
	Data().SetUInt24Value( 15, applicationVersion );
}
uint32_t InternalFrameIdentify::GetHmosVersion()const
{
	return Data().GetUInt24Value( 18 );
}
void InternalFrameIdentify::SetHmosVersion( uint32_t hmosVersion )
{
	Data().SetUInt24Value( 18, hmosVersion );
}
uint32_t InternalFrameIdentify::GetDefaultRfAddress()const
{
	return Data().GetUInt24Value( 21 );
}
void InternalFrameIdentify::SetDefaultRfAddress( uint32_t rfAddress )
{
	Data().SetUInt24Value( 21, rfAddress );
}

std::string InternalFrameIdentify::GetSerialNumber()const
{
	return Data().GetRange( 24, 10 ).GetStringValue(0);
}

void InternalFrameIdentify::SetSerialNumber(const std::string& serialNumber)
{
	Data().SetRange( 24, BinaryData(serialNumber.c_str()).GetRange(0, 10) );
}

std::string InternalFrameIdentify::GetIdentification()const
{
	return Data().GetStringValue( 34 );
}

void InternalFrameIdentify::SetIdentification(const std::string& identification)
{
	//CN: Overwriting identifaction string in same InternalFrameIdentify object fails for different identification string sizes (leftover from previous string). 
	//Thus it will be resized according to new string length before writing.
	if(Data().size() > 35) {
		Data().resize(34+identification.size());
	}
	Data().SetStringValue( 34, identification );
}

std::string InternalFrameIdentify::GetIdentificationHmIP() const 
{
	const std::string internalId = GetIdentification();
	if(internalId == IDENTIFICATION_INTERNAL_APP) 
	{
		return IDENTIFICATION_HMIP_APP;
	}
	else if(internalId == IDENTIFICATION_INTERNAL_BOOTLOADER) 
	{
		return IDENTIFICATION_HMIP_BOOTLOADER;
	}
	else {
		return std::string("UNKNOWN");
	}
}

std::string InternalFrameIdentify::GetIdentificationBidcos() const 
{
	const std::string internalId = GetIdentification();
	if(internalId == IDENTIFICATION_INTERNAL_APP) 
	{
		return IDENTIFICATION_BIDCOS_APP;
	}
	else if(internalId == IDENTIFICATION_INTERNAL_BOOTLOADER) 
	{
		return IDENTIFICATION_BIDCOS_BOOTLOADER;
	}
	else {
		return std::string("UNKNOWN");
	}
}

std::string InternalFrameIdentify::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " Internal Identify ";

	s += GetIdentification();

	s += " SGTIN=";
	s += GetSgtin().ToString();

	s += " Vapp=";
	int version = GetApplicationVersion();
	snprintf( buffer, sizeof(buffer), "%d.%d.%d", version >> 16, (version >> 8) & 0xff, version & 0xff );
	s += buffer;

	s += " Vbl=";
	version = GetBootloaderVersion();
	snprintf( buffer, sizeof(buffer), "%d.%d.%d", version >> 16, (version >> 8) & 0xff, version & 0xff );
	s += buffer;

	s += " Vos=";
	version = GetHmosVersion();
	snprintf( buffer, sizeof(buffer), "%d.%d.%d", version >> 16, (version >> 8) & 0xff, version & 0xff );
	s += buffer;

	s += " SN=";
	s += GetSerialNumber();
	s += " RF-Address=";
	snprintf( buffer, sizeof(buffer), "0x%06" PRIX32, GetDefaultRfAddress() );
	s += buffer;

	return s;
}
