/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CRC16_H_
#define _CRC16_H_

#include <string>
#include <cstdint>

class Crc16
{
private:
	const unsigned short m_polynom;
	uint32_t crc_reg;
public:
	Crc16(unsigned short stratValue = 0xffff,unsigned short polynom = 0x8005);
	~Crc16(void);
	void Init(unsigned short startValue);
	unsigned short getCrcValue(void);
	void update(unsigned char value);
	unsigned char getHighByte(void);
	unsigned char getLowByte(void);


};

#endif //_CRC16_H_
