/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosInterfaceMessage.h"
#include <stdlib.h>
#include <stdio.h>
#include <Logger.h>

const BidcosInterfaceMessage::MessageDefinition BidcosInterfaceMessage::s_message_definitions[] = {
    //opcode, # of parameters, string of one character for each parameter. Character semantics:
    //  '1'-'4': size in bytes of numerical paramer
    //  's': Character string
    //  'b': Binary string
    { CMD_HELLO, 7, 8, "s2s33421" },
	{ CMD_RESPONSE, 6, 6, "42412b" },
	{ CMD_EVENT, 6, 6, "32412b" },
    { CMD_KEY_INDEXES, 4, 4, "1111" },
    { CMD_SET_IV, 1, 1, "b" },
	{ CMD_SEND, 6, 6, "41414b" },
	{ CMD_ADD_DEVICE, 4, 4, "311b" },
	{ CMD_DELETE_DEVICE, 1, 1, "3" },
	{ CMD_CLEAR_DEVICES, 0, 0, "" },
	{ CMD_TIME, 4, 4, "4114" },
    { CMD_SET_ADDRESS, 1, 1, "3" },
    { CMD_KEEPALIVE, 0, 0, "" },
    { CMD_GET_KEY_INDEXES, 0, 0, },
    { CMD_SET_KEY, 3, 3, "11b" },
    { CMD_DUMP, 2, 2, "11" },
    { CMD_DUMP_RESULT, 3, 3, "11b" },
    { 0 }
};

BidcosInterfaceMessage::BidcosInterfaceMessage()
{
    msg_def = NULL;
}

BidcosInterfaceMessage::~BidcosInterfaceMessage()
{
}

bool BidcosInterfaceMessage::FromString(const std::string& s)
{
	if( s.size() < 1 )return false;
    vec_params.clear();

    if( GetMessageDefinition(s[0]) ){
    	std::string::size_type pos = 1;
	    while( (pos != std::string::npos) && (vec_params.size() < msg_def->max_param_count) )
	    {
		    std::string::size_type end_pos = s.find_first_of(",\r\n", pos);
            std::string s_param=s.substr(pos, end_pos-pos);
            BinaryType param;
            if( msg_def->fields[vec_params.size()] != 's' ){
                for( unsigned int i=0;i<s_param.size();i+=2 ){
                    param.append(1, (unsigned char)strtol(s_param.substr(i, 2).c_str(), NULL, 16));
                }
            }else{
                for( unsigned int i=0;i<s_param.size();i++ ){
                    param.append(1, (unsigned char)s_param[i]);
                }
            }
            vec_params.push_back( param );
		    pos=s.find(',', end_pos);
		    if(pos != std::string::npos)pos++;
	    }
    }
    if( !msg_def || (vec_params.size() < msg_def->min_param_count) || (msg_def->max_param_count < vec_params.size() ) ){
        msg_def = NULL;
        std::string msg_dump;
        for(unsigned int i=0;i<s.size();i++){
            char c=s[i];
            if(c>=' ' && c<=0x7e && c!='\\'){
                msg_dump.append(1, c);
            }else{
                char buf[8];
                snprintf(buf, sizeof(buf), "\\x%02x", int(c)&0xff);
                msg_dump+=buf;
            }
        }
        LOG(Logger::LOG_ERROR, "Invalid Message received: %s", msg_dump.c_str());
        return false;
    }
    return true;
}

bool BidcosInterfaceMessage::FromBinary(const BidcosInterfaceMessage::BinaryType& b)
{
	if( b.size() < 1 )return false;
    if( !GetMessageDefinition(b[0]) ){
        LOG(Logger::LOG_ERROR, "Invalid Message received");
        return false;
    }

    vec_params.clear();

    unsigned int index=1;
    for(unsigned int i=0;i<msg_def->max_param_count;i++){
        if(index >= b.size())return false;
        unsigned int param_size=msg_def->fields[i];
        if(param_size>='1' && param_size<='4'){
            //numeric value
            param_size-='0';
        } else {
            param_size=b[index++];
        }
        vec_params.push_back(b.substr(index, param_size));
        if(vec_params.back().size() != param_size)return false;
        index += param_size;
    }
    return vec_params.size() == msg_def->max_param_count;
}

std::string BidcosInterfaceMessage::ToString()
{
    char buf[4];
	std::string s;
    if(!msg_def)return s;
	s.append(1, msg_def->opcode);
	for(unsigned int i=0;i<vec_params.size();i++)
	{
		if(i)s+=',';

        if( msg_def->fields[i] != 's' ){
            for( unsigned int j=0;j<vec_params[i].size();j++ ){
                snprintf(buf, sizeof(buf), "%02X", ((int)vec_params[i][j])&0xff);
                s += buf;
            }
        }else{
            for( unsigned int j=0;j<vec_params[i].size();j++ ){
                s.append(1, (char)vec_params[i][j]);
            }
        }
	}
	s+="\r\n";
    return s;
}

BidcosInterfaceMessage::BinaryType BidcosInterfaceMessage::ToBinary()
{
	BinaryType b;
    if(!msg_def)return b;
	b.append(1, msg_def->opcode);
	for(unsigned int i=0;i<vec_params.size();i++)
	{
        if( msg_def->fields[i] == 'b' || msg_def->fields[i] == 's' ){
            b.append( 1, vec_params[i].size() );
        }
        b += vec_params[i];
	}
    return b;
}


int BidcosInterfaceMessage::GetSIntParam(unsigned int index)
{
	if( index >= vec_params.size() )return 0;
    int value=0;
    for( unsigned int i=0;i<vec_params[index].size();i++ ){
        value <<= 8;
        value |= vec_params[index][i];
    }
    if(value & (1<<(vec_params[index].size()*8-1))){
        //negative value needs sign extension
        value |= 0xffffffff<<(vec_params[index].size()*8);
    }
    return value;
}

unsigned int BidcosInterfaceMessage::GetUIntParam(unsigned int index)
{
	if( index >= vec_params.size() )return 0;
    int value=0;
    for( unsigned int i=0;i<vec_params[index].size();i++ ){
        value <<= 8;
        value |= vec_params[index][i];
    }
    return value;
}

std::string BidcosInterfaceMessage::GetStringParam(unsigned int index)
{
    std::string s;
	if( index >= vec_params.size() )return "";
    for(unsigned int i=0;i<vec_params[index].size();i++)s.append(1, (char)vec_params[index][i]);
	return s;
}

BidcosFrame BidcosInterfaceMessage::GetFrameParam(unsigned int index)
{
	BidcosFrame frame;
	if( index >= vec_params.size() )return frame;
    for(unsigned int i=0;i<vec_params[index].size();i++)frame.SetByteData(i, vec_params[index][i]);
	return frame;
}


bool BidcosInterfaceMessage::PutUIntParam( unsigned int index, unsigned int value)
{
    if(!msg_def)return false;
    if(index >= msg_def->max_param_count) return false;
    unsigned int size=msg_def->fields[index];
    if(size>'4' || size <'1')return false;
    size -= '0';
	if(index >= vec_params.size())vec_params.resize(index+1);
    vec_params[index].clear();
    while(size>0){
        size--;
        vec_params[index].append(1, (value>>(8*size))&0xff);
    }
	return true;
}

bool BidcosInterfaceMessage::PutSIntParam( unsigned int index, int value)
{
    return PutUIntParam(index, value);
}

bool BidcosInterfaceMessage::PutStringParam( unsigned int index, const std::string& value)
{
    if(!msg_def)return false;
    if(index >= msg_def->max_param_count) return false;
    if(msg_def->fields[index] != 's')return false;
	if(index >= vec_params.size())vec_params.resize(index+1);
    vec_params[index].clear();
    for(unsigned int i=0;i<value.size();i++)vec_params[index].append(1, value[i]);
    return true;
}

bool BidcosInterfaceMessage::PutFrameParam( unsigned int index, const StructuredFrame* frame)
{
    if(!msg_def)return false;
    if(index >= msg_def->max_param_count) return false;
    if(msg_def->fields[index] != 'b')return false;
	if(index >= vec_params.size())vec_params.resize(index+1);
    vec_params[index].clear();
    for(unsigned int i=0;i<frame->GetSize();i++)vec_params[index].append(1, frame->GetByteData(i));
    return true;
}


bool BidcosInterfaceMessage::PutBinaryParam( unsigned int index, const BinaryType& b)
{
    if(!msg_def)return false;
    if(index >= msg_def->max_param_count) return false;
    if(msg_def->fields[index] != 'b')return false;
	if(index >= vec_params.size())vec_params.resize(index+1);
    vec_params[index]=b;
    return true;
}

bool BidcosInterfaceMessage::PutBinaryParam( unsigned int index, const std::string& s)
{
    if(!msg_def)return false;
    if(index >= msg_def->max_param_count) return false;
    if(msg_def->fields[index] != 'b')return false;
	if(index >= vec_params.size())vec_params.resize(index+1);
    BinaryType b((unsigned char*)s.data(), s.size());
    vec_params[index]=b;
    return true;
}

BidcosInterfaceMessage::BinaryType BidcosInterfaceMessage::GetBinaryParam( unsigned int index )
{
	if( index >= vec_params.size() )return BinaryType();
    return vec_params[index];
}

unsigned int BidcosInterfaceMessage::GetParamCount()
{
    return vec_params.size();
}

int BidcosInterfaceMessage::GetCommand()
{
    if( !msg_def )return 0;
	return msg_def->opcode;
}

bool BidcosInterfaceMessage::SetCommand(int cmd)
{
    return GetMessageDefinition(cmd);
}

bool BidcosInterfaceMessage::GetMessageDefinition(int cmd)
{
    msg_def=s_message_definitions;
    while(msg_def->opcode && (msg_def->opcode!=cmd))msg_def++;
    if(!msg_def->opcode){
        msg_def=NULL;
        return false;
    }else{
        return true;
    }
}

