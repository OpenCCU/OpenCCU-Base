/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// BidcosRemoteInterface.cpp: Implementierung der Klasse BidcosRemoteInterface.
//
//////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "BidcosRemoteInterface.h"
#include "BidcosInterfaceConcentrator.h"
#include "BidcosInterfaceConnection.h"
#include <Logger.h>
#include <utils.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <cstring>
#include <cstdio>

#include <TimeZoneInfo.h>
#ifndef WIN32
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

BidcosRemoteInterface::BidcosRemoteInterface()
{
    given_bidcos_address=0;
	exit=false;
    thread_rx=0;
	cur_tx_frame=NULL;
    cur_tx_response_status=RESPONSE_IDLE;
	pthread_mutex_init(&mutex_exit, NULL);
	pthread_mutex_init(&mutex_bidcos_response, NULL);
	pthread_mutex_init(&mutex_socket_response, NULL);
	pthread_mutex_init(&mutex_remote_timer_offset, NULL);
	pthread_mutex_init(&mutex_keys, NULL);
	pthread_cond_init(&cond_bidcos_response, NULL);
	pthread_cond_init(&cond_socket_response, NULL);
    aes_keys[AES_KEY_TYPE_DEFAULT].index=0;
    aes_keys[AES_KEY_TYPE_CURRENT_USER].index=0;
    aes_keys[AES_KEY_TYPE_PREVIOUS_USER].index=0;
    aes_keys[AES_KEY_TYPE_TEMP].index=0;
    socket_response_message=NULL;
    socket_response_expected_command=0;
    keepalive_response_expected=false;
	debug_dump_interval=0;
    remote_timer_offset = 0;
}

BidcosRemoteInterface::~BidcosRemoteInterface()
{
	pthread_mutex_destroy(&mutex_exit);
	pthread_mutex_destroy(&mutex_bidcos_response);
	pthread_mutex_destroy(&mutex_socket_response);
	pthread_mutex_destroy(&mutex_keys);
	pthread_mutex_destroy(&mutex_remote_timer_offset);
	pthread_cond_destroy(&cond_bidcos_response);
	pthread_cond_destroy(&cond_socket_response);
}

bool BidcosRemoteInterface::SendFrame(BidcosFrame* frame)
{
    BidcosInterfaceMessage msg;
    msg.SetCommand(BidcosInterfaceMessage::CMD_SEND);
    msg.PutUIntParam(0, static_cast<unsigned int>(frame->GetTimestamp())); //id
    msg.PutUIntParam(1, SCHEDULE_ASAP); //schedule
    msg.PutUIntParam(2, 0); //time
    msg.PutUIntParam(3, SEND_FLAG_RESPONSE); //flags
    msg.PutUIntParam(4, static_cast<unsigned int>(GetRemoteTimer() + 2000)); //timeout
    msg.PutFrameParam(5, frame);

    pthread_mutex_lock(&mutex_bidcos_response);
    cur_tx_frame=frame;
    cur_tx_response_status=RESPONSE_WAIT;
    pthread_mutex_unlock(&mutex_bidcos_response);

    if(!GetConnection()->OutputMessageToInterface(msg))
    {
        pthread_mutex_lock(&mutex_bidcos_response);
        cur_tx_frame=NULL;
        cur_tx_response_status=RESPONSE_IDLE;
        pthread_mutex_unlock(&mutex_bidcos_response);
        return false;
    }

	struct timespec abs_timeout=millis2abstime(2000);

    pthread_mutex_lock(&mutex_bidcos_response);
    while(cur_tx_response_status==RESPONSE_WAIT){
        if( pthread_cond_timedwait(&cond_bidcos_response, &mutex_bidcos_response, &abs_timeout) != 0 )break;
    }
    ResponseStatus status=cur_tx_response_status;
    cur_tx_frame=NULL;
    cur_tx_response_status=RESPONSE_IDLE;
    pthread_mutex_unlock(&mutex_bidcos_response);

	return status==RESPONSE_OK;
}

bool BidcosRemoteInterface::AddDevice(int address)
{
	if(!BidcosInterface::AddDevice(address))return false;
    if(!IsConnected())return true;
    if(!RemoteAddDevice(address))
    {
        LOG(Logger::LOG_WARNING, "Adding new device to LAN-IF failed");
    }
    return true;
}

bool BidcosRemoteInterface::RemoveDevice(int address)
{
	if(!BidcosInterface::RemoveDevice(address))return false;
    if(!IsConnected())return true;
    if(!RemoteRemoveDevice(address))
    {
        LOG(Logger::LOG_WARNING, "Removing device from LAN-IF failed");
    }
    return true;
}

bool BidcosRemoteInterface::SetDeviceAesPolicy(int address, int aes_key, uint64 aes_channels)
{
	if(!BidcosInterface::SetDeviceAesPolicy(address, aes_key, aes_channels))return false;
    if(!IsConnected())return true;
    if(!RemoteAddDevice(address))
    {
        LOG(Logger::LOG_WARNING, "Changing device on LAN-IF failed");
    }
    return true;
}

bool BidcosRemoteInterface::AddDeviceWakeupRequest(int address,bool lazyConfig)
{
	if(!BidcosInterface::AddDeviceWakeupRequest(address,lazyConfig))return false;
    if(!IsConnected())return true;
    RemoteAddDevice(address);
	return true;
}

bool BidcosRemoteInterface::RemoveDeviceWakeupRequest(int address)
{
	if(!BidcosInterface::RemoveDeviceWakeupRequest(address))return false;
    if(!IsConnected())return true;
    RemoteAddDevice(address);
	return true;
}

bool BidcosRemoteInterface::SetAESKey(AesKeyType type, int index, const std::string &key)
{
    if(type == AES_KEY_TYPE_DEFAULT)return false;

    pthread_mutex_lock(&mutex_keys);
    aes_keys[type].index=index;
    aes_keys[type].key=key;
    pthread_mutex_unlock(&mutex_keys);

    return true;
}

bool BidcosRemoteInterface::RemoteSendAESKey(AesKeyType type)
{
    bool retval=false;
    if(type == AES_KEY_TYPE_DEFAULT)return false;

    std::string key;
    int index;

    pthread_mutex_lock(&mutex_keys);
    index=aes_keys[type].index;
    key=aes_keys[type].key;
    pthread_mutex_unlock(&mutex_keys);

    BidcosInterfaceMessage set_key_message;
    set_key_message.SetCommand(BidcosInterfaceMessage::CMD_SET_KEY);
    set_key_message.PutUIntParam(0, (unsigned int) type);
    set_key_message.PutUIntParam(1, index);
    set_key_message.PutBinaryParam(2, key);
    BidcosInterfaceMessage set_key_response;
    set_key_response.SetCommand(BidcosInterfaceMessage::CMD_KEY_INDEXES);
    if(!SendSynchronousMessage(set_key_message, &set_key_response, 300)){
        LOG(Logger::LOG_WARNING, "Timeout waiting for set key message");
        return false;
    }

    if( set_key_response.GetUIntParam(type) != (unsigned int)index )
    {
        LOG(Logger::LOG_WARNING, "Error setting key on %s, Index[%d] is %u, expected %d", GetSerialNumber().c_str(), (int)type, set_key_response.GetUIntParam(type), index);
    }else{
        retval=true;
    }
/*
    LOG(Logger::LOG_DEBUG, "%s AES keys: %u, %u, %u, %u", GetSerialNumber().c_str(),
        set_key_response.GetUIntParam(0), 
        set_key_response.GetUIntParam(1), 
        set_key_response.GetUIntParam(2), 
        set_key_response.GetUIntParam(3));
*/
    return retval;
}

bool BidcosRemoteInterface::SetAesKeyTemp(int index, const std::string& data)
{
	SetAESKey(AES_KEY_TYPE_TEMP, index, data);
    if(IsConnected())return RemoteSendAESKey(AES_KEY_TYPE_TEMP);
    else return true;
}

bool BidcosRemoteInterface::SetAesKeyUser(int index, const std::string& data, int last_index, const std::string& last_data)
{
    bool retval=true;
	SetAESKey(AES_KEY_TYPE_CURRENT_USER, index, data);
	SetAESKey(AES_KEY_TYPE_PREVIOUS_USER, last_index, last_data);
    if(IsConnected()){
        retval &= RemoteSendAESKey(AES_KEY_TYPE_CURRENT_USER);
        retval &= RemoteSendAESKey(AES_KEY_TYPE_PREVIOUS_USER);
    }
    return retval;
}

bool BidcosRemoteInterface::SendSynchronousMessage(BidcosInterfaceMessage& msg, BidcosInterfaceMessage* response, uint64_t timeout)
{
    bool retval=false;
    pthread_mutex_lock(&mutex_socket_response);
    if(socket_response_message){
        pthread_mutex_unlock(&mutex_socket_response);
        return false;
    }

    if(thread_rx && !pthread_equal( thread_rx, pthread_self())){
        //rx thread is running and we are not the rx thread
        socket_response_expected_command=response->GetCommand();
        if(!GetConnection()->OutputMessageToInterface(msg))goto cleanup;

	    struct timespec abs_timeout=millis2abstime(timeout);
        while(!socket_response_message){
            pthread_cond_timedwait(&cond_socket_response, &mutex_socket_response, &abs_timeout);
        }
        if(socket_response_message){
            *response = *socket_response_message;
            retval=true;
        }
    }else{
        BidcosInterfaceMessage input_dummy_msg;

        // read all pending messages
        while( GetConnection()->RetrieveNextInputMessage(0, &input_dummy_msg) );

        if(!GetConnection()->OutputMessageToInterface(msg))goto cleanup;
        uint64_t wait_end=time_millis()+timeout;
        int64_t time_remain;
        while( (!retval) && (time_remain=wait_end - time_millis()) > 0 ){
            BidcosInterfaceMessage rx_msg;
	        while( (!retval) && GetConnection()->RetrieveNextInputMessage(time_remain, &rx_msg)){
                if(rx_msg.GetCommand() == response->GetCommand()){
                    *response = rx_msg;
                    retval = true;
                }
            }
        }
	}
cleanup:
    if(socket_response_message){
        *response=*socket_response_message;
        delete socket_response_message;
        socket_response_message=NULL;
    }
    socket_response_expected_command=0;
    pthread_mutex_unlock(&mutex_socket_response);
    return retval;
}

int BidcosRemoteInterface::GetAddress()
{
	return given_bidcos_address;
}

bool BidcosRemoteInterface::StartInterface(int bidcos_address)
{

    given_bidcos_address=bidcos_address;
    pthread_mutex_lock(&mutex_exit);
	exit=false;
	pthread_mutex_unlock(&mutex_exit);
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setstacksize(&attr, 512*1024);
	pthread_create(&thread_rx, &attr, _RxThreadFunction, reinterpret_cast<void*>(this));
	pthread_attr_destroy( &attr );
    return true;
}

bool BidcosRemoteInterface::StopInterface()
{
	void* status;
	pthread_mutex_lock(&mutex_exit);
	exit=true;
	pthread_mutex_unlock(&mutex_exit);

	if(thread_rx)pthread_join(thread_rx, &status);
	thread_rx=0;
	return true;
}


/*static*/ void* BidcosRemoteInterface::_RxThreadFunction(void* arg)
{
	return ((BidcosRemoteInterface*)arg)->RxThreadFunction();
}

void* BidcosRemoteInterface::RxThreadFunction()
{
  uint64_t next_keepalive_time=time_millis()+KEEPALIVE_INTERVAL;
  uint64_t next_timeinfo_time=time_millis()+TIMEINFO_INTERVAL;
	uint64_t next_debug_dump_time = time_millis()+30000;
	while(true)
	{
        pthread_mutex_lock(&mutex_exit);
        if(exit)break;
        pthread_mutex_unlock(&mutex_exit);

		while( !IsConnected() )
		{
            next_keepalive_time=time_millis()+KEEPALIVE_INTERVAL;
            next_timeinfo_time=time_millis()+TIMEINFO_INTERVAL;
			next_debug_dump_time = time_millis()+30000;
			if(!Connect())sleep(15);
            pthread_mutex_lock(&mutex_exit);
            if(exit)break;
            pthread_mutex_unlock(&mutex_exit);
		}

        pthread_mutex_lock(&mutex_exit);
        if(exit)break;
        pthread_mutex_unlock(&mutex_exit);

		while( (!GetConnection()->HasError()) && GetConnection()->IsConnected() )
		{
            int timeout = 1000;
    		BidcosInterfaceMessage msg;
            while(GetConnection()->RetrieveNextInputMessage(timeout, &msg)){
                HandleInputMessage(msg);
                timeout = 0;
            }

            pthread_mutex_lock(&mutex_exit);
            if(exit)break;
            pthread_mutex_unlock(&mutex_exit);

            if( int64_t(next_keepalive_time - time_millis()) <= 0 )
            {
                if( (keepalive_response_expected) || (!SendKeepalive()) )
                {
	    		    break;
                }
                next_keepalive_time=time_millis()+KEEPALIVE_INTERVAL;
                keepalive_response_expected=true;
            }
            if( int64_t(next_timeinfo_time - time_millis()) <= 0 )
            {
                if(!RemoteSetTime())
                {
	    		    break;
                }
                next_timeinfo_time=time_millis()+TIMEINFO_INTERVAL;
            }
            if( (debug_dump_interval > 0) && (int(next_debug_dump_time - time_millis()) <= 0) )
            {
				RemoteRequestDump();
                next_debug_dump_time=time_millis()+debug_dump_interval*1000;
            }
		}
		GetConnection()->Disconnect();
	}
    return 0;
}

void BidcosRemoteInterface::HandleCommandResponse(unsigned int ref_id, unsigned int flags, unsigned int timestamp, int auth_key, int rssi, BidcosFrame& frame)
{
    frame.SetInterfaceId(GetSerialNumber());
    if(rssi < 1000)frame.SetRSSI(rssi);
	pthread_mutex_lock( &mutex_bidcos_response );
	if( (cur_tx_response_status==RESPONSE_WAIT) && (cur_tx_frame != NULL) && (ref_id == static_cast<unsigned int>(cur_tx_frame->GetTimestamp()) ) )
	{
		if( flags & FLAG_OK_RESPONSE ){
			frame.SetTimestamp(RemoteTimestamp2HostTimestamp(timestamp));
            if(flags & (FLAG_AUTH_REQ_MASTER | FLAG_AUTH_REQ_SLAVE)){
                cur_tx_frame->SetAuthKey(auth_key);
                frame.SetAuthKey(auth_key);
            }
            if( !cur_tx_frame->CheckAndAddResponse(frame) ){
                LOG( Logger::LOG_DEBUG, "Response not accepted: %s", frame.ToString().c_str());
            }
			cur_tx_response_status=RESPONSE_OK;
			frame.SetUnreachReason(BidcosFrame::UNREACH_FALSE);
		}else if( flags & FLAG_OK_SENT ){
			cur_tx_response_status=RESPONSE_OK;
			frame.SetUnreachReason(BidcosFrame::UNREACH_FALSE);
		}else{
            if(flags & (FLAG_AUTH_REQ_MASTER | FLAG_AUTH_REQ_SLAVE)){
                cur_tx_frame->SetAuthKey(auth_key);
            }
			cur_tx_response_status=RESPONSE_FAIL;

			frame.SetUnreachReason(BidcosFrame::UNREACH_NO_RESPONSE);
		}
		pthread_cond_signal( &cond_bidcos_response );
        pthread_mutex_unlock( &mutex_bidcos_response );
//        LOG( Logger::LOG_DEBUG, "Response received @%u flags=0x%08X: %s", time_millis(), flags, frame.ToString().c_str());
		if( flags & (FLAG_DUTY_CYCLE_90|FLAG_DUTY_CYCLE_FULL) )
		{
			LOG( Logger::LOG_WARNING, "%s: Duty cycle %s used", GetSerialNumber().c_str(), (flags & FLAG_DUTY_CYCLE_FULL)?"fully":"nearly");
		}
    }else{
        pthread_mutex_unlock( &mutex_bidcos_response );
        //if necessary, pretend this frame was received as an event
		if(frame.IsValid()){
			LOG( Logger::LOG_DEBUG, "%s: Processing unexpected response as event: %s", GetSerialNumber().c_str(), frame.ToString().c_str());
			HandleCommandEvent(flags, timestamp, auth_key, rssi, frame);
		}
    }
}

void BidcosRemoteInterface::HandleCommandEvent(unsigned int flags, unsigned int timestamp, int auth_key, int rssi, BidcosFrame& frame)
{
    if(frame.GetSenderAddress() == given_bidcos_address){
        // Ingnore frame sent by an other Lan-Interface
        return;
    }
    frame.SetTimestamp(RemoteTimestamp2HostTimestamp(timestamp));
    if(rssi < 1000)frame.SetRSSI(rssi);

    if( (flags & (FLAG_AUTH_REQ_MASTER | FLAG_FAIL_AUTH)) == FLAG_AUTH_REQ_MASTER )frame.SetAuthKey(auth_key);
    if( flags & FLAG_WOKENUP )frame.SetDeviceWokenup(true);
    frame.SetInterfaceId(GetSerialNumber());
    if( flags & FLAG_PRELIMINARY ){
        frame.SetPreliminary(true);
    } else {
    	pthread_mutex_lock( &mutex_bidcos_response );
	    if( (cur_tx_response_status==RESPONSE_WAIT) && (cur_tx_frame != NULL) )
	    {
            if(cur_tx_frame->CheckAndAddResponse(frame)){
                // use this frame only for RSSI based interface update, for nothing else
                frame.SetPreliminary(true);
            }
        }
        pthread_mutex_unlock( &mutex_bidcos_response );
    }
	if( flags & (FLAG_DUTY_CYCLE_90|FLAG_DUTY_CYCLE_FULL) )
	{
			LOG( Logger::LOG_WARNING, "%s: Duty cycle %s used", GetSerialNumber().c_str(), (flags & FLAG_DUTY_CYCLE_FULL)?"fully":"nearly");
	}
    GetConcentrator()->ProcessReceivedFrame(frame);
}

void BidcosRemoteInterface::HandleInputMessage(BidcosInterfaceMessage& msg)
{
    pthread_mutex_lock(&mutex_socket_response);
    if(msg.GetCommand() == socket_response_expected_command)
    {
        if(socket_response_message)delete socket_response_message;
        socket_response_message=new BidcosInterfaceMessage(msg);
        pthread_cond_signal(&cond_socket_response);
        pthread_mutex_unlock(&mutex_socket_response);
        return;
    }else{
        pthread_mutex_unlock(&mutex_socket_response);
    }
    //LOG(Logger::LOG_DEBUG, "%s RX: %s", GetSerialNumber().c_str(), msg.ToString().c_str());
	switch(msg.GetCommand())
	{
    case BidcosInterfaceMessage::CMD_RESPONSE:
    {
        BidcosFrame frame=msg.GetFrameParam(5);
		HandleCommandResponse(msg.GetUIntParam(0), msg.GetUIntParam(1), msg.GetUIntParam(2), msg.GetUIntParam(3), msg.GetSIntParam(4), frame);
        break;
    }
	case BidcosInterfaceMessage::CMD_EVENT:
    {
        BidcosFrame frame=msg.GetFrameParam(5);
		HandleCommandEvent(msg.GetUIntParam(1), msg.GetUIntParam(2), msg.GetUIntParam(3), msg.GetSIntParam(4), frame);
        break;
    }
	case BidcosInterfaceMessage::CMD_HELLO:
	    LOG(Logger::LOG_DEBUG, "%s RX: %s", GetSerialNumber().c_str(), msg.ToString().c_str());
		int paramCount = msg.GetParamCount();
        if (paramCount >= 7) {
			int firmwareVersion = msg.GetUIntParam(BidcosInterfaceMessage::PARAM_INDEX_HELLO_FWVERS);
			SetFirmwareVersion(firmwareVersion);

			if (paramCount >= 8)
			{
				int dutyCycle = msg.GetUIntParam(BidcosInterfaceMessage::PARAM_INDEX_HELLO_DUTYCYCLE);
				SetDutyCycle(dutyCycle);
			}

			UpdateRemoteTimerOffset(msg.GetUIntParam(BidcosInterfaceMessage::PARAM_INDEX_HELLO_TIMER));
            keepalive_response_expected=false;
        }
        break;
	}
}

bool BidcosRemoteInterface::RemoteClearDeviceList()
{
        BidcosInterfaceMessage clear_message;
        clear_message.SetCommand(BidcosInterfaceMessage::CMD_CLEAR_DEVICES);
        return GetConnection()->OutputMessageToInterface(clear_message);

}

bool BidcosRemoteInterface::RemoteAddAllDevices()
{
    t_device_list devices;
    ListDevices(&devices);
    for(t_device_list::iterator it=devices.begin();it!=devices.end();it++)
    {
        if(!RemoteAddDevice(*it))return false;
    }
    return true;
}

bool BidcosRemoteInterface::RemoteAddDevice(int address)
{
    int aes_key;
    uint64 aes_channels;
    if(!GetDeviceAesPolicy(address, &aes_key, &aes_channels))return false;

    unsigned int flags=0;
    if(aes_channels)flags |= DEVICE_FLAG_AES;
    if(NeedsWakeup(address))flags |= DEVICE_FLAG_WAKEUP;

    std::string aes_channel_bits;
    aes_channel_bits.reserve(8);

    while(aes_channels){
        aes_channel_bits.push_back(aes_channels&0xff);
        aes_channels>>=8;
    }

    BidcosInterfaceMessage msg;
    msg.SetCommand(BidcosInterfaceMessage::CMD_ADD_DEVICE);
    msg.PutUIntParam(0, address);
    msg.PutUIntParam(1, flags);
    msg.PutUIntParam(2, aes_key);
    msg.PutBinaryParam(3, aes_channel_bits);
    return GetConnection()->OutputMessageToInterface(msg);
}

bool BidcosRemoteInterface::RemoteSetBidcosAddress()
{
    BidcosInterfaceMessage msg;
    msg.SetCommand(BidcosInterfaceMessage::CMD_SET_ADDRESS);
    msg.PutUIntParam(0, given_bidcos_address);
    return GetConnection()->OutputMessageToInterface(msg);
}

bool BidcosRemoteInterface::RemoteSendAesKeys()
{
    bool retval=true;
    for(int i=1;i<4;i++){
        retval &= RemoteSendAESKey((AesKeyType)i);
    }
    return retval;
}

bool BidcosRemoteInterface::RemoteSetTime()
{
    const unsigned int EPOCH_CENTURY_OFFSET =
        ( 3600 // seconds per hour
        * 
        24 )  // hours per day
        * 
        ( 365 // days per year
        * 
        30    // years 1.1.1970 to 1.1.2000
        + 
        7 );   // extra days for number of leap years from 1970 to 2000

    time_t utc_now=time( NULL );
    unsigned int seconds_since_2000 = (unsigned int)(utc_now - EPOCH_CENTURY_OFFSET); // set time to elapsed seconds from 1.1.2000 00:00 until now

    TimeZoneInfo tzi(utc_now);

    int utc_offset=int(tzi.GetUTCOffset()/1800);
    time_t change_offset_seconds=0;
    time_t change_time=0;
    int change_offset_hours=0;
    if(tzi.CalcNextChange(4, &change_time, &change_offset_seconds)){
        change_offset_hours=int(change_offset_seconds/3600);
        change_time-=EPOCH_CENTURY_OFFSET;
    }

    BidcosInterfaceMessage msg;
    msg.SetCommand(BidcosInterfaceMessage::CMD_TIME);
    msg.PutUIntParam(0, seconds_since_2000);
    msg.PutSIntParam(1, utc_offset);
    msg.PutSIntParam(2, change_offset_hours);
    msg.PutUIntParam(3, (unsigned int)change_time);
    //LOG(Logger::LOG_DEBUG, "Sending time info: %s", msg.ToString().c_str());
    return GetConnection()->OutputMessageToInterface(msg);
}

bool BidcosRemoteInterface::SendKeepalive()
{
    BidcosInterfaceMessage msg;
    msg.SetCommand(BidcosInterfaceMessage::CMD_KEEPALIVE);
    return GetConnection()->OutputMessageToInterface(msg);
}

bool BidcosRemoteInterface::RemoteRemoveDevice(int address)
{
    BidcosInterfaceMessage msg;
    msg.SetCommand(BidcosInterfaceMessage::CMD_DELETE_DEVICE);
    msg.PutUIntParam(0, address);
    return GetConnection()->OutputMessageToInterface(msg);
}

uint64_t BidcosRemoteInterface::GetRemoteTimer()
{
    pthread_mutex_lock( &mutex_remote_timer_offset );
    unsigned int offset=remote_timer_offset;
    pthread_mutex_unlock( &mutex_remote_timer_offset );

    return time_millis()+offset;
}

void BidcosRemoteInterface::UpdateRemoteTimerOffset(uint64_t remote_timer)
{
    uint64_t now=time_millis();
    pthread_mutex_lock( &mutex_remote_timer_offset );
    remote_timer_offset=remote_timer-now;
    pthread_mutex_unlock( &mutex_remote_timer_offset );
}

uint64_t BidcosRemoteInterface::RemoteTimestamp2HostTimestamp(uint64_t remote_timestamp)
{
    pthread_mutex_lock( &mutex_remote_timer_offset );
    uint64_t offset=remote_timer_offset;
    pthread_mutex_unlock( &mutex_remote_timer_offset );

    return remote_timestamp-offset;
}

bool BidcosRemoteInterface::Init(std::map<std::string, std::string>& params)
{
    if(!BidcosInterface::Init(params))return false;
	debug_dump_interval = strtol(params["Debug Dump Interval"].c_str(), NULL, 0);
    return true;
}

bool BidcosRemoteInterface::DonateAddress(unsigned int* native_address, unsigned int* given_address)
{
    // this should never happen
    if(IsConnected())return false;
    bool retval=false;

    uint32_t remote_timer;
    if(GetConnection()->Connect( given_address, native_address, &remote_timer))
    {
        retval = true;
        GetConnection()->Disconnect();
    }
    return retval;
}

bool BidcosRemoteInterface::IsConnected()
{
    return GetConnection()->IsConnected();
}

bool BidcosRemoteInterface::Connect()
{
    unsigned int given_address=given_bidcos_address;
    unsigned int native_address;
    uint32_t remote_timer;
    if(!GetConnection()->Connect( &given_address, &native_address, &remote_timer))return false;

    UpdateRemoteTimerOffset(remote_timer);

    if(!RemoteClearDeviceList()){
        GetConnection()->Disconnect();
        return false;
    }

    if(!RemoteSendAesKeys()){
        GetConnection()->Disconnect();
        return false;
    }
    
    RemoteAddAllDevices();

    RemoteSetTime();

    keepalive_response_expected=false;
    return true;
}

bool BidcosRemoteInterface::RemoteRequestDump()
{
	for( int i=0;i<80;i++){
		BidcosInterfaceMessage msg;
		msg.SetCommand(BidcosInterfaceMessage::CMD_DUMP);
		msg.PutUIntParam(0, 0);
		msg.PutUIntParam(1, i);
		BidcosInterfaceMessage response;
		response.SetCommand( BidcosInterfaceMessage::CMD_DUMP_RESULT );
		if(!SendSynchronousMessage(msg, &response, 500)){
			LOG( Logger::LOG_ERROR, "No response from %s requesting debug dump (%s)", GetSerialNumber().c_str(), msg.ToString().c_str());
			return false;
		}
		LOG( Logger::LOG_INFO, "%s job[%02d]=%s", GetSerialNumber().c_str(), i,response.ToString().c_str());
	}
	{
		BidcosInterfaceMessage msg;
		msg.SetCommand(BidcosInterfaceMessage::CMD_DUMP);
		msg.PutUIntParam(0, 1);
		msg.PutUIntParam(1, 0);
		BidcosInterfaceMessage response;
		response.SetCommand( BidcosInterfaceMessage::CMD_DUMP_RESULT );
		if(!SendSynchronousMessage(msg, &response, 500)){
			LOG( Logger::LOG_ERROR, "No response from %s requesting debug dump (%s)", GetSerialNumber().c_str(), msg.ToString().c_str());
			return false;
		}
		LOG( Logger::LOG_INFO, "%s bidcos jobs=%s", GetSerialNumber().c_str(), response.ToString().c_str());
	}
	{
		BidcosInterfaceMessage msg;
		msg.SetCommand(BidcosInterfaceMessage::CMD_DUMP);
		msg.PutUIntParam(0, 2);
		msg.PutUIntParam(1, 0);
		BidcosInterfaceMessage response;
		response.SetCommand( BidcosInterfaceMessage::CMD_DUMP_RESULT );
		if(!SendSynchronousMessage(msg, &response, 500)){
			LOG( Logger::LOG_ERROR, "No response from %s requesting debug dump (%s)", GetSerialNumber().c_str(), msg.ToString().c_str());
			return false;
		}
		LOG( Logger::LOG_INFO, "%s control jobs=%s", GetSerialNumber().c_str(), response.ToString().c_str());
	}
	return true;
}

void BidcosRemoteInterface::SetFirmwareVersion(int version)
{
	char buffer[10];
	snprintf(buffer, sizeof(buffer), "%d", version);

	std::string firmwareVersion(buffer);
	BidcosInterface::SetFirmwareVersion(firmwareVersion);
}
