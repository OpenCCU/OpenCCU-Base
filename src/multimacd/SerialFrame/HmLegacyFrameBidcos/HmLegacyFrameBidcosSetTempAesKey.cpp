/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosSetTempAesKey.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosSetTempAesKey, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_SetTempAesKey);
/// @endcond


HmLegacyFrameBidcosSetTempAesKey::HmLegacyFrameBidcosSetTempAesKey(void) : HmLegacyFrameBidcosSetAesKey( FrameType_SetTempAesKey )
{
}


HmLegacyFrameBidcosSetTempAesKey::~HmLegacyFrameBidcosSetTempAesKey(void)
{
}

