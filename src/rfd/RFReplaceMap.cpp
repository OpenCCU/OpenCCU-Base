/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/



#include "RFReplaceMap.h"
#include <string>
#include <OSCompat.h>
#include "RFManager.h"
#include <Logger.h>

RFReplaceMap::RFReplaceMap() :
		isInit(false) {
// TODO Automatisch generierter Konstruktorstub

}

RFReplaceMap::~RFReplaceMap() {
// TODO !CodeTemplates.destructorstub.tododesc!
}

bool RFReplaceMap::isReplaceble(RFDevice* oldInst, RFDevice* newInst) {
	if (!isInit) {
		initReplaceMap();
	}
	BidcosFrame *sysinfoOld = oldInst->GetStoredSysinfo();
	BidcosFrame *sysinfoNew = newInst->GetStoredSysinfo();
	typepointervec_t::iterator allIt;
	Type *keyType = NULL;
	for(allIt = alltypes.begin(); allIt != alltypes.end();++allIt)
	{
		if((*allIt)->MatchFrame(*sysinfoNew))
		{
			keyType = (*allIt);
			break;
		}
	}
	typevec_t::iterator replacebleIt;
	for(replacebleIt = replaceMap[keyType].begin(); replacebleIt != replaceMap[keyType].end();++replacebleIt)
	{
		if(replacebleIt->MatchFrame(*sysinfoOld))
		{
			return true;
		}
	}
	return false;
}

void RFReplaceMap::initReplaceMap() {
	std::string replaceMap =
			RFManager::GetSingleton()->GetConfigPropertyMap().GetStringValue(
					"Replacemap File");

	XMLResults xmlResult;
	XMLNode rootNode = XMLNode::parseFile(replaceMap.c_str(), "RF", &xmlResult);
	if (!xmlResult.error) {
		LoadFromXml(rootNode);
	} else {
		LOG(Logger::LOG_ERROR, "unable to load replace map");
	}
	isInit = true;

}

bool RFReplaceMap::LoadFromXml(XMLNode& node) {

	bool retVal = false;
	int i = 0;
	if (node.isEmpty()) {
		return false;
	}
	XMLNode replace_node = node.getChildNode("replace", &i);
	while (!replace_node.isEmpty()) {
		Type *type = new Type;
		XMLNode new_type = replace_node.getChildNode("new_type");
		if (!new_type.isEmpty()) {
			type->InitFromXml(new_type, node);
			alltypes.push_back(type);
		}
		XMLNode replaceable_types = replace_node.getChildNode(
				"replaceable_types");
		if (!replaceable_types.isEmpty()) {
			int j = 0;
			XMLNode r_type_node = replaceable_types.getChildNode("type", &j);
			while (!r_type_node.isEmpty()) {
				Type r_type;
				r_type.InitFromXml(r_type_node, node);
				replaceMap[type].push_back(r_type);
				r_type_node = replaceable_types.getChildNode("type",&j);
			}

		}
		replace_node = node.getChildNode("replace", &i);
	}

	return retVal;
}

bool RFReplaceMap::Type::InitFromXml(XMLNode& node, XMLNode& root_node)
{
//    LOG(Logger::LOG_DEBUG, "RFDeviceDescription::Type::InitFromXml()");
    const char* temp=node.getAttribute("name");
	if(!temp){
		LOG(Logger::LOG_WARNING, "<type> attribute \"name\" not found");
		return false;
	}
	name=temp;


	temp=node.getAttribute("updatable");
	isUpdatable = temp && temp[0] == 't';


	type=0x00;
	direction=DIR_FROM_DEVICE;

    temp=node.getAttribute("priority");
	if(temp){
		priority=strtol(temp, NULL, 0);
		if(priority<0){
			LOG(Logger::LOG_WARNING, "<type> attribute \"priority\" invalid value");
			return false;
		}
	}

	if(!FrameDescription::InitFromXml(node, root_node))return false;
	if(!params.size())type=-1;
	return true;
}
