/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _ACTOR_MESSAGE_H_
#define _ACTOR_MESSAGE_H_

#include "dllexport.h"

namespace EQ3 
{
	class ELVUTILS_DLLEXPORT ActorMessage
	{
	public:
		virtual ~ActorMessage();
		virtual ActorMessage* Clone() const = 0;
	};
}

#endif
