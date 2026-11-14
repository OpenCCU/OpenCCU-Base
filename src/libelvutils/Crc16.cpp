/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "Crc16.h"

Crc16::Crc16(unsigned short stratValue,unsigned short polynom):m_polynom(polynom),crc_reg(stratValue)
{
}
Crc16::~Crc16(void)
{
}
void Crc16::Init(unsigned short startValue)
{
	crc_reg = startValue;
}
unsigned short Crc16::getCrcValue(void)
{
	return crc_reg & 0xffff;
}
void Crc16::update(unsigned char value)
{
	for (int i = 0; i < 8; ++i)
    {
		if ((((crc_reg & 0x8000)) ^ static_cast<uint32_t>((value & 0x80) << 8)) == 0x8000)
        {
			crc_reg = ((crc_reg << 1) ^ m_polynom);
        }
        else
        {
			crc_reg <<= 1;

        }
        value <<= 1;
     }
}

unsigned char Crc16::getHighByte(void)
{
	return (crc_reg >> 8) & 0xff;
}
unsigned char Crc16::getLowByte(void)
{
	return crc_reg & 0xff;
}
