/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "netconfigcmd.h"

#include "include/networkconfig.h"


NetConfigCmd::NetConfigCmd() : Command("netconfigcmd")
{
}

NetConfigCmd::~NetConfigCmd()
{
}

int NetConfigCmd::execute()
{
	networkconfig config;
	const int argc = params.size();
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(params.at(i).c_str(), "-h") == 0 && argc > i + 1){
			i++;
			config.Setconfig_dnsname(params.at(i));
		}
		if (strcmp(params.at(i).c_str(), "-i") == 0 && argc > i + 1){
			i++;
			config.Setcurrent_ip(params.at(i));
		}
		else if (strcmp(params.at(i).c_str(), "-g") == 0 && argc > i + 1)
		{
			i++;
			config.Setcurrent_gateway(params.at(i));
		}
		else if (strcmp(params.at(i).c_str(), "-n") == 0 && argc > i + 1)
		{
			i++;
			config.Setcurrent_netmask(params.at(i));
		}
		else if (strcmp(params.at(i).c_str(), "-d1") == 0 && argc > i + 1)
		{
			i++;
			config.Setcurrent_dnssrv1(params.at(i));
		}
		else if (strcmp(params.at(i).c_str(), "-d2") == 0 && argc > i + 1)
		{
			i++;
			config.Setcurrent_dnssrv2(params.at(i));
		}
		else
		{
			printf("%s", help().c_str());
		}
	}
	int retVal = config.WriteConfigFile();
	return retVal;
}

std::string NetConfigCmd::help()
{
	std::string s("netconfigcmd [-h hostname] [-i ip-address] [-g gateway] [-n netmask] [-d1 dns-server] [-d2 dns-server]\n");
	s.append("  Sets the networkinformation\n");
	s.append("  Options:\n");
	s.append("    -h: hostname\n");
	s.append("    -i: current ip address\n");
	s.append("    -g: current gateway\n");
	s.append("    -n: current (sub) netmask\n");
	s.append("    -d1: current primary dns server\n");
	s.append("    -d2: current secondary dns server\n");
	return s;
}
