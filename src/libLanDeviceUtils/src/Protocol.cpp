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

