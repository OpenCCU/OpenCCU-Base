/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#ifndef SERIALFRAME_ROUTERFRAME_ROUTERFRAME_H_
#define SERIALFRAME_ROUTERFRAME_ROUTERFRAME_H_

#include "../SerialFrame.h"

class RouterFrame: public SerialFrame {
public:
	enum FrameType
	{
		FrameType_Invalid = 0xff,
		FrameType_SetSNCAddress = 1,
		FrameType_EnableRouting = 2,
		FrameType_Response = 3,
		FrameType_AddStaticRoute = 4,
		FrameType_RemoveStaticRoute = 5,
		FrameType_AddMulticastFilter = 6,
		FrameType_RemoveMulticastFilter = 7,
		FrameType_GetRoutingKey = 8,
		FrameType_GetRouterAddress = 9,
		FrameType_HandleRoutedFrameFromLAN = 0x0a,
		FrameType_EventRoutedFrame = 0x0b,
		FrameType_EventRouterAddressChanged = 0x0c,

	};

	RouterFrame(FrameType frameType = FrameType_Invalid);
	virtual ~RouterFrame();
	FrameType GetFrameType()const;
	void SetFrameType( FrameType frameType );
};

#endif /* SERIALFRAME_ROUTERFRAME_ROUTERFRAME_H_ */
