/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// Hexfile.cpp: Implementierung der Klasse Hexfile.
//
//////////////////////////////////////////////////////////////////////

#include "Hexfile.h"
#include "Logger.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

Hexfile::Hexfile():start(0)
{

}

Hexfile::~Hexfile()
{

}

int Hexfile::Read(const std::string& filename)
{
    char line[256];
    FILE* f;
    f=fopen(filename.c_str(), "r");
    
    if(!f){
        return -1;
    }
	this->filename=filename;
	start=-1;
	buffer.clear();
    while(fgets(line, 256, f)){
        unsigned char reclen=0;
        int address=0xffff;
        unsigned char rectype=0xff;
        unsigned char checksum=0;
        unsigned char i=0;
        unsigned int datapos=0;
        if(line[i++]!=':'){
            fclose(f);
            return -1;
        }
//        printf(line);
        while(line[i]&&line[i+1]){
            unsigned char byte=Hexbyte(line+i);
            checksum+=byte;
            if(i==1)reclen=byte;
            else if(i==3)address=byte<<8;
            else if(i==5){
                address|=byte;
                //printf("address=%04X\n", address);
            }
            else if(i==7)rectype=byte;
            else if(i>=9){
                //printf("%d ",datapos);
                if(rectype==0x00){
	                if(datapos<reclen){
						if((address+datapos+1) > buffer.size()){
							buffer.resize(address+datapos+1);
						}
                        buffer[address+datapos]=byte;
		                datapos++;
	                }else{
		                if((address<start) || (start<0))start=address;
			            break;
				    }
				}
            }
            i+=2;
        }
        //printf("\ni=%d\n", (int)i);
        if(checksum){
            LOG(Logger::LOG_WARNING, "Hex record checksum error");
        }
        if(rectype==0x01){
            fclose(f);
			buffer.erase(buffer.begin(), buffer.begin()+start);
            return 1;
        }else if(rectype>0x01){
            LOG(Logger::LOG_WARNING, "Record type 0x%X not supported", (int)rectype);
        }
    }
    LOG(Logger::LOG_ERROR, "End-Of-File record missing");
    printf("End-Of-File record missing\n");
    fclose(f);
    return -1;

}

ucVec& Hexfile::GetBuffer()
{
	return buffer;
}

int Hexfile::GetStart()
{
	return start;
}

unsigned char Hexfile::Hexbyte(const char *s)
{
    unsigned char b=0;
    unsigned char i;
    for(i=0;i<2;i++){
        b<<=4;
        if(*s>='0' && *s<='9')b+=*s-'0';
        else if(*s>='a' && *s<='f')b+=*s-'a'+0x0a;
        else if(*s>='A' && *s<='F')b+=*s-'A'+0x0a;
        s++;
    }
    return b;

}

int Hexfile::Modify(int address, const ucVec &data)
{
	unsigned int min_size=address+data.size();
	if(buffer.size()<min_size)buffer.resize(min_size);
	std::copy(data.begin(), data.end(), buffer.begin()+address);
	return 1;
}

const std::string& Hexfile::GetFilename()
{
	return filename;
}
