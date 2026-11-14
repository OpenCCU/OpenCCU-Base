/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef BACKBONE_FRAME_H_
#define BACKBONE_FRAME_H_
#include "../SerialFrame.h"

class BackboneFrame : public SerialFrame {

public:

    enum FrameType {
        FrameType_Invalid = 0xff,
        //0x00 //reserved
        //0x01 //not assigned yet
        FrameType_EnableRouting = 0x02,
        FrameType_Response = 0x03,
        //0x04 // not assigned yet
        FrameType_GetBackboneInclusionRequestData = 0x05,
        FrameType_HandleBackboneInclusionAccept = 0x06,
        FrameType_GetBackboneInclusionAcceptDataForMasterKeyInclusion = 0x07,
        FrameType_GetBackboneInclusionAcceptDataForLocalKeyInclusion = 0x08,
        FrameType_GetBackboneKey = 0x09,
        FrameType_SetBackboneKey = 0x0A,
        FrameType_EventCheckRouteToDestination = 0x0B,
        FrameType_RouteToDestinationState = 0x0C,
        FrameType_GetEncryptionBackboneKey = 0x0D,
        FrameType_SetEncryptionBackboneKey = 0x0E
    };

	BackboneFrame(FrameType frameType = FrameType_Invalid);
	virtual ~BackboneFrame(void);
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
};
#endif //BACKBONE_FRAME_H_