/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "UtilitySearchDevices.h"
#include <UDPDatagramSender.h>
#include <Protocol.h>
#include <EQ3ConfigProtocol.h>
#include <LanifCfgProtocol.h>
#include <Constants.h>
#include <InternalUtilities.h>

using namespace std;
using namespace LDU;

UtilitySearchDevices::UtilitySearchDevices(void)
{
}

UtilitySearchDevices::~UtilitySearchDevices(void)
{
}

bool UtilitySearchDevices::searchDeviceBySerial(const std::string& serial, LanDevice& lanDevice)
{
	return searchDeviceByTypeAndSerial("*", serial, lanDevice);
}

bool UtilitySearchDevices::searchDeviceByTypeAndSerial(const std::string devType, const std::string& serial, LanDevice& dev) {
	//Search device over all protocolls and routing schemes
	//Multicast:
	//----------
	EQ3ConfigProtocol* pProtEQ3Config = NULL;
	LanifCfgProtocol* pProtLanifCfg = NULL;
	pProtEQ3Config = (EQ3ConfigProtocol*)Protocol::createProtocol(PROTOCOL_EQ3CONFIG);
	string msg = pProtEQ3Config->getIdentifyFrame(devType, serial);
	vector<LanDevice> devs = sendMessage(msg, pProtEQ3Config, Constants::MULTICAST_ADDRESS, ROUTINGSCHEME_MULTICAST) ;
	//Cleanup and return if found
	if(devs.size() > 0) {
		goto CLEANUP_AND_RETURN;
	}
	//Next protocol
	pProtLanifCfg = (LanifCfgProtocol*)Protocol::createProtocol(PROTOCOL_EQ3LANIFCFG);
	msg = pProtLanifCfg->getIdentifyFrame(devType, serial);
	devs = sendMessage(msg, pProtLanifCfg, Constants::MULTICAST_ADDRESS, ROUTINGSCHEME_MULTICAST);	
	if(devs.size() > 0) {
		goto CLEANUP_AND_RETURN;
	}
	//Broadcast:
	//----------
	msg = pProtEQ3Config->getIdentifyFrame(devType, serial);
	devs = sendMessage(msg, pProtEQ3Config, Constants::BROADCAST_ADDRESS, ROUTINGSCHEME_BROADCAST) ;
	//Cleanup and return if found
	if(devs.size() > 0) {
		goto CLEANUP_AND_RETURN;
	}
	//Next protocol
	msg = pProtLanifCfg->getIdentifyFrame(devType, serial);
	devs = sendMessage(msg, pProtLanifCfg, Constants::BROADCAST_ADDRESS, ROUTINGSCHEME_BROADCAST);	
	if(devs.size() > 0) {
		goto CLEANUP_AND_RETURN;
	}
	//Nothing found -> return default constructed LanDevice
CLEANUP_AND_RETURN:
	if(pProtEQ3Config != NULL) {
		delete pProtEQ3Config;
		pProtEQ3Config = NULL;
	}
	if(pProtLanifCfg != NULL) {
		delete pProtLanifCfg;
		pProtLanifCfg = NULL;
	}
	if(devs.size() > 0) {
		dev = devs.at(0);
		return true;
	}
	else {
		return false;
	}
}

bool UtilitySearchDevices::searchDevices(const vector<std::string>& devTypeFilters, const int protocols, const int routingSchemes, const std::string& firstAddress, const std::string& secondAddress, std::vector<LanDevice>& devices) {
	//Parse args
	if (devTypeFilters.size() >= 1) {
		vector<string> addresses = createAddressesByRoutingSchemes(routingSchemes, firstAddress, secondAddress);
		vector<Protocol*> protos = createProtocolsByProtocolSchemeInt(protocols);
		for(unsigned int f = 0; f < devTypeFilters.size(); f++) {//For every devTypeFilter
			//For every protocol
			for(unsigned int p = 0; p < protos.size(); p++) {
				//For every address
				for(unsigned int a = 0; a < addresses.size(); a++) {
					Protocol* pProt = protos.at(p);
					const string msg = pProt->getIdentifyFrame( devTypeFilters.at(f), "*");
					RoutingSchemeEnum rscheme;
					if(addresses.at(a).compare(Constants::BROADCAST_ADDRESS) == 0) {
						rscheme = ROUTINGSCHEME_BROADCAST;
					}
					else if(addresses.at(a).compare(Constants::MULTICAST_ADDRESS) == 0) {
						rscheme = ROUTINGSCHEME_MULTICAST;
					}
					else {
						rscheme = ROUTINGSCHEME_UNICAST;
					}
					vector<LanDevice> newDevs = sendMessage( msg, pProt, addresses.at(a).c_str(), rscheme);
					mergeLanDeviceVectorAIntoB(newDevs, devices);
				}
			}
		}
		for(unsigned int i = 0; i < protos.size(); i++) {
			delete protos.at(i);
		}
	}
	else  {
		return false;
	}
	return true;
}

void UtilitySearchDevices::mergeLanDeviceVectorAIntoB(const std::vector<LanDevice>& a, std::vector<LanDevice>& b) const {
	for(unsigned int i = 0; i < a.size(); i++) {
		bool notInside = true;
		string serialA = a.at(i).getSerialNumber();
		for(unsigned int j = 0; j < b.size(); j++) {
			if(serialA.compare( b.at(j).getSerialNumber() ) == 0) {
				notInside = false;
				break;
			}
		}
		if(notInside) {
			b.push_back(a.at(i));
		}
	}
}

vector<LanDevice> UtilitySearchDevices::sendMessage(const std::string& msg, const Protocol* pProt, const std::string& address, const RoutingSchemeEnum& routingScheme) {
	vector<std::string> responses;
	vector<LanDevice> devs;
	UDPDatagramSender sender(address.c_str(), pProt->getProtocolDefaultPort(), pProt->getProtocolDefaultReplyPort(), routingScheme);
	bool success = sender.send(msg, responses, 2000, 1000);
	if(success) {
		devs = pProt->parseIdentifyResponses(responses);
		for(unsigned int i = 0; i < devs.size(); i++) {
			devs.at(i).setReachedByRoutingScheme(routingScheme);
		}
	}
	return devs;
}


std::vector<std::string> UtilitySearchDevices::createAddressesByRoutingSchemes(const int routingSchemes, const std::string& addrFrom, const std::string& addrTo) const {
	vector<string> addresses;
	bool unicast = ((routingSchemes & ROUTINGSCHEME_UNICAST) == ROUTINGSCHEME_UNICAST);
	bool multicast = ((routingSchemes & ROUTINGSCHEME_MULTICAST) == ROUTINGSCHEME_MULTICAST);
	bool broadcast = ((routingSchemes & ROUTINGSCHEME_BROADCAST) == ROUTINGSCHEME_BROADCAST);
	if(broadcast) {
		addresses.push_back( Constants::BROADCAST_ADDRESS );
	}
	if(multicast) {
		addresses.push_back( Constants::MULTICAST_ADDRESS );
	}	
	if( unicast ) {
		if(!addrFrom.empty()) {//first address given
			if(addrTo.empty() ) {
				addresses.push_back( addrFrom );//second address NOT given
			}
			else {//second address given -> addresss range
				int l1First, l2First, l3First, l4First;
				extractIPAddressComponents(addrFrom, l1First, l2First, l3First, l4First);
				int l1Last, l2Last , l3Last, l4Last;
				extractIPAddressComponents(addrTo, l1Last, l2Last, l3Last, l4Last);
				int l2CurrentLast = 255;
				int l3CurrentLast = 255;
				int l4CurrentLast = 255;
				for(int a = l1First; a <= l1Last; a++) {
					if(a == l1Last) {
						l2CurrentLast = l2Last;
					}
					for(int b = l2First; b <= l2CurrentLast; b++) {
						if(b == l2Last) {
							l3CurrentLast = l3Last;
						}
						for(int c = l3First; c <= l3CurrentLast; c++) {
							if(c == l3Last) {
								l4CurrentLast = l4Last;
							}
							for(int d = l4First; d <= l4CurrentLast; d++) {
								string address;
								assembleIPAddressFromComponents(a, b, c, d, address);
								addresses.push_back(address);
							}
						}
					}
				}
			}
		}
	}
	return addresses;
}

std::vector<Protocol*> UtilitySearchDevices::createProtocolsByProtocolSchemeInt(const int protocols) const {
	vector<Protocol*> protos;
	if((protocols & PROTOCOL_EQ3LANIFCFG) == PROTOCOL_EQ3LANIFCFG) {
		protos.push_back( Protocol::createProtocol(PROTOCOL_EQ3LANIFCFG) );
	}
	if((protocols & PROTOCOL_EQ3CONFIG) == PROTOCOL_EQ3CONFIG) {
		protos.push_back( Protocol::createProtocol(PROTOCOL_EQ3CONFIG) );
	}
	return protos;
}
