/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2_LGWPORTWRAPPER_H_
#define _CCU2_LGWPORTWRAPPER_H_

#include <CCU2PortWrapper.h>
#include <pthread.h>
#include <cstdint>

//struct pthread_mutex_t_;

namespace ulc
{
	class UnifiedLanCommController;
	class LanConnection;
}

namespace ldu {
	class TCPEncryption;
}

namespace HM2 {

class CCU2LGWCommController;

class LGWPortWrapper : public CCU2PortWrapper
{
public:

	enum InfoLEDState
	{
		LED_UNDEFINED = 255,
		LED_OFF = 0,
		LED_ON = 1,
		LED_BLINK_SLOW = 2,
		LED_BLINK_FAST = 3
	};

	LGWPortWrapper(CCU2LGWCommController* pCCU2CommController);
	~LGWPortWrapper();



    //! Lesen von vorhandenen Daten. Kehrt sofort zurück, wenn keine Daten vorhanden sind
	/*!
	 *  \param data Zeiger auf den String, an den die gelesenen Zeichen angehängt werden
	 *  \return Anzahl gelesener Zeichen
	 */
	virtual int ReadData(std::string* data)/*=0*/;
	//! Senden von Daten. Sendet alle Zeichen in \c data.
	/*!
	 *  \param data Referenz auf die zu sendenden Daten
	 *  \return Anzahl gesendeter Zeichen
	 */
	virtual int SendData(const std::string& data)/*=0*/;
	//! Warten auf Daten zum Lesen
	/*!
	 *  \param msTime Zeit in ms, die maximal gewartet wird
	 *  \return 0 bei Zeitüberschreitung, >0 sonst
	 */
	virtual int WaitForData(int msTime);

	bool connect(const std::string& hostIP, const unsigned int port, const std::string& encKey, const std::string& desiredSerial);
	virtual void Disconnect();

	void reconnect();

	bool IsConnected();

	std::string getSerial();

	void setInfoLEDState(const InfoLEDState& state);

	void setHostIPAssignedByUser(bool assignedByUser = true);

private:

	/** \brief UnifiedLanCommController handles connection initialization.*/
	ulc::UnifiedLanCommController* pCommController;

	/** \brief If true, incoming and outgoing messages will be en-/decrypted.*/
	bool encryptionEnabled;

	/** \brief Pointer on TCPEncryption object from UnifiedLanCommController.*/
	ldu::TCPEncryption* pEncryption;

	/** \brief connection host ip.*/
	std::string hostIP;
	/** \brief connection port.*/
	unsigned int port;
	/*Encryption key*/
	std::string encKey;
	
	/** \brief Serial number of Gateway.
	* \details Set on first successful connect.
	*/
	std::string serial;

	/** \brief Keepalive thread.*/
	pthread_t keepAliveThread;
	/** \brief Abort boolean to stop keepalive thread.*/
	volatile bool keepAliveThreadActive;

	volatile bool bidcosChannelKeepAliveThreadActive;

	/** \brief Status of the info LED*/
	InfoLEDState infoLEDState;

	/**\brief Mutex for (re-)creation of UnifiedLanCommController*/
	pthread_mutex_t mutexULCCommController;
	pthread_mutex_t mutexReconnect;
	pthread_mutex_t mutexBlockRXTX;

	volatile uint64_t timestampLastBidcosCommunication;
	
	/** \brief Thread which sends any request (currently getDutyCycle) to coprocessor.
	* \details Reason: Avoiding tcp inactivity timeouts
	*/
	pthread_t bidcosChannelKeepAliveThread;

	/** \brief Tells if asynchronous reconnect is in progress */
	volatile bool reconnectPending;

	/** \brief Blocks communication over send/receive methods if true while reconnecting*/
	volatile bool blockRXTX;

	/** \brief Tells if an ip address was given (overridden) by the user.*/
	bool hostIPWasAssignedByUser;

	CCU2LGWCommController* pCCU2CommController;

	/** \brief Thread function for keepalive thread (port 2001) */
	static void* keepAliveThreadFunction(void* param);

	/** \brief Thread function for bidcos keepalive thread (port 2000) */
	static void* bidcosChannelKeepAliveThreadFunction(void* param);

	static void* reconnectThreadFunction(void* param);

	/** \brief Starts keep alive thread (port 2001)*/
	void startKeepAliveThread();
	/**\brief Stops keep alive thread (port 2001)*/
	void stopKeepAliveThread();

	/** Performs asynchronous reconnect. */
	void asyncReconnect();
	
	void writeLGWStatusToFile(const std::string& serial, const std::string& statusText);

	/** \brief Starts thread to send keepalive over bidcos channel (port 2000) */
	void startBidcosChannelKeepAliveThread();
	/** \brief Stops thread to send keepalive over bidcos channel (port 2000) */
	void stopBidcosChannelKeepAliveThread();

};

} //namepsace
#endif
