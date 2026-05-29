/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosFrameDetermineValue.h"
#include "RFManager.h"
#include "BidcosFrameDecoder.h"
#include <Logger.h>

BidcosFrameDetermineValue::BidcosFrameDetermineValue(void)
{
}

BidcosFrameDetermineValue::~BidcosFrameDetermineValue(void)
{
}

bool BidcosFrameDetermineValue::CheckReceiveComplete(uint64_t* wait_time_ms)
{
	int i=0;
  uint64_t last_timestamp=GetTimestamp();
	while(true)
	{
		BidcosFrame* response=GetResponse(i);
		if(!response)break;
        last_timestamp=response->GetTimestamp();
		if(response->MatchType(BidcosFrame::FT_INFO_PARAM_ASYNC_PAIRS)){
			if(
				response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_CH) == GetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL) &&
				response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_PEER_A) == GetIntValue(BidcosFrame::FIELD_CONFIG_PEER_A) &&
				response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_PEER_CH) == GetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH) &&
				response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_LIST) == GetIntValue(BidcosFrame::FIELD_CONFIG_PARAM_LIST)
			){
				int size=response->GetSize();
				for(int i=16;i<=(size-1);i+=2){
					uint32_t test=1;
					response->GetIntValue(i, 0, 2, 0, &test);
					if(!test)return true;
				}
			}
		}else if(response->MatchType(BidcosFrame::FT_INFO_PARAM_RESPONSE_SEQ)){
			if(
				response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_CH) == GetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL) &&
				response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_PEER_A) == GetIntValue(BidcosFrame::FIELD_CONFIG_PEER_A) &&
				response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_PEER_CH) == GetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH) &&
				response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_LIST) == GetIntValue(BidcosFrame::FIELD_CONFIG_PARAM_LIST)
			){
				if(!response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_OFFSET))
				{
					return true;
				}
			}
		}
		i++;
	}
    if(wait_time_ms)
    {
    	if((*wait_time_ms) < 10000) {
    		*wait_time_ms = 10000;
    	}
        uint64_t next_receive_time=last_timestamp+(*wait_time_ms); //was 10000
        uint64_t now=time_millis();
        if(next_receive_time>now)*wait_time_ms=next_receive_time-now;
        else *wait_time_ms=0;
    }
    return false;
}
