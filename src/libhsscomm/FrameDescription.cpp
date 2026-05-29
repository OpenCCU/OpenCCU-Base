/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "FrameDescription.h"
#include "StructuredFrame.h"
#include <Logger.h>
#include <string.h>

using namespace XmlRpc;

FrameDescription::FrameDescription(void):id("")
, valueForwarding(false)
{
	direction=DIR_TO_DEVICE;
	type=-1;
	fixed_channel=0;
	channel_field = 0;
	receiver_channel_field=0;
	receiver_mask=(1<<StructuredFrame::RCV_UNKNOWN)|(1<<StructuredFrame::RCV_BROADCAST)|(1<<StructuredFrame::RCV_CENTRAL)|(1<<StructuredFrame::RCV_OTHER);
}

FrameDescription::~FrameDescription(void)
{
}

FrameDescription::Parameter::Parameter()
{
	index_bytes=0;
	index_bits=0;
	size_bits=0;
	size_bytes=0;
	type=TYPE_INTEGER;
	flags=FLAG_NONE;
	cond_op=OP_EQ;
}

FrameDescription::Parameter::Parameter(const FrameDescription::Parameter& p)
{
	*this=p;
	if(type==TYPE_STRING){
		value.as_string=new std::string(*p.value.as_string);
	}
}

FrameDescription::Parameter::~Parameter()
{
	if(type==TYPE_STRING){
		delete value.as_string;
	}
}

bool FrameDescription::Parameter::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    const char* temp=node.getAttribute("type");
	if(temp){
		if(strcmp(temp, "integer")==0)type=TYPE_INTEGER;
		else if(strcmp(temp, "string")==0){
			type=TYPE_STRING;
			value.as_string=new std::string();
		}else return false;
	}

	temp=node.getAttribute("param");
	if(temp){
		param_name=temp;
	}

    temp=node.getAttribute("index");
	if(temp){
		std::string s=temp;
		std::string::size_type pos=s.find('.');
		if(pos)index_bytes=strtol(s.substr(0, pos).c_str(), NULL, 0);
		if(pos!=std::string::npos)index_bits=strtol(s.substr(pos+1).c_str(), NULL, 0);
		flags|=FLAG_CHECK;
	}

    temp=node.getAttribute("size");
	if(temp){
		std::string s=temp;
		std::string::size_type pos=s.find('.');
		if(pos)size_bytes=strtol(s.substr(0, pos).c_str(), NULL, 0);
		if(pos!=std::string::npos)size_bits=strtol(s.substr(pos+1).c_str(), NULL, 0);
		flags|=FLAG_CHECK;
	}

    temp=node.getAttribute("signed");
	if(temp && temp[0]=='t')flags|=FLAG_SIGNED;

    temp=node.getAttribute("const_value");
	if(temp){
		if(type==TYPE_INTEGER)value.as_int=strtol(temp, NULL, 0);
		else if(type==TYPE_STRING){
			*value.as_string=temp;
		}
		flags |= FLAG_CONST;
	}

    temp=node.getAttribute("omit_if");
	if(temp){
		if(flags & FLAG_CONST)return false;
		if(type==TYPE_INTEGER)value.as_int=strtol(temp, NULL, 0);
		else if(type==TYPE_STRING){
			*value.as_string=temp;
		}
		flags |= FLAG_OMIT;
	}

	static const char COND_OPS[]="EQ,LE,LT,GT,GE,NE,";
	temp=node.getAttribute("cond_op");
	if(temp){
		const char* p=strstr(COND_OPS, temp);
		if(!p || p[2]!=','){
			LOG(Logger::LOG_WARNING, "cond_op unknown: %s", temp);
			return false;
		}
		cond_op=(p-COND_OPS)/3;
	}

    return true;

}

bool FrameDescription::Parameter::FrameCheckValue(StructuredFrame& frame)
{
	switch(type){
		case TYPE_INTEGER:
		{
			uint32_t val;
			if(!frame.GetIntValue(index_bytes, index_bits, size_bytes, size_bits, &val))return false;
//			LOG(Logger::LOG_DEBUG, "FrameCheckValue() %d.%d:%d.%d, op=%d, args=(%lu, %d)", index_bytes, index_bits, size_bytes, size_bits, cond_op, val, value.as_int);
			switch(cond_op){
				case OP_EQ:
					return val==value.as_int;
				case OP_LT:
					return val<value.as_int;
				case OP_LE:
					return val<=value.as_int;
				case OP_GT:
					return val>value.as_int;
				case OP_GE:
					return val>=value.as_int;
				case OP_NE:
					return val!=value.as_int;
			}
		}
		case TYPE_STRING:
		{
			std::string val;
			if(!frame.GetStringValue(index_bytes, size_bytes, &val))return false;
			return val==*value.as_string;
		}
		default:
			return false;
	}
}

bool FrameDescription::Parameter::FrameSetValue(StructuredFrame* frame)
{
	switch(type){
		case TYPE_INTEGER:
			return frame->SetIntValue(index_bytes, index_bits, size_bytes, size_bits, value.as_int);
		case TYPE_STRING:
			return frame->SetStringValue(index_bytes, size_bytes, *value.as_string);
		default:
			return false;
	}
}

bool FrameDescription::Parameter::MatchFrame(ValueStore* store, StructuredFrame& frame)
{
	//	LOG(Logger::LOG_DEBUG, "Parameter::MatchFrame(%p, %s)", inst, frame.DumpToString().c_str());
	try{
		XmlRpcValue param;
		if(flags & FLAG_CONST){
			if((flags & FLAG_CHECK) && !FrameCheckValue(frame))return false;
			if(store){
				switch(type){
					case TYPE_INTEGER:
						param=(int)value.as_int;
						break;
					case TYPE_STRING:
						param=*value.as_string;
						break;
				}
			}
		}else if(store){
			switch(type){
				case TYPE_INTEGER:
					{
						uint32_t val;
						if(!frame.GetIntValue(index_bytes, index_bits, size_bytes, size_bits, &val))return false;
						//sign extend
						if((flags & FLAG_SIGNED) && (val&(1<<(size_bytes*8+size_bits-1)))){
							val|=0xffffffff<<(size_bytes*8+size_bits);
						}
						param=(int)val;
						break;
					}
				case TYPE_STRING:
					{
						std::string val;
						if(!frame.GetStringValue(index_bytes, size_bytes, &val))return false;
						param=val;
						break;
					}
				default:
					return false;
			}
		}
		if(param.valid())store->SetStoredValue(param_name, param, ValueStore::FLAG_VOLATILE);
	}catch(XmlRpcException){
		return false;
	}
	return true;
}

bool FrameDescription::Parameter::InitFrame(ValueStore* store, StructuredFrame* frame)
{
	if(flags & FLAG_CONST){
		FrameSetValue(frame);
	}else{
		try{
			XmlRpcValue param;
			if(!store->GetStoredValue(param_name, &param)){
				LOG(Logger::LOG_WARNING, "Could not initialize frame. Value for parameter %s not found.", param_name.c_str());
				return false;
			}
			switch(type){
				case TYPE_INTEGER:
					if((flags & FLAG_OMIT) && ((int&)param==(int)value.as_int))return true;
					frame->SetIntValue(index_bytes, index_bits, size_bytes, size_bits, (int)param);
					break;
				case TYPE_STRING:
					if((flags & FLAG_OMIT) && ((std::string&)param==*value.as_string))return true;
					frame->SetStringValue(index_bytes, size_bytes, (std::string&)param);
					break;
				default:
					return false;
			}
		}catch(XmlRpcException& e){
			LOG(Logger::LOG_WARNING, "Could not initialize frame (parameter %s). %s", param_name.c_str(), e.getMessage().c_str());
			return false;
		}
	}
	return true;
}

bool FrameDescription::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    const char* temp=node.getAttribute("type");
	if(temp){
		if(temp[0]=='#')type=temp[1];
		else type=strtol(temp, NULL, 0);
	}
	if(type==-1)return false;

	temp=node.getAttribute("subtype_index");
	if(temp){
		type|=strtol(temp, NULL, 0)<<8;
		temp=node.getAttribute("subtype");
		if(!temp)return false;
		type|=strtol(temp, NULL, 0)<<16;
	}

	temp=node.getAttribute("id");
	if(temp){
		id=temp;
	}
	if(id.empty())return false;

	temp=node.getAttribute("direction");
	if(temp){
		if(strcmp(temp, "from_device")==0)direction=DIR_FROM_DEVICE;
		else if(strcmp(temp, "to_device")==0)direction=DIR_TO_DEVICE;
		else return false;
	}

	temp=node.getAttribute("channel_field");
	if(temp){
		channel_field=StructuredFrame::FieldIdFromString(temp);
	}else{
		channel_field=0;
	}
	temp=node.getAttribute("receiver_channel_field");
	if(temp){
		receiver_channel_field=StructuredFrame::FieldIdFromString(temp);
	}else{
		receiver_channel_field=0;
	}
	temp=node.getAttribute("fixed_channel");
	if(temp){
		if(temp[0]=='*')fixed_channel=ALL_CHANNELS;
		else fixed_channel=strtoul(temp, NULL, 0);
	}
	temp=node.getAttribute("allowed_receivers");
	if(temp){
		//frames from unknown receiver type are always accepted
		receiver_mask=(1<<StructuredFrame::RCV_UNKNOWN);
		if(strstr(temp, "CENTRAL"))receiver_mask|=(1<<StructuredFrame::RCV_CENTRAL);
		if(strstr(temp, "BROADCAST"))receiver_mask|=(1<<StructuredFrame::RCV_BROADCAST);
		if(strstr(temp, "OTHER"))receiver_mask|=(1<<StructuredFrame::RCV_OTHER);
	}

	temp=node.getAttribute("value_forwarding");
	valueForwarding = temp && temp[0] == 't';

	int i=0;
    XMLNode param_node=node.getChildNode("parameter", &i);
	while(!param_node.isEmpty()){
		params.push_back(Parameter());
		Parameter& param=params.back();
		if(!param.InitFromXml(param_node, root_node)){
			return false;
		}
		param_node=node.getChildNode("parameter", &i);
	}
	return true;
}

bool FrameDescription::InitFrame(StructuredFrame* frame, ValueStore* store, int channel/*=0*/, int receiver_channel/*=0*/)
{
//	if(!to_device)return false;
	if(!frame->SetType(type))return false;
	if(channel_field)frame->SetIntValue(channel_field, channel);
	if(receiver_channel_field)frame->SetIntValue(receiver_channel_field, receiver_channel);
	params_t::iterator it;
	for(it=this->params.begin();it!=this->params.end();it++)
	{
		Parameter& param=*it;
		if(!param.InitFrame(store, frame)){
			return false;
		}
	}
	return true;
}

bool FrameDescription::MatchFrame(StructuredFrame& frame, int* channel)
{
	matched_values.ClearStoredValues();
	if(!(receiver_mask & (1<<frame.GetReceiverType())))return false;
	if(!frame.MatchType(type)){
//		LOG(Logger::LOG_DEBUG, "MatchFrame() type mismatch, type=0x%X", type);
		return false;
	}
	if(channel)*channel=channel_field?frame.GetIntValue(channel_field):fixed_channel;

	params_t::iterator it;
	for(it=this->params.begin();it!=this->params.end();it++)
	{
		Parameter& param=*it;
		if(!param.MatchFrame(&matched_values, frame)){
//			LOG(Logger::LOG_DEBUG, "MatchFrame() parameter mismatch");
			return false;
		}
	}
	return true;
}
