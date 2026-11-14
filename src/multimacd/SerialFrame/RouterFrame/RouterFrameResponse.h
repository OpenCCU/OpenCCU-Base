/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/



#ifndef SERIALFRAME_ROUTERFRAME_ROUTERFRAMERESPONSE_H_
#define SERIALFRAME_ROUTERFRAME_ROUTERFRAMERESPONSE_H_

#include "RouterFrame.h"

class RouterFrameResponse: public RouterFrame {
public:
	RouterFrameResponse();
	virtual ~RouterFrameResponse();
	bool isResponseFrame();
};

#endif /* SERIALFRAME_ROUTERFRAME_ROUTERFRAMERESPONSE_H_ */
