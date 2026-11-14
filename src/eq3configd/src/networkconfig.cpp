/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "../include/networkconfig.h"
#include <fstream>
#include <sstream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

const char * keyFileName = "/etc/config/crypttool.cfg";
const char * configFileName = "/etc/config/netconfig";

const std::string HOSTNAME = "HOSTNAME";
const std::string MODE = "MODE";
const std::string CRYPT = "CRYPT";
const std::string IP = "IP";
const std::string NETMASK = "NETMASK";
const std::string GATEWAY = "GATEWAY";
const std::string NAMESERVER1 = "NAMESERVER1";
const std::string NAMESERVER2 = "NAMESERVER2";
const std::string CURRENT_IP = "CURRENT_IP";
const std::string CURRENT_GATEWAY = "CURRENT_GATEWAY";
const std::string CURRENT_NETMASK = "CURRENT_NETMASK";
const std::string CURRENT_NAMESERVER1 = "CURRENT_NAMESERVER1";
const std::string CURRENT_NAMESERVER2 = "CURRENT_NAMESERVER2";

networkconfig::networkconfig()
{
	// ctor
	this->LoadConfigFile();
}

networkconfig::~networkconfig()
{
	// dtor
}

// Gets the last key from the config file
/*static*/ std::string networkconfig::GetCurrentAesKey(void)
{
	std::ifstream infile;
	infile.open(keyFileName, std::ios_base::in);

	std::string key;
	int index = -1;

	if(infile.is_open())
	{
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream iss(line);
			if (!(iss >> index >> key)) 
			{ 
				break; 
			}
		}

		infile.close();
	}

	return key;
}

std::string networkconfig::Getcurrent_ip() const
{
	return current_ip;
}

void networkconfig::Setcurrent_ip(std::string& val)
{
	this->current_ip = val;
}

std::string networkconfig::Getcurrent_gateway() const
{
	return this->current_gateway;
}

void networkconfig::Setcurrent_gateway(std::string& val)
{
	this->current_gateway = val;
}

std::string networkconfig::Getcurrent_netmask() const
{
	return this->current_netmask;
}

void networkconfig::Setcurrent_netmask(std::string& val)
{
	this->current_netmask = val;
}

std::string networkconfig::Getcurrent_dnssrv1() const
{
	return this->current_dnssrv1;
}

void networkconfig::Setcurrent_dnssrv1(std::string& val)
{
	this->current_dnssrv1 = val;
}

std::string networkconfig::Getcurrent_dnssrv2() const
{
	return this->current_dnssrv2;
}

void networkconfig::Setcurrent_dnssrv2(std::string& val)
{
	this->current_dnssrv2 = val;
}

std::string networkconfig::Getconfig_ip() const
{
	return this->config_ip;
}

void networkconfig::Setconfig_ip(std::string& val)
{
	this->config_ip = val;
}

std::string networkconfig::Getconfig_gateway() const
{
	return this->config_gateway;
}

void networkconfig::Setconfig_gateway(std::string& val)
{
	this->config_gateway = val;
}

std::string networkconfig::Getconfig_netmask() const
{
	return this->config_netmask;
}

void networkconfig::Setconfig_netmask(std::string& val)
{
	this->config_netmask = val;
}

std::string networkconfig::Getconfig_dnssrv1() const
{
	return this->config_dnssrv1;
}

void networkconfig::Setconfig_dnssrv1(std::string& val)
{
	this->config_dnssrv1 = val;
}

std::string networkconfig::Getconfig_dnssrv2() const
{
	return this->config_dnssrv2;
}

void networkconfig::Setconfig_dnssrv2(std::string& val)
{
	this->config_dnssrv2 = val;
}

std::string networkconfig::Getconfig_dnsname() const
{
	return this->config_dnsname;
}

void networkconfig::Setconfig_dnsname(std::string val)
{
	this->config_dnsname = val;
}

unsigned char networkconfig::Getconfig_crypt() const
{
	return this->config_crypt;
}

void networkconfig::Setconfig_crypt(unsigned char val)
{
	this->config_crypt = val;
}

unsigned char networkconfig::Getconfig_ipflags() const
{
	return this->config_ipflags;
}

void networkconfig::Setconfig_ipflags(unsigned char val)
{
	this->config_ipflags = val;
}

unsigned char networkconfig::Getconfig_dnsnamelenght() const
{
	return 16;
}

// Gets the settings from the config file
int networkconfig::LoadConfigFile()
{
	std::string findStr;
	findStr.append(1, '=');
	std::ifstream infile;
	infile.open(configFileName, std::ios_base::in);

	std::string key;
	std::string value;
	std::string::size_type index = std::string::npos;

	if(infile.is_open())
	{
		std::string line;
		while (std::getline(infile, line))
		{
			index = line.find(findStr, 0);
			if(index != std::string::npos)
			{
				key = line.substr(0, index);
				value = line.substr(index + 1);
				if(value.compare("") == 0 || value.compare(" ") == 0)
				{
					// replace empty values with 0.0.0.0
					value = "0.0.0.0";
				}

				if(key.compare(HOSTNAME) == 0)
				{
					this->config_dnsname = value;
				}
				else if(key.compare(MODE) == 0)
				{
					if(value.compare("DHCP") == 0)
					{
						this->config_ipflags = 0x01;
					}
					else
					{
						this->config_ipflags = 0x00;
					}
				}
				else if(key.compare(CRYPT) == 0)
				{
					if(value.compare("1") == 0)
					{
						this->config_crypt = 0x01;
					}
					else
					{
						this->config_crypt = 0x00;
					}
				}
				else if(key.compare(IP) == 0)
				{
					this->config_ip = value;
				}
				else if(key.compare(NETMASK) == 0)
				{
					this->config_netmask = value;
				}
				else if(key.compare(GATEWAY) == 0)
				{
					this->config_gateway = value;
				}
				else if(key.compare(NAMESERVER1) == 0)
				{
					this->config_dnssrv1 = value;
				}
				else if(key.compare(NAMESERVER2) == 0)
				{
					this->config_dnssrv2 = value;
				}
				else if(key.compare(CURRENT_IP) == 0)
				{
					this->current_ip = value;
				}
				else if(key.compare(CURRENT_GATEWAY) == 0)
				{
					this->current_gateway = value;
				}
				else if(key.compare(CURRENT_NETMASK) == 0)
				{
					this->current_netmask = value;
				}
				else if(key.compare(CURRENT_NAMESERVER1) == 0)
				{
					this->current_dnssrv1 = value;
				}
				else if(key.compare(CURRENT_NAMESERVER2) == 0)
				{
					this->current_dnssrv2 = value;
				}
			}
		}

		infile.close();

		if(!networkconfig::GetCurrentAesKey().empty())
		{
			this->config_crypt += 0x02;
		}
		return 0;
	}
	else
	{
		infile.close();
		return 1;
	}
}

// Writes the information to the config file
int networkconfig::WriteConfigFile() const
{
	std::ofstream outfile;
	outfile.open(configFileName, std::ios_base::out | std::ios_base::trunc);

	if(outfile.is_open())
	{
		std::stringstream ss;
		ss << HOSTNAME << '=' << this->config_dnsname << std::endl;
		ss << MODE << '=';
		if(this->config_ipflags == 0)
		{
			ss << "MANUAL" << std::endl;
		}
		else
		{
			ss << "DHCP" << std::endl;
		}

		ss << CURRENT_IP << '=' << this->current_ip << std::endl;
		ss << CURRENT_NETMASK << '=' << this->current_netmask << std::endl;
		ss << CURRENT_GATEWAY << '=' << this->current_gateway << std::endl;
		ss << CURRENT_NAMESERVER1 << '=' << this->current_dnssrv1 << std::endl;
		ss << CURRENT_NAMESERVER2 << '=' << this->current_dnssrv2 << std::endl;
		ss << IP << '=' << this->config_ip << std::endl;
		ss << NETMASK << '=' << this->config_netmask << std::endl;
		ss << GATEWAY << '=' << this->config_gateway << std::endl;
		ss << NAMESERVER1 << '=' << this->config_dnssrv1 << std::endl;
		ss << NAMESERVER2 << '=' << this->config_dnssrv2 << std::endl;
		
		ss << CRYPT << '=';
		if((this->config_crypt & 0x01) == 0)
		{
			ss << "0" << std::endl;
		}
		else
		{
			ss << "1" << std::endl;
		}

		outfile << ss.str();

		outfile.close();
		return 0;
	}
	else
	{
		outfile.close();
		return 1;
	}
}

// Appends the current network information to the response string
void networkconfig::AppendCurrentConfigToResponse(std::string & response) const
{
	unsigned int tempvalue(0);
	// Ack
	response.append(1, (char)0x01);
	// ip
	tempvalue = (unsigned int)inet_addr(this->current_ip.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
	// gateway
	tempvalue = (unsigned int)inet_addr(this->current_gateway.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
	// netmask
	tempvalue = (unsigned int)inet_addr(this->current_netmask.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
	// dns1
	tempvalue = (unsigned int)inet_addr(this->current_dnssrv1.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
	// dns2
	tempvalue = (unsigned int)inet_addr(this->current_dnssrv2.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
}

// Appends the current network configuration to the response string
void networkconfig::AppendConfigToResponse(std::string & response) const
{
	unsigned int tempvalue(0);
	// Ack
	response.append(1, (char)0x01);
	// ip
	tempvalue = (unsigned int)inet_addr(this->config_ip.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
	// gateway
	tempvalue = (unsigned int)inet_addr(this->config_gateway.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
	// netmask
	tempvalue = (unsigned int)inet_addr(this->config_netmask.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
	// dns1
	tempvalue = (unsigned int)inet_addr(this->config_dnssrv1.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
	// dns2
	tempvalue = (unsigned int)inet_addr(this->config_dnssrv2.c_str());
	response.append(1, (char)(tempvalue & 0xff));
	response.append(1, (char)((tempvalue >> 8) & 0xff));
	response.append(1, (char)((tempvalue >> 16) & 0xff));
	response.append(1, (char)((tempvalue >> 24) & 0xff));
	// dhcp / autoip flags
	response.append(1, (char)this->config_ipflags);
	// crypt flags
	response.append(1, (char)this->config_crypt);
	// dnsname length
	response.append(1, (char)this->Getconfig_dnsnamelenght());
	// dnsname
	response.append(this->config_dnsname);
	response.append(1, (char)0x00);
}

// Sets the network configuration and appends the ack or nak to the response string
void networkconfig::SetConfigAndAppendResultResponse(std::string & response, std::string & payload)
{
	if(payload.size() < 23)
	{
		// Data to short send nak
		response.append(1, (char)0x00);
	}
	else
	{
		std::string         zeroStr;
		zeroStr.append(1, (char)0x00);
		char * buffp = new char[20];
		// Convert data to ip address as string
		sprintf(buffp, "%u.%u.%u.%u", (unsigned char)payload.at(0), (unsigned char)payload.at(1), (unsigned char)payload.at(2), (unsigned char)payload.at(3));
		std::string tempip(buffp);
		this->config_ip = tempip;

		// Convert data to gateway address as string
		sprintf(buffp, "%u.%u.%u.%u", (unsigned char)payload.at(4), (unsigned char)payload.at(5), (unsigned char)payload.at(6), (unsigned char)payload.at(7));
		std::string tempgateway(buffp);
		this->config_gateway = tempgateway;

		// Convert data to netmask as string
		sprintf(buffp, "%u.%u.%u.%u", (unsigned char)payload.at(8), (unsigned char)payload.at(9), (unsigned char)payload.at(10), (unsigned char)payload.at(11));
		std::string tempnetmask(buffp);
		this->config_netmask = tempnetmask;

		// Convert data to primary dns server address as string
		sprintf(buffp, "%u.%u.%u.%u", (unsigned char)payload.at(12), (unsigned char)payload.at(13), (unsigned char)payload.at(14), (unsigned char)payload.at(15));
		std::string tempdnssrv1(buffp);
		this->config_dnssrv1 = tempdnssrv1;

		// Convert data to secondary dns server address as string
		sprintf(buffp, "%u.%u.%u.%u", (unsigned char)payload.at(16), (unsigned char)payload.at(17), (unsigned char)payload.at(18), (unsigned char)payload.at(19));
		std::string tempdnssrv2(buffp);
		this->config_dnssrv2 = tempdnssrv2;

		// Get ip flags from data
		this->config_ipflags = (unsigned char)payload.at(20);

		// Get crypt flag from data
		this->config_crypt = (unsigned char)payload.at(21);

		// Get last index of dns name and get it from data
		std::string::size_type index = payload.find(zeroStr, 22);
		if(index != std::string::npos)
		{
			std::string tempdnsname = payload.substr(22, index - 22);
			this->config_dnsname = tempdnsname;
		}

		// Write config to file
		if(this->WriteConfigFile() == 0)
		{
			response.append(1, (char)0x01);
		}
		else
		{
			response.append(1, (char)0x00);
		}
		
		// Add is default key flag to crypt flag
		if(!networkconfig::GetCurrentAesKey().empty())
		{
			this->config_crypt += 0x02;
		}

		delete[] buffp;
	}
}
