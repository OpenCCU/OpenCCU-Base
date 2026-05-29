/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosUsbInterface.h"
#include <Logger.h>

BidcosUsbInterface::BidcosUsbInterface(void)
{
}

BidcosUsbInterface::~BidcosUsbInterface(void)
{
}

BidcosInterfaceConnection* BidcosUsbInterface::GetConnection()
{
    return &conn;
}

bool BidcosUsbInterface::Init(std::map<std::string, std::string>& params)
{
    if(!GetConnection()->Init(params))return false;
    if(!BidcosRemoteInterface::Init(params))return false;
    if(!GetConnection()->Init(params))return false;
    return true;
}
