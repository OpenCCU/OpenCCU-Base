/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LANCONNECTION_H_
#define _LANCONNECTION_H_

#include <string>
#include <vector>
#include <pthread.h>

namespace ulc {

//Forward declaration
class LanConnectionListener;

/** \brief Instance of this class represents a TCP/IP connection to a remote host determined by host IP and port.
* \details Grants public access to send() and receive() functions, to send and reveive data.
* In addition class UnifiedLanCommController has access to connect()/disconnect() methods.
*/
class LanConnection {

	friend class UnifiedLanCommController;// This was planned. I know it's not nice. :-)

public:
	LanConnection(const std::string& hostIP, const int port);
	virtual ~LanConnection();

	/** \brief Sends data to remote host.
	 * \param data Data to send.
	 * \return True on success, otherwise false.*/
	bool send(const std::string& data);

	/** \brief Receives data from remote host.
	 * \param readBytes Bytes received from remote host.
	 * \return False on error, otherwise true.
	 */
	bool receive(std::string& bytesRead);

	/** \brief Return connection state.
 	 * \return True if connection is established, otherwise false.*/
	bool isConnected() const;

	bool connectionError() const;

	int waitForData(unsigned int msTimeout);

	void addLanConnectionListener(LanConnectionListener* listener);
	void removeLanConnectionListener(LanConnectionListener* listener);

	bool receiveNonBlocking(std::string& bytesRead);

protected:
	/** Socket (file descriptor/handle).*/
	int sock;

	/** \brief Connects to remote host.*/
	bool connect();//It's protected to ensure that only UnifiedCommController has access.
	/** \brief Disconnects from remote host.*/
	void disconnect();//It's protected to ensure that only UnifiedCommController has access.
private:
	/** \brief IP-Address of remote host.*/
	std::string hostIP;
	/** \brief Remote host port.*/
	int port;
	/** \brief Receive buffer.*/
	char receiveBuffer[4096];

	bool connectionClosing;
	bool inSelect;

	/** \brief List of LanConnectionListener*/
	std::vector<LanConnectionListener*> lanConnectionListener;

	void perror(const char* func) const;

	void notifyOnDisconnect();

	pthread_mutex_t sockMutex;
};

}
#endif
