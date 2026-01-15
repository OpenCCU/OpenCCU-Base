/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOSCOMM_LGWPORTWRAPPER_H_
#define _BIDCOSCOMM_LGWPORTWRAPPER_H_

#include <PortWrapper.h>
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

class LGWPortWrapper : public PortWrapper
{
public:
	LGWPortWrapper();
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

	/** \brief Performs connect and does initialization.
	 * \param hostIP IP address of the gateway.
	 * \param port Port number.
	 * \param encKey Encryption key (hexadecimal md5 checksum of the password).
	 * \param desiredSerial Expected serial number of the gateway.
	 * \return True on success, otherwise false.
	 */
	bool connect(const std::string& hostIP, const unsigned int port, const std::string& encKey, const std::string& desiredSerial);

	/** \brief Disconnects from gateway.*/
	void disconnect();

	/** \brief Reconnects on connection loss.*/
	void reconnect();

	/** \brief Checks if connction is established.
	 * \return True if connection is established, otherwise false.
	 */
	bool isConnected();

	/** \brief Returns the serial number of conncted gateway.*/
	std::string getSerial();

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
	/**Encryption key*/
	std::string encKey;

	std::string serial;

	void writeLGWStatusToFile(const std::string& serial, const std::string& statusText);

};


#endif
