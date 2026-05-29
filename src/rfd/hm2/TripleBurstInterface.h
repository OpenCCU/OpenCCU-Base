/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _TRIPLEBURSTINTERFACE_H_
#define _TRIPLEBURSTINTERFACE_H_

class BidcosFrame;

class ITripleBurstInterface {

public:
	virtual ~ITripleBurstInterface() {};

	virtual bool SendFrameTripleBurst(BidcosFrame* frame) = 0;
};

#endif
