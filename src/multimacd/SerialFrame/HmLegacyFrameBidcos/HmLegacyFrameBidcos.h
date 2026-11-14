/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "../HmLegacyFrame.h"
class HmLegacyFrameBidcos :
	public HmLegacyFrame
{
public:
	enum FrameType 
	{
		FrameType_SetRfAddress,
		FrameType_GetRfAddress,
		FrameType_TxTelegram,
		FrameType_SetAesKey,
		FrameType_Response,
		FrameType_RxTelegram,
		FrameType_PeerAdd,
		FrameType_PeerRemove,
		FrameType_GetPeers,
		FrameType_PeerActivateAuth,
		FrameType_PeerDeactivateAuth,
		FrameType_SetTempAesKey,
		FrameType_PeerSetAesKeyId,
		FrameType_PeerGetAesKeyId,
		FrameType_GetPreviousAesKeyHash,
		FrameType_SetPreviousAesKey,
		FrameType_GetDefaultRfAddress,
	};
	virtual ~HmLegacyFrameBidcos(void);
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
	virtual std::string ToString()const;
protected:
	HmLegacyFrameBidcos(FrameType frameType);
};

