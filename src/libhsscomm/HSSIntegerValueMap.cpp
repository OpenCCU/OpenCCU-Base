/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSIntegerValueMap.cpp: Implementierung der Klasse HSSIntegerValueMap.
//
//////////////////////////////////////////////////////////////////////

#include "HSSIntegerValueMap.h"
#include <Logger.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSIntegerValueMap::HSSIntegerValueMap()
{
}

HSSIntegerValueMap::~HSSIntegerValueMap()
{

}

bool HSSIntegerValueMap::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSIntegerValueMap::InitFromXml()");

    int i=0;
    XMLNode entry_node=node.getChildNode("value_map", &i);
    while(!entry_node.isEmpty()){
        const char* temp=entry_node.getAttribute("device_value");
        if(!temp)return false;

        int device_value=strtol(temp, NULL, 0);

        temp=entry_node.getAttribute("parameter_value");
        if(!temp)return false;

        int parameter_value=strtol(temp, NULL, 0);

//		LOG(Logger::LOG_DEBUG, "parameter_value=0x%X, device_value=0x%X", parameter_value, device_value);

        temp=entry_node.getAttribute("from_device");
		if(!temp || temp[0]=='t'){
			if(from_device_map.find(device_value)==from_device_map.end()){
				from_device_map[device_value]=parameter_value;
			}
		}

        temp=entry_node.getAttribute("to_device");
		if(!temp || temp[0]=='t'){
			if(to_device_map.find(parameter_value)==to_device_map.end()){
				to_device_map[parameter_value]=device_value;
			}
		}

        temp=entry_node.getAttribute("mask");
		if(temp){
			masked_value_t mv;
			mv.mask=strtol(temp, NULL, 0);
			mv.dev_value=device_value&mv.mask;
			mv.param_value=parameter_value;
			masked_vec.push_back(mv);
		}

        entry_node=node.getChildNode("value_map", &i);
    }
    return true;
}

int HSSIntegerValueMap::MapToDevice(int val)
{
    map_t::iterator it=to_device_map.find(val);
    if(it==to_device_map.end())return val;
    return it->second;
}

int HSSIntegerValueMap::MapFromDevice(int val)
{
//	LOG(Logger::LOG_DEBUG, "HSSIntegerValueMap::MapFromDevice(0x%X)", val);
    map_t::iterator it=from_device_map.find(val);
    if(it!=from_device_map.end())return it->second;
	for(masked_vec_t::iterator it=masked_vec.begin();it!=masked_vec.end();it++){
		if(it->dev_value==(val & it->mask))return it->param_value;
	}
	return val;
}
