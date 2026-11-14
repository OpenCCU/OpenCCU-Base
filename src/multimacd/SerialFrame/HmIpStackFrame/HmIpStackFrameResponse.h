/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/



#ifndef SERIALFRAME_HMIPSTACKFRAME_HMIPSTACKFRAMERESPONSE_H_
#define SERIALFRAME_HMIPSTACKFRAME_HMIPSTACKFRAMERESPONSE_H_

#include "HmIpStackFrame.h"

class HmIpStackFrameResponse: public HmIpStackFrame {
public:
	HmIpStackFrameResponse();
	virtual ~HmIpStackFrameResponse();
	bool isResponseFrame();
};

#endif /* SERIALFRAME_HMIPSTACKFRAME_HMIPSTACKFRAMERESPONSE_H_ */
