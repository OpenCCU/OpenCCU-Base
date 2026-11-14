/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "BinaryData.h"
class BidcosTelegram
{
public:
	enum 
	{
		Flag_Wakeup = 1,
		Flag_WakeMeUp = 2,
		Flag_Broadcast = 4,
		Flag_Reserved = 8,
		Flag_Burst = 16,
		Flag_BiDi = 32,
		Flag_Repeated = 64,
		Flag_RepeatEnable = 128
	};
	enum FrameType
	{
		FrameType_Sysinfo = 0x00,
		FrameType_Configuration = 0x01,
		FrameType_Ack = 0x02,
		FrameType_AesSolution = 0x03,
		FrameType_AesContainer = 0x04,
		FrameType_Testmode = 0x09,
		FrameType_Info = 0x10,
		FrameType_CentralCommand = 0x11,
		FrameType_Wakeup = 0x12,
		FrameType_CallCcu = 0x13,
		FrameType_Alarm = 0x3d,
		FrameType_Simulation = 0x3e,
		FrameType_TimeInfo = 0x3f,
		FrameType_SwitchUnconditional = 0x40,
		FrameType_SwitchConditional = 0x41,
		FrameType_SwitchLevel = 0x42,
		FrameType_HvacSetpoint = 0x58,
		FrameType_HvacControlMode = 0x59,
		FrameType_HvacTemperatureInfo = 0x5a,
		FrameType_WeatherDataRequest = 0x7f,
		FrameType_WeatherData = 0x70,
		FrameType_WeatherForecast = 0x71,
		FrameType_DataloggerCommand = 0x51,
		FrameType_DataloggerData = 0x52,
		FrameType_MeasurementCyclic = 0x53,
		FrameType_MeasurementSpontaenous = 0x54,
		FrameType_TextMessage = 0x55,
		FrameType_TextOrganization = 0x56,
		FrameType_EnergyCyclic = 0x5e,
		FrameType_EnergySpontaenous = 0x5f,
		FrameType_Update = 0xca,
		FrameType_TrxRegister = 0xcb,
	};
	enum
	{
		AckCode_Ack,
		AckCode_AckWithStatus,
		AckCode_AckParameterIgnored,
		AckCode_AckParameterCorrected,
		AckCode_AckWithAesChallenge,
		AckCode_Nak = 0x80,
		AckCode_NakBusy,
		AckCode_NakOomPartiallyProcessed,
		AckCode_NakOomNotProcessed,
		AckCode_NakUnknownTarget,
		AckCode_NakUnknownChannel
	};
	enum
	{
		AesContainerType_KeyExchange = 1,
	};
	BidcosTelegram(const BinaryData& telegramData);
	BidcosTelegram(void);
	~BidcosTelegram(void);
	uint8_t GetTelegramCounter()const;
	void SetTelegramCounter( uint8_t telegramCounter );
	uint8_t GetFlags()const;
	void SetFlags( uint8_t flags );
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
	uint32_t GetSenderAddress()const;
	void SetSenderAddress( uint32_t senderAddress );
	uint32_t GetReceiverAddress()const;
	void SetReceiverAddress( uint32_t receiverAddress );
	const BinaryData GetPayload()const;
	void SetPayload( const BinaryData& payload );
	const BinaryData& GetRawData()const;
	bool CheckCrc()const;
	bool CheckAndStripCrc();
	void AddCrc();
	uint8_t GetAckCode()const;

	virtual std::string ToString()const;
private:
	enum {
		Crc16Poly = 0x8005,
	};
	BinaryData _binaryData;
};

