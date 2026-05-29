/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFLoggingManager.h"
#include "RFController.h"
#include "BidcosFrameDecoder.h"
#include <utils.h>
#include <AsyncXmlRpcSender.h>
#include <OSCompat.h>
#include <Logger.h>

RFLoggingManager::RFLoggingManager(void)
{
}

RFLoggingManager::~RFLoggingManager(void)
{
}

bool RFLoggingManager::Init(const char* config_filename)
{
    std::string absolute_filename=OSCompat::FixPath(config_filename);
    if(config_file.ReadFromFile(absolute_filename.c_str())<=0){
        LOG(Logger::LOG_ERROR, "Config file %s not found", absolute_filename.c_str());
        return false;
    }

    GetInterfaceConcentrator()->SetPromicuousMode(true);

    if(!GetInterfaceConcentrator()->CreateInterfacesFromFile(config_file))
    {
       LOG(Logger::LOG_WARNING, "Error initializing interfaces");
	   return false;
    }

	GetInterfaceConcentrator()->SetBidcosAddress(GetBidcosAddress());

	if(!GetInterfaceConcentrator()->StartInterfaces())
	{
		LOG(Logger::LOG_ERROR, "Error starting interfaces");
	}
	return true;
}

void RFLoggingManager::ProcessIncomingFrame(BidcosFrame& frame)
{
    if( filtered_addresses.size() )
    {
        if( ( filtered_addresses.find(frame.GetSenderAddress()) == filtered_addresses.end() ) && ( filtered_addresses.find(frame.GetReceiverAddress()) == filtered_addresses.end() ) )
        {
            return;
        }
    }
	LOG(Logger::LOG_INFO, "%lu: %s\n%s", time_millis(), frame.ToString().c_str(), BidcosFrameDecoder::ToString(&frame).c_str());
}

void RFLoggingManager::SetAddressFilter( const std::vector<int>& addresses )
{
    filtered_addresses.clear();
    for( size_t i=0; i<addresses.size(); i++)
    {
        filtered_addresses.insert(addresses[i]);
    }
}
