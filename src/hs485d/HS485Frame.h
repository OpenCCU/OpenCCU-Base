/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485FRAME_H_
#define _HS485FRAME_H_

#include <StructuredFrame.h>

class HS485Frame :  public StructuredFrame
{
public:
	enum{
		CTRL_IFRAME=0x18,
		CTRL_BOOT_IFRAME=0x10,
		CTRL_SYN=0x80,
		CTRL_ACK=0x19
	};

	enum{
		FIELD_CTRL          	=STRUCTURED_FRAME_FIELD_INT(4, 0, 1, 0),
		FIELD_SENDER          	=STRUCTURED_FRAME_FIELD_INT(5, 0, 4, 0),
		FIELD_RECEIVER         	=STRUCTURED_FRAME_FIELD_INT(0, 0, 4, 0),
		FIELD_ADD_DESCRIPTION	=STRUCTURED_FRAME_FIELD_STR(11, 4),
		FIELD_ADD_SERIAL		=STRUCTURED_FRAME_FIELD_STR(15, 10)
	};

	enum{
		FT_ADD					= 'A'
	};

    HS485Frame(void);
    ~HS485Frame(void);
    std::string GetPayload();
    void SetPayload(const std::string& s);
    uint32_t GetSenderAddress();
    uint32_t GetReceiverAddress();
    void SetSenderAddress(uint32_t address);
    void SetReceiverAddress(uint32_t address);
    void SetCtrl(int ctrl);
    int GetCtrl();
};

#endif
