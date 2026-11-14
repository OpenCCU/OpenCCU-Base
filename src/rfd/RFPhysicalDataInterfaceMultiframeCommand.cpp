/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * RFPhysicalDataInterfaceSplittedCommand.cpp
 *
 *  Created on: May 30, 2014
 *      Author: user
 */

#include "RFPhysicalDataInterfaceMultiframeCommand.h"

#include <Logger.h>
#include <type_registry.h>
#include <RFDevice.h>
#include <RFDeviceDescription.h>

#include <string.h>


using namespace XmlRpc;

static hsscomm::type_registry::factory<RFPhysicalDataInterfaceMultiframeCommand> RFPhysicalDataInterfaceCommandFactory;

RFPhysicalDataInterfaceMultiframeCommand::RFPhysicalDataInterfaceMultiframeCommand()
: HSSPhysicalDataInterface()
, frameLimit(10)
, sendImmidiately(false)
, wakeupOnImmediateSendFailure(true)
{
}

RFPhysicalDataInterfaceMultiframeCommand::~RFPhysicalDataInterfaceMultiframeCommand()
{
}

bool RFPhysicalDataInterfaceMultiframeCommand::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_multiframe_command", tag)==0) {
    	return true;
	}
    else {
    	return false;
    }
}

bool RFPhysicalDataInterfaceMultiframeCommand::InitFromXml(XMLNode &node, XMLNode &root_node)
{

	//TODO max_frames=x

	//valueId
	const char* temp=node.getAttribute("value_id");
	if(temp) {
		valueId=temp;
	}

	int i=0;
	XMLNode set_node = node;
	XMLNode frameNode = set_node.getChildNode("multiframe_command_frame", &i);
	if(frameNode.isEmpty()) {
		LOG(Logger::LOG_ERROR, "\"multiframe_command_frame\" is missing");
		return false;
	}
	//read attribute max_frames
	temp=frameNode.getAttribute("max_frames");
	if(temp) {
		frameLimit = strtol(temp, NULL, 0);
	}

	//read attribute payload_index
	temp=frameNode.getAttribute("payload_index");
	if(temp) {
		frameStructure.payloadIndex = strtol(temp, NULL, 0);
		if(frameStructure.payloadIndex == 0) {
			LOG(Logger::LOG_ERROR, "Invalid value for attribute \"payload_index\". Must be an integer greater than 0.");
			return false;
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "Attribute \"payload_index\" is missing.");
		return false;
	}

	//read frame description
	temp=frameNode.getAttribute("type");
	if(temp){
		if(temp[0]=='#')frameStructure.frameType=temp[1];
		else frameStructure.frameType=strtol(temp, NULL, 0);
	}
	if(frameStructure.frameType==-1) {
		LOG(Logger::LOG_ERROR, "Missing or unparseable frame type.");
		return false;
	}

	temp=frameNode.getAttribute("subtype_index");
	if(temp){
		frameStructure.frameType|=strtol(temp, NULL, 0)<<8;
		temp=frameNode.getAttribute("subtype");
		if(!temp)return false;
		frameStructure.frameType|=strtol(temp, NULL, 0)<<16;
	}

	temp=frameNode.getAttribute("channel_field");
	if(temp){
		frameStructure.frameChannelField=StructuredFrame::FieldIdFromString(temp);
	}else{
		frameStructure.frameChannelField=0;
	}

	temp=frameNode.getAttribute("send_immidiately");
	if(temp) {
		if(strcmp(temp, "true") == 0) {
			sendImmidiately = true;
		}
		else {//default
			sendImmidiately = false;
		}
	}
	
	temp=frameNode.getAttribute("wakeup_on_immidiate_send_failure");
	if(temp) {
		if(strcmp(temp, "false") == 0) {
			wakeupOnImmediateSendFailure = false;
		}
		else {//default
			wakeupOnImmediateSendFailure = true;
		}
	}
	return true;//TODO Implement InitFromXml
}

bool RFPhysicalDataInterfaceMultiframeCommand::SetupInstance(LogicalInstance* inst)
{
/*	if(value_id.empty())return true; // copied from ...InterfaceCommand
	if(no_init)return true;
	HSSLogicalType* lt=GetParent()->GetParent()->GetLogicalType();
	XmlRpcValue val=lt->GetDefault();
	lt->EnforceConstraints(inst, &val, HSSLogicalType::OP_WRITE);
	if(!GetParent()->GetParent()->LogicalToPhysical(inst, val, &val))return false;
	inst->SetStoredValue(value_id, val, ValueStore::FLAG_VOLATILE);*/
	return true;
}

bool RFPhysicalDataInterfaceMultiframeCommand::SetDefaultConfig(LogicalInstance* inst,XmlRpcValue val)
{
	return false;//Currently not supported. Maybe one day if GetData is getting implemented.
}

bool RFPhysicalDataInterfaceMultiframeCommand::GetData(LogicalInstance* inst, XmlRpcValue* param)
{
	return false;//Not implemented yet. Maybe read from data store...
}



bool RFPhysicalDataInterfaceMultiframeCommand::PutData(LogicalInstance* inst, XmlRpcValue& param)
{
	RFChannel* channel=dynamic_cast<RFChannel*>(inst);
	if(channel == NULL) {
		LOG(Logger::LOG_DEBUG, "RFPhysicalDataInterfaceMultiframeCommand::PutData() called with non-channel instance.");
		return false;
	}
	/*if(!value_id.empty()){
		if(!inst->SetStoredValue(value_id, param, ValueStore::FLAG_VOLATILE))return false;
	}*/

	//Calculation and validation of sizes and frames to send.
	const unsigned int framePayloadSize = maxBidcosFrameSize - frameStructure.payloadIndex;
	const unsigned int dataSize = static_cast<int>(param.size());
	unsigned int framesToSend = 1;
	if(dataSize > framePayloadSize) {
		framesToSend = dataSize / framePayloadSize;
		if(dataSize % framePayloadSize != 0) {
			framesToSend++;
		}
	}
	if(framesToSend > frameLimit) {
		LOG(Logger::LOG_ERROR, "RFPhysicalDataInterfaceMultiframeCommand::PutData(): Amount of data exceeds defined limit.");
		return false;
	}
	//LOG(Logger::LOG_ALL, "RFPhysicalDataInterfaceMultiframeCommand::PutData(): Frame payload size: %u", framePayloadSize);
	//LOG(Logger::LOG_ALL, "RFPhysicalDataInterfaceMultiframeCommand::PutData(): Framelimit: %u", frameLimit);
	//LOG(Logger::LOG_ALL, "RFPhysicalDataInterfaceMultiframeCommand::PutData(): Frames to send: %u", framesToSend);
	//Send data
	unsigned int dataIndex = 0;
	if(channel->GetDevice()->RxNeedsWakeup() && (!channel->GetDevice()->IsEnoughSpaceLeftInWakeupFrameQueue(framesToSend))) {
		LOG(Logger::LOG_ERROR, "RFPhysicalDataInterfaceMultiframeCommand::PutData(): Not enough space in wakeup message queue.");
		return false;
	}
	for(unsigned int i = 0; i < framesToSend; i++) {
		//Initialize frame...
		BidcosFrame frame;
		frame.SetType(frameStructure.frameType);
		if(frameStructure.frameChannelField != 0) {
			frame.SetIntValue(frameStructure.frameChannelField, channel->GetIndex());
		}
		const std::string& data = param;
		//LOG(Logger::LOG_ALL, "Param is: %s", HM2::toDebugHexStr(data).c_str());
		//Add user data
		for(unsigned int c = 0; (c < framePayloadSize) && (dataIndex < data.size()) ; c++) {
			//LOG(Logger::LOG_ALL, "RFPhysicalDataInterfaceMultiframeCommand::PutData(): Writing data portion byte %u of %u bytes. Value: 0x%.2X", dataIndex+1, data.size(), (unsigned char)data.at(c));
			//LOG(Logger::LOG_ALL, "RFPhysicalDataInterfaceMultiframeCommand::PutData(): Calling frame.SetByteData(%u, 0x%.2X). Value: 0x%.2X", frameStructure.payloadIndex+c, data.at(dataIndex));
			frame.SetByteData(frameStructure.payloadIndex+c, data.at(dataIndex));
			dataIndex++;
		}
		//Send the frame...
		if((sendImmidiately == false) && channel->GetDevice()->RxNeedsWakeup()){
			channel->GetDevice()->QueueAfterWakeupFrame(frame);
			continue;
		}
		if(!channel->GetDevice()->SendFrame(&frame)){
			if(wakeupOnImmediateSendFailure) {
				channel->GetDevice()->QueueAfterWakeupFrame(frame);
				continue;
			}
			else {
				return false;
			}
		}
		//Process answer if there is one...
		bool auth_violate=channel->GetAES() && (frame.GetAuthKey()!=channel->GetDevice()->GetAESKey());
		if(auth_violate){
			LOG(Logger::LOG_WARNING, "RFPhysicalDataInterfaceMultiframeCommand::PutData(): Authentication violation. Send failed.");
			return false;
		}
		BidcosFrame* response=frame.GetResponse();
		if(response != NULL) {
			//LOG(Logger::LOG_ALL, "RFPhysicalDataInterfaceMultiframeCommand::PutData(): Got answer");
			channel->GetDevice()->ProcessIncomingFrame(*response);//TODO check if return code must be checked
		}

	}
	return true;
}

