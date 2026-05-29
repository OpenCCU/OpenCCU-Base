/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosFrameDecoder.h"
#include "BidcosFrame.h"
#include <string.h>
#include <stdio.h>
#include <cinttypes>

BidcosFrameDecoder::FrameDescription BidcosFrameDecoder::FrameDescriptions[]=
{
		{
			-1, "Generic",{
				{BidcosFrame::FIELD_COUNTER, "CNT"},
				{BidcosFrame::FIELD_RPT_ENABLE, "RPTEN"},
				{BidcosFrame::FIELD_RPTED, "RPTED"},
				{BidcosFrame::FIELD_BIDI, "BIDI"},
				{BidcosFrame::FIELD_BURST, "BURST"},
				{BidcosFrame::FIELD_WAKEUP, "WAKEUP"},
				{BidcosFrame::FIELD_WAKEMEUP, "WAKEMEUP"},
				{BidcosFrame::FIELD_BCAST, "BCAST"},
				{BidcosFrame::FIELD_TYPE, "TYPE", "0x%02X"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_SYSINFO, "SYSINFO",{
				{BidcosFrame::FIELD_SYSINFO_SWVER, "SYSINFO_SWVER", "0x%02X"},
				{BidcosFrame::FIELD_SYSINFO_TYPE, "SYSINFO_TYPE", "0x%04X"},
				{BidcosFrame::FIELD_SYSINFO_SERIAL, "SYSINFO_SERIAL", "%s"},
				{BidcosFrame::FIELD_SYSINFO_CODE, "SYSINFO_CODE", "0x%04X"},
				{BidcosFrame::FIELD_SYSINFO_CH_A, "SYSINFO_CH_A"},
				{BidcosFrame::FIELD_SYSINFO_CH_B, "SYSINFO_CH_B"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG, "CONFIG",{
				{BidcosFrame::FIELD_CONFIG_CHANNEL, "CONFIG_CHANNEL"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_CLEAR, "CONFIG_CLEAR",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_PEER_ADD, "CONFIG_PEER_ADD",{
				{BidcosFrame::FIELD_CONFIG_PEER_A, "CONFIG_PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_CONFIG_PEER_CH_A, "CONFIG_PEER_CHANNEL_A"},
				{BidcosFrame::FIELD_CONFIG_PEER_CH_B, "CONFIG_PEER_CHANNEL_B"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_PEER_REMOVE, "CONFIG_PEER_REMOVE",{
				{BidcosFrame::FIELD_CONFIG_PEER_A, "CONFIG_PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_CONFIG_PEER_CH, "CONFIG_PEER_CHANNEL"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_PEER_LIST_REQ, "CONFIG_PEER_LIST_REQ",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_PARAM_REQ, "CONFIG_PARAM_REQ",{
				{BidcosFrame::FIELD_CONFIG_PEER_A, "CONFIG_PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_CONFIG_PEER_CH, "CONFIG_PEER_CHANNEL"},
				{BidcosFrame::FIELD_CONFIG_PARAM_LIST, "CONFIG_PARAM_LIST"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_START, "CONFIG_START",{
				{BidcosFrame::FIELD_CONFIG_PEER_A, "CONFIG_PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_CONFIG_PEER_CH, "CONFIG_PEER_CHANNEL"},
				{BidcosFrame::FIELD_CONFIG_PARAM_LIST, "CONFIG_PARAM_LIST"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_END, "CONFIG_END",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_WRITE_INDEX, "CONFIG_WRITE_INDEX",{
				{11, "DATA", "data"}, 
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_WRITE_OFFSET, "CONFIG_WRITE_OFFSET",{
				{11, "DATA", "data"}, 
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_SERIAL_REQ, "CONFIG_SERIAL_REQ",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_ENTER, "CONFIG_ENTER",{
				{BidcosFrame::FIELD_CONFIG_SERIAL, "CONFIG_SERIAL", "%s"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_RPTR_ADD_LINK, "CONFIG_RPTR_ADD_LINK",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_RPTR_DEL_LINK, "CONFIG_RPTR_DEL_LINK",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_RPTR_LINK_LIST_REQ, "CONFIG_RPTR_LINK_LIST_REQ",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_STATUS_REQ, "CONFIG_STATUS_REQ",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONFIG_DETERMINE, "CONFIG_STATUS_REQ",{
				{BidcosFrame::FIELD_CONFIG_CHANNEL, "CHANNEL"},
				{BidcosFrame::FIELD_CONFIG_PEER_A, "PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_CONFIG_PEER_CH, "PEER_CHANNEL"},
				{BidcosFrame::FIELD_CONFIG_PARAM_LIST, "LIST"},
				{BidcosFrame::FIELD_CONFIG_PARAM_INDEX, "INDEX"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_ACK, "ACK",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_ACK_STATUS, "ACK_STATUS",{
				{BidcosFrame::FIELD_ACK_CHANNEL, "CHANNEL"},
				{BidcosFrame::FIELD_ACK_STATUS, "STATUS"},
				{BidcosFrame::FIELD_ACK_STATE, "STATE"},
				{BidcosFrame::FIELD_ACK_CLOCK, "CLOCK"},
				{BidcosFrame::FIELD_ACK_LOWBAT, "LOWBAT"},
				{BidcosFrame::FIELD_ACK_DC90, "DUTY_CYCLE"},
				{BidcosFrame::FIELD_ACK_RSSI, "RSSI"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_ACK_PARAM_IGNORED, "ACK_PARAM_IGNORED",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_ACK_PARAM_CORRECTED, "ACK_PARAM_CORRECTED",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_ACK_W_AES_CHALLENGE, "ACK_W_AES_CHALLENGE",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_NACK, "NACK",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_NACK_BUSY, "NACK_BUSY",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_NACK_MEMFULL, "NACK_MEMFULL",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_NACK_MEMFULL_PART, "NACK_MEMFULL_PART",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_NACK_TARGET_INVALID, "NACK_TARGET_INVALID",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_NACK_CHANNEL_INVALID, "FT_NACK_CHANNEL_INVALID",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_AES_SOLUTION, "AES_SOLUTION",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_AES_CONTAINER, "AES_CONTAINER",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_AES_CONTAINER_KEY, "AES_CONTAINER_KEY",{
				{BidcosFrame::FIELD_CONTAINER_KEY_PART, "PART"},
				{BidcosFrame::FIELD_CONTAINER_KEY_IDX, "INDEX"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_TESTMODE, "TESTMODE",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_INFO_STATUS , "INFO_ACTUATOR_STATUS",{
				{BidcosFrame::FIELD_INFO_STATUS_CH, "CHANNEL"},
				{BidcosFrame::FIELD_INFO_STATUS_ST, "STATUS"},
				{BidcosFrame::FIELD_INFO_STATUS_STATE, "STATE"},
				{BidcosFrame::FIELD_INFO_STATUS_CLOCK, "CLOCK"},
				{BidcosFrame::FIELD_INFO_STATUS_LOWBAT, "LOWBAT"},
				{BidcosFrame::FIELD_INFO_STATUS_DC90, "DUTY_CYCLE"},
				{BidcosFrame::FIELD_INFO_STATUS_RSSI, "RSSI"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_INFO_PEER_LIST , "INFO_PEER_LIST",{
				{BidcosFrame::FIELD_INFO_LINK_PEER_0A, "LINK_PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_INFO_LINK_PEER_0CH, "LINK_PEER_CHANNEL"},
				{BidcosFrame::FIELD_INFO_LINK_PEER_1A, "LINK_PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_INFO_LINK_PEER_1CH, "LINK_PEER_CHANNEL"},
				{BidcosFrame::FIELD_INFO_LINK_PEER_2A, "LINK_PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_INFO_LINK_PEER_2CH, "LINK_PEER_CHANNEL"},
				{BidcosFrame::FIELD_INFO_LINK_PEER_3A, "LINK_PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_INFO_LINK_PEER_3CH, "LINK_PEER_CHANNEL"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_INFO_PARAM_RESPONSE_PAIRS , "INFO_PARAM_RESPONSE_PAIRS",{
				{10, "DATA", "data"}, 
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_INFO_PARAM_RESPONSE_SEQ , "INFO_PARAM_RESPONSE_SEQ",{
				{BidcosFrame::FIELD_INFO_PARAM_OFFSET, "INFO_PARAM_OFFSET"},
				{11, "DATA", "data"}, 
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_INFO_PARAM_ASYNC_PAIRS , "INFO_PARAM_ASYNC_PAIRS",{
				{BidcosFrame::FIELD_INFO_PARAM_CH, "CHANNEL"},
				{BidcosFrame::FIELD_INFO_PARAM_PEER_A, "PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_INFO_PARAM_PEER_CH, "PEER_CHANNEL"},
				{BidcosFrame::FIELD_INFO_PARAM_LIST, "LIST"},
				{16, "DATA", "data"}, 
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_INFO_PARAM_ASYNC_SEQ , "FT_INFO_PARAM_ASYNC_SEQ",{
				{BidcosFrame::FIELD_INFO_PARAM_CH, "CHANNEL"},
				{BidcosFrame::FIELD_INFO_PARAM_PEER_A, "PEER_ADDRESS", "0x%06X"},
				{BidcosFrame::FIELD_INFO_PARAM_PEER_CH, "PEER_CHANNEL"},
				{BidcosFrame::FIELD_INFO_PARAM_LIST, "LIST"},
				{BidcosFrame::FIELD_INFO_PARAM_OFFSET2, "OFFSET"},
				{17, "DATA", "data"}, 
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CENTRAL_LOCK, "CENTRAL_LOCK",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CENTRAL_UNLOCK, "CENTRAL_UNLOCK",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CENTRAL_RAMP_START, "CENTRAL_RAMP_START",{
				{BidcosFrame::FIELD_CENTRAL_CHANNEL, "CHANNEL"},
				{BidcosFrame::FIELD_CENTRAL_LEVEL, "LEVEL"},
				{BidcosFrame::FIELD_CENTRAL_RAMPTIME, "RAMPTIME"},
				{BidcosFrame::FIELD_CENTRAL_ONTIME, "ONTIME"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CENTRAL_RAMP_STOP, "CENTRAL_RAMP_STOP",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CENTRAL_LOCK, "CENTRAL_RESET",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_WAKEUP, "WAKEUP",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_SIMULATION, "SIMULATION",{
				{BidcosFrame::FIELD_SIM_TYPE, "SIM_TYPE", "0x%02X"},
				{BidcosFrame::FIELD_SIM_SENDER, "SIM_SENDER", "0x%06X"},
				{13, "DATA", "data"}, 
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_TIME, "TIME",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_SWITCH, "SWITCH",{
				{BidcosFrame::FIELD_SWITCH_COUNTER, "COUNTER"},
				{BidcosFrame::FIELD_SWITCH_CHANNEL, "CHANNEL"},
				{BidcosFrame::FIELD_SWITCH_LOWBAT, "LOWBAT"},
				{BidcosFrame::FIELD_SWITCH_DURATION, "DURATION"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_CONDITIONAL_SWITCH, "CONDITIONAL_SWITCH",{
				{BidcosFrame::FIELD_SWITCH_COUNTER, "COUNTER"},
				{BidcosFrame::FIELD_SWITCH_CHANNEL, "CHANNEL"},
				{BidcosFrame::FIELD_SWITCH_LOWBAT, "LOWBAT"},
				{BidcosFrame::FIELD_SWITCH_DURATION, "DURATION"},
				{BidcosFrame::FIELD_SWITCH_LEVEL, "CONDITION"},
				{BidcosFrame::FIELD_SWITCH_COND_TIMES, "TIMES"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_LEVEL, "LEVEL",{
				{BidcosFrame::FIELD_SWITCH_COUNTER, "COUNTER"},
				{BidcosFrame::FIELD_SWITCH_CHANNEL, "CHANNEL"},
				{BidcosFrame::FIELD_SWITCH_LOWBAT, "LOWBAT"},
				{BidcosFrame::FIELD_SWITCH_DURATION, "DURATION"},
				{BidcosFrame::FIELD_SWITCH_LEVEL, "LEVEL"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_HEATING, "HEATING",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_HAZARD_SENSOR, "HAZARD_SENSOR",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_HAZARD_SENSOR_COND, "HAZARD_SENSOR_COND",{
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_WEATHER, "WEATHER",{
				{BidcosFrame::FIELD_WEATHER_LOWBAT, "LOWBAT"},
				{BidcosFrame::FIELD_WEATHER_TEMP, "TEMP"},
				{BidcosFrame::FIELD_WEATHER_HUMIDITY, "HUMIDITY"},
				{BidcosFrame::FIELD_WEATHER_RAINING, "RAINING"},
				{BidcosFrame::FIELD_WEATHER_RAINCNT, "RAINCNT"},
				{BidcosFrame::FIELD_WEATHER_WINDSPEED, "WINDSPEED"},
				{BidcosFrame::FIELD_WEATHER_WINDDIR, "WINDDIR"},
				{BidcosFrame::FIELD_WEATHER_WDIR_RANGE, "WINDDIR_RANGE"},
				{BidcosFrame::FIELD_WEATHER_BRIGHTNESS, "BRIGHTNESS"},
				{0, NULL}
			}
		},
		{
			BidcosFrame::FT_WEATHER_REQUEST, "WEATHER_REQUEST",{
				{0, NULL}
			}
		},
		{
			-2, "Generic",{
				{9, "DATA", "data"}, 
				{0, NULL}
			}
		}
};

BidcosFrameDecoder::BidcosFrameDecoder(void)
{
}

BidcosFrameDecoder::~BidcosFrameDecoder(void)
{
}

std::string BidcosFrameDecoder::ToString(const BidcosFrame* frame)
{
	char buffer[256];
	std::string result;

	bool del_frame=false;
	if(frame->MatchType(BidcosFrame::FT_SIMULATION)){
		snprintf(buffer, sizeof(buffer), "Simulated from 0x%06X\n", frame->GetSenderAddress());
		result=buffer;
		BidcosFrame* sim_frame=new BidcosFrame();
		*sim_frame=*frame;
		sim_frame->TransformFromSimulationMessage();
		frame=sim_frame;
		del_frame=true;
	}
	const char* type="";
	bool is_generic=true;
	for(unsigned int i=0;i<sizeof(FrameDescriptions)/sizeof(FrameDescription);i++){
		FrameDescription& fd=FrameDescriptions[i];
		bool type_matched=(fd.type>=0) && frame->MatchType(fd.type);
		if(fd.type<0 || type_matched){
			is_generic &= !type_matched;
			if(fd.type!=-2)type=fd.name;
			else if(!is_generic)continue;

			FrameField* p_field=fd.fields;
			while(p_field->name){
				const char*format="%d";
				if(p_field->format)format=p_field->format;
				if(strcmp(format, "%s")==0){
					snprintf(buffer, sizeof(buffer), format, frame->GetStringValue(p_field->id).c_str());
				}else if(strcmp(format, "data")==0){
					int index=p_field->id;
					int size=frame->GetSize();
					char* p=buffer;
					while(index<size){
						sprintf(p, "%02X ", ((int)frame->GetByteData(index))&0xff);
						p+=3;
						index++;
					}
				}else{
					snprintf(buffer, sizeof(buffer), format, (int)frame->GetIntValue(p_field->id));
				}
				if(!i && p_field!=fd.fields)result+=",";
				else result+="  ";
				result+=p_field->name;
				if(i)result+=" = ";
				else result+="=";
				result+=buffer;
				if(i)result+="\n";
				p_field++;
			}
			if(!i)result+="\n";
		}
	}
	std::string timestamp;
	if(frame->HasTimestamp()){
		snprintf(buffer, sizeof(buffer), " @%" PRIu64, frame->GetTimestamp());
		timestamp=buffer;
	}

	std::string extra_info;
	if(frame->WasAuthenticated()){
		snprintf(buffer, sizeof(buffer), " AES(%d)", frame->GetAuthKey());
		extra_info+=buffer;
	}
    if(frame->IsPreliminary()){
        extra_info+=" PRELIM";
    }
    int rssi=frame->GetRSSI();
    if(rssi!=BidcosFrame::INVALID_RSSI_VALUE){
    snprintf(buffer, sizeof(buffer), " RSSI=%ddB", rssi);
		extra_info+=buffer;
    }

	snprintf(buffer, sizeof(buffer), "%s%s 0x%06X -> 0x%06X %s [%s]:\n", timestamp.c_str(), extra_info.c_str(), (int)frame->GetSenderAddress(), (int)frame->GetReceiverAddress(), type, frame->GetInterfaceId().c_str());

	if(del_frame)delete frame;
	result=std::string(buffer)+result;
	return result;
}
