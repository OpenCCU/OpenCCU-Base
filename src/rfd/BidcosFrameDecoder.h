/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOS_FRAME_DECODER_H_
#define _BIDCOS_FRAME_DECODER_H_

#include <string>
class BidcosFrame;

//! Hilfsklasse zum Übersetzen von BidCoS-RF-Nachrichten in Klartext zum Loggen
class BidcosFrameDecoder
{
public:
	BidcosFrameDecoder(void);
	~BidcosFrameDecoder(void);
	static std::string ToString(const BidcosFrame* frame);
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
#endif //_BIDCOS_FRAME_DECODER_H_
