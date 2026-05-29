/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _MULTICAST_SENDER_H_
#define _MULTICAST_SENDER_H_

#include "dllexport.h"
#include <string>
#include <vector>

class ELVUTILS_DLLEXPORT MulticastSender
{
public:
    MulticastSender(const char* mcast_group, unsigned int port, bool broadcast=false);
    ~MulticastSender(void);
    bool SendUDPMessage(const std::string& msg, std::vector<std::string>* responses, unsigned int timeout, unsigned int max_response_count);
private:
    void perror(const char* func);
    enum
    {
        UDP_RX_BUFFER_SIZE = 2000
    };
    std::string group;
    int port;
    bool use_broadcast;
};

#endif
