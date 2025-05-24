#include <HS485CommMessageLGW.h>
#include <Logger.h>
#include <HMWLGWUtils.h>

HS485CommMessageLGW::HS485CommMessageLGW()
: HS485CommMessage()
{

}

HS485CommMessageLGW::HS485CommMessageLGW(HMWLGWTransportFrame& transportFrame)
: HS485CommMessage()
{
	hmwlgwCommand.parseFromRawData( transportFrame.getPayload() );
}


HS485CommMessageLGW::~HS485CommMessageLGW()
{
}


void HS485CommMessageLGW::SetHMWLGWCommand(const HMWLGWCommandType cmdType)
{
	hmwlgwCommand.setCommandType(cmdType);
}

void HS485CommMessageLGW::SetHMWLGWCommandData(const std::string& commandData)
{
	hmwlgwCommand.setCommandData(commandData);
}


int HS485CommMessageLGW::MapIndex(int index)//protected - maybe not needed here
{
	LOG(Logger::LOG_ALL, "HS485CommMessageLGW::MapIndex(): Not implemeted yet.");
	return 0;
}

bool HS485CommMessageLGW::MatchType(unsigned long type)
{
	LOG(Logger::LOG_ALL, "HS485CommMessageLGW::MatchType(): Not implemeted yet.");
	return false;
}

HMWLGWCommandType HS485CommMessageLGW::GetHMWLGWCommand() {
	return hmwlgwCommand.getCommandType();
}

void HS485CommMessageLGW::SetType(unsigned long type)//protected - maybe not needed here
{
	LOG(Logger::LOG_ALL, "HS485CommMessageLGW::SetType(): Not implemeted yet.");
}

unsigned long HS485CommMessageLGW::GetType()//protected - maybe not needed here
{
	LOG(Logger::LOG_ALL, "HS485CommMessageLGW::GetType(): Not implemeted yet.");
	return 0;
}

uint32_t HS485CommMessageLGW::GetSenderAddress()
{
	return (uint32_t)hmwlgwCommand.getSenderAddress();
}

uint32_t HS485CommMessageLGW::GetReceiverAddress()
{
	return (uint32_t)hmwlgwCommand.getReceiverAddress();
}

int HS485CommMessageLGW::GetCtrl()
{
	return (int)hmwlgwCommand.getCtrlByte();
};


void HS485CommMessageLGW::SetCtrl(int flags)
{
	hmwlgwCommand.setCtrlByte((unsigned char)flags);
}

void HS485CommMessageLGW::SetSenderAddress(uint32_t address)
{
	//LOG(Logger::LOG_ALL, "HS485CommMessageLGW::SetSenderAddress()");
	hmwlgwCommand.setSenderAddress(address);
}

void HS485CommMessageLGW::SetReceiverAddress(uint32_t address)
{
	//LOG(Logger::LOG_ALL, "HS485CommMessageLGW::SetReceiverAddress(): Not implemeted yet.");
	hmwlgwCommand.setReceiverAddress(address);
};

void HS485CommMessageLGW::SetFrame(const StructuredFrame& frame)
{
//	LOG(Logger::LOG_ALL, "HS485CommMessageLGW::SetFrame()");
	HS485Frame hFrame = (HS485Frame)(HS485Frame&)frame;
	if(frame.GetSize() >= 0) {//FIXME insert correct size
		hmwlgwCommand.setReceiverAddress(hFrame.GetReceiverAddress());
		hmwlgwCommand.setCtrlByte(hFrame.GetCtrl());
		hmwlgwCommand.setSenderAddress(hFrame.GetSenderAddress());
		hmwlgwCommand.setCommandData(hFrame.GetPayload());
	}
}

HS485Frame HS485CommMessageLGW::ExtractFrame()
{
//	LOG(Logger::LOG_ALL, "HS485CommMessageLGW::ExtractFrame().");
    HS485Frame frame;
    frame.SetCtrl(hmwlgwCommand.getCtrlByte());
    frame.SetReceiverAddress(hmwlgwCommand.getReceiverAddress());
    frame.SetSenderAddress(hmwlgwCommand.getSenderAddress());
    frame.SetPayload(hmwlgwCommand.getCommandData());
    //LOG(Logger::LOG_ALL, "Frame payload: %s", toDebugHexStr(hmwlgwCommand.getCommandData()).c_str());

    return frame;
}

void HS485CommMessageLGW::SetTimeout(int to)
{
	//LOG(Logger::LOG_ALL, "HS485CommMessageLGW::SetTimeout(): Not implemeted yet.");
	hmwlgwCommand.setTimeout(to);
	//if(GetCommand()==CMD_SEND || GetCommand()==CMD_BOOT_SEND)SetByteData(0, to);
}

bool HS485CommMessageLGW::ProcessResponse(CommMessage *m, t_state* new_state)//protected - maybe not needed here
{
	if(eepRomUsage) {
		//LOG(Logger::LOG_ALL, "HS485CommMessageLGW::ProcessResponse(): -> Forwarding to ProcessEEPRomResponse()");
		return ProcessEEPRomResponse(m, new_state);
	}
	else {
//		LOG(Logger::LOG_ALL, "HS485CommMessageLGW::ProcessResponse()");
		HS485CommMessageLGW* pResponseMsg = (HS485CommMessageLGW*)m;
		bool retVal = true;
		switch(pResponseMsg->hmwlgwCommand.getCommandType())//... ????
		{
			case LGW_RPL_RESPONSE:
				if(pResponseMsg->hmwlgwCommand.isResponseOf(hmwlgwCommand))	{
					LOG(Logger::LOG_DEBUG, "LGW_RPL_RESPONSE");
					if(collect_responses) {
						m->SetParent(this);
						vect_responses.push_back(m);
						*new_state = ACKED;
					}
					else {
//						LOG(Logger::LOG_DEBUG, "--- Converting response to event ---");
						*new_state = ACKED;//Responses, which, explicit, no one wants to handle, will be handled as events.
						pResponseMsg->hmwlgwCommand.transformResponseToEvent(this->hmwlgwCommand);
						goto HANDLE_EVENT;/*This implementations follows CommController implementation and is important because some devices
						 	 	 	 	   * do not send events, but a reply, after changing their state.*/
					}

				}
				else {//TODO pass through as event
					LOG(Logger::LOG_DEBUG, "Not a valid response.");
				}

				break;
			case LGW_RPL_ANSWER:
				if(pResponseMsg->getCommandData().size() == 1) {
					switch(pResponseMsg->getCommandData().at(0))
					{
						case 0:
							//LOG(Logger::LOG_ALL, "LGW_RPL_ANSWER OK");
							*new_state = ACKED;//RECEIVED;//should be RECEIVED, could be ACKED :-)
							break;
						default:
							LOG(Logger::LOG_ERROR, "LGW_RPL_ANSWER ERROR - request was %X", (unsigned int)hmwlgwCommand.getCommandType());
							*new_state = DROPPED;
							break;
					}
				}
				else {
					LOG(Logger::LOG_ERROR, "LGW_RPL_ANSWER Wrong size for an answer.");
					//TODO Error handling
				}
				break;
			case LGW_RPL_DISCOVERY_RESULT: //does that work ? aybe other different counter for nore than one resut
					m->SetParent(this);
					vect_responses.push_back(m);
				break;
			case LGW_EVT_EVENT:
				//LOG(Logger::LOG_ALL, "HS485CommMessageLGW::ProcessResponse(): Event");
HANDLE_EVENT:
				m->SetCommand(CMD_EVENT);
				retVal = false;
				break;
			case LGW_EVT_DISCOVERY_END:
				//LOG(Logger::LOG_ALL, "HS485CommMessageLGW::ProcessResponse(): Discovery End");
				m->SetParent(this);
				vect_responses.push_back(m);
				*new_state = ACKED;
				break;
			default:
				LOG(Logger::LOG_ERROR, "HS485CommMessageLGW::ProcessResponse(): Not a response or event");
				retVal = false;
				break;
		}
		return retVal;
	}
}

bool HS485CommMessageLGW::ProcessEEPRomResponse(CommMessage* m, t_state* new_state)
{
//	LOG(Logger::LOG_ALL, "HS485CommMessageLGW::ProcessEEPRomResponse()");
	HS485CommMessageLGW* hs485_msg=(HS485CommMessageLGW*)m;
	//LOG(Logger::LOG_DEBUG, "HS485CommMessageEEPromUsage::ProcessResponse():\n%s\n%s", HS485CommMessageDecoder::ToString(hs485_msg).c_str(), HS485CommMessageDecoder::ToString(this).c_str());

	pthread_mutex_lock(&mutex);
    if(state==WAIT_ACK){
    	const HMWLGWCommandType commandType = hs485_msg->hmwlgwCommand.getCommandType();
	   	if( ((commandType == LGW_RPL_RESPONSE) && hs485_msg->hmwlgwCommand.isResponseOf(hmwlgwCommand)) ||
    		((commandType == LGW_EVT_EVENT) && (hs485_msg->GetSenderAddress()==GetReceiverAddress()) ))
		{
			std::string my_payload=ExtractFrame().GetPayload();
			std::string response_payload=hs485_msg->ExtractFrame().GetPayload();
			if(response_payload.size()<(unsigned int)((my_payload[IDX_COUNT]+7)/8)+4){
				LOG(Logger::LOG_ERROR, "HS485CommMessageLGW::ProcessEEPRomResponse(): response size mismatch");
			    pthread_mutex_unlock(&mutex);
			    return false;
			}
			if(response_payload[IDX_COMMAND]!='e'){
				LOG(Logger::LOG_ERROR, "HS485CommMessageLGW::ProcessEEPRomResponse(): response command mismatch");
			    pthread_mutex_unlock(&mutex);
			    return false;
			}
			if(response_payload.substr(IDX_ADDRESS, 3) != my_payload.substr(IDX_ADDRESS, 3)){
				LOG(Logger::LOG_ERROR, "HS485CommMessageLGW::ProcessEEPRomResponse(): response range mismatch");
			    pthread_mutex_unlock(&mutex);
			    return false;
			}
			m->SetParent(this);
			vect_responses.push_back(m);
		    *new_state=ACKED;
			pthread_mutex_unlock(&mutex);
			return true;
		}
    }
    else {
    	LOG(Logger::LOG_DEBUG, "HS485CommMessageLGW::ProcessEEPRomResponse(): Request not in state WAIT_ACK");
    }
	pthread_mutex_unlock(&mutex);
    return false;
}

bool HS485CommMessageLGW::TransformToSimulationMessage()
{
//	LOG(Logger::LOG_ALL, "HS485CommMessageLGW::TransformToSimulationMessage()");
/*	int sender_index=MapIndex(5);
	data+=data.substr(sender_index, 4);
    SetType(GetType() | 0x80);
	return true;
	*/
	std::string s = hmwlgwCommand.getCommandData();
	unsigned char cmd = s.at(0);
	cmd = cmd | (unsigned char)0x80;
	s.replace(0, 1, std::string(1,cmd));
	s.append(convertUnsignedIntToBigEndianString(hmwlgwCommand.getSenderAddress()));
	hmwlgwCommand.setCommandData(s);
	return true;
}

std::string HS485CommMessageLGW::PrepareRawData()
{
	//LOG(Logger::LOG_ALL, "HS485CommMessageLGW::PrepareRawData()");
	std::string foo = hmwlgwCommand.assembleCmdRawData();
	//LOG(Logger::LOG_ALL, "HS485CommMessageLGW::PrepareRawData() HMWLGWCommand assembled: %s", toDebugHexStr(foo).c_str());
	HMWLGWTransportFrame transportFrame;
	transportFrame.setPayload(foo);
	std::string tframeData = transportFrame.getFrameData();
	//LOG(Logger::LOG_ALL, "HS485CommMessageLGW::PrepareRawData() HMWLGWTransportFrame assembled: %s", toDebugHexStr(tframeData).c_str());
	return tframeData;

}

std::string HS485CommMessageLGW::getCommandData()
{
	return hmwlgwCommand.getCommandData();
}

