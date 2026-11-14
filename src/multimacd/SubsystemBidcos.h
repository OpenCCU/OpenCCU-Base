/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "Subsystem.h"
#include "BidcosContext.h"
#include "SerialFrame/HmLegacyFrameBidcos.h"
#include "BidcosMacController.h"
#include "SerialFrame/InternalFrame.h"
#include <queue>

class BinaryData;
class HmLegacyFrameSystem;
class LowLevelMacFrameRxTelegram;

class SubsystemBidcos :
	public Subsystem
{
public:
	SubsystemBidcos(void);
	~SubsystemBidcos(void);
	std::size_t ConsumeDownstreamBytes( uint8_t* buf, std::size_t len );
protected:
	bool CheckIfResponsibleForUpstreamFrame( const SerialFrame* frame );
	bool ProcessUpstreamFrame( SerialFrame* frame );
	bool ProcessDownstreamFrame( SerialFrame* frame );
	bool OnUpstreamConnect();
	void OnWorkerThreadStarted();
private:
	enum
	{
		MaxTxQueueSize = 5
	};

	void ProcessUpstreamFrameInternal( const InternalFrame& internalFrame );
	bool ProcessDownstreamSystemFrame( HmLegacyFrameSystem* frame );
	bool ProcessDownstreamHmLegacyFrame( HmLegacyFrameBidcos* frame );
	bool ProcessSetAesKeyFrame( HmLegacyFrameBidcosSetAesKey& frame );
	bool ProcessSetRfAddressFrame( HmLegacyFrameBidcosSetRfAddress& frame );
	bool ProcessGetRfAddressFrame( HmLegacyFrameBidcosGetRfAddress& frame );
	bool ProcessGetDefaultRfAddressFrame( HmLegacyFrameBidcosGetDefaultRfAddress& frame );
	bool ProcessPeerAddFrame( HmLegacyFrameBidcosPeerAdd& frame );
	bool ProcessPeerRemoveFrame( HmLegacyFrameBidcosPeerRemove& frame );
	bool ProcessPeerActivateAuthFrame( HmLegacyFrameBidcosPeerActivateAuth& frame );
	bool ProcessPeerDeactivateAuthFrame( HmLegacyFrameBidcosPeerDeactivateAuth& frame );
	bool ProcessPeerSetAesKeyIdFrame( HmLegacyFrameBidcosPeerSetAesKeyId& frame );
	bool ProcessPeerGetAesKeyIdFrame( HmLegacyFrameBidcosPeerGetAesKeyId& frame );
	bool ProcessGetPeersFrame( HmLegacyFrameBidcosGetPeers& frame );
	bool ProcessTxTelegramFrame( HmLegacyFrameBidcosTxTelegram& frame );
	bool CheckBidcosReceiverAndSendResponse( LowLevelMacFrameRxTelegram& frame );
	void ProcessTxQueue();
	uint32_t CanSleep();

	static bool CheckDutyCycleEventThreshold( uint8_t dutyCycleOld, uint8_t dutyCycleNew );


	unsigned char _rxBuf[1024];
	size_t _rxBufLength;

	BidcosContext _bidcosContext;

	std::queue<HmLegacyFrameBidcosTxTelegram*> _txQueue;

	BidcosMacController _bidcosMacController;

	InternalFrameIdentify _internalIdentifyFrame;
	uint8_t _dutyCycle;

	static const uint8_t DUTY_CYCLE_EVENT_THRESHOLDS[];

	friend class BidcosMacController;

};

