/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// BidcosLanInterface.cpp: Implementierung der Klasse BidcosLanInterface.
//
//////////////////////////////////////////////////////////////////////

#include "BidcosLanInterface.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

BidcosLanInterface::BidcosLanInterface()
{
}

BidcosLanInterface::~BidcosLanInterface()
{
}

BidcosInterfaceConnection* BidcosLanInterface::GetConnection()
{
    return &conn;
}

bool BidcosLanInterface::Init(std::map<std::string, std::string>& params)
{
    if(!BidcosRemoteInterface::Init(params))return false;
    return GetConnection()->Init(params);
}
