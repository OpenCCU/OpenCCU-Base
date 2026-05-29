/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/



#ifndef RFREPLACEMAP_H_
#define RFREPLACEMAP_H_
#include <string>
#include <set>
#include <xmlParser.h>
#include "RFDevice.h"
#include <FrameDescription.h>

class RFReplaceMap
{
public:
    typedef enum Replaceable_e
    {
        NOT_REPLACEABLE,
        REPLACEABLE,
        UNDEFINED,
    } Replaceable_t;
    RFReplaceMap();
    virtual ~RFReplaceMap();
    bool isReplaceble(RFDevice *oldInst, RFDevice *newInst);
    class Type:public FrameDescription{
        public:
    		//! Konstruktor
            Type(){
    			type=-1;
    			priority=0;
    			isUpdatable = false;
            };
    		//! Einlesen aus einer XML-Datei
            bool InitFromXml(XMLNode& node, XMLNode& root_node);
    		//! Gibt die Priorit�t zur�ck. Bestimmt damit den R�ckgabewert von RFDeviceDescription::Matches().
    		int GetPriority(){return priority;};
    		bool IsUpdatable(){return isUpdatable;};
    	protected:
    		//! Ger�tename (englische Langbezeichnung aus der XML-Datei), wird derzeit nicht verwendet
    		std::string name;
    		//! Priorit�t
    		int priority;
    		bool isUpdatable;
        };
private:
    void initReplaceMap();
    bool LoadFromXml(XMLNode& node);

    typedef std::vector<Type*> typepointervec_t;
    typedef std::vector<Type> typevec_t;
    typedef std::map<Type *, typevec_t> replaceMap_t;
    typepointervec_t alltypes;
    replaceMap_t replaceMap;
    Type * getType(RFDevice *inst);
    Type * getType(const std::string &id, const std::string &fwVersion);
    bool isInit;

};

#endif /* RFREPLACEMAP_H_ */
