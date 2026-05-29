/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosInterfaceConcentrator.h"
#include "BidcosInterface.h"
#include "BidcosFrameDecoder.h"
#include "BidcosFrame.h"
#include <Logger.h>
#include "RFManager.h"
#include <utils.h>
#include <PropertyMap.h>
#include <TripleBurstInterface.h>
#include <BidcosFrameWaitTime.h>
#include <stdio.h>
#ifdef WIN32
#include "win_pipe.h"
#define close(x) closesocket(x)
#else
#include <unistd.h>
#endif

BidcosInterfaceConcentrator::BidcosInterfaceConcentrator(void)
{
	bidcos_address=0;
	default_interface=NULL;
	pthread_mutex_init(&mutex_devices, NULL);
	pthread_mutex_init(&mutex_rx_frame_queue, NULL);
	pthread_mutex_init(&mutex_cur_tx_frame, NULL);
    pthread_cond_init(&cond_cur_tx_frame, NULL);
    cur_tx_frame = NULL;
	if(pipe(pipe_fds)){
		perror("pipe()");
	}
	setfd(pipe_fds[0]);
    promiscuous_mode=false;
}

BidcosInterfaceConcentrator::~BidcosInterfaceConcentrator(void)
{
    while(map_interfaces.size()){
        delete map_interfaces.begin()->second;
        map_interfaces.erase(map_interfaces.begin());
    }
	::close(pipe_fds[0]);
	::close(pipe_fds[1]);
	pthread_mutex_destroy(&mutex_devices);
	pthread_mutex_destroy(&mutex_rx_frame_queue);
	pthread_mutex_destroy(&mutex_cur_tx_frame);
	pthread_cond_destroy(&cond_cur_tx_frame);
}

BidcosInterfaceConcentrator::DeviceData::DeviceData()
{
	aes_key=0;
	aes_channels=0;
	flags=0;
	address=0;
	tx_counter=0;
	rx_counter=0;
	roam_inhibit_valid_until = 0;
}

BidcosInterfaceConcentrator::DeviceData::~DeviceData()
{
}

int BidcosInterfaceConcentrator::DeviceData::GetTxCounter()
{
	tx_counter=(tx_counter+1)&0x7f;
	return tx_counter;
}

int BidcosInterfaceConcentrator::DeviceData::GetTxCounterWithoutIncrement() 
{
	return tx_counter;
}

void BidcosInterfaceConcentrator::DeviceData::SetTxCounter(const int counter)
{
	tx_counter= counter & 0x7f;
}

void BidcosInterfaceConcentrator::DeviceData::UpdateTxCounter(int received_counter)
{
	tx_counter=(received_counter+8)&0xff;
}


bool BidcosInterfaceConcentrator::AddInterface(BidcosInterface* bi)
{
	std::string serial=bi->GetSerialNumber();
	if(serial.empty()){
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::AddInterface(): no serial number");
		return false;
	}
	map_interfaces[serial]=bi;
	bi->SetConcentrator(this);
	UpdateDefaultInterface();
	return true;
}

bool BidcosInterfaceConcentrator::CreateInterfacesFromFile(PropertyMap& config)
{
    PropertyMap::StringList sections=config.ListSections();
    for(PropertyMap::StringList::iterator it=sections.begin();it!=sections.end();it++)
    {
        std::string& section=*it;
        if(section.find("Interface ")==0)
        {
            config.SetCurrentSection(section);
            std::string type=config.GetStringValue("Type", "");
            BidcosInterface* bif=BidcosInterface::CreateFromType(type);
            if(!bif){
                LOG( Logger::LOG_ERROR, "Interface type %s not supported", type.c_str());
                continue;
            }
            PropertyMap::Section interface_section=config.GetSection(section);
            if(!bif->Init(interface_section))
            {
//                LOG( Logger::LOG_ERROR, "Configuration for %s not complete", bif->GetSerialNumber().c_str());
                delete bif;
                continue;
            }
            AddInterface(bif);
        }
    }
    if(map_interfaces.empty()){
        LOG(Logger::LOG_ERROR, "No BidCoS-Interface available");
        return false;
    }
    return true;
}

bool BidcosInterfaceConcentrator::BegInterfacesForAddress(unsigned int* address)
{
    //return the given address that has been given to the majority of interfaces
    //if no such address exists, return an arbitrary native address

    typedef std::map<unsigned int, unsigned int> t_count_map;
    t_count_map count_map;

    // let's see what our interfaces have to offer and store it in a map
	t_map_interfaces::iterator it;
	for(it=map_interfaces.begin();it!=map_interfaces.end();it++){
        unsigned int given_address=0;
        unsigned int native_address=0;
		if(it->second->DonateAddress(&native_address, &given_address))
        {
            if(given_address)count_map[given_address]++;
            count_map[native_address];
        }
	}
    if(count_map.empty())return false;
    //search for the given address with the most entries

    *address=count_map.begin()->first;

    for(t_count_map::iterator it=count_map.begin();it!=count_map.end();it++)
    {
        if(it->second > count_map[*address])*address=it->first;
    }
    return true;
}

void BidcosInterfaceConcentrator::UpdateDefaultInterface()
{
	if(!default_interface && !map_interfaces.empty()){
		default_interface=map_interfaces.begin()->second;
		LOG(Logger::LOG_DEBUG, "Default interface is now %s", default_interface->GetSerialNumber().c_str());
	}else{
		bool default_interface_still_exists=false;
		t_map_interfaces::iterator it;
		for(it=map_interfaces.begin();it!=map_interfaces.end();it++){
			if(it->second == default_interface){
				default_interface_still_exists=true;
				break;
			}
		}
		if(!default_interface_still_exists){
			if(!map_interfaces.empty()){
				default_interface=map_interfaces.begin()->second;
				LOG(Logger::LOG_DEBUG, "BidcosInterfaceConcentrator::UpdateDefaultInterface(): default interface removed. default interface changed to %s", default_interface->GetSerialNumber().c_str());
			}else{
				default_interface=NULL;
				LOG(Logger::LOG_DEBUG, "BidcosInterfaceConcentrator::UpdateDefaultInterface(): default interface removed.");
			}
		}
	}
}

bool BidcosInterfaceConcentrator::RemoveDeviceFromInterface(DeviceData& dev_data)
{
    BidcosInterface* cur_interface=GetInterface(dev_data.cur_interface_id);
    if(!cur_interface)return false;
	if(!cur_interface->RemoveDevice(dev_data.address))return false;
    return true;
}

bool BidcosInterfaceConcentrator::AddDeviceToInterface(DeviceData& dev_data)
{
    BidcosInterface* cur_interface = GetInterface(dev_data.cur_interface_id);
    if (!cur_interface)
        return false;
    if (!cur_interface->AddDevice(dev_data.address))
        return false;
    cur_interface->SetDeviceAesPolicy(dev_data.address, dev_data.aes_key, dev_data.aes_channels);
    if (dev_data.flags & DeviceData::FLAG_WAKEUP)
    {
        if(dev_data.flags & DeviceData::FLAG_LAZY_CONFIG)
        {
            cur_interface->AddDeviceWakeupRequest(dev_data.address,true);
        }
        else
        {
            cur_interface->AddDeviceWakeupRequest(dev_data.address);
        }
    }
    else
        cur_interface->RemoveDeviceWakeupRequest(dev_data.address);
    return true;
}

bool BidcosInterfaceConcentrator::RemoveInterface(const std::string& serial)
{
	t_map_interfaces::iterator it=map_interfaces.find(serial);
	if(it==map_interfaces.end())return false;
    BidcosInterface* bi=it->second;
	map_interfaces.erase(it);
	UpdateDefaultInterface();

	bi->SetConcentrator(NULL);
    delete bi;
    return true;
}

bool BidcosInterfaceConcentrator::ProcessReceivedFrame(const BidcosFrame& frame)
{
    if( frame.MatchType(BidcosFrame::FT_ACK_W_AES_CHALLENGE) || frame.MatchType(BidcosFrame::FT_AES_SOLUTION) ) {
        if(!promiscuous_mode)return true;
    }
	if( frame.GetReceiverAddress() == bidcos_address )
	{
		int sender = frame.GetSenderAddress();
		pthread_mutex_lock(&mutex_devices);
		t_map_devices::iterator it=map_devices.find(sender);
		if(it != map_devices.end()){
			it->second.UpdateTxCounter(frame.GetTelegramCounter());
		}
		pthread_mutex_unlock(&mutex_devices);
	}

    pthread_mutex_lock(&mutex_cur_tx_frame);
    bool mark_preliminary=false;
    if(cur_tx_frame)
    {
        if( cur_tx_frame->GetInterfaceId() == frame.GetInterfaceId() ){
            if( (!frame.IsPreliminary()) && (!cur_tx_frame->CheckReceiveComplete(NULL)) ) {
                if(cur_tx_frame->CheckAndAddResponse(frame))
                {
					pthread_cond_signal(&cond_cur_tx_frame);
					if ((frame.MatchType(BidcosFrame::FT_ACK_OR_NACK))
							&& (frame.MatchType(BidcosFrame::FT_ACK_STATUS))) {
						mark_preliminary = false;
					}
					else
					{
						mark_preliminary = true;
					}
                }
            }
        }else{
            if( frame.GetSenderAddress() == cur_tx_frame->GetReceiverAddress() ){
                // this is an intermediate frame from a communication initiated by us on an other interface
                // only use this frame to decide which interface to use in the future
                mark_preliminary=true;
            }
        }
    }
    pthread_mutex_unlock(&mutex_cur_tx_frame);

    //LOG(Logger::LOG_DEBUG, "RX: %s", BidcosFrameDecoder::ToString(&frame).c_str());
    //LOG(Logger::LOG_DEBUG, "accepted_as_response=%s", accepted_as_response?"true":"false");

    pthread_mutex_lock(&mutex_rx_frame_queue);
	rx_frame_queue.push_front(frame);
	if(rx_frame_queue.size() > RX_QUEUE_MAX_SIZE){
		rx_frame_queue.pop_back();
	}
    if(mark_preliminary){
        // use this frame only for RSSI based interface update, for nothing else
        rx_frame_queue.front().SetPreliminary(true);
    }
	pthread_mutex_unlock(&mutex_rx_frame_queue);
	//tell the XmlRpc main loop that we now have some input
	#ifdef WIN32
		send(pipe_fds[1], "", 1, 0);
	#else
	if(write(pipe_fds[1], "", 1) != 1)
		return false;
	#endif
	return true;
}

bool BidcosInterfaceConcentrator::SendFrame(BidcosFrame* frame)
{
	int sender=frame->GetSenderAddress();
	int receiver=frame->GetReceiverAddress();
	if(!sender){
		frame->SetSenderAddress(GetBidcosAddress());
	}else if(sender != GetBidcosAddress()){
		frame->TransformToSimulationMessage();
		frame->SetSenderAddress(GetBidcosAddress());
	}

	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(receiver);
	if(it != map_devices.end()){
		frame->SetTelegramCounter(it->second.GetTxCounter());
	}
	pthread_mutex_unlock(&mutex_devices);

	//find out which interface to use
	BidcosInterface* iface=NULL;
	pthread_mutex_lock(&mutex_devices);
	it=map_devices.find(receiver);
	if(it!=map_devices.end()){
        CheckAndUpdateInterfaceAssociation(it->second);
        iface=GetInterface(it->second.cur_interface_id);
	}
	pthread_mutex_unlock(&mutex_devices);
    if(!iface)iface=default_interface;
	if(!iface){
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SendFrame(): no suitable interface found");
		return false;
	}
	frame->ClearResponses();
	frame->ResetAuthKey();
    frame->SetTimestamp(time_millis());
    frame->SetInterfaceId(iface->GetSerialNumber());


    LOG(Logger::LOG_DEBUG, "TX: %s", BidcosFrameDecoder::ToString(frame).c_str());
    uint64_t waitTime = WAIT_TIME_DEFAULT;
    pthread_mutex_lock(&mutex_cur_tx_frame);
    cur_tx_frame=frame;
    pthread_mutex_unlock(&mutex_cur_tx_frame);
    if(!SendFrame(frame, iface, receiver, waitTime)){//writes waitTime!
        pthread_mutex_lock(&mutex_cur_tx_frame);
        cur_tx_frame=NULL;
        pthread_mutex_unlock(&mutex_cur_tx_frame);
        return false;
    }
    pthread_mutex_lock(&mutex_cur_tx_frame);
//    LOG(Logger::LOG_DEBUG, "start wait responses:%u", time_millis());
	while(!frame->CheckReceiveComplete(&waitTime)){
        if(!waitTime)break;
        if(waitTime > 30000)
        {
            LOG(Logger::LOG_ERROR, "Requested wait time is %lu ms. Aborting.", waitTime);
            break;
        }
		struct timespec abs_timeout=millis2abstime(waitTime);
		if( pthread_cond_timedwait(&cond_cur_tx_frame, &mutex_cur_tx_frame, &abs_timeout)!=0 )break;
	}
    cur_tx_frame=NULL;
	pthread_mutex_unlock(&mutex_cur_tx_frame);

	pthread_mutex_lock(&mutex_devices);
	it=map_devices.find(receiver);
	if(it!=map_devices.end()){
        it->second.flags|=DeviceData::FLAG_ROAM_INHIBIT;
        it->second.roam_inhibit_valid_until=time_millis()+AFTER_SEND_ROAM_INHIBIT_TIME;
	}
	pthread_mutex_unlock(&mutex_devices);

    BidcosFrame* response=frame->GetResponse();
    if(response && (response->GetRSSI() != BidcosFrame::INVALID_RSSI_VALUE)){
		RFManager* rfManager = RFManager::GetSingleton();
        rfManager->UpdateRssiInfo(receiver, response->GetInterfaceId(), response->GetRSSI());

		RFDevice* device = rfManager->GetRFDevice(receiver);
		if (device != NULL) {
			device->SetPeerRSSI(response->GetRSSI());
		}
    }

//    LOG(Logger::LOG_DEBUG, "end wait responses:%u", time_millis());
	return frame->CheckReceiveComplete(NULL);
}

bool BidcosInterfaceConcentrator::SendFrame(BidcosFrame* frame, BidcosInterface* iface, const int receiver, uint64_t& waitTime)
{
	//Check if device needs burst / triple burst
	waitTime = WAIT_TIME_DEFAULT;
	bool tripleBurst = false;
	if( (frame->GetCtrl() & BidcosFrame::CTRL_BURST) != 0) {
		waitTime = WAIT_TIME_SINGLE_BURST;
		pthread_mutex_lock(&mutex_devices);
		t_map_devices::iterator it=map_devices.find(receiver);
		pthread_mutex_unlock(&mutex_devices);
		if(it!=map_devices.end()) {//if burst, check for triple burst
			if( (it->second.flags & DeviceData::FLAG_TRIPLE_BURST) != 0) {
				//check if interface supports that, otherwise -> error
				if(iface->SupportsTripleBurst()) {
					tripleBurst = true;
				}
				else {
					LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SendFrame(): Interface does not support desired burst type.");
					return false;
				}
			}
		}
	}
	//send the frame
	if(tripleBurst) {
		ITripleBurstInterface* pTBI = dynamic_cast<ITripleBurstInterface*>( iface );
		if(pTBI != NULL) {
			waitTime = WAIT_TIME_TRIPLE_BURST;
			return pTBI->SendFrameTripleBurst(frame);
		}
		else {
			return false;
		}
	}
	else {//normal send
		return iface->SendFrame(frame);
	}
}


bool BidcosInterfaceConcentrator::PerformMulticastSend(std::set<RFDevice*>& peer_devices, BidcosFrame* frame, bool burst)
{
	bool retval = true;
	bool firstPeer = true;
	uint64_t broadcastBurstTime = 0;
	//find highest telegram counter
	int highestCounter = 0;
	for(std::set<RFDevice*>::iterator it=peer_devices.begin();it!=peer_devices.end();it++) {
		std::map<int, DeviceData>::iterator mdIt=map_devices.find((*it)->GetAddress());
		if(mdIt->second.GetTxCounterWithoutIncrement() > highestCounter) {
			highestCounter = mdIt->second.GetTxCounterWithoutIncrement();
		}
	}
	//Send messages to peers
	for(std::set<RFDevice*>::iterator it=peer_devices.begin();it!=peer_devices.end();it++) {
		//LOG(Logger::LOG_ALL, "RFChannel::SendToPeers() Sending others" );
		RFDevice* dev=*it;
		BidcosFrame f=*frame;
		//f.SetSenderAddress(GetBidcosAddress()); is already set!! this method is also used for simulated telegrams. If we set the central address, it would not work
		//Manipulate telegram counter to increase avoid collision.
		std::map<int, DeviceData>::iterator mdIt=map_devices.find(dev->GetAddress());
		if(mdIt != map_devices.end()) {
			mdIt->second.SetTxCounter(highestCounter);
		}
		if(firstPeer) {
			firstPeer = false;
			//LOG(Logger::LOG_ALL, "RFChannel::SendToPeers() Sending broadcast to first device" );
			f.SetCtrl(BidcosFrame::CTRL_BIDI | BidcosFrame::CTRL_RPT_ENABLE | BidcosFrame::CTRL_BCAST | (burst?BidcosFrame::CTRL_BURST:0));			
			f.SetReceiverAddress(dev->GetAddress());
			broadcastBurstTime = time_millis();
			retval &= this->SendFrame(&f);
			continue;
				/*First frame can't be sent using RFDevice because RFDevice->Send() overrides broadcast flag*/
		}
		else if(burst){
			//LOG(Logger::LOG_ALL, "RFChannel::SendToPeers() Setting burst time." );
			dev->setLastBurstTime(broadcastBurstTime);
		}
		retval &= dev->SendFrame(&f);
	}
	return retval;
}

bool BidcosInterfaceConcentrator::SendUpdataFrame(const std::string &uFrame, int deviceAddress)
{
	int frameLength = uFrame.size(), index = 0, frameCounter = 0;
    bool first = true, retVal;

	int sender=GetBidcosAddress();
	
	//find out which interface to use
	BidcosInterface* iface=NULL;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(deviceAddress);
	if(it!=map_devices.end()){
        CheckAndUpdateInterfaceAssociation(it->second);
        iface=GetInterface(it->second.cur_interface_id);
		frameCounter = it->second.GetTxCounter();
		
	}
	else
	{
		pthread_mutex_unlock(&mutex_devices);
		return false;
	}
	pthread_mutex_unlock(&mutex_devices);
    if(!iface)iface=default_interface;
	if(!iface){
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SendFrame(): no suitable interface found");
		return false;
	}
    retVal = true;
    while (index < frameLength - (2 + 48 + 2))
    {
        BidcosFrame rfUpdateFrame;
		rfUpdateFrame.SetReceiverAddress(deviceAddress);
		rfUpdateFrame.SetType(BidcosFrame::FT_FIRMWAREUPDATE);
		rfUpdateFrame.SetReceiverAddress(deviceAddress);
		rfUpdateFrame.SetSenderAddress(sender);
		rfUpdateFrame.SetTelegramCounter(frameCounter);
		rfUpdateFrame.ClearResponses();
		rfUpdateFrame.ResetAuthKey();
		rfUpdateFrame.SetTimestamp(time_millis());
		rfUpdateFrame.SetInterfaceId(iface->GetSerialNumber());
		LOG(Logger::LOG_DEBUG, "Send UpdateTelegram");
		if (first)
        {	
			rfUpdateFrame.SetCtrl(0); // reset Ctrl
			rfUpdateFrame.SetStringValue(9,50,uFrame.substr(index,50));
            first = false;
            index += 50;
        }
        else
        {
			rfUpdateFrame.SetCtrl(0); // reset Ctrl
			rfUpdateFrame.SetStringValue(9,48,uFrame.substr(index,48));
            index += 48;
        }
		pthread_mutex_lock(&mutex_cur_tx_frame);
		cur_tx_frame=&rfUpdateFrame;
		pthread_mutex_unlock(&mutex_cur_tx_frame);
		if(!iface->SendFrame(&rfUpdateFrame)){
			pthread_mutex_lock(&mutex_cur_tx_frame);
			cur_tx_frame=NULL;
			pthread_mutex_unlock(&mutex_cur_tx_frame);
			return false;
		}
		
	    //LOG(Logger::LOG_DEBUG, "start wait responses:%u", time_millis());
		uint64_t wait_time;
		pthread_mutex_lock(&mutex_cur_tx_frame);
		while(!rfUpdateFrame.CheckReceiveComplete(&wait_time)){
			if(!wait_time)break;
			if(wait_time > 30000)
			{
				LOG(Logger::LOG_ERROR, "Requested wait time is %lu ms. Aborting.", wait_time);
				break;
			}
			struct timespec abs_timeout=millis2abstime(wait_time);
			if( pthread_cond_timedwait(&cond_cur_tx_frame, &mutex_cur_tx_frame, &abs_timeout)!=0 )break;
		}
		cur_tx_frame=NULL;
		pthread_mutex_unlock(&mutex_cur_tx_frame);
		if (!retVal)
        {
			return retVal;
        }		
    }
	BidcosFrame rfUpdateFrame;
	rfUpdateFrame.SetReceiverAddress(deviceAddress);
	rfUpdateFrame.SetType(BidcosFrame::FT_FIRMWAREUPDATE);
	rfUpdateFrame.SetReceiverAddress(deviceAddress);
	rfUpdateFrame.SetSenderAddress(sender);
	rfUpdateFrame.SetTelegramCounter(frameCounter);
	rfUpdateFrame.ClearResponses();
	rfUpdateFrame.ResetAuthKey();
	rfUpdateFrame.SetTimestamp(time_millis());
	rfUpdateFrame.SetInterfaceId(iface->GetSerialNumber());
	rfUpdateFrame.SetCtrl(BidcosFrame::CTRL_BIDI); // reset Ctrl
	rfUpdateFrame.SetStringValue(9,frameLength - index,uFrame.substr(index,frameLength - index));
	LOG(Logger::LOG_DEBUG, "Send Final UpdateTelegram");
	pthread_mutex_lock(&mutex_cur_tx_frame);
	cur_tx_frame=&rfUpdateFrame;
	pthread_mutex_unlock(&mutex_cur_tx_frame);
	if(!iface->SendFrame(&rfUpdateFrame)){
		pthread_mutex_lock(&mutex_cur_tx_frame);
		cur_tx_frame=NULL;
		pthread_mutex_unlock(&mutex_cur_tx_frame);
		pthread_mutex_lock(&mutex_devices);
		t_map_devices::iterator it=map_devices.find(deviceAddress);
		if(it!=map_devices.end()){
			 it->second.tx_counter = (frameCounter-1)&0x7f;
		}
		pthread_mutex_unlock(&mutex_devices);
		return false;
	}
	
//    LOG(Logger::LOG_DEBUG, "start wait responses:%u", time_millis());
	pthread_mutex_lock(&mutex_cur_tx_frame);
	uint64_t wait_time;
	while(!rfUpdateFrame.CheckReceiveComplete(&wait_time)){
		if(!wait_time)break;
		if(wait_time > 30000)
		{
			LOG(Logger::LOG_ERROR, "Requested wait time is %lu ms. Aborting.", wait_time);
			break;
		}
		struct timespec abs_timeout=millis2abstime(wait_time);
		if( pthread_cond_timedwait(&cond_cur_tx_frame, &mutex_cur_tx_frame, &abs_timeout)!=0 )break;
	}

	cur_tx_frame=NULL;
	pthread_mutex_unlock(&mutex_cur_tx_frame);
    //TODO: Senden in einer Methode auslagern eventuel mit handling des Roming
    return retVal;
}

void BidcosInterfaceConcentrator::SetBidcosAddress(int address)
{
	bidcos_address=address;
}

int BidcosInterfaceConcentrator::GetBidcosAddress()
{
	return bidcos_address;
}

bool BidcosInterfaceConcentrator::BegInterfacesForKeys(int* current_key, int* previous_key)
{
	if(!default_interface)return false;
    int default_index;
    int temp_index;
	return default_interface->GetAesKeyIndexes(&default_index, current_key, previous_key, &temp_index);
}

bool BidcosInterfaceConcentrator::SetAesKeyTemp(int index, const std::string& data)
{
	bool success=true;
	t_map_interfaces::iterator it;
	for(it=map_interfaces.begin();it!=map_interfaces.end();it++){
		if(!it->second->SetAesKeyTemp(index, data))success=false;
	}
	return success;
}

bool BidcosInterfaceConcentrator::SetAesKeyUser(int index, const std::string& data, int last_index, const std::string& last_data)
{
	bool success=true;
	t_map_interfaces::iterator it;
	for(it=map_interfaces.begin();it!=map_interfaces.end();it++){
		if(!it->second->SetAesKeyUser(index, data, last_index, last_data))success=false;
	}
	return success;
}

void BidcosInterfaceConcentrator::SetPromicuousMode(bool pm)
{
    promiscuous_mode=pm;
}

bool BidcosInterfaceConcentrator::StartInterfaces()
{
	if(!bidcos_address){
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::StartInterfaces(): address not set");
		return false;
	}
	bool success=true;
	t_map_interfaces::iterator it;
	for(it=map_interfaces.begin();it!=map_interfaces.end();it++){
		if(!it->second->StartInterface(bidcos_address))success=false;
	}
	return success;
}

bool BidcosInterfaceConcentrator::StopInterfaces()
{
	bool success=true;
	t_map_interfaces::iterator it;
	for(it=map_interfaces.begin();it!=map_interfaces.end();it++){
		if(!it->second->StopInterface())success=false;
	}
	return success;
}


bool BidcosInterfaceConcentrator::AddDevice(int address, const std::string& current_interface, bool roaming_allowed)
{
	RFDevice* device = RFManager::GetSingleton()->GetRFDevice(address);
	return AddDevice(device, address, current_interface, roaming_allowed);
}

bool BidcosInterfaceConcentrator::AddDevice(RFDevice* device, int address, const std::string& current_interface, bool roaming_allowed)
{
    if(address == bidcos_address)return true;
	

	pthread_mutex_lock(&mutex_devices);
	DeviceData& dev_data=map_devices[address];
    dev_data.address=address;
    dev_data.flags &= ~DeviceData::FLAG_TEMPORARY;
    RemoveDeviceFromInterface(dev_data);
    if(!GetInterface(current_interface) && default_interface){
        dev_data.cur_interface_id=default_interface->GetSerialNumber();
    } else {
        dev_data.cur_interface_id=current_interface;
    }
	if(roaming_allowed)dev_data.flags |= DeviceData::FLAG_ROAMING;

	//Triple burst feature
	RFDeviceDescription::BurstMode burstMode = device->GetDeviceDescription()->GetBurstMode();
	if(burstMode == RFDeviceDescription::BURST_MODE_TRIPLE) {
		dev_data.flags |= DeviceData::FLAG_TRIPLE_BURST;
		if( ! GetInterface(dev_data.cur_interface_id)->SupportsTripleBurst() ) {
			if(GetInterface(current_interface)) {
				dev_data.cur_interface_id = current_interface;
			}
			else if(default_interface){
				dev_data.cur_interface_id = default_interface->GetSerialNumber();
			}
			else {
				LOG(Logger::LOG_ERROR, "Unable to assign an interface for device %s", device->GetSerial().c_str());
			}
		}
	}
	//e.o. triple burst feature

	AddDeviceToInterface(dev_data);
	pthread_mutex_unlock(&mutex_devices);
	return true;
}

bool BidcosInterfaceConcentrator::RemoveDevice(int address)
{
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
    if(it==map_devices.end()){
    	pthread_mutex_unlock(&mutex_devices);
        return false;
    }
	RemoveDeviceFromInterface(it->second);

	map_devices.erase(address);
	pthread_mutex_unlock(&mutex_devices);
	return true;
}

bool BidcosInterfaceConcentrator::SetDeviceAesPolicy(int address, int aes_key, uint64 aes_channels)
{
    if(address == bidcos_address)return true;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it==map_devices.end()){
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SetDeviceAesPolicy(): unknown device, address=0x%06X", address);
		pthread_mutex_unlock(&mutex_devices);
		return false;
	}
	it->second.aes_key=aes_key;
	it->second.aes_channels=aes_channels;
    BidcosInterface* cur_interface=GetInterface(it->second.cur_interface_id);
    if(cur_interface){
		cur_interface->SetDeviceAesPolicy(address, aes_key, aes_channels);
	}else{
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SetDeviceAesPolicy(): no interface associated with device 0x%06X", address);
    	pthread_mutex_unlock(&mutex_devices);
		return false;
	}
	pthread_mutex_unlock(&mutex_devices);
	return true;
}

bool BidcosInterfaceConcentrator::AddDeviceWakeupRequest(int address,bool lazyConfig)
{
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it==map_devices.end()){
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::AddDeviceWakeupRequest(): unknown device, address=0x%06X", address);
		return false;
	}
	it->second.flags |= DeviceData::FLAG_WAKEUP;
	if(lazyConfig) it->second.flags |= DeviceData::FLAG_LAZY_CONFIG;
    BidcosInterface* cur_interface=GetInterface(it->second.cur_interface_id);
	if(cur_interface){
	    if(cur_interface->SupportLazyConfig())
	    {
	        cur_interface->AddDeviceWakeupRequest(address,lazyConfig);
	    }
	    else
	    {
	        cur_interface->AddDeviceWakeupRequest(address);
	    }
	}else{
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::AddDeviceWakeupRequest(): no interface associated with device 0x%06X", address);
		return false;
	}
	pthread_mutex_unlock(&mutex_devices);
	return true;
}

bool BidcosInterfaceConcentrator::RemoveDeviceWakeupRequest(int address)
{
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it==map_devices.end()){
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::RemoveDeviceWakeupRequest(): unknown device, address=0x%06X", address);
		return false;
	}
	it->second.flags &= ~DeviceData::FLAG_WAKEUP;
	it->second.flags &= ~DeviceData::FLAG_LAZY_CONFIG;
    BidcosInterface* cur_interface=GetInterface(it->second.cur_interface_id);
	if(cur_interface){
		cur_interface->RemoveDeviceWakeupRequest(address);
	}else{
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::RemoveDeviceWakeupRequest(): no interface associated with device 0x%06X", address);
		return false;
	}
   	pthread_mutex_unlock(&mutex_devices);
	return true;
}

unsigned BidcosInterfaceConcentrator::handleEvent(unsigned eventType)
{
	char buffer[32];
#ifdef WIN32  
	int rc = recv(getfd(), buffer, 32, 0);
	if (rc < 0)
	{
    static bool warningPrinted = false;
    if (!warningPrinted)
    {
      LOG(Logger::LOG_WARNING, "BidcosInterfaceConcentrator::handleEvent: receive error: network cable might be disconnected");
      LOG(Logger::LOG_ERROR, "Critical Error: restart BidCoS-Service");
      warningPrinted = true;
    }
    
		usleep(50 * 1000);
	}
#else
	if(read(getfd(), buffer, 32) < 0)
		LOG(Logger::LOG_WARNING, "BidcosInterfaceConcentrator::handleEvent: read() error");
#endif
	while(true){
		bool has_frame=false;
		BidcosFrame frame;
		pthread_mutex_lock(&mutex_rx_frame_queue);
		if(rx_frame_queue.size()){
			frame=rx_frame_queue.back();
			rx_frame_queue.pop_back();
			has_frame=true;
		}
		pthread_mutex_unlock(&mutex_rx_frame_queue);
		if(!has_frame)break;
        //LOG(Logger::LOG_DEBUG, "Frame: %s", BidcosFrameDecoder::ToString(&frame).c_str());
		int sender=frame.GetSenderAddress();

        if(frame.GetRSSI() != BidcosFrame::INVALID_RSSI_VALUE){
			RFManager* rfManager = RFManager::GetSingleton();
			rfManager->UpdateRssiInfo(sender, frame.GetInterfaceId(), frame.GetRSSI());

			RFDevice* device = rfManager->GetRFDevice(sender);
			if (device != NULL) {
				device->SetPeerRSSI(frame.GetRSSI());
			}
        }

    	pthread_mutex_lock(&mutex_devices);
		t_map_devices::iterator it=map_devices.find(sender);
        bool frame_already_seen=true;
		if(it!=map_devices.end()){
            // first check if our associated interface is still the first choice
            // there is no point in doing this check with authenticated frames, because
            // we only get them from exactly one interface
            // on the other hand, we will accept authenticated frames even if the default interface has
            // changed in the meantime
            if(!frame.WasAuthenticated()){
                if( it->second.flags & ( DeviceData::FLAG_ROAMING | DeviceData::FLAG_TEMPORARY ) ){
                    if(it->second.last_received_frame!=frame){
                        KillTimer( TIMER_ID_CHECK_ROAMING | sender );
                        CheckAndUpdateInterfaceAssociation(it->second);
			            it->second.last_received_frame=frame;
                        frame_already_seen=false;
                    } else if(it->second.last_received_frame.GetRSSI() < frame.GetRSSI()){
                        it->second.last_received_frame.SetRSSI(frame.GetRSSI());
                        it->second.last_received_frame.SetInterfaceId(frame.GetInterfaceId());
                        KillTimer( TIMER_ID_CHECK_ROAMING | sender );
                        RequestTimer( ROAMING_CHECK_DELAY, TIMER_ID_CHECK_ROAMING | sender );
                    }
                }

                BidcosInterface* cur_interface=GetInterface(it->second.cur_interface_id);
                if( (cur_interface && ( frame.GetInterfaceId() != it->second.cur_interface_id )) ||
                    ( (!cur_interface) && frame_already_seen ) ){
               	        pthread_mutex_unlock(&mutex_devices);
                        //LOG(Logger::LOG_DEBUG, "Duplicate frame or not from default interface");
                        continue;
                }
            }
           	pthread_mutex_unlock(&mutex_devices);
        }else{
            if(frame.MatchType(BidcosFrame::FT_SYSINFO) && !promiscuous_mode){
            	DeviceData& dev_data=map_devices[sender];
                dev_data.address=sender;
                dev_data.flags=DeviceData::FLAG_TEMPORARY;
                dev_data.last_received_frame=frame;
                RequestTimer( ROAMING_CHECK_DELAY, TIMER_ID_CHECK_ROAMING | sender );
               	pthread_mutex_unlock(&mutex_devices);
                continue;
            }
           	pthread_mutex_unlock(&mutex_devices);
            if(!promiscuous_mode)continue;
        }
        if(frame.IsPreliminary()){
            //LOG(Logger::LOG_DEBUG, "Preliminary frame");
            continue;
        }
		RFManager::GetSingleton()->ProcessIncomingFrame(frame);
    }
    return 1;
}

bool BidcosInterfaceConcentrator::ListInterfaces(std::vector<BidcosInterface*>* vec_interfaces, BidcosInterface** default_interface)
{
	t_map_interfaces::iterator it;
	for(it=map_interfaces.begin();it!=map_interfaces.end();it++){
        vec_interfaces->push_back(it->second);
	}
    *default_interface=this->default_interface;
    return true;
}

BidcosInterface* BidcosInterfaceConcentrator::GetInterface(const std::string& interface_id)
{
	t_map_interfaces::iterator it=map_interfaces.find(interface_id);
	if(it!=map_interfaces.end())return it->second;
    else return NULL;
}
bool BidcosInterfaceConcentrator::CheckAndUpdateInterfaceAssociation(DeviceData& dev_data)
{
    if(!(dev_data.flags & DeviceData::FLAG_ROAMING))return false;
    if(dev_data.flags & DeviceData::FLAG_ROAM_INHIBIT){
        if( int(dev_data.roam_inhibit_valid_until - time_millis()) > 0 )return false;
        else dev_data.flags &= ~DeviceData::FLAG_ROAM_INHIBIT;
    }
    if(!dev_data.last_received_frame.IsValid())return false;
    if(dev_data.last_received_frame.GetInterfaceId().empty())return false;
    if(dev_data.cur_interface_id == dev_data.last_received_frame.GetInterfaceId())return false;
    RemoveDeviceFromInterface(dev_data);
    dev_data.cur_interface_id=dev_data.last_received_frame.GetInterfaceId();
    AddDeviceToInterface(dev_data);
    LOG(Logger::LOG_DEBUG, "Device 0x%06X is now associated with interface %s because of %s", dev_data.address, dev_data.cur_interface_id.c_str(), BidcosFrameDecoder::ToString(&dev_data.last_received_frame).c_str());
    dev_data.last_received_frame=BidcosFrame();
    return true;
}

void BidcosInterfaceConcentrator::OnTimer(uint32_t cookie)
{
    if( (cookie & 0xff000000)==TIMER_ID_CHECK_ROAMING ){
        int address=cookie&0xffffff;
        BidcosFrame last_frame;
    	pthread_mutex_lock(&mutex_devices);
		t_map_devices::iterator it=map_devices.find(address);
        if(it != map_devices.end()){
            last_frame=it->second.last_received_frame;
            if(!(it->second.flags&DeviceData::FLAG_TEMPORARY)){
                CheckAndUpdateInterfaceAssociation(it->second);
            }else{
                map_devices.erase(address);
            }
        }
        pthread_mutex_unlock(&mutex_devices);
        if(last_frame.IsValid() && last_frame.MatchType(BidcosFrame::FT_SYSINFO)){
            // Sysinfo frame has been held back
            RFManager::GetSingleton()->ProcessIncomingFrame(last_frame);
        }
    }
}

std::string BidcosInterfaceConcentrator::GetDefaultInterfaceId()
{
    if(default_interface)return default_interface->GetSerialNumber();
    else return "";
}
bool BidcosInterfaceConcentrator::IsInterfaceUpdatable(int address)
{
	bool retVal = false;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it==map_devices.end()){
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::IsInterfaceUpdatable(): unknown device, address=0x%06X", address);
		return false;
	}
    BidcosInterface* cur_interface=GetInterface(it->second.cur_interface_id);
	if(cur_interface){
		retVal = cur_interface->Updateable();
	}else{
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::IsInterfaceUpdatable(): no interface associated with device 0x%06X", address);
		return false;
	}
	pthread_mutex_unlock(&mutex_devices);
	return retVal;
}
bool BidcosInterfaceConcentrator::SetInterfaceTo100kMode(int address)
{
	bool retVal = false;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it==map_devices.end()){
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SetInterfaceTo100kMode(): unknown device, address=0x%06X", address);
		return false;
	}
    BidcosInterface* cur_interface=GetInterface(it->second.cur_interface_id);
	if(cur_interface){
		retVal = cur_interface->Set100kMode();
	}else{
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SetInterfaceTo100kMode(): no interface associated with device 0x%06X", address);
		return false;
	}
	pthread_mutex_unlock(&mutex_devices);
	return retVal;
}
bool BidcosInterfaceConcentrator::SetInterfaceTo10kMode(int address)
{
	bool retVal = false;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it==map_devices.end()){
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SetInterfaceTo10kMode(): unknown device, address=0x%06X", address);
		return false;
	}
    BidcosInterface* cur_interface=GetInterface(it->second.cur_interface_id);
	if(cur_interface){
		retVal = cur_interface->Set10kMode();
	}else{
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SetInterfaceTo10kMode(): no interface associated with device 0x%06X", address);
		return false;
	}
	pthread_mutex_unlock(&mutex_devices);
	return retVal;
}

bool BidcosInterfaceConcentrator::DutyCycleForUpdate(int address,UpdateFile &uFile)
{
	int dutycycle = 0;
	uint32_t bytesToSend = 0;
	int upFrameCount = uFile.getUpdateFrameCount();
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it==map_devices.end()){
    	pthread_mutex_unlock(&mutex_devices);
		LOG(Logger::LOG_ERROR, "BidcosInterfaceConcentrator::SetInterfaceTo10kMode(): unknown device, address=0x%06X", address);
		return false;
	}
	pthread_mutex_unlock(&mutex_devices);
    BidcosInterface* cur_interface=GetInterface(it->second.cur_interface_id);
	if(cur_interface)
	{
		dutycycle  = cur_interface->GetDutyCycle();
		if(dutycycle == -1)
			return false;
	}
	
	for(int i = 0; i< upFrameCount;++i)
	{
		int framesToSend = uFile.getFrameLength(i)/48 + 1;
		bytesToSend += uFile.getFrameLength(i) + 19 * framesToSend;
	}
	//450000 Byte entsprechen bei 100k Datenrate 100%
	if((uint32_t)(450000 - (4500 * dutycycle)) > (bytesToSend + 90000)) // nach dem Update sollen noch mehr als 20% DutyCycle �brigsein
	{
		return true;
	}
	return false;
}
