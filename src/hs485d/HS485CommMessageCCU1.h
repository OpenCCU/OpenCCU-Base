/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485_COMM_MESSAGE_CCU1_H_
#define _HS485_COMM_MESSAGE_CCU1_H_

#include <HS485CommMessageCCU1.h>
#include <HS485CommMessage.h>


//! Spezialisierte Klasse für die Verarbeitung von BidCoS-Wired-Nachrichten
class HS485CommMessageCCU1 : public HS485CommMessage
{

	friend class HS485ControllerCCU1;

public:
/*	enum{
		CMD_SEND		=	0x00,
		CMD_BOOT_SEND	=	0x100,
		CMD_DISCOVERY	=	0x01,
		CMD_RESPONSE	=	0x80,
		CMD_ERROR		=	0x81,
		CMD_EVENT		=	0x82,
		CMD_DISC_RESULT	=	0x83,
		CMD_DISC_END	=	0x84,
	};
*/


	inline HS485CommMessage* GetParent(){return (HS485CommMessage*)parent;};
	void SetFrame(const StructuredFrame& frame);
    HS485Frame ExtractFrame();
	inline HS485CommMessage* GetResponse(unsigned int index=0){return (HS485CommMessage*)CommMessage::GetResponse(index);};
	virtual void SetTimeout(int to);
	virtual bool TransformToSimulationMessage();
	HS485CommMessageCCU1(void);
	virtual ~HS485CommMessageCCU1(void);
	bool MatchType(uint32_t type);
	uint32_t GetSenderAddress();
	uint32_t GetReceiverAddress();
	void SetSenderAddress(uint32_t address);
	void SetReceiverAddress(uint32_t address);
	void SetCtrl(int flags);
	int GetCtrl();
protected:
	int MapIndex(int index);
  uint32_t GetType();
	virtual bool ProcessResponse(CommMessage* m, t_state* new_state);
	virtual bool ProcessEEPRomResponse(CommMessage* m, t_state* new_state);
	void SetType(uint32_t type);
	friend class HS485CommMessageDecoder;
};
#endif //_HS485_COMM_MESSAGE_H_
