/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "tinythread.h"
#include "TelegramQueue.h"
#include "IDownstreamSerialFrameSink.h"
#include "IUpstreamSerialFrameSink.h"
#include "MacControllerQueueItem.h"
#include "SerialFrame/InternalFrame.h"
#include <stdint.h>
#include <map>

#pragma once
class SerialFrame;

class MacController : public IDownstreamSerialFrameSink, public IUpstreamSerialFrameSink
{
public:
	MacController(void);
	~MacController(void);
	bool Start( const std::string& device );
	bool Stop();
	void SetUpstreamFrameSink( IUpstreamSerialFrameSink& frameSink );
	void OnDownstreamFrame( SerialFrame* frame );
	void OnUpstreamFrame( SerialFrame* frame );
	bool IsInitializationDone() const;
private:
	enum SendResult
	{
		SendResult_Success,
		SendResult_Failed,
		SendResult_TryAgain,
	};
	enum CoproState
	{
		CoproState_Unknown,
		CoproState_Identify,
		CoproState_IdentifyLegacyBootloader,
		CoproState_Bootloader,
		CoproState_StartApplication,
		CoproState_GetVersion,
		CoproState_GetSgtin,
		CoproState_GetSerialNumber,
		CoproState_GetRfAddress,
		CoproState_GetTimer,
		CoproState_Application,
		CoproState_WaitExit,
	};
	enum TrxSubsystemState
	{
		TrxSubsystemState_Idle,
		TrxSubsystemState_WaitApplicationResponse,
		TrxSubsystemState_WaitDutyCycleResponse
	};

	enum
	{
		DutyCyclePollInterval = 10000,
		TrxSubsystemResponseTimeout = 1000,
		InitTimeout = 1000,
	};

    int OpenPort(const char* device);

	SendResult SendFrameToCoprocessor( SerialFrame* frame );
	SendResult SendInitSequenceFrameToCoprocessor( SerialFrame* frame );
	void HandleFrameFromCoprocessor( SerialFrame* frame );
	void HandleInitFrameFromCoprocessor( SerialFrame* frame );
	bool HandleTrxSubsystemFrameFromCoprocessor( SerialFrame* frame );

	uint8_t GetNextDownstreamSequenceCounter();

	void OnTrxSubsystemTimer();
	void OnInitSequenceTimeout();

	void ThreadFunction();
	static void sThreadFunction(void* arg)
	{
		reinterpret_cast<MacController*>(arg)->ThreadFunction();
	}
	tthread::thread _thread;
	int _fd;
	volatile bool _exit;
	uint8_t _txBuffer[1024];

	typedef std::map<uint16_t, SerialFrame*> MapOpenFrames;
	MapOpenFrames _mapOpenFrames;

	TelegramQueue<MacControllerQueueItem*> _incomingQueue;
	IUpstreamSerialFrameSink* _frameSink;

	uint8_t _nextDownstreamSequenceCounter;
	uint32_t _lastRequestFrameId;

	uint32_t _nextTrxSubsystemTimer;
	uint32_t _nextDutyCyclePoll;
	uint32_t _initSequenceTimer;

	InternalFrameIdentify _internalIdentifyFrame;

	CoproState _coproState;

	TrxSubsystemState _trxSubsystemState;

	int32_t _initSendErrCnt;
	volatile bool _initializationDone;
};

