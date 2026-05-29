/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RF_COMM_MESSAGE_DECODER_H_
#define _RF_COMM_MESSAGE_DECODER_H_

#include <string>
class RFCommMessage;

//! Hilfsklasse zum Übersetzen von BidCoS-RF-Nachrichten in Klartext zum Loggen
class RFCommMessageDecoder
{
public:
	RFCommMessageDecoder(void);
	~RFCommMessageDecoder(void);
	static std::string ToString(RFCommMessage* msg);
protected:
};
#endif //_RF_COMM_MESSAGE_DECODER_H_
