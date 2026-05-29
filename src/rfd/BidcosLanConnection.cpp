/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "MulticastSender.h"
#include <Logger.h>
#include <utils.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "BidcosLanConnection.h"

#include <SocketSendFlag.h>

#ifdef WIN32
#define close(x) closesocket(x)
#endif

#if 0
#define DUMP_MESSAGE(prefix, s) \
    std::string msg_dump;\
    for(unsigned int i=0;i<s.size();i++){\
        char c=s[i];\
        if(c>=' ' && c<=0x7e && c!='\\'){\
            msg_dump.append(1, c);\
        }else{\
            char buf[8];\
            snprintf(buf, sizeof(buf), "\\x%02x", int(c)&0xff);\
            msg_dump+=buf;\
        }\
    }\
    LOG(Logger::LOG_DEBUG, prefix "%s", msg_dump.c_str());
#else
#define DUMP_MESSAGE(prefix, s)
#endif


BidcosLanConnection::BidcosLanConnection(void)
{
	sock_fd=-1;
	remote_port=1000;
	pthread_mutex_init(&mutex_socket, NULL);
	pthread_mutex_init(&mutex_remote_address, NULL);
	pthread_mutex_init(&mutex_send, NULL);
    encrypt_ctx=NULL;
    decrypt_ctx=NULL;
    connection_error = false;
    crypt = false;

#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD( 2, 2 );
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		LOG(Logger::LOG_ERROR, "Could not initialize winsock2 library");
	}
#endif
}

BidcosLanConnection::~BidcosLanConnection(void)
{
    pthread_mutex_destroy(&mutex_socket);
    pthread_mutex_destroy(&mutex_remote_address);
    pthread_mutex_destroy(&mutex_send);
    if(encrypt_ctx){
        EVP_CIPHER_CTX_free(encrypt_ctx);
        encrypt_ctx=NULL;
    }
    if(decrypt_ctx){
        EVP_CIPHER_CTX_free(decrypt_ctx);
        decrypt_ctx=NULL;
    }
}

bool BidcosLanConnection::OutputMessageToInterface(BidcosInterfaceMessage& msg)
{
    std::string s=msg.ToString();
    DUMP_MESSAGE("<", s);
    if(crypt && encrypt_ctx){
        unsigned int size=s.size();
        char* data=new char[size];
        EVP_CipherUpdate(encrypt_ctx, (unsigned char*)data, (int*)&size, (const unsigned char*)s.c_str(), size);
        s.assign(data, size);
        delete[] data;
    }
	unsigned int count=0;
	int retval;
    pthread_mutex_lock(&mutex_send);
	while(count<s.size()){
		retval=send(sock_fd, s.data()+count, s.size()-count, SOCKET_SEND_FLAGS);
		if(retval<0){
            pthread_mutex_unlock(&mutex_send);
			Disconnect();
			return false;
		}
		count+=retval;
	}
    pthread_mutex_unlock(&mutex_send);
	return true;
}

bool BidcosLanConnection::WaitForInput(int msTime)
{
	fd_set inFd, outFd, excFd;
	FD_ZERO(&inFd);
	FD_ZERO(&outFd);
	FD_ZERO(&excFd);

	FD_SET(sock_fd, &inFd);
	FD_SET(sock_fd, &excFd);
	
	// Check for events
	int nEvents;
	struct timeval tv;
	tv.tv_sec = msTime/1000;
	tv.tv_usec = (msTime%1000)*1000;
	nEvents = select(sock_fd+1, &inFd, &outFd, &excFd, &tv);
//	LOG(Logger::LOG_DEBUG, "t=%d, nEvents=%d, msTime=%d", time_millis(), nEvents, msTime);
	if(nEvents == -1){
		LOG(Logger::LOG_ERROR, "select error");
        connection_error = true;
        return false;
	}
    if(FD_ISSET(sock_fd, &excFd)){
        connection_error = true;
        return false;
    }
	return nEvents>0;
}

bool BidcosLanConnection::FillInputBuffer(int msTime)
{
    if( !WaitForInput(msTime) )return false;
    char buf[256];
    int count;
    count=recv(sock_fd, buf, 256, 0);
//	LOG(Logger::LOG_DEBUG, "count=%d", count);
    if(count<=0){
	    if(IsConnected())Disconnect();
        connection_error = true;
        return false;
    }else{ // count > 0
        if(crypt && decrypt_ctx) {
            EVP_CipherUpdate(decrypt_ctx, (unsigned char*)buf, (int*)&count, (unsigned char*)buf, count);
        }
        input_buffer.insert(input_buffer.end(), buf, buf+count);

        DUMP_MESSAGE( ">", std::string(buf, count) );
        return true;
    }
}

std::string BidcosLanConnection::DetermineIP()
{
    std::string msg="eQ3LanIf";
    msg+=serial_number;
    msg.append(1, UDP_CMD_GET_NETADDRESS);
    std::vector<std::string> responses;
    MulticastSender mcast_sender("224.0.0.1", 23272);
    if(!mcast_sender.SendUDPMessage(msg, &responses, 1000, 1))return "";
    if(responses.empty())return "";
    if(responses[0].size()<24)return "";
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%u.%u.%u.%u",
        (unsigned int)(unsigned char)responses[0][20], 
        (unsigned int)(unsigned char)responses[0][21], 
        (unsigned int)(unsigned char)responses[0][22], 
        (unsigned int)(unsigned char)responses[0][23]);
    return buffer;
}

bool BidcosLanConnection::Connect(unsigned int* given_bidcos_address, unsigned int* native_bidcos_address, uint32_t* remote_timer)
{
	if( IsConnected() )return true;

    connection_error = false;
    crypt = false;

    std::string host;
	pthread_mutex_lock(& mutex_remote_address);
    if( remote_host.empty()){
        host=DetermineIP();
    }else{
        host=remote_host;
    }
   	pthread_mutex_unlock(& mutex_remote_address);
    if(host.empty()){
        LOG( Logger::LOG_ERROR, "%s: Could not determine IP", serial_number.c_str());
        return false;
    }

    struct sockaddr_in addr;

	pthread_mutex_lock(& mutex_socket);
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd==-1){
        LOG( Logger::LOG_ERROR, "%s: Could not open socket", serial_number.c_str());
        connection_error = true;
    }

    if( !connection_error ){
	    addr.sin_family = AF_INET;
        addr.sin_port = htons(remote_port);
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        memset(&(addr.sin_zero), 0, sizeof(addr.sin_zero));

        // Clear the input buffer
        input_buffer.clear();

        if(connect(sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr))==-1){
            LOG( Logger::LOG_ERROR, "%s: Could not connect to host", serial_number.c_str());
            connection_error = true;
        }
    }

    if( (!connection_error) && encrypt_ctx && decrypt_ctx && !lan_encryption_key.empty() ){
        BidcosInterfaceMessage input_iv_message;
	    if(!RetrieveNextInputMessage(2000, &input_iv_message)){
            LOG( Logger::LOG_ERROR, "%s: Timeout waiting for initialization vector message", serial_number.c_str());
		    connection_error = true;
	    }
        if( !connection_error ){
            if( input_iv_message.GetCommand() == BidcosInterfaceMessage::CMD_SET_IV){
                EVP_EncryptInit_ex(encrypt_ctx, EVP_aes_128_cfb(), NULL, (unsigned char*)lan_encryption_key.c_str(), input_iv_message.GetBinaryParam(0).c_str());
            }else{
                if( input_iv_message.GetCommand() == BidcosInterfaceMessage::CMD_HELLO ){
                    LOG( Logger::LOG_ERROR, "Lan Interface %s is not configured for encryption while we are", serial_number.c_str() );
                }else{
                    LOG( Logger::LOG_ERROR, "%s: Invalid initialization vector message received: %s", serial_number.c_str(), input_iv_message.ToString().c_str());
                }
		        connection_error = true;
            }
        }

        if( !connection_error ){
            BidcosInterfaceMessage output_iv_msg;
            output_iv_msg.SetCommand(BidcosInterfaceMessage::CMD_SET_IV);

            //generate a unique IV using the current time in seconds as well as milliseconds resolution
            unsigned char iv[16];
            RAND_bytes(iv, 16);
            EVP_DecryptInit_ex(decrypt_ctx, EVP_aes_128_cfb(), NULL, (unsigned char*)lan_encryption_key.c_str(), iv);

            output_iv_msg.PutBinaryParam(0, BidcosInterfaceMessage::BinaryType(iv, 16));
            if(OutputMessageToInterface(output_iv_msg)){
                crypt = true;
            }else{
                LOG( Logger::LOG_ERROR, "%s: Error sending initialization vector message", serial_number.c_str());
		        connection_error = true;
            }
        }
    }

    BidcosInterfaceMessage input_hello_msg;
    if( (!connection_error) && !RetrieveNextInputMessage(2000, &input_hello_msg)){
        LOG( Logger::LOG_ERROR, "%s: Timeout waiting for hello message", serial_number.c_str());
        connection_error = true;
    }

	if( (!connection_error) && (input_hello_msg.GetCommand() == BidcosInterfaceMessage::CMD_HELLO) ){
    	*native_bidcos_address=input_hello_msg.GetUIntParam(BidcosInterfaceMessage::PARAM_INDEX_HELLO_NATIVE_ADDRESS);
        *remote_timer=input_hello_msg.GetUIntParam(BidcosInterfaceMessage::PARAM_INDEX_HELLO_TIMER);
    }else{
        if( input_hello_msg.GetCommand() == BidcosInterfaceMessage::CMD_SET_IV ){
            LOG( Logger::LOG_ERROR, "Lan Interface %s is configured for encryption while we are not", serial_number.c_str() );
        }else{
            LOG( Logger::LOG_WARNING, "%s: Invalid hello message received: %s", serial_number.c_str(), input_hello_msg.ToString().c_str());
        }
        connection_error = true;
	}

    if( !connection_error ){
        //read all buffered events from the Lan Interface
        BidcosInterfaceMessage dummy_input_message;
        while(RetrieveNextInputMessage(2000, &dummy_input_message));

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
   	pthread_mutex_unlock(& mutex_socket);

    if(!connection_error){
        LOG(Logger::LOG_INFO, "Connected to Lan Interface %s at %s", serial_number.c_str(), host.c_str());
		ReportConnectionStatus(serial_number, true);
        return true;
    }else{
        Disconnect();
        return false;
    }
}

bool BidcosLanConnection::Disconnect()
{
	struct timespec abs_timeout=millis2abstime(500);
    if( !pthread_mutex_timedlock(&mutex_socket, &abs_timeout) ){
        bool was_connected = (sock_fd>=0);
        if(sock_fd!=-1)close(sock_fd);
        sock_fd=-1;
	    pthread_mutex_unlock(& mutex_socket);
        if(was_connected){
            LOG(Logger::LOG_INFO, "Disconnected from Lan Interface %s", serial_number.c_str());
			ReportConnectionStatus(serial_number, false);
        }
        return true;
    }
	return false;
}

bool BidcosLanConnection::IsConnected()
{
	bool connected=false;
	struct timespec abs_timeout=millis2abstime(100);
    if( !pthread_mutex_timedlock(&mutex_socket, &abs_timeout) ){
        connected = (sock_fd>=0);
        pthread_mutex_unlock(&mutex_socket);
    }
	return connected;
}

bool BidcosLanConnection::HasError()
{
    return connection_error;
}

bool BidcosLanConnection::GetMessageFromInputBuffer(BidcosInterfaceMessage* msg)
{
	std::string::size_type start_pos=input_buffer.find_first_not_of("\r\n");
    if(start_pos==std::string::npos){
//        LOG(Logger::LOG_DEBUG, "Buffer empty");
        return false;
    }
	std::string::size_type end_pos=input_buffer.find_first_of("\r\n", start_pos);
    if( (end_pos==std::string::npos) || (start_pos==end_pos) ){
//        LOG(Logger::LOG_DEBUG, "Not complete");
        return false;
    }
	msg->FromString(input_buffer.substr(start_pos, end_pos-start_pos));
	end_pos=input_buffer.find_first_not_of("\r\n", end_pos);
    if(end_pos == std::string::npos)input_buffer.clear();
	else input_buffer=input_buffer.substr(end_pos);
//    LOG(Logger::LOG_DEBUG, "Next Input Message=%s", msg->ToString().c_str());
	return true;
}

bool BidcosLanConnection::RetrieveNextInputMessage(int timeout, BidcosInterfaceMessage* msg)
{
    while(true){
        if(GetMessageFromInputBuffer(msg))return true;
        if(!FillInputBuffer(timeout))return false;
    }
}

bool BidcosLanConnection::Init(std::map<std::string, std::string>& params)
{
    serial_number=params["Serial Number"];
    if(serial_number.empty())return false;

	pthread_mutex_lock(& mutex_socket);
    if(!params["IP Address"].empty())
    {
        remote_host=params["IP Address"];
    }
    if(params["Port"].size())remote_port=strtol(params["Port"].c_str(), NULL, 0);

	pthread_mutex_unlock(& mutex_socket);
    if(params["Encryption Key"].size()){
        StructuredFrame sf;
        sf.FromString(params["Encryption Key"]);
        sf.GetStringValue(0, 16, &lan_encryption_key);
        if(encrypt_ctx != NULL){
            EVP_CIPHER_CTX_free(encrypt_ctx);
            encrypt_ctx=NULL;
        }
        if(decrypt_ctx != NULL){
            EVP_CIPHER_CTX_free(decrypt_ctx);
            decrypt_ctx=NULL;
        }
        encrypt_ctx = EVP_CIPHER_CTX_new();
        decrypt_ctx = EVP_CIPHER_CTX_new();
        if( (encrypt_ctx == NULL) || (decrypt_ctx == NULL) ){
            LOG( Logger::LOG_ERROR, "%s: Could not create OpenSSL EVP_CIPHER_CTX structures", serial_number.c_str() );
            return false;
        }
    }else{
        lan_encryption_key.clear();
    }
    return true;
}

