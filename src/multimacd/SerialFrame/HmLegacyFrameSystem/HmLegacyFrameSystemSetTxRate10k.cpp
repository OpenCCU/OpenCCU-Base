/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemSetTxRate10k.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemSetTxRate10k, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_SetTxRate10k);
/// @endcond


HmLegacyFrameSystemSetTxRate10k::HmLegacyFrameSystemSetTxRate10k(void) : HmLegacyFrameSystem( FrameType_SetTxRate10k )
{
}


HmLegacyFrameSystemSetTxRate10k::~HmLegacyFrameSystemSetTxRate10k(void)
{
}

std::string HmLegacyFrameSystemSetTxRate10k::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmSystem SetTxRate10k ";
	s += Data().ToString();
	return s;
}
