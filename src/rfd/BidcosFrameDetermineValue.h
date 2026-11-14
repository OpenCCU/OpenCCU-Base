/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOS_FRAME_DETERMINE_VALUE_H_
#define _BIDCOS_FRAME_DETERMINE_VALUE_H_

#include "BidcosFrame.h"

//! Spezialisierte Nachrichtenklasse für das Bestimmen von Parameterwerten
/*! Unterscheidet sich nur in CheckReceiveComplete(uint64_t* wait_time_ms) von BidcosFrame.
 */
class BidcosFrameDetermineValue:public BidcosFrame
{
public:
	BidcosFrameDetermineValue(void);
	~BidcosFrameDetermineValue(void);
protected:
	//! Ermittelt, ob eine passende asynchrone Parametermitteilung empfangen wurde
	bool CheckReceiveComplete(uint64_t* wait_time_ms);

};

#endif //_BIDCOS_FRAME_DETERMINE_VALUE_H_
