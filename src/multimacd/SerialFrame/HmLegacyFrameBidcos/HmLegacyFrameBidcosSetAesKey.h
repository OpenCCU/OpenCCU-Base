/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameBidcos.h"
class HmLegacyFrameBidcosSetAesKey :
	public HmLegacyFrameBidcos
{
public:
	HmLegacyFrameBidcosSetAesKey(void);
	virtual ~HmLegacyFrameBidcosSetAesKey(void);
	BinaryData GetKeyData()const;
	void SetKeyData( const BinaryData& keyData );
	void SetKeyIndex( uint8_t keyIndex );
	uint8_t GetKeyIndex()const;
	virtual std::string ToString()const;
protected:
	HmLegacyFrameBidcosSetAesKey( FrameType frameType );
};

