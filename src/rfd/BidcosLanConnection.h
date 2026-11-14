/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOSLANCONNECTION_H_
#define _BIDCOSLANCONNECTION_H_

#include "BidcosInterfaceConnection.h"

#include <map>
#include <string>
#include <vector>
#include <set>
#include "BidcosRemoteInterface.h"

//forward declaration
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;


//! Implementierung von BidcosInterfaceConnection für die Kommunikation mit Lan-Interfaces
class BidcosLanConnection :
    public BidcosInterfaceConnection
{
public:
    //! Konstruktor
    BidcosLanConnection(void);
    //! Destruktor
    virtual ~BidcosLanConnection(void);
    virtual bool Init(std::map<std::string, std::string>& params);

    bool Connect(unsigned int* given_bidcos_address, unsigned int* native_bidcos_address, uint32_t* remote_timer);
    bool Disconnect();

    bool IsConnected();

    bool OutputMessageToInterface(BidcosInterfaceMessage& msg);

	bool RetrieveNextInputMessage(int timeout, BidcosInterfaceMessage* msg);

    bool HasError();
private:

    //! Füllt den Eingabepuffer und wartet dabei maximal \c msTime ms auf Daten
	bool FillInputBuffer(int msTime);

    //! Wartet maximal \c msTime ms darauf, dass lesbare Daten vorhanden sind
	bool WaitForInput(int msTime);

    //! Liefert die nächste im Eingabepuffer bereits vorhandene Nachricht zurück
    bool GetMessageFromInputBuffer(BidcosInterfaceMessage* msg);

    //! Aufzählung für die UDP-Kommunikation mit den Lan-Interface zum Ermitteln der IP-Adresse
    enum
    {
        UDP_CMD_GET_NETADDRESS	 = 'N',
        UDP_CMD_RESPONSE         = '>'
    };

    //! Seriennummer des Lan-Interfaces
  	std::string serial_number;

    //! Ermittelt per UDP-Broadcasts anhand der Seriennummer die IP-Adresse des Lan-Interfaces
    std::string DetermineIP();
    //! Socketdescriptor für die Kommunikation mit dem Lan-Interface
	int sock_fd;

    //! Eingabepuffer. Enthält die vom Lan-Interface empfangenen Rohdaten im Klartext.
	std::string input_buffer;

    //! IP-Adresse des Lan-Interfaces, falls diese konfiguriert ist.
	std::string remote_host;
    //! TCP-Port des Lan-Interfaces.
	int remote_port;

    //! Mutex für Zugriff auf sock_fd
	pthread_mutex_t mutex_socket;
    //! Mutex für Zugriff auf remote_host
	pthread_mutex_t mutex_remote_address;
    //! Mutex für Senden aus OutputMessageToSocket heraus
	pthread_mutex_t mutex_send;

    //! AES-Schlüssel für die Netzwerkkommunikation, falls konfiguriert
    std::string lan_encryption_key;

    //! OpenSSL Kontext für Verschlüsselung
    EVP_CIPHER_CTX* encrypt_ctx;
    EVP_CIPHER_CTX* decrypt_ctx;
    //! Gibt an, ob ein Verbindungsfehler aufgetreten ist
    bool connection_error;
    //! Gibt an, ob die Kommunikation aktuell verschlüsselt ist
    bool crypt;
};

#endif
