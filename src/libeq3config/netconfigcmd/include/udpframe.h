/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef UDPFRAME_H
#define UDPFRAME_H

#include <string>


class udpframe
{
    public:
        udpframe(std::string& data);
        virtual ~udpframe();

        std::string Getdevicetype() const;
        std::string Getserialnumber() const;
        std::string Getpayload() const;
        unsigned char Getversion() const;
        unsigned char Getcounter() const;
        unsigned int Getsenderid() const;
        char Getopcode() const;

        bool IsTarget(const std::string type, const std::string serial) const;
        std::string GetResponseHeader(const std::string type, const std::string serial) const;

    protected:
    private:
        std::string devicetype;
        std::string serialnumber;
        std::string payload;
        unsigned char version;
        unsigned char counter;
        unsigned int senderid;
        char opcode;
};

#endif // UDPFRAME_H
