/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFConfigData.h"
#include "RFLogicalInstance.h"
#include "BidcosFrameParamReq.h"
#include "BidcosFrameDecoder.h"
#include "BidcosFrameDetermineValue.h"
#include <Logger.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

//const uint32_t RFConfigData::VALID_TIME=60000;
//const uint32_t RFConfigData::VALID_TIME=10000;


RFConfigData::RFConfigData(void)
{
	peer_address=peer_channel=0;
	invalid=false;
	initDefault=false;
}

RFConfigData::~RFConfigData(void)
{
}

bool RFConfigData::ReadFromDevice(RFLogicalInstance* inst, int list)
{
	if(invalid){
		LOG(Logger::LOG_ERROR, "RFConfigData::ReadFromDevice() invalid target");
		return false;
	}
	//LOG(Logger::LOG_DEBUG, "RFConfigData::ReadFromDevice()");
	int channel=inst->GetIndex();
	if(channel<0)channel=0;
	BidcosFrameParamReq frame;
	frame.SetType(BidcosFrame::FT_CONFIG_PARAM_REQ);
	frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, channel);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_A, peer_address);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH, peer_channel);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PARAM_LIST, list);
	if(!inst->SendFrame(&frame))return false;
	if(!frame.GetResponse())return false;
	if(frame.GetResponse()->IsNack()){
		if(frame.GetResponse()->MatchType(BidcosFrame::FT_NACK_TARGET_INVALID))invalid=true;
		return false;
	}
	int i=0;
	BidcosFrame* response=frame.GetResponse(i);
	while(response){
		int frame_size=response->GetSize();
		if(response->MatchType(BidcosFrame::FT_INFO_PARAM_RESPONSE_PAIRS)){
			int pos=10;
			while(pos+1<frame_size)
			{
				uint32_t index_value=0;
				if(!response->GetIntValue(pos, 0, 2, 0, &index_value))break;
				pos+=2;
				if(!IsDevDirty(list, (unsigned char)(index_value>>8)))SetByteData(list, (unsigned char)(index_value>>8), (unsigned char)(index_value&0xff), false, true);
			}
		}else if(response->MatchType(BidcosFrame::FT_INFO_PARAM_RESPONSE_SEQ)){
			uint32_t index=response->GetIntValue(BidcosFrame::FIELD_INFO_PARAM_OFFSET);
			if(!index)break;
			int pos=11;
			while(pos<frame_size)
			{
				uint32_t value;
				if(!response->GetIntValue(pos, 0, 1, 0, &value))
				{
					lists.erase(list);
					return false;
				}
				pos++;
				if(!IsDevDirty(list, (unsigned char)index))SetByteData(list, (unsigned char)index, (unsigned char)(value&0xff), false, true);
				index++;
			}
		}
		i++;
		response=frame.GetResponse(i);
	}
	lists_t::iterator it=lists.find(list);
	if(it!=lists.end()){
		chunks_t& chunks=it->second;
		chunks_t::iterator chunk_it;
		for(chunk_it=chunks.begin();chunk_it!=chunks.end();chunk_it++){
			Chunk& chunk=chunk_it->second;
			chunk.SetRead();
		}
	}
	return true;
}

bool RFConfigData::SendConfigData(RFLogicalInstance* inst, int list, const std::vector<unsigned char>& data)
{
	if(invalid){
		LOG(Logger::LOG_ERROR, "RFConfigData::SendConfigData() invalid target");
		return false;
	}
	if(data.empty())return true;
	int index=inst->GetIndex();
	if(index<0)index=0;
	BidcosFrame start_frame;
	start_frame.SetType(BidcosFrame::FT_CONFIG_START);
	start_frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	start_frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, index);
	start_frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_A, peer_address);
	start_frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH, peer_channel);
	start_frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PARAM_LIST, list);
	if(!inst->SendFrame(&start_frame))return false;
	if(!start_frame.GetResponse())return false;
	if(start_frame.GetResponse()->IsNack()){
		if(start_frame.GetResponse()->MatchType(BidcosFrame::FT_NACK_TARGET_INVALID))invalid=true;
		return false;
	}
	//LOG(Logger::LOG_DEBUG, "RFConfigData::SendConfigData() start response OK");

	int msg_index=11;
	BidcosFrame frame;
	frame.SetType(BidcosFrame::FT_CONFIG_WRITE_INDEX);
	frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, index);
	for(unsigned int i=0;i<data.size();i++){
		frame.SetIntValue(msg_index, 0, 1, 0, data[i]);
		msg_index++;
		if(msg_index>=25){
			if(!inst->SendFrame(&frame))return false;
			if((!frame.GetResponse()) || frame.GetResponse()->IsNack())return false;
			msg_index=11;
			frame.Resize(11);
		}
	}
	if(msg_index>11){
		if(!inst->SendFrame(&frame))return false;
		if((!frame.GetResponse()) || frame.GetResponse()->IsNack())return false;
	}

	BidcosFrame end_frame;
	end_frame.SetType(BidcosFrame::FT_CONFIG_END);
	end_frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	end_frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, index);
	if(!inst->SendFrame(&end_frame))return false;
	if((!end_frame.GetResponse()) || end_frame.GetResponse()->IsNack())return false;
	return true;
}

bool RFConfigData::CommitToDevice(RFLogicalInstance* inst)
{
//	LOG(Logger::LOG_DEBUG, "RFConfigData::CommitToDevice()");
	int channel=inst->GetIndex();
	if(channel<0)channel=0;

	lists_t::iterator lists_it;
	for(lists_it=lists.begin();lists_it!=lists.end();lists_it++){
		int list=lists_it->first;
		//LOG(Logger::LOG_DEBUG, "RFConfigData::CommitToDevice() list=%d", list);
		chunks_t& chunks=lists_it->second;
		std::vector<unsigned char> data;
		chunks_t::iterator it;
		for(it=chunks.begin();it!=chunks.end();it++){
			int chunk_base=(it->first)&~(Chunk::SIZE-1);
			Chunk& chunk=it->second;
//			LOG(Logger::LOG_DEBUG, "RFConfigData::CommitToDevice() list=%d", list);
			for(int i=0;i<Chunk::SIZE;i++){
//				LOG(Logger::LOG_DEBUG, "RFConfigData::CommitToDevice() list=%d", data_it->first);
				if(chunk.IsDevDirty(i)){
					data.push_back(chunk_base+i);
					data.push_back(chunk.GetByte(i));
				}
			}
		}
		if(!SendConfigData(inst, list, data))return false;
	}

	for(lists_it=lists.begin();lists_it!=lists.end();lists_it++){
		chunks_t& chunks=lists_it->second;
		chunks_t::iterator it;
		for(it=chunks.begin();it!=chunks.end();it++){
			it->second.SetDevClean();
		}
	}

	return true;
}

uint32_t RFConfigData::GetValue(int list, unsigned char by_pos, unsigned char bi_pos, unsigned char by_size, unsigned char bi_size, uint32_t mask /*=0xffffffff*/)
{
	if(!by_size){
		uint32_t v=GetByteData(list, by_pos);
		//take size into account for mask
		mask&=(0xff>>(8-bi_size));
		v >>= bi_pos;
		v&=mask;
		return v;
	}else{
		uint32_t val=0;
		for(int i=by_pos;i<by_pos+by_size;i++){
			unsigned int v=GetByteData(list, i);
			val<<=8;
			val|=v;
		}
		return val&mask;
	}
}

bool RFConfigData::DetermineValue(RFLogicalInstance* inst, int list, unsigned char by_pos)
{
	LOG(Logger::LOG_DEBUG, "RFConfigData::DetermineValue()");
	int channel=inst->GetIndex();
	if(channel<0)channel=0;
	BidcosFrameDetermineValue frame;
	frame.SetType(BidcosFrame::FT_CONFIG_DETERMINE);
	frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, channel);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_A, peer_address);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH, peer_channel);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PARAM_LIST, list);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PARAM_INDEX, by_pos);
//	frame.SetResponseTimeout(10000);
	if(!inst->SendFrame(&frame))return false;
	int i=0;
	BidcosFrame* response=frame.GetResponse(i);
	while(response){
//		LOG(Logger::LOG_DEBUG, "response: %s", RFCommMessageDecoder::ToString(response).c_str());
		if(	response->MatchType(BidcosFrame::FT_INFO_PARAM_ASYNC_PAIRS) ||
			response->MatchType(BidcosFrame::FT_INFO_PARAM_ASYNC_SEQ)){
			ProcessAsyncParamInfo(*response);
		}
		i++;
		response=frame.GetResponse(i);
	}
	return true;
}

void RFConfigData::PutValue(int list, unsigned char by_pos, unsigned char bi_pos, unsigned char by_size, unsigned char bi_size, uint32_t value, uint32_t mask /*=0xffffffff*/)
{
	//LOG(Logger::LOG_DEBUG, "PutValue(%d, %d, %d, %d, %d, 0x%08X, 0x%08X)", list, (int)by_pos, (int)bi_pos, (int)by_size, (int)bi_size, value, mask);
	if(!by_size){
		//take size into account for mask
		mask&=(0xff>>(8-bi_size))<<bi_pos;
		value <<= bi_pos;
		value &= mask;
		int v=GetByteData(list, by_pos);
		v&=~mask;
		v|=value;
		SetByteData(list, by_pos, v, true, true);
	}else{
		for(int i=by_pos+by_size-1;i>=by_pos;i--){
			uint32_t byte_mask=mask&0xff;
			uint32_t byte_value=(value&0xff)&byte_mask;
			if(byte_mask!=0xff){
				byte_value |= GetByteData(list, i) & ~byte_mask;
			}
			SetByteData(list, i, (unsigned char)byte_value, true, true);
			value>>=8;
			mask>>=8;
		}
	}
}

unsigned char RFConfigData::GetByteData(int list, unsigned char index)
{
	unsigned char value=0;

	lists_t::iterator lists_it=lists.find(list);
	if(lists_it!=lists.end()){
		chunks_t& chunks=lists_it->second;
		int chunk=index&~(Chunk::SIZE-1);
		chunks_t::iterator it=chunks.find(chunk);
		if(it!=chunks.end()){
			value=it->second.GetByte((list<<8)|index);
		}
	}
	//LOG(Logger::LOG_DEBUG, "GetByteData(%d, %d)=%02X", list, (int)index, (int)value);
	return value;
}

void RFConfigData::InitDefault(void)
{
	initDefault = true;
}
void RFConfigData::SetByteData(int list, unsigned char index, unsigned char value, bool dev_dirty, bool file_dirty)
{
//	LOG(Logger::LOG_DEBUG, "SetByteData(%d, %d, %02X, %d, %d)", list, (int)index, (int)value, (int)dev_dirty, (int)file_dirty);
	if(index)
	{
		int chunk=index&~(Chunk::SIZE-1);
		lists[list][chunk].SetByte((list<<8)|index, value, dev_dirty, file_dirty);
	}
}

bool RFConfigData::IsDevDirty(int list, unsigned char index)
{
	int chunk=index&~(Chunk::SIZE-1);
	return lists[list][chunk].IsDevDirty(index);
}

bool RFConfigData::IsDevDirty()
{
	lists_t::iterator lists_it;
	for(lists_it=lists.begin();lists_it!=lists.end();lists_it++){
		chunks_t& chunks=lists_it->second;
		chunks_t::iterator it;
		for(it=chunks.begin();it!=chunks.end();it++){
			if(it->second.IsDevDirty())return true;
		}
	}
	return false;
}

void RFConfigData::SetDevDirty()
{
	lists_t::iterator lists_it;
	for(lists_it=lists.begin();lists_it!=lists.end();lists_it++){
		chunks_t& chunks=lists_it->second;
		chunks_t::iterator it;
		for(it=chunks.begin();it!=chunks.end();it++){
			it->second.SetDevDirty();
		}
	}
}

bool RFConfigData::SaveToXml(XMLNode* node)
{
	char buffer[16];

	lists_t::iterator lists_it;
	for(lists_it=lists.begin();lists_it!=lists.end();lists_it++){
		XMLNode list_node=node->addChildConst("list");
		snprintf(buffer, sizeof(buffer), "%d", lists_it->first);
		list_node.addAttributeConst("index", buffer);
		chunks_t& chunks=lists_it->second;
		chunks_t::iterator it;
		for(it=chunks.begin();it!=chunks.end();it++){
			XMLNode chunk_node=list_node.addChildConst("chunk");
			snprintf(buffer, sizeof(buffer), "0x%02X", it->first);
			chunk_node.addAttributeConst("address", buffer);
			if(!it->second.SaveToXml(&chunk_node))return false;
		}
	}
	return true;
}

bool RFConfigData::LoadFromXml(XMLNode& node)
{
	lists.clear();

	int i=0;
	XMLNode list_node=node.getChildNode("list", &i);
	while(!list_node.isEmpty()){
		const char* temp=list_node.getAttribute("index");
		if(!temp)return false;
		int list_index=strtol(temp, NULL, 0);

		int j=0;
		XMLNode chunk_node=list_node.getChildNode("chunk", &j);
		while(!chunk_node.isEmpty()){
			const char* temp=chunk_node.getAttribute("address");
			if(!temp)return false;
			int chunk_address=strtol(temp, NULL, 0);
			Chunk& chunk=lists[list_index][chunk_address];
			if(!chunk.LoadFromXml(chunk_node)){
				LOG(Logger::LOG_ERROR, "Error loading list %d chunk 0x%02X", list_index, chunk_address);
				return false;
//			}else{
//				LOG(Logger::LOG_DEBUG, "Chunk loaded: list %d chunk 0x%02X", list_index, chunk_address);
			}
			chunk_node=list_node.getChildNode("chunk", &j);
		}
		list_node=node.getChildNode("list", &i);
	}
	return true;
}

RFConfigData::Chunk::Chunk(void)
{
    dev_dirty_bits=0;
    file_dirty_bits=0;
    used_bits=0;
    memset(data, 0, SIZE);
    must_be_read=true;
}

void RFConfigData::Chunk::SetByte(unsigned int address, unsigned char b, bool dev_dirty, bool file_dirty)
{
    address&=SIZE-1;
    bool changed=(data[address]!=b) || !(used_bits&(1<<address));
    data[address]=b;
    used_bits|=(1<<address);
    if(dev_dirty){
        if(changed)dev_dirty_bits|=(1<<address);
    }else{
        dev_dirty_bits&=~(1<<address);
    }
    if(file_dirty){
        if(changed)file_dirty_bits|=(1<<address);
    }else{
        file_dirty_bits&=~(1<<address);
    }
}

bool RFConfigData::Chunk::SaveToXml(XMLNode* node)
{
	char buffer[SIZE*2+1];

	snprintf(buffer, sizeof(buffer), "%" PRIX32, dev_dirty_bits);
	node->addAttributeConst("dev_dirty", buffer);
	snprintf(buffer, sizeof(buffer), "%" PRIX32, file_dirty_bits);
	node->addAttributeConst("file_dirty", buffer);
	snprintf(buffer, sizeof(buffer), "%" PRIX32, used_bits);
	node->addAttributeConst("used", buffer);

	node->addAttributeConst("must_be_read", must_be_read?"true":"false");

	uint32_t used=used_bits;
	int i=0;
	buffer[0]=0;
	while(used){
		if(used&0x01){
			sprintf(buffer+2*i, "%02X", ((int)data[i])&0xff);
		}else{
			buffer[2*i]=buffer[2*i+1]='0';
			buffer[2*i+2]=0;
		}
		i++;
		used>>=1;
	}
	node->addAttributeConst("data", buffer);
	file_dirty_bits=0;
	return true;
}

bool RFConfigData::Chunk::LoadFromXml(XMLNode& node)
{
	const char* temp=node.getAttribute("data");
	if(!temp)return false;
	int count=strlen(temp)/2;
	for(int i=0;i<count;i++){
		char buffer[4];
		buffer[2]=0;
		strncpy(buffer, temp+2*i, 2);
		data[i]=(unsigned char)strtol(buffer, NULL, 16);
	}

	temp=node.getAttribute("used");
	if(!temp)return false;
	used_bits=strtoul(temp, NULL, 16);

	temp=node.getAttribute("dev_dirty");
	if(!temp)return false;
	dev_dirty_bits=strtoul(temp, NULL, 16);

	temp=node.getAttribute("must_be_read");
	must_be_read=(temp && temp[0]=='t');

	file_dirty_bits=0;
	return true;
}
void RFConfigData::PushData(int list, unsigned char by_pos, unsigned char bi_pos, unsigned char by_size, unsigned char bi_size, uint32_t value, uint32_t mask /*=0xffffffff*/)
{
	chunks_t::iterator chunkIT;
	PutValue(list,by_pos,bi_pos,by_size,bi_size,value,mask);// setze die Werte 
	lists_t::iterator listIt = lists.find(list);
	if(listIt != lists.end())
	{
		for(chunkIT = listIt->second.begin();chunkIT != listIt->second.end();chunkIT++)
		{
			chunkIT->second.SetRead(); // unterbindet das lesen vom Ger�t 
		}
	}
}
bool RFConfigData::ProcessAsyncParamInfo(BidcosFrame& frame)
{
	LOG(Logger::LOG_DEBUG, "RFConfigData::ProcessAsyncParamInfo()");

	int list=frame.GetIntValue(BidcosFrame::FIELD_INFO_PARAM_LIST);
	int frame_size=frame.GetSize();
	if(frame.MatchType(BidcosFrame::FT_INFO_PARAM_ASYNC_PAIRS)){
		int pos=16;
		while(pos+1<frame_size)
		{
			uint32_t index_value=0;
			if(!frame.GetIntValue(pos, 0, 2, 0, &index_value))break;
			pos+=2;
			SetByteData(list, (unsigned char)(index_value>>8), (unsigned char)(index_value&0xff), false, true);
		}
	}else if(frame.MatchType(BidcosFrame::FT_INFO_PARAM_ASYNC_SEQ)){
		uint32_t index=frame.GetIntValue(BidcosFrame::FIELD_INFO_PARAM_OFFSET2);
		if(index){
			int pos=17;
			while(pos<frame_size)
			{
				uint32_t value;
				if(!frame.GetIntValue(pos, 0, 1, 0, &value))
				{
					break;
				}
				pos++;
				SetByteData(list, (unsigned char)index, (unsigned char)(value&0xff), false, true);
				index++;
			}
		}
	}
	return true;
}

bool RFConfigData::MustBeRead(int list)
{
	lists_t::iterator it=lists.find(list);
	if(it==lists.end()) {
		if(initDefault)
		{
			return false;
		}
		else
		{
			return true;
		}
  }
	chunks_t& chunks=it->second;
	chunks_t::iterator chunk_it;
	for(chunk_it=chunks.begin();chunk_it!=chunks.end();chunk_it++){
		Chunk& chunk=chunk_it->second;
		if(chunk.MustBeRead())
			return true;
	}
	return false;
}
