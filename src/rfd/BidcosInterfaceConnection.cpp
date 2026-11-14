/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <Logger.h>
#include <XmlRpc.h>

#include "RFManager.h"
#include "BidcosInterfaceConnection.h"

using namespace XmlRpc;

BidcosInterfaceConnection::BidcosInterfaceConnection(void)
{
}

BidcosInterfaceConnection::~BidcosInterfaceConnection(void)
{
}

void BidcosInterfaceConnection::ReportConnectionStatus(const std::string& serial_number, bool status) const
{
	XmlRpcValue value(status);
	RFManager::GetSingleton()->ReportEvent(serial_number, "CONNECTED", value);
}

