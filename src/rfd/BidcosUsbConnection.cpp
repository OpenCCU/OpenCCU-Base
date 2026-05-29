/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <Logger.h>
#include <utils.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "BidcosUsbConnection.h"

BidcosUsbConnection::BidcosUsbConnection(void)
{
	pthread_mutex_init(&mutex_connection, NULL);
	pthread_mutex_init(&mutex_send, NULL);
    dev_index=0;
    connection_error=false;
}

BidcosUsbConnection::~BidcosUsbConnection(void)
{
    pthread_mutex_destroy(&mutex_connection);
    pthread_mutex_destroy(&mutex_send);
}

bool BidcosUsbConnection::OutputMessageToInterface(BidcosInterfaceMessage& msg)
{
    BidcosInterfaceMessage::BinaryType b=msg.ToBinary();
    pthread_mutex_lock(&mutex_send);
    memset(output_buffer, 0, sizeof(output_buffer));
    memcpy(output_buffer+1, b.data(), b.size()<sizeof(output_buffer)?b.size():sizeof(output_buffer)-1);
    //LOG(Logger::LOG_DEBUG, "output_buffer=%s", StructuredFrame(std::string((char*)output_buffer, sizeof(output_buffer))).ToString().c_str());
    bool success=hiddev.SetReport_Interrupt(output_buffer, sizeof(output_buffer), SET_REPORT_TIMEOUT) == HID_DEVICE_SUCCESS;
    pthread_mutex_unlock(&mutex_send);
	return success;
}

bool BidcosUsbConnection::FillInputBuffer(int msTime)
{
    DWORD bytesReturned;
    BYTE get_report_status=hiddev.GetReport_Interrupt(temp_input_buffer, sizeof(temp_input_buffer), 1, &bytesReturned, msTime);
    
    if( get_report_status != HID_DEVICE_SUCCESS ){
        if( get_report_status != HID_DEVICE_TRANSFER_TIMEOUT ){
            connection_error = true;
        }
        return false;
    }
    if(!bytesReturned)return false;
    //LOG(Logger::LOG_DEBUG, "input_buffer=%s", StructuredFrame(std::string((char*)temp_input_buffer, sizeof(temp_input_buffer))).ToString().c_str());
    BidcosInterfaceMessage msg;
    if(msg.FromBinary(BidcosInterfaceMessage::BinaryType(temp_input_buffer+1, bytesReturned-1))){
	   //LOG(Logger::LOG_DEBUG, "input message[%d]=%s", queue_input_messages.size(), msg.ToString().c_str());
        queue_input_messages.push(msg);
    }
    return true;
}

bool BidcosUsbConnection::Connect(unsigned int* given_bidcos_address, unsigned int* native_bidcos_address, uint32_t* remote_timer)
{
	pthread_mutex_lock(& mutex_connection);

    BYTE open_status=hiddev.Open( dev_index, VENDOR_ID, PRODUCT_ID);
    if((open_status != HID_DEVICE_ALREADY_OPENED) && (open_status != HID_DEVICE_SUCCESS)){
       	pthread_mutex_unlock(&mutex_connection);
        Disconnect();
        connection_error = true;
        return false;
    }

    connection_error = false;

    BidcosInterfaceMessage keepalive_msg;
    keepalive_msg.SetCommand(BidcosInterfaceMessage::CMD_KEEPALIVE);
    keepalive_msg.PutUIntParam(0, *given_bidcos_address);
    if(!connection_error && !OutputMessageToInterface(keepalive_msg)){
        LOG( Logger::LOG_ERROR, "%s: Error requesting hello message", serial_number.c_str());
        connection_error = true;
    }

	time_t now = time(NULL);
    BidcosInterfaceMessage input_hello_msg;
	while( !connection_error )
	{
		if( time(NULL) > (now + 3) )
		{
			connection_error = true;
		}
		if( (!connection_error) && !RetrieveNextInputMessage(2000, &input_hello_msg)){
			LOG( Logger::LOG_ERROR, "%s: Timeout waiting for hello message", serial_number.c_str());
			connection_error = true;
		}

		if( (!connection_error) && (input_hello_msg.GetCommand() == BidcosInterfaceMessage::CMD_HELLO) ){
    		*native_bidcos_address=input_hello_msg.GetUIntParam(BidcosInterfaceMessage::PARAM_INDEX_HELLO_NATIVE_ADDRESS);
			*remote_timer=input_hello_msg.GetUIntParam(BidcosInterfaceMessage::PARAM_INDEX_HELLO_TIMER);
			break;
		}else{
			LOG( Logger::LOG_WARNING, "%s: Invalid hello message received: %s", serial_number.c_str(), input_hello_msg.ToString().c_str());
		}
	}

    if( !connection_error ){
        //read all buffered events from the USB Interface
        BidcosInterfaceMessage dummy_input_message;
        while(RetrieveNextInputMessage(100, &dummy_input_message));

        if(*given_bidcos_address){
            BidcosInterfaceMessage output_address_msg;
            output_address_msg.SetCommand(BidcosInterfaceMessage::CMD_SET_ADDRESS);
            output_address_msg.PutUIntParam(0, *given_bidcos_address);
            if(!OutputMessageToInterface(output_address_msg)){
                LOG( Logger::LOG_ERROR, "%s: Error setting remote bidcos address", serial_number.c_str());
                connection_error = true;
            }
        }else{
            *given_bidcos_address=input_hello_msg.GetUIntParam(BidcosInterfaceMessage::PARAM_INDEX_HELLO_GIVEN_ADDRESS);
        }
    }
   	pthread_mutex_unlock(&mutex_connection);

    if(!connection_error){
        LOG(Logger::LOG_INFO, "Connected to USB Interface %s", serial_number.c_str());
		ReportConnectionStatus(serial_number, true);
        return true;
    }else{
        Disconnect();
        return false;
    }
}

bool BidcosUsbConnection::Disconnect()
{
    if( IsConnected() ){
        LOG(Logger::LOG_DEBUG, "Disconnected from USB interface %s", serial_number.c_str());
		ReportConnectionStatus(serial_number, false);
    }
	struct timespec abs_timeout=millis2abstime(500);
    if( !pthread_mutex_timedlock(&mutex_connection, &abs_timeout) ){
        hiddev.Close();
	    pthread_mutex_unlock(& mutex_connection);
        return true;
    }
	return false;
}

bool BidcosUsbConnection::IsConnected()
{
	bool connected=false;
	struct timespec abs_timeout=millis2abstime(100);
    if( !pthread_mutex_timedlock(&mutex_connection, &abs_timeout) ){
        connected = hiddev.IsOpened()!=0;
        pthread_mutex_unlock(&mutex_connection);
    }
	return connected;
}

bool BidcosUsbConnection::HasError()
{
    return connection_error;
}

bool BidcosUsbConnection::RetrieveNextInputMessage(int timeout, BidcosInterfaceMessage* msg)
{
    while(true){
        if(!queue_input_messages.empty()){
            *msg = queue_input_messages.front();
            queue_input_messages.pop();
            return true;
        }
        if(!FillInputBuffer( timeout ))return false;
    }
}

bool BidcosUsbConnection::Init(std::map<std::string, std::string>& params)
{
    std::string& sernum=params["Serial Number"];

    DWORD count=hiddev.GetConnectedDeviceNum(VENDOR_ID, PRODUCT_ID);
    for(unsigned int i=0;i<count;i++){
        dev_index=i;
        if(sernum.empty()){
            if(CheckAvailable()){
                sernum=GetSerialNumber();
                serial_number=sernum;
                return true;
            }else if(count==1){
                LOG(Logger::LOG_ERROR, "USB-Interface %s is in use by an other application", GetSerialNumber().c_str());
                return false;
            }
        }else if(sernum==GetSerialNumber()){
            if(!CheckAvailable()){
                LOG(Logger::LOG_ERROR, "USB-Interface %s is in use by an other application", sernum.c_str());
                return false;
            }
            serial_number=sernum;
            return true;
        }
    }
    if(!count)LOG(Logger::LOG_ERROR, "No USB interface found");
    else LOG(Logger::LOG_ERROR, "All USB-Interfaces are in use by other applications");
    return false;
}

std::string BidcosUsbConnection::GetSerialNumber()
{
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    if(hiddev.GetSerialString( dev_index, VENDOR_ID, PRODUCT_ID, buffer, sizeof(buffer) ) != HID_DEVICE_SUCCESS)return "";
    return buffer;
}

bool BidcosUsbConnection::CheckAvailable()
{
    BYTE result=hiddev.Open( dev_index, VENDOR_ID, PRODUCT_ID);
    if((result != HID_DEVICE_ALREADY_OPENED) && (result != HID_DEVICE_SUCCESS))return false;
    if(result == HID_DEVICE_SUCCESS){
        hiddev.Close();
    }
    return true;
}
