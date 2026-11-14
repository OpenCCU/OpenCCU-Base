/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosFrameEnterConfig.h"
#include "RFManager.h"
#include "BidcosFrameDecoder.h"

#include <Logger.h>

BidcosFrameEnterConfig::BidcosFrameEnterConfig(void)
{
}

BidcosFrameEnterConfig::~BidcosFrameEnterConfig(void)
{
}

bool BidcosFrameEnterConfig::CheckReceiveComplete(uint64_t* wait_time_ms)
{
	int i=0;
  uint64_t last_timestamp=GetTimestamp();
	while(true)
	{
		BidcosFrame* response=GetResponse(i);
		if(!response)break;
        last_timestamp=response->GetTimestamp();
		if(response->MatchType(BidcosFrame::FT_SYSINFO)){
			if(response->GetStringValue(FIELD_SYSINFO_SERIAL) == this->GetStringValue(FIELD_CONFIG_SERIAL)){
				return true;
			}
		}
		i++;
	}
    if(wait_time_ms)
    {
    	if((*wait_time_ms) < 2000) {
    		*wait_time_ms = 2000;
    	}
        uint64_t next_receive_time=last_timestamp+(*wait_time_ms);///was 2000
        uint64_t now=time_millis();
        if(next_receive_time>now)*wait_time_ms=next_receive_time-now;
        else *wait_time_ms=0;
    }
    return false;
}

bool BidcosFrameEnterConfig::CheckAndAddResponse(const BidcosFrame& response)
{
	if(response.MatchType(BidcosFrame::FT_SYSINFO)){
		if(response.GetStringValue(FIELD_SYSINFO_SERIAL) == GetStringValue(FIELD_CONFIG_SERIAL)){
			return AddResponse(response);
		}
	}
	return false;
}

