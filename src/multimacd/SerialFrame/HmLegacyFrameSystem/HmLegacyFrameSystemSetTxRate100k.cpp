/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameSystemSetTxRate100k.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameSystemSetTxRate100k, int> m((HmLegacyFrame::SubsystemType_System<<8)|HmLegacyFrameSystem::FrameType_SetTxRate100k);
/// @endcond


HmLegacyFrameSystemSetTxRate100k::HmLegacyFrameSystemSetTxRate100k(void) : HmLegacyFrameSystem( FrameType_SetTxRate100k )
{
}


HmLegacyFrameSystemSetTxRate100k::~HmLegacyFrameSystemSetTxRate100k(void)
{
}

	uint16_t HmLegacyFrameSystemSetTxRate100k::GetSyncword()const
	{
		return Data().GetUInt16Value( 0 );
	}

	void HmLegacyFrameSystemSetTxRate100k::SetSyncword( uint16_t syncword )
	{
		Data().SetUInt16Value( 0, syncword );
	}

std::string HmLegacyFrameSystemSetTxRate100k::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmSystem SetTxRate100k ";
	s += Data().ToString();
	return s;
}
