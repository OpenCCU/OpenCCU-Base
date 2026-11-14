/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include <stdint.h>
class Crc16
{
public:
	Crc16(uint16_t poly, uint16_t regStart = 0xffff);
	~Crc16(void);
	void Start(uint16_t reg);
	void Update( uint8_t b );
	uint16_t Result()const;
private:
	uint16_t _poly;
	uint16_t _register;
};

