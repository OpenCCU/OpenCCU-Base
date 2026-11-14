/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosTelegram.h"
#include "Crc16.h"
#include <stdio.h>
#include <cinttypes>

BidcosTelegram::BidcosTelegram(const BinaryData& telegramData)
{
	_binaryData = telegramData;
}

BidcosTelegram::BidcosTelegram()
{
}

BidcosTelegram::~BidcosTelegram(void)
{
}

uint8_t BidcosTelegram::GetTelegramCounter()const
{
	return _binaryData.GetUInt8Value( 0 );
}

void BidcosTelegram::SetTelegramCounter( uint8_t telegramCounter )
{
	_binaryData.SetUInt8Value( 0, telegramCounter );
}

uint8_t BidcosTelegram::GetFlags()const
{
	return _binaryData.GetUInt8Value( 1 );
}

void BidcosTelegram::SetFlags( uint8_t flags )
{
	_binaryData.SetUInt8Value( 1, flags );
}

BidcosTelegram::FrameType BidcosTelegram::GetFrameType()const
{
	return (FrameType)_binaryData.GetUInt8Value( 2 );
}

void BidcosTelegram::SetFrameType( FrameType frameType )
{
	_binaryData.SetUInt8Value( 2, (uint8_t)frameType );
}

uint32_t BidcosTelegram::GetSenderAddress()const
{
	return _binaryData.GetUInt24Value( 3 );
}

void BidcosTelegram::SetSenderAddress( uint32_t senderAddress )
{
	_binaryData.SetUInt24Value( 3, senderAddress );
}

uint32_t BidcosTelegram::GetReceiverAddress()const
{
	return _binaryData.GetUInt24Value( 6 );
}

void BidcosTelegram::SetReceiverAddress( uint32_t receiverAddress )
{
	_binaryData.SetUInt24Value( 6, receiverAddress );
}

const BinaryData BidcosTelegram::GetPayload()const
{
	if( _binaryData.size() <= 9 )
	{
		return BinaryData();
	}
	return _binaryData.GetRange( 9, _binaryData.size() - 9 );
}

void BidcosTelegram::SetPayload( const BinaryData& payload )
{
	_binaryData.SetRange( 9, payload );
}

const BinaryData& BidcosTelegram::GetRawData()const
{
	return _binaryData;
}

uint8_t BidcosTelegram::GetAckCode()const
{
	return _binaryData.GetUInt8Value( 9 );
}

std::string BidcosTelegram::ToString()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "#%02X[", GetTelegramCounter() );
	std::string s = buffer;
	uint8_t flags = GetFlags();
	if( flags & Flag_BiDi )
	{
		s += "BiDi|";
	}
	if( flags & Flag_Broadcast )
	{
		s += "BC|";
	}
	if( flags & Flag_Burst )
	{
		s += "WOR|";
	}
	if( flags & Flag_Repeated )
	{
		s += "Rpted|";
	}
	if( flags & Flag_RepeatEnable )
	{
		s += "Ren|";
	}
	if( flags & Flag_Wakeup )
	{
		s += "Wup|";
	}
	if( flags & Flag_WakeMeUp )
	{
		s += "WMup|";
	}
	if( s[s.length()-1] == '|' )
	{
		s[s.length()-1] = ']';
	}else{
		s += "]";
	}
	s += " ";
	snprintf( buffer, sizeof(buffer), "%06" PRIX32 "->%06" PRIX32 " ", GetSenderAddress(), GetReceiverAddress() );
	s += buffer;
	switch( GetFrameType() )
	{
	case FrameType_Sysinfo:
		s += "Sysinfo";
		s += " SN=";
		s += GetPayload().GetRange( 3, 10 ).GetStringValue(0);
		s += " Type=";
		snprintf( buffer, sizeof(buffer), "0x%04" PRIX16 "/0x%04" PRIX16, GetPayload().GetUInt16Value( 1 ), GetPayload().GetUInt16Value( 12 ) );
		s += buffer;
		s += "CH=";
		snprintf( buffer, sizeof(buffer), "%" PRIu16 "/%" PRIu16, GetPayload().GetUInt8Value( 14 ), GetPayload().GetUInt16Value( 15 ) );
		return s;
	case FrameType_Configuration:
		s += "Configuration";
		break;
	case FrameType_Ack:
		s += "Ack";
		break;
	case FrameType_AesSolution:
		s += "AesSolution";
		break;
	case FrameType_AesContainer:
		s += "AesContainer";
		break;
	case FrameType_Testmode:
		s += "Testmode";
		break;
	case FrameType_Info:
		s += "Info";
		break;
	case FrameType_CentralCommand:
		s += "CentralCommand";
		break;
	case FrameType_Wakeup:
		s += "Wakeup";
		break;
	case FrameType_CallCcu:
		s += "CallCcu";
		break;
	case FrameType_Alarm:
		s += "Alarm";
		break;
	case FrameType_Simulation:
		s += "Simulation";
		break;
	case FrameType_TimeInfo:
		s += "TimeInfo";
		break;
	case FrameType_SwitchUnconditional:
		s += "SwitchUnconditional";
		break;
	case FrameType_SwitchConditional:
		s += "SwitchConditional";
		break;
	case FrameType_SwitchLevel:
		s += "SwitchLevel";
		break;
	case FrameType_HvacSetpoint:
		s += "HvacSetpoint";
		break;
	case FrameType_HvacControlMode:
		s += "HvacControlMode";
		break;
	case FrameType_HvacTemperatureInfo:
		s += "HvacTemperatureInfo";
		break;
	case FrameType_WeatherDataRequest:
		s += "WeatherDataRequest";
		break;
	case FrameType_WeatherData:
		s += "WeatherData";
		break;
	case FrameType_WeatherForecast:
		s += "WeatherForecast";
		break;
	case FrameType_DataloggerCommand:
		s += "DataloggerCommand";
		break;
	case FrameType_DataloggerData:
		s += "DataloggerData";
		break;
	case FrameType_MeasurementCyclic:
		s += "MeasurementCyclic";
		break;
	case FrameType_MeasurementSpontaenous:
		s += "MeasurementSpontaenous";
		break;
	case FrameType_TextMessage:
		s += "TextMessage";
		break;
	case FrameType_TextOrganization:
		s += "TextOrganization";
		break;
	case FrameType_EnergyCyclic:
		s += "EnergyCyclic";
		break;
	case FrameType_EnergySpontaenous:
		s += "EnergySpontaenous";
		break;
	case FrameType_Update:
		s += "Update";
		break;
	case FrameType_TrxRegister:
		s += "TrxRegister";
		break;
	}
	s += ": ";
	s += GetPayload().ToString().c_str();
	return s;

}
