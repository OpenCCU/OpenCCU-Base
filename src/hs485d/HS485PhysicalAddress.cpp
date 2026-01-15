/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485PhysicalAddress.cpp: Implementierung der Klasse HS485PhysicalAddress.
//
//////////////////////////////////////////////////////////////////////

#include "HS485PhysicalAddress.h"
#include <Logger.h>

#include <string>
#include <cstring>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HS485PhysicalAddress::HS485PhysicalAddress()
{
    index_condition=-1;
    byte_increment=0;
    bit_increment=0;
	bit_address=0;
	byte_address=0;
}

HS485PhysicalAddress::~HS485PhysicalAddress()
{

}

bool HS485PhysicalAddress::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HS485PhysicalAddress::InitFromXml()");
    const char* temp=node.getAttribute("index_condition");
    if(temp){
        index_condition=strtol(temp, NULL, 0);
    }

	XMLNode paramset_node;
	if(index_condition<0){
		temp=node.getAttribute("step");
		if(temp){
			std::string s=temp;
			std::string::size_type pos=s.find('.');
			if(pos)byte_increment=strtoul(s.substr(0, pos).c_str(), NULL, 0);
			if(pos!=std::string::npos)bit_increment=strtoul(s.substr(pos+1).c_str(), NULL, 0);
		}else{
			paramset_node=node.getParentNode();
			while(!paramset_node.isEmpty()){
				if(strcmp(paramset_node.getName(), "paramset")==0)break;
				paramset_node=paramset_node.getParentNode();
			}
			if(!paramset_node.isEmpty()){
				temp=paramset_node.getAttribute("address_step");
				if(temp){
					std::string s=temp;
					std::string::size_type pos=s.find('.');
					if(pos)byte_increment=strtoul(s.substr(0, pos).c_str(), NULL, 0);
					if(pos!=std::string::npos)bit_increment=strtoul(s.substr(pos+1).c_str(), NULL, 0);
				}
			}
		}
	}

    temp=node.getAttribute("index");
	if(temp){
		byte_address=bit_address=0;
		std::string s;
		if(temp[0]=='+'){
			s=temp+1;
			if(paramset_node.isEmpty()){
				paramset_node=node.getParentNode();
				while(!paramset_node.isEmpty()){
					if(strcmp(paramset_node.getName(), "paramset")==0)break;
					paramset_node=paramset_node.getParentNode();
				}
			}
			if(!paramset_node.isEmpty()){
				temp=paramset_node.getAttribute("address_start");
				if(temp){
					std::string s=temp;
					std::string::size_type pos=s.find('.');
					if(pos)byte_address=strtoul(s.substr(0, pos).c_str(), NULL, 0);
					if(pos!=std::string::npos)bit_address=strtoul(s.substr(pos+1).c_str(), NULL, 0);
				}
			}
		}else{
			s=temp;
		}
		std::string::size_type pos=s.find('.');
		if(pos)byte_address+=strtoul(s.substr(0, pos).c_str(), NULL, 0);
		if(pos!=std::string::npos)bit_address+=strtoul(s.substr(pos+1).c_str(), NULL, 0);
	}
    return true;
}

bool HS485PhysicalAddress::CalculateAddress(unsigned int index, unsigned int* byte, unsigned int* bit)
{
	if(index_condition>=0 && index!=(unsigned int)index_condition)return false;

	*bit=bit_address+bit_increment*index;
	*byte=byte_address+byte_increment*index;

	if(*bit>=8){
		*byte+=(*bit)/8;
		*bit %= 8;
	}
//	LOG(Logger::LOG_DEBUG, "CalculateAddress(%d, %d, %d)", index, *byte, *bit);
	return true;
}
