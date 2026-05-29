/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosFrameLinkPeerReq.h"
#include "RFManager.h"
#include "BidcosFrameDecoder.h"

#include <Logger.h>

BidcosFrameLinkPeerReq::BidcosFrameLinkPeerReq(void)
{
}

BidcosFrameLinkPeerReq::~BidcosFrameLinkPeerReq(void)
{
}

bool BidcosFrameLinkPeerReq::CheckReceiveComplete(uint64_t* wait_time_ms)
{
	int i=0;
  uint64_t last_timestamp=GetTimestamp();
	while(true)
	{
		BidcosFrame* response=GetResponse(i);
		if(!response)break;
        last_timestamp=response->GetTimestamp();
		if(response->MatchType(BidcosFrame::FT_INFO_PEER_LIST)){
			int size=response->GetSize();
			for(int i=10;i<=size-3;i+=4){
				uint32_t test=1;
				response->GetIntValue(i, 0, 4, 0, &test);
				if(!test)return true;
			}
		}
		i++;
	}
    if(wait_time_ms)
    {
    	if((*wait_time_ms) < 1000) {
    		*wait_time_ms = 1000;
    	}
        uint64_t next_expected_receive_time=last_timestamp+(*wait_time_ms);//was 1000
        uint64_t now=time_millis();
        if(next_expected_receive_time>now)*wait_time_ms=next_expected_receive_time-now;
        else *wait_time_ms=0;
    }
    return false;
}
