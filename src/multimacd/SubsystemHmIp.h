/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "Subsystem.h"
#include "SerialFrame/InternalFrame.h"
#include "SerialFrame/CommonCommandFrame.h"
#include "SerialFrame/TrxAdapterFrame.h"
#include "SerialFrame/HmIpStackFrame.h"
#include "SerialFrame/RouterFrame.h"

class SubsystemHmIp :
	public Subsystem
{
public:
	SubsystemHmIp(void);
	~SubsystemHmIp(void);
	std::size_t ConsumeDownstreamBytes( uint8_t* buf, std::size_t len );
protected:
	bool CheckIfResponsibleForUpstreamFrame( const SerialFrame* frame );
	bool ProcessUpstreamFrame( SerialFrame* frame );
	bool ProcessDownstreamFrame( SerialFrame* frame );
	bool OnUpstreamConnect();
	void OnWorkerThreadStarted();
private:
	bool ProcessDownstreamFrameCommon( const CommonCommandFrame& frame );
	bool ProcessDownstreamFrameTrxAdapter( const TrxAdapterFrame& frame );
	void ProcessUpstreamFrameInternal( const InternalFrame& internalFrame );

	unsigned char _rxBuf[1024];
	size_t _rxBufLength;
	InternalFrameIdentify _internalIdentifyFrame;

	uint8_t _dutyCycle;

	bool _bootloaderMode;

};

