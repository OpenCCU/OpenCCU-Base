/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "Protocol.h"
#include "LanifCfgProtocol.h"
#include "EQ3ConfigProtocol.h"

using namespace LDU;

/*
Protocol::Protocol(void)
{

}
*/
Protocol::~Protocol(void)
{
}

Protocol* Protocol::createProtocol(const ProtocolEnum& protType) {
	Protocol* prot;
	switch(protType) {
		case PROTOCOL_EQ3LANIFCFG:
			prot = new LanifCfgProtocol();
			break;
		case PROTOCOL_EQ3CONFIG:
			prot = new EQ3ConfigProtocol();
			break;
		default:
			prot = NULL;
			break;
	}
	return prot;
}

