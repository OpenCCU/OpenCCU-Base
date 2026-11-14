/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BACKBONE_FRAME_RESPONSE_H_
#define _BACKBONE_FRAME_RESPONSE_H_

#include "BackboneFrame.h"

class BackboneFrameResponse : public BackboneFrame {

public:
    BackboneFrameResponse();
    virtual ~BackboneFrameResponse();
    virtual bool isResponseFrame();
};

#endif //_BACKBONE_FRAME_RESPONSE_H_