#include "HS485CommMessage.h"
#include <Logger.h>

HS485CommMessage::HS485CommMessage(void)
: CommMessage()
, eepRomUsage(false)
{
	//LOG(Logger::LOG_DEBUG, "HS485CommMessage::HS485CommMessage() this=%p", this);
	SetCommand(CMD_SEND);
	SetReceiveMasks(1, 1, 2);
	SetResponseTimeout(2000);
}

HS485CommMessage::~HS485CommMessage(void)
{
	//LOG(Logger::LOG_DEBUG, "HS485CommMessage::~HS485CommMessage() this=%p", this);
}

int HS485CommMessage::MapIndex(int index)
{
	switch(GetCommand()){
		case CMD_SEND:
            // one byte timeout before frame data
			index+=1;
			break;
		case CMD_BOOT_SEND:
            // sender is missing while receiver is 4 bytes
			if(index>=9)index-=3;//accessing data -> subtract 4 bytes for the missing sender address and add one byte for the timeout
			else if(index>=5)index=-1;//attempting to access sender address -> invalid
			else index+=1;//one byte timeout before fame
			break;
		case CMD_RESPONSE:
            // receiver and sender missing, only ctrl and data
			if(index==4)index=0;
			else if(index>=9)index-=8;
			else index=-1;
			break;
		case CMD_EVENT:
            // complete frame: receiver, ctrl, sender, data
			break;
		case CMD_DISCOVERY:
		case CMD_ERROR:
		case CMD_DISC_RESULT:
		case CMD_DISC_END:
		default:
			index=-1;
	}
	return index;
}

bool HS485CommMessage::MatchType(unsigned long type)
{
	if(!type)return true;
	int index=MapIndex(9);
    if(index<0)return false;
	if(GetByteData(index) != (type&0xff))return false;
	int subtype_field=(type>>8)&0xff;
	if(!subtype_field)return true;
	unsigned long subtype=GetByteData(MapIndex(subtype_field));
	return subtype == type>>16;
}

void HS485CommMessage::SetType(unsigned long type)
{
	int index=MapIndex(9);
    if(index<0)return;
	SetByteData(index, type&0xff);
	int subtype_field=(type>>8)&0xff;
	int subtype=type>>16;
	if(subtype_field){
		SetByteData(MapIndex(subtype_field), subtype);
	}
}

unsigned long HS485CommMessage::GetType()
{
	int index=MapIndex(9);
    if(index<0)return 0;
	return GetByteData(index);
}

uint32_t HS485CommMessage::GetSenderAddress()
{
//	LOG(Logger::LOG_DEBUG, "HS485CommMessage::GetSenderAddress()");
	uint32_t address;
	int index=MapIndex(5);
    if( (index>=0) && GetIntValue(index, 0, 4, 0, &address))return address;
	if(parent)return GetParent()->GetReceiverAddress();
	return 0;
}

uint32_t HS485CommMessage::GetReceiverAddress()
{
//	LOG(Logger::LOG_DEBUG, "HS485CommMessage::GetReceiverAddress()");
	uint32_t address;
	int index=MapIndex(0);
	if( (index>=0) && GetIntValue(index, 0, 4, 0, &address))return address;
	if(parent)return GetParent()->GetSenderAddress();
	return 0;
}

int HS485CommMessage::GetCtrl()
{
//	LOG(Logger::LOG_DEBUG, "HS485CommMessage::GetCtrl()");
	int index=MapIndex(4);
	if(index>=0)return GetByteData(index);
	return -1;
};


void HS485CommMessage::SetCtrl(int flags)
{
//	LOG(Logger::LOG_DEBUG, "HS485CommMessage::SetCtrl()");
	int index=MapIndex(4);
	if(index>=0)SetByteData(index, flags);
};

void HS485CommMessage::SetSenderAddress(uint32_t address)
{
//	LOG(Logger::LOG_DEBUG, "HS485CommMessage::SetSenderAddress()");
	int index=MapIndex(5);
    if(index<0)return;
	SetIntValue(index, 0, 4, 0, address);
};

void HS485CommMessage::SetReceiverAddress(uint32_t address)
{
//	LOG(Logger::LOG_DEBUG, "HS485CommMessage::SetReceiverAddress(0x%08lX)", address);
	int index=MapIndex(0);
    if(index<0)return;
	SetIntValue(index, 0, 4, 0, address);
};

/*
void HS485CommMessage::SetPayload(const std::string& msg)
{
//	LOG(Logger::LOG_DEBUG, "HS485CommMessage::SetPayload()");
	int index=MapIndex(9);
	data.resize(index);
	data.append(msg);
}

std::string HS485CommMessage::GetPayload()
{
//	LOG(Logger::LOG_DEBUG, "HS485CommMessage::GetPayload()");
	int index=MapIndex(9);
	if((int)data.size()<=index)return "";
	return data.substr(index);
}
*/

void HS485CommMessage::SetFrame(const StructuredFrame& frame)
{
    for(unsigned int i=0;i<frame.GetSize();i++)
    {
        int index=MapIndex(i);
        if(index>=0)SetByteData(index, frame.GetByteData(i));
    }
}

HS485Frame HS485CommMessage::ExtractFrame()
{
    HS485Frame frame;
    switch(GetCommand()){
        case CMD_RESPONSE:
        case CMD_EVENT:
        case CMD_SEND:
        case CMD_BOOT_SEND:
            for(int i=0;i<150;i++)
            {
                int index=MapIndex(i);
                if(index<0)continue;
                if(index>=(int)GetSize())break;
                frame.SetByteData(i, GetByteData(index));
            }
            break;
        default:
            break;
    }
    return frame;
}

void HS485CommMessage::SetTimeout(int to)
{
	if(GetCommand()==CMD_SEND || GetCommand()==CMD_BOOT_SEND)SetByteData(0, to);
}

bool HS485CommMessage::ProcessResponse(CommMessage *m, t_state* new_state)
{
	if(!CommMessage::ProcessResponse(m, new_state))return false;
	if(m->GetParent())return true;
	//the message is a response but was not collected into the responses vector.
	//If the message command is CMD_RESPONSE we will transform it into a CMD_EVENT-message
	if(m->GetCommand() != CMD_RESPONSE)return true;
	HS485CommMessage* msg=(HS485CommMessage*)m;
    HS485Frame frame=msg->ExtractFrame();
	msg->SetCommand(CMD_EVENT);
    msg->SetFrame(frame);
	msg->SetSenderAddress(GetReceiverAddress());
	msg->SetReceiverAddress(GetSenderAddress());
	return true;
}

bool HS485CommMessage::TransformToSimulationMessage()
{
	int sender_index=MapIndex(5);
	data+=data.substr(sender_index, 4);
    SetType(GetType() | 0x80);
	return true;
}

bool HS485CommMessage::isEEPRomUsage()
{
	return eepRomUsage;
}

void HS485CommMessage::setEEPRomUsage(const bool enabled)
{
	eepRomUsage = enabled;
}
