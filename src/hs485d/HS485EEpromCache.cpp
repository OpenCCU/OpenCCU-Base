/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485EEPromCache.cpp: Implementierung der Klasse HS485EEPromCache.
//
//////////////////////////////////////////////////////////////////////

#include "HS485EEpromCache.h"
#include "HS485Device.h"
#include <Logger.h>
#include <cstring>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

const unsigned int HS485EEPromCache::CHUNK_SIZE=16;
//const unsigned int HS485EEPromCache::VALID_TIME=30000;
const unsigned int HS485EEPromCache::VALID_TIME=300000;

HS485EEPromCache::HS485EEPromCache(HS485Device* dev_instance)
:dev(dev_instance)
{
	flush_inhibit=false;
}

HS485EEPromCache::~HS485EEPromCache()
{
	flush_inhibit=false;
}

bool HS485EEPromCache::GetData(unsigned int address, unsigned int count, data_t *data)
{
    data->resize(count);
    unsigned int pos=0;
    while(count){
        unsigned int chunk_index=address/CHUNK_SIZE;
        unsigned int chunk_offset=address%CHUNK_SIZE;
        if(!EnsureChunkValid(chunk_index))return false;

        unsigned int nb_bytes=CHUNK_SIZE-chunk_offset;
        if(count<nb_bytes)nb_bytes=count;

        data_t& chunk_data=chunks[chunk_index].GetData();
        std::copy(chunk_data.begin()+chunk_offset, chunk_data.begin()+chunk_offset+nb_bytes, data->begin()+pos);
		pos+=nb_bytes;
        address+=nb_bytes;
        count-=nb_bytes;
    }
#if 0
	char buffer[4];
	std::string data_string;
	for(unsigned int i=0;i<data->size();i++){
		snprintf(buffer, sizeof(buffer), "%02X ", (int)(*data)[i]);
		data_string+=buffer;
	}
	LOG(Logger::LOG_DEBUG, "GetData() dev=%08lX, eep_addr=0x%X, data=%s", dev->address, address-count, data_string.c_str());
#endif
    return true;
}

bool HS485EEPromCache::GetShadowData(unsigned int address, unsigned int count, data_t *data)
{
    data->resize(count);
    unsigned int pos=0;
    while(count){
        unsigned int chunk_index=address/CHUNK_SIZE;
        unsigned int chunk_offset=address%CHUNK_SIZE;
        if(!EnsureChunkValid(chunk_index))return false;

        unsigned int nb_bytes=CHUNK_SIZE-chunk_offset;
        if(count<nb_bytes)nb_bytes=count;

        data_t& chunk_data=chunks[chunk_index].GetShadowData();
        std::copy(chunk_data.begin()+chunk_offset, chunk_data.begin()+chunk_offset+nb_bytes, data->begin()+pos);
		pos+=nb_bytes;
        address+=nb_bytes;
        count-=nb_bytes;
    }
#if 0
	char buffer[4];
	std::string data_string;
	for(unsigned int i=0;i<data->size();i++){
		snprintf(buffer, sizeof(buffer), "%02X ", (int)(*data)[i]);
		data_string+=buffer;
	}
	LOG(Logger::LOG_DEBUG, "GetData() dev=%08lX, eep_addr=0x%X, data=%s", dev->address, address-count, data_string.c_str());
#endif
    return true;
}

bool HS485EEPromCache::PutData(unsigned int address, const data_t& data)
{
#if 0
	char buffer[4];
	std::string data_string;
	for(unsigned int i=0;i<data.size();i++){
		snprintf(buffer, sizeof(buffer), "%02X ", (int)data[i]);
		data_string+=buffer;
	}
	LOG(Logger::LOG_DEBUG, "PutData() dev=%08lX, eep_addr=0x%X, data=%s", dev->address, address, data_string.c_str());
#endif
    unsigned int count=data.size();
    unsigned int pos=0;
    while(count){
        unsigned int chunk_index=address/CHUNK_SIZE;
        unsigned int chunk_offset=address%CHUNK_SIZE;
        if(!EnsureChunkValid(chunk_index))return false;

        unsigned int nb_bytes=CHUNK_SIZE-chunk_offset;
        if(count<nb_bytes)nb_bytes=count;

        data_t& chunk_data=chunks[chunk_index].GetShadowData();
        std::copy(data.begin()+pos, data.begin()+pos+nb_bytes, chunk_data.begin()+chunk_offset);
		chunks[chunk_index].SetState(Chunk::DIRTY);

		pos+=nb_bytes;
        address+=nb_bytes;
        count-=nb_bytes;
    }
    return true;
}

bool HS485EEPromCache::EnsureChunkValid(unsigned int index)
{
    if(chunks.size()<=index)chunks.resize(index+1);
    Chunk& ch=chunks[index];
    Chunk::state_t state=ch.GetState();
    if(state==Chunk::CLEAN || state==Chunk::DIRTY)return true;
	if(dev->ReadEEProm(index*CHUNK_SIZE, CHUNK_SIZE, &ch.GetData())){
		ch.SetState(Chunk::CLEAN);
		return true;
	}
//	LOG(Logger::LOG_DEBUG, "HS485EEPromCache::EnsureChunkValid(%d)=false", index);
	return false;
}

bool HS485EEPromCache::GenerateEmptyChunks(unsigned int eeprom_size)
{
	data_t used_bits;
	unsigned int chunk_count=(eeprom_size+CHUNK_SIZE-1)/CHUNK_SIZE;
	chunks.resize(chunk_count);
	if(!dev->GetEEPromUsage(0, CHUNK_SIZE, eeprom_size/CHUNK_SIZE, &used_bits))return false;
	for(unsigned int i=0;(i<chunk_count) && (i<(used_bits.size()*8));i++){
		if(!(used_bits[i/8]&(1<<(i%8)))){
			chunks[i].GetData().clear();
			chunks[i].SetState(Chunk::CLEAN);
		}
	}
	return true;
}

bool HS485EEPromCache::Flush()
{
	if(flush_inhibit)return true;
	bool success=true;
//	LOG(Logger::LOG_DEBUG, "HS485EEPromCache::Flush()");
	for(unsigned int i=0;i<chunks.size();i++){
		if(chunks[i].GetState()==Chunk::DIRTY){
//			LOG(Logger::LOG_DEBUG, "HS485EEPromCache::Flush() WriteChunk(%d)", i);
			if(!WriteChunk(i)){
				LOG(Logger::LOG_ERROR, "HS485EEPromCache::Flush() WriteChunk(%d) failed", i);
				success=false;
			}
		}
	}
	return success;
}

void HS485EEPromCache::Clear()
{
	chunks.clear();
}

bool HS485EEPromCache::IsDirty()
{
	for(unsigned int i=0;i<chunks.size();i++){
		if(chunks[i].GetState()==Chunk::DIRTY)return true;
	}
	return false;
}

bool HS485EEPromCache::WriteChunk(unsigned int index)
{
    if(chunks.size()<=index)return false;
    Chunk& ch=chunks[index];
	if(dev->WriteEEProm(index*CHUNK_SIZE, ch.GetShadowData())){
		ch.SetState(Chunk::CLEAN);
		return true;
	}
	return false;
}

bool HS485EEPromCache::Chunk::SaveToXml(XMLNode* node)
{
	char buffer[CHUNK_SIZE*2+1];

	snprintf(buffer, sizeof(buffer), "%d", (int)state);
	node->addAttributeConst("state", buffer);

	bool empty=true;
	for(unsigned int i=0;i<data.size();i++){
		sprintf(buffer+2*i, "%02X", data[i]);
		if(data[i]!=0xff)empty=false;
	}
	if(empty)*buffer=0;
	node->addAttributeConst("data", buffer);
	return true;
}

bool HS485EEPromCache::Chunk::LoadFromXml(XMLNode& node)
{
	const char* temp=node.getAttribute("data");
	if(!temp)return false;
	int count=strlen(temp)/2;
	data.resize(count);
	for(int i=0;i<count;i++){
		char buffer[4];
		buffer[2]=0;
		strncpy(buffer, temp+2*i, 2);
		data[i]=strtol(buffer, NULL, 16);
	}

	temp=node.getAttribute("state");
	if(!temp)return false;
	state=(state_t)strtol(temp, NULL, 0);
	return true;
}

bool HS485EEPromCache::SaveToXml(XMLNode* node)
{
	char buffer[8];
	for(unsigned int i=0;i<chunks.size();i++){
		XMLNode chunk_node=node->addChildConst("chunk");
		snprintf(buffer, sizeof(buffer), "%d", i);
		chunk_node.addAttributeConst("index", buffer);
		if(!chunks[i].SaveToXml(&chunk_node))return false;
	}
	return true;
}

bool HS485EEPromCache::LoadFromXml(XMLNode& node)
{
	chunks.clear();
	chunks.resize(node.nChildNode("chunk"));
	int i=0;
	XMLNode chunk_node=node.getChildNode("chunk", &i);
	while(!chunk_node.isEmpty()){
		const char* temp=chunk_node.getAttribute("index");
		if(!temp)return false;
		unsigned int chunk_index=strtoul(temp, NULL, 0);
		if(chunk_index>=chunks.size())chunks.resize(chunk_index+1);
		if(!chunks[chunk_index].LoadFromXml(chunk_node))return false;
		chunk_node=node.getChildNode("chunk", &i);
	}
	return true;
}
