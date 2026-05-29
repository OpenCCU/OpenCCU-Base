/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RF_COMM_MESSAGE_AES_KEY_H_
#define _RF_COMM_MESSAGE_AES_KEY_H_

#include "RFCommMessage.h"

//! Spezialisierte Nachrichtenklasse für das Setzen und Abfragen von AES-Schlüsseln vom ARM7
/*! Unterscheidet sich nur in der Verarbeitung von Antwortnachrichten in ProcessResponse() von
 *  RFCommMessage.
 */
class RFCommMessageAESKey:public RFCommMessage
{
public:
	RFCommMessageAESKey(void);
	~RFCommMessageAESKey(void);
protected:
	//! Verarbeitet Schlüsselmitteilungen (INFO_KEYS) als gültige Antworten
	bool ProcessResponse(CommMessage *m, t_state* new_state);

};

#endif //_RF_COMM_MESSAGE_AES_KEY_H_
