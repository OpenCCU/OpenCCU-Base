/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "Crc16.h"


Crc16::Crc16(uint16_t poly, uint16_t regStart /*= 0xffff*/)
{
	_poly = poly;
	_register = regStart;
}


Crc16::~Crc16(void)
{
}

void Crc16::Start(uint16_t reg)
{
	_register = reg;
}

void Crc16::Update( uint8_t b )
{
	unsigned char i;
	for (i = 0; i < 8; i++)
	{
		if (((_register & 0x8000) >> 8) ^ (b & 0x80))
		{
			_register = (_register << 1) ^ _poly;
		}
		else
		{
			_register = (_register << 1);
		}
		b <<= 1;
	}
}

uint16_t Crc16::Result()const
{
	return _register;
}
