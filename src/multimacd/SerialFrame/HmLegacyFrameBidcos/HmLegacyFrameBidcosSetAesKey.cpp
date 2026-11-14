/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosSetAesKey.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosSetAesKey, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_SetAesKey);
/// @endcond


HmLegacyFrameBidcosSetAesKey::HmLegacyFrameBidcosSetAesKey(void) : HmLegacyFrameBidcos( FrameType_SetAesKey )
{
}

HmLegacyFrameBidcosSetAesKey::HmLegacyFrameBidcosSetAesKey( FrameType frameType )  : HmLegacyFrameBidcos( frameType )
{
}


HmLegacyFrameBidcosSetAesKey::~HmLegacyFrameBidcosSetAesKey(void)
{
}

BinaryData HmLegacyFrameBidcosSetAesKey::GetKeyData()const
{
	return Data().GetRange( 0, 16 );
}

void HmLegacyFrameBidcosSetAesKey::SetKeyData( const BinaryData& keyData )
{
	Data().SetRange( 0, keyData.GetRange( 0, 16 ) );
}

void HmLegacyFrameBidcosSetAesKey::SetKeyIndex( uint8_t keyIndex )
{
	Data().SetUInt8Value( 16, keyIndex );
}

uint8_t HmLegacyFrameBidcosSetAesKey::GetKeyIndex()const
{
	return Data().GetUInt8Value( 16 );
}

std::string HmLegacyFrameBidcosSetAesKey::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer),"#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos SetAesKey";
	switch( GetFrameType() )
	{
	case FrameType_SetAesKey:
		s+=".cur ";
		snprintf( buffer, sizeof(buffer),"(%d) ", GetKeyIndex() );
		s+=buffer;
		break;
	case FrameType_SetTempAesKey:
		s+=".tmp ";
		break;
	case FrameType_SetPreviousAesKey:
		s+=".prv ";
		snprintf( buffer, sizeof(buffer), "(%d) ", GetKeyIndex() );
		s+=buffer;
		break;
	default:
		s+=" ";
	}
	s += GetKeyData().ToString();
	return s;
}

