/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
class SerialFrame;
class MacControllerQueueItem
{
public:
	enum Direction
	{
		Direction_Upstream,
		Direction_Downstream,
	};
	MacControllerQueueItem(SerialFrame* frame, Direction direction);
	~MacControllerQueueItem(void);
	SerialFrame* GetFrame( bool clear );
	Direction GetDirection();
private:
	
	SerialFrame* _frame;
	Direction _direction;
};

