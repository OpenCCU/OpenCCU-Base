/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485_COMM_MESSAGE_H_
#define _HS485_COMM_MESSAGE_H_

#include "CommMessage.h"
#include "HS485Frame.h"



//! Spezialisierte Klasse f�r die Verarbeitung von BidCoS-Wired-Nachrichten
class HS485CommMessage :
	public CommMessage
{
	friend class HS485Controller;
	//friend class HS485ControllerCCU1;
	//friend class HS485ControllerLGW;

protected:
	HS485CommMessage(void);


public:
	enum{
		CMD_SEND		=	0x00,
		CMD_BOOT_SEND	=	0x100,
		CMD_DISCOVERY	=	0x01,
		CMD_RESPONSE	=	0x80,
		CMD_ERROR		=	0x81,
		CMD_EVENT		=	0x82,
		CMD_DISC_RESULT	=	0x83,
		CMD_DISC_END	=	0x84,
	};

	virtual ~HS485CommMessage(void);

	virtual inline HS485CommMessage* GetParent(){return (HS485CommMessage*)parent;};
	virtual void SetFrame(const StructuredFrame& frame);
    virtual HS485Frame ExtractFrame();
	virtual inline HS485CommMessage* GetResponse(unsigned int index=0){return (HS485CommMessage*)CommMessage::GetResponse(index);};
	virtual void SetTimeout(int to);
	virtual bool TransformToSimulationMessage();

	virtual bool MatchType(uint32_t type);
	virtual uint32_t GetSenderAddress();
	virtual uint32_t GetReceiverAddress();
	virtual void SetSenderAddress(uint32_t address);
	virtual void SetReceiverAddress(uint32_t address);
	virtual void SetCtrl(int flags);
	virtual int GetCtrl();

	/** \brief Returns value of eepRomUsage.
	 * \return Value of eepRomUsage*/
	virtual bool isEEPRomUsage();

	/** \brief Sets value of eepRomUsage.
	 * \param enabled New value of eepRomUsage*/
	virtual void setEEPRomUsage(const bool enabled);
protected:
	/** \brief Indices for EEPRomUsage messages.*/
	enum{IDX_COMMAND=0, IDX_ADDRESS=1, IDX_BLOCKSIZE=3, IDX_COUNT=4};

	/** \brief Determines wether this message is a EEPROM message or not.*/
	bool eepRomUsage;

	virtual int MapIndex(int index);
  virtual uint32_t GetType();
	virtual bool ProcessResponse(CommMessage* m, t_state* new_state);
	virtual void SetType(uint32_t type);
	friend class HS485CommMessageDecoder;
};
#endif //_HS485_COMM_MESSAGE_H_
