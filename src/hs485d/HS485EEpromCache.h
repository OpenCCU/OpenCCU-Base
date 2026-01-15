/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485EEpromCache.h: Schnittstelle für die Klasse HS485EEPromCache.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HS485EEPROMCACHE_H__EA8AC4FA_BE1E_491D_94B8_336B496F9E6B__INCLUDED_)
#define AFX_HS485EEPROMCACHE_H__EA8AC4FA_BE1E_491D_94B8_336B496F9E6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <utils.h>
#include <xmlParser.h>

class HS485Device;

//! Jede Instanz dieser Klasse verwaltet die im EEProm eines HS485-Gerätes gespeicherten Konfigurationsdaten
/*!
 *  Diese Klasse kümmert sich um die Kommunikation der EEProm-Daten zwischen Zentrale und Geräten.
 */
class HS485EEPromCache  
{
public:
    typedef std::vector<unsigned char> data_t;
	bool Flush();
	HS485EEPromCache(HS485Device* dev);
	virtual ~HS485EEPromCache();
	bool GetData(unsigned int address, unsigned int count, data_t *data);
	bool GetShadowData(unsigned int address, unsigned int count, data_t *data);
	bool PutData(unsigned int address, const data_t& data);
	bool IsDirty();
	bool GenerateEmptyChunks(unsigned int eeprom_size);
	bool SaveToXml(XMLNode* node);
	bool LoadFromXml(XMLNode& node);
	void Clear();
	void SetFlushInhibit(bool fi){flush_inhibit=fi;};
protected:
	bool EnsureChunkValid(unsigned int index);
	bool WriteChunk(unsigned int index);
	bool ReadChunk(unsigned int index);
    static const unsigned int CHUNK_SIZE;
    static const unsigned int VALID_TIME;
    class Chunk
    {
    public:
        typedef enum{FREE, CLEAN, DIRTY} state_t;
        Chunk(){
            state=FREE;
        };
        data_t& GetData(){
			if(data.empty()){
				data.insert(data.begin(), CHUNK_SIZE, 0xff);
			}
            return data;
        };
        data_t& GetShadowData(){
			if(shadow_data.empty()){
				shadow_data=GetData();
			}
            return shadow_data;
        };
        state_t GetState(){
			return state;
        };
        void SetState(state_t state){
			if(state==CLEAN && shadow_data.size()){
				data=shadow_data;
				shadow_data.clear();
			}
            this->state=state;
        };
		bool SaveToXml(XMLNode* node);
		bool LoadFromXml(XMLNode& node);
    protected:
        data_t data;
        data_t shadow_data;
        state_t state;
    };
    typedef std::vector<Chunk> chunks_t;
    chunks_t chunks;
    HS485Device* dev;
	bool flush_inhibit;
};

#endif // !defined(AFX_HS485EEPROMCACHE_H__EA8AC4FA_BE1E_491D_94B8_336B496F9E6B__INCLUDED_)
