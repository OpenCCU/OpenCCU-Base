/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef NETWORKCONFIG_H
#define NETWORKCONFIG_H

#include <string.h>
#include <string>

class networkconfig
{
    public:
        networkconfig();
        virtual ~networkconfig();

	static std::string GetCurrentAesKey(void);

        std::string Getcurrent_ip() const;
        void Setcurrent_ip(std::string& val);

        std::string Getcurrent_gateway() const;
        void Setcurrent_gateway(std::string& val);

        std::string Getcurrent_netmask() const;
        void Setcurrent_netmask(std::string& val);

        std::string Getcurrent_dnssrv1() const;
        void Setcurrent_dnssrv1(std::string& val);

        std::string Getcurrent_dnssrv2() const;
        void Setcurrent_dnssrv2(std::string& val);

        std::string Getconfig_ip() const;
        void Setconfig_ip(std::string& val);

        std::string Getconfig_gateway() const;
        void Setconfig_gateway(std::string& val);

        std::string Getconfig_netmask() const;
        void Setconfig_netmask(std::string& val);

        std::string Getconfig_dnssrv1() const;
        void Setconfig_dnssrv1(std::string& val);

        std::string Getconfig_dnssrv2() const;
        void Setconfig_dnssrv2(std::string& val);

        std::string Getconfig_dnsname() const;
        void Setconfig_dnsname(std::string val);

        unsigned char Getconfig_crypt() const;
        void Setconfig_crypt(unsigned char val);

        unsigned char Getconfig_ipflags() const;
        void Setconfig_ipflags(unsigned char val);

        unsigned char Getconfig_dnsnamelenght() const;

        int LoadConfigFile();
        int WriteConfigFile() const;
		
	void AppendCurrentConfigToResponse(std::string & response) const;
	void AppendConfigToResponse(std::string & response) const;
	void SetConfigAndAppendResultResponse(std::string & response, std::string & payload);
		
    protected:
    private:
        std::string current_ip;
        std::string current_gateway;
        std::string current_netmask;
        std::string current_dnssrv1;
        std::string current_dnssrv2;
        std::string config_ip;
        std::string config_gateway;
        std::string config_netmask;
        std::string config_dnssrv1;
        std::string config_dnssrv2;
        std::string config_dnsname;
        unsigned char config_crypt;
        unsigned char config_ipflags;
};

#endif // NETWORKCONFIG_H
