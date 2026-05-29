/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _UNIFIEDLANCOMMCONTROLLER_H_
#define _UNIFIEDLANCOMMCONTROLLER_H_

#include <DLLImportExportULC.h>
#include <LanConnection.h>
#include <string>
#include <LanConnectionListener.h>

#include <TCPEncryption.h>
#include <pthread.h>


namespace ulc {

class UnifiedLanProtocolMessage;

LIBUNIFIEDLANCOMM_API class UnifiedLanCommController
{
public:

	enum ConnectError {
		NOT_CONNECTED = 0,
		NO_CONNECT_ERROR,
		CONNECT_FAILED,
		WRONG_KEY,
		UNKNOWN_INITIALIZATION_ERROR
	};

	UnifiedLanCommController(const std::string& hostIP, const int port);
	virtual ~UnifiedLanCommController();
	void setEncryptionKey(const std::string key);
	void setDesiredSerial(const std::string desiredSerial);

	/** \brief Returns value of encryptionEnabled.
	 * \return True, if encryption is enabled, otherwise False.*/
	bool isEncryptionEnabled() const;

	/** \brief Establishes connection.
	 * \return LanConnection or NULL on error.*/
	bool connect();
	void disconnect();
	bool isConnected();

	bool send(const std::string& data);
	bool receive(std::string& data);

	/** \brief Returns pointer on TCPEncryption.
	 * \details Important: Ownership of this pointer stays in UnifiedLanCommController.*/
	ldu::TCPEncryption* getTCPEncryption();

	/** \brief Returns the serial number of the gateway.*/
	std::string getSerial();

	bool receiveNonBlocking(std::string& data);

	ConnectError getConnectError() const;
	std::string getConnectErrorAsString() const;

private:

	enum ConnectionState {
		CONN_STATE_NOT_CONNECTED,//!No connection
		CONN_STATE_CONNECTED,//!Connection established, waiting for Hello
		CONN_STATE_RECEIVED_HELLO,//!Received Hello;
		CONN_STATE_RECEIVED_IV,//!IV received
		CONN_STATE_RECEIVED_SWITCHING_PROTOCOL,//!Waiting until protocol switched
		CONN_STATE_READY//!Connection initialized and ready to be used.
	};

	volatile ConnectionState connectionState;
	ConnectError connectError;

	LanConnection connection;

	ldu::TCPEncryption encryption;

	bool encryptionEnabled;

	std::string encKey;

	std::string desiredSerial;
	std::string lgwSerial;

	/** \brief Receive thread.*/
	pthread_t receiveThread; //pthread_t
	volatile bool receiveThreadActive;

	/**\brief Used while establishing a new connection.*/
	pthread_mutex_t pRxTxMutex;

	/** \brief Mutex to sync connection as well as r/w ops on it.*/
	pthread_mutex_t connectionMutex;

	/** \brief Starts receive thread.*/
	void startReceiveThread();

	/** \brief Stops receive thread.*/
	void stopReceiveThread();

	static void* receiveThreadFunction(void* params);



	void handleIncomingMessage(const UnifiedLanProtocolMessage& msg);
	void handleHelloMessage(const UnifiedLanProtocolMessage& msg);
	void handleSetInitVectorMessage(const UnifiedLanProtocolMessage& msg);
	void handleSwitchProtocolMessage(const UnifiedLanProtocolMessage& msg);

	/** \brief Waits for reaching given connection state.
	 * \details Polling with a sleep. Sleeptime between polls is timoutMillis / 10
	 * \param connState ConnectionState to wait for.
	 * \param timoutMillis Timeout in milliseconds.
	 * \return True if state was reached, false when timeout caused about.
	 */
	bool waitForState(const ConnectionState& connState, const unsigned int timeoutMillis);

	std::string createIV();
	
	/** \brief Disconnect method for class internal use.*/
	void disconnect(const bool setMemberConnectError);

};

}
#endif
