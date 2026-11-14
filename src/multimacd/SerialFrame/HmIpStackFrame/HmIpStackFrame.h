/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#ifndef SERIALFRAME_HMIPSTACKFRAME_HMIPSTACKFRAME_H_
#define SERIALFRAME_HMIPSTACKFRAME_HMIPSTACKFRAME_H_

#include "../SerialFrame.h"

class HmIpStackFrame: public SerialFrame {
public:
	enum FrameType
	{
		FrameType_Invalid = 0xff,
		FrameType_SetDefaultIp = 0,
		FrameType_GetDefaultIp = 1,
		FrameType_SetNetworkKey = 2,
		FrameType_SendProtocolFrame = 3,
		FrameType_AddLinkPartner = 4,
		FrameType_RemoveLinkPartner = 5,
		FrameType_Response = 6,
		FrameType_RxFrameEvent = 7,
		FrameType_SetSecurityCounter = 8,
		FrameType_GetSecurityCounter = 0x0a,
		FrameType_SetCsmaCaAttempts = 0x0b,
		FrameType_GetCsmaCaAttempts = 0x0c,
		FrameType_SetSendAttempts = 0x0d,
		FrameType_GetSendAttempts = 0x0e,
		FrameType_GetInclusionData = 0x11,
		FrameType_GetLinkPartnerList = 0x12,
		FrameType_GetEncNetworkKey = 0x13,
		FrameType_SetEncNetworkKey = 0x14,
		FrameType_SendOnOtauInterval = 0x15,
		FrameType_OtauFrameSend = 0x16,
		FrameType_CancelOtauInterval = 0x17,

	};

	HmIpStackFrame(FrameType frameType = FrameType_Invalid);
	virtual ~HmIpStackFrame();
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
};

#endif /* SERIALFRAME_HMIPSTACKFRAME_HMIPSTACKFRAME_H_ */
