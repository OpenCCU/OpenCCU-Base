/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include <stdint.h>
#include "BinaryData.h"


class BidcosAesKey
{
public:
	enum KeyId
	{
		KeyId_Default,
		KeyId_Current,
		KeyId_Previous,
		KeyId_Temp
	};

	BidcosAesKey();
	BidcosAesKey(uint8_t index, const BinaryData& data);
	~BidcosAesKey(void);
	uint8_t GetIndex()const;
	//!Get Session key. The caller is responsible for deleting the returned uint8_t array after use.
	uint8_t* GetSessionKey( const BinaryData& random )const;
	const BinaryData& GetBytes()const;
private:
	uint8_t _index;
	BinaryData _data;
};

