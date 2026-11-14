/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "MacControllerQueueItem.h"
#include "SerialFrame/SerialFrame.h"

MacControllerQueueItem::MacControllerQueueItem(SerialFrame* frame, Direction direction)
	:_frame( frame ), _direction( direction )
{
}


MacControllerQueueItem::~MacControllerQueueItem(void)
{
	if( _frame )
	{
		delete _frame;
	}
}

SerialFrame* MacControllerQueueItem::GetFrame( bool clear )
{
	SerialFrame* frame = _frame;
	if( clear )
	{
		_frame = NULL;
	}
	return frame;
}

MacControllerQueueItem::Direction MacControllerQueueItem::GetDirection()
{
	return _direction;
}

