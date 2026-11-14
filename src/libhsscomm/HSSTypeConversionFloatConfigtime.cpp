/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionFloatConfigtime.cpp: Implementierung der Klasse HSSTypeConversionFloatConfigtime.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionFloatConfigtime.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionFloatConfigtime> HSSTypeConversionFloatConfigtimeFactory;


using namespace XmlRpc;

static const double DEFAULT_FACTORS[]={0.1, 1.0, 5.0, 10.0, 60.0, 300.0, 600.0, 3600.0};

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionFloatConfigtime::HSSTypeConversionFloatConfigtime()
{
	for(unsigned int i=0;i<sizeof(DEFAULT_FACTORS)/sizeof(DEFAULT_FACTORS[0]);i++){
		factors.push_back(DEFAULT_FACTORS[i]);
	}
	value_size=5;
}

HSSTypeConversionFloatConfigtime::~HSSTypeConversionFloatConfigtime()
{

}

bool HSSTypeConversionFloatConfigtime::CheckCreationTag(const char *tag)
{
    if(strcmp("type_conversion_float_configtime", tag)==0)return true;
    return false;
}

bool HSSTypeConversionFloatConfigtime::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    const char* temp=node.getAttribute("factors");
	if(temp){
		factors.clear();
		std::string s=temp;
		std::string::size_type left=0;
		while(true){
			std::string::size_type right=s.find(',', left);
			factors.push_back(atof(s.substr(left, right-left).c_str()));
			if(right==std::string::npos)break;
			left=right+1;
		}
	}
    temp=node.getAttribute("value_size");
	if(temp){
		value_size=0;
		std::string s=temp;
		std::string::size_type pos=s.find('.');
		if(pos)value_size=8*strtoul(s.substr(0, pos).c_str(), NULL, 0);
		if(pos!=std::string::npos)value_size+=strtoul(s.substr(pos+1).c_str(), NULL, 0);
	}
    return true;
}

bool HSSTypeConversionFloatConfigtime::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
	int max_value=(1<<value_size)-1;
	try{
		double f_time=(double&)in;
		unsigned int i;
		for(i=0;i<factors.size();i++){
			if(f_time<=max_value*factors[i])break;
		}
		f_time=f_time/factors[i]+0.5;
		int i_time=(int)f_time;
		if(i_time>max_value)i_time=max_value;
		i_time|=i<<value_size;
        ((int&)*out)=i_time;
    }catch(XmlRpcException){
        return false;
    }
	//LOG(Logger::LOG_DEBUG, "HSSTypeConversionFloatConfigtime::LogicalToPhysical(%f, %d) value_size=%d", (double)in, (int)*out, value_size);
    return true;
}

bool HSSTypeConversionFloatConfigtime::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
	int max_value=(1<<value_size)-1;
	int max_factor=factors.size()-1;
    try{
		int i_time=(int&)in;
		(double&)*out=double(i_time&max_value)*factors[(i_time>>value_size)&max_factor];
    }catch(XmlRpcException){
        return false;
    }
	//LOG(Logger::LOG_DEBUG, "HSSTypeConversionFloatConfigtime::PhysicalToLogical(%d, %f) value_size=%d, max_factor=%d", (int)in, (double)*out, value_size, max_factor);
    return true;
}
