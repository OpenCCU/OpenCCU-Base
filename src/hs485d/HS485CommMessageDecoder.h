/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485_COMM_MESSAGE_DECODER_H_
#define _HS485_COMM_MESSAGE_DECODER_H_

#include <string>
#include <StructuredFrame.h>
class HS485CommMessage;

//! Hilfsklasse zum Übersetzen von BidCoS-Wired-Nachrichten in Klartext zum Loggen
class HS485CommMessageDecoder
{
public:
	HS485CommMessageDecoder(void);
	~HS485CommMessageDecoder(void);
	static std::string ToString(HS485CommMessage* msg);
	static std::string Format(const StructuredFrame& frame, std::string::size_type start, bool start_with_char);
protected:
	typedef struct{
		int id;
		const char* name;
		const char* format;
	}FrameField;

	typedef struct{
		int type;
		const char* name;
		FrameField fields[16];
	}FrameDescription;
	static FrameDescription FrameDescriptions[];
};
#endif //_HS485_COMM_MESSAGE_DECODER_H_
