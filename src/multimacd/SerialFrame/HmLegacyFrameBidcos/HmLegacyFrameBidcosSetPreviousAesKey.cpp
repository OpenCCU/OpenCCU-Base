/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosSetPreviousAesKey.h"
#include <stdio.h>

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosSetPreviousAesKey, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_SetPreviousAesKey);
/// @endcond


HmLegacyFrameBidcosSetPreviousAesKey::HmLegacyFrameBidcosSetPreviousAesKey(void) : HmLegacyFrameBidcosSetAesKey( FrameType_SetPreviousAesKey )
{
}


HmLegacyFrameBidcosSetPreviousAesKey::~HmLegacyFrameBidcosSetPreviousAesKey(void)
{
}
