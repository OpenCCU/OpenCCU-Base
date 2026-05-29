/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2_LGWPORTWRAPPERMOD_H_
#define _CCU2_LGWPORTWRAPPERMOD_H_

#include <CCU2PortWrapperMod.h>
#include <cstdint>
#include <pthread.h>

//struct pthread_mutex_t_;

namespace ulc
{
	class UnifiedLanCommController;
	class LanConnection;
}

namespace ldu {
	class TCPEncryption;
}

namespace HM2Mod {

class CCU2CommControllerMod;

class LGWPortWrapperMod : public CCU2PortWrapperMod
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

	LGWPortWrapperMod(CCU2CommControllerMod* pCCU2CommController);
	~LGWPortWrapperMod();



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

	pthread_t keepAliveThread;
	bool keepAliveThreadActive;

	//Status of the info LED
	InfoLEDState infoLEDState;

	/**\brief Mutex for (re-)creation of UnifiedLanCommController*/
	pthread_mutex_t mutexULCCommController;
	pthread_mutex_t mutexReconnect;

	volatile uint64_t timestampLastSentMessage;
	
	/** \brief Thread which sends any request (currently getDutyCycle) to coprocessor.
	* \details Reason: Avoiding tcp inactivity timeouts
	*/
	pthread_t bidcosChannelKeepAliveThread;

	bool forceReconnect;

	volatile bool reconnectPending;

	static void* keepAliveThreadFunction(void* param);

	static void* bidcosChannelKeepAliveThreadFunction(void* param);

	static void* reconnectThreadFunction(void* param);

	void startKeepAliveThread();
	void asyncReconnect();

	CCU2CommControllerMod* pCCU2CommController;
	
	void writeLGWStatusToFile(const std::string& serial, const std::string& statusText);

};

} //namepsace
#endif
