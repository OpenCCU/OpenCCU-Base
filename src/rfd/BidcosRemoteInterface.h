/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOS_REMOTE_INTERFACE_H_
#define _BIDCOS_REMOTE_INTERFACE_H_

#include <map>
#include <string>
#include <vector>
#include <set>
#include "BidcosFrame.h"
#include "BidcosInterface.h"
#include "BidcosInterfaceMessage.h"

class BidcosInterfaceConnection;

//! Generische Interfaceklasse für das Homematic-Lan-Interface und das Homematic-USB-Interface

class BidcosRemoteInterface: public BidcosInterface
{
public:
	//! Konstanten für die AES-Schlüsseltypen
	enum AesKeyType{
		AES_KEY_TYPE_DEFAULT=0, //!< Standardschlüssel ("ELV-Schlüssel")
		AES_KEY_TYPE_CURRENT_USER, //!< Aktiver Benutzerschlüssel
		AES_KEY_TYPE_PREVIOUS_USER, //!< Vorheriger Benutzerschlüssel
		AES_KEY_TYPE_TEMP, //!< Temporärschlüssel
	};

    //! Flags für Response- und Event-Nachrichten
    enum
    {
        FLAG_OK_RESPONSE = 1,//!< Nachricht wurde gesendet und eine Antwort empfangen
        FLAG_OK_SENT = 2,//!< Nachricht wurde gesendet, es wurde keine Antwort erwartet
        FLAG_FAIL_TIMEOUT = 4,//!< Nachricht konnte nicht rechtzeitig gesendet werden
        FLAG_FAIL_NO_RESPONSE = 8,//!< Nachricht wurde gesendet, es wurde aber keine Antwort empfangen
        FLAG_FAIL_AUTH = 16,//!< Authentifizierungsfehler
        FLAG_AUTH_REQ_SLAVE = 32,//!< Das angesprochene Gerät hat eine Authentifizierung vom Interface verlangt
        FLAG_AUTH_REQ_MASTER = 64,//!< Das Interface hat eine Authentifizierung vom Gerät verlangt
        FLAG_WOKENUP = 128,//!< Das Gerät wurde aufgeweckt
        FLAG_PRELIMINARY = 256,//!< Der empfangene Rahmen ist Teil einer längeren Kommunikation
        FLAG_DUTY_CYCLE_90 = 512,//!< Der Duty-Cycle des entsprechenden Lan-Interfaces ist zu 90% ausgeschöpft
        FLAG_DUTY_CYCLE_FULL = 1024//!< Der Duty-Cycle des entsprechenden Lan-Interfaces ist komplett ausgeschöpft
    };

	//! Konstruktor
	BidcosRemoteInterface();
	//! Destruktor
	virtual ~BidcosRemoteInterface();

    bool SendFrame(BidcosFrame* frame);
   	bool AddDevice(int address);
	bool RemoveDevice(int address);
	bool SetDeviceAesPolicy(int address, int aes_key, uint64 aes_channels);
	bool AddDeviceWakeupRequest(int address,bool lazyConfig = false);
	bool RemoveDeviceWakeupRequest(int address);

    bool SetAesKeyTemp(int index, const std::string& data);
	bool SetAesKeyUser(int index, const std::string& data, int last_index, const std::string& last_data);

    virtual bool Init(std::map<std::string, std::string>& params);

    //! Liefert die Bidcos-Adresse des Interfaces zurück
    int GetAddress();

    bool IsConnected();

    bool DonateAddress(unsigned int* native_address, unsigned int* given_address);

	bool StartInterface(int bidcos_address);
	bool StopInterface();
protected:
    //! Muss von abgeleiteten Klassen überschrieben werden, um ein spezialisiertes Verbindungsobjekt zurückzuliefern
    virtual BidcosInterfaceConnection* GetConnection()=0;
private:

    //! Flags für den Send-Befehl
    enum
    {
        SEND_FLAG_RESPONSE = 1,//!< Es wird eine Antwort erwartet
    };

    //! Flags für den AddDevice-Befehl
    enum
    {
        DEVICE_FLAG_AES = 1,//!< Kommunikation mit dem Gerät wird AES-authentifiziert
        DEVICE_FLAG_WAKEUP = 2//!< Gerät soll aufgeweckt werden
    };

    //! Schedule-Werte für den Send-Befehl
    enum
    {
        SCHEDULE_ASAP = 0 //!< So schnell wie möglich senden
    };

    //! Enum für den Status beim Warten auf eine Antwort
	enum ResponseStatus
	{
		RESPONSE_WAIT,//! Antwort wird erwartet
		RESPONSE_OK,//! Antwort wurde empfangen
		RESPONSE_FAIL,//! Antwort wurde nicht empfangen
		RESPONSE_IDLE//! Es wird derzeit keine Antwort erwartet
	};

    //! Zeitintervalle
    enum
    {
        KEEPALIVE_INTERVAL=10000, //!< Alle 10 Sekunden eine Keepalive-Nachricht senden
        TIMEINFO_INTERVAL=900000 //!< Alle 15 Minuten eine Zeit-Nachricht senden
    };

    //! Verbindung herstellen
    bool Connect();

    //! Eine Keepalive-Nachricht senden
    bool SendKeepalive();

    //! Eine Nachricht senden und auf die Antwort warten
    /*!
     *  \param msg Die zu sendende Nachricht
     *  \param response Zeiger zum Speichern der Antwort
     *         Vor dem Aufruf dieser Methode muss mit BidcosInterfaceMessage::SetCommand() der Opcode der
     *         erwarteten Antwort gesetzt werden.
     *  \param timeout Zeit in ms für das Warten auf die Antwort
     */
    bool SendSynchronousMessage(BidcosInterfaceMessage& msg, BidcosInterfaceMessage* response, uint64_t timeout);

    //! Hilfsmethode für den Empfangsthread
	static void* _RxThreadFunction(void* arg);
    //! Empfangsthread. Wird von StartInterface() gestartet
	void* RxThreadFunction();

    //! Setzt einen AES-Schlüssel
    /*!
     *  \param type Zu modifizierender Schlüsseltyp
     *  \param index Index des zu setzenden Schlüssels
     *  \param data 16 Byte langer zu setzender Schlüssel. MD5-Hash des vom Benutzer eingegebenen Schlüssels
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool SetAESKey(AesKeyType type, int index, const std::string& key);

    //! Wird im Kontext des Empfangsthreads bei eingehenden Nachrichten aufgerufen
	void HandleInputMessage(BidcosInterfaceMessage& msg);
    //! Wird im Kontext des Empfangsthreads bei eingehenden Ereignisnachrichten aufgerufen
    void HandleCommandEvent(unsigned int flags, unsigned int timestamp, int auth_key, int rssi, BidcosFrame& frame);
    //! Wird im Kontext des Empfangsthreads bei eingehenden Antwortnachrichten aufgerufen
    void HandleCommandResponse(unsigned int ref_id, unsigned int flags, unsigned int timestamp, int auth_key, int rssi, BidcosFrame& frame);

    //! Überträgt die Bidcos-Adresse an das Interface
    bool RemoteSetBidcosAddress();
    //! Löscht die Geräteliste des Interfaces
    bool RemoteClearDeviceList();
    //! Überträgt die komplette Geräteliste zum Interface
    bool RemoteAddAllDevices();
    //! Überträgt ein neues Gerät zum Interface
    bool RemoteAddDevice(int address);
    //! Entfernt ein Gerät aus dem Interface
    bool RemoteRemoveDevice(int address);
    //! Überträgt die Zeitinformationen zum Interface
    bool RemoteSetTime();
    //! Überträgt die AES-Schlüssel für alle Schlüsseltypen zum Interface
    bool RemoteSendAesKeys();
    //! Überträgt einen AES-Schlüssel zum Interface
	bool RemoteSendAESKey(AesKeyType type);
    //! Fordert einen debug-dump vom Interface an
	bool RemoteRequestDump();

    //! Synchronisiert auf Basis einer empfangenen Hello-Nachricht den Offset zwischen Timer des Interfaces und lokalem Timer
    void UpdateRemoteTimerOffset(uint64_t remote_timer);

    //! Berechnet den aktuellen Wert des Timers im Interface
    uint64_t GetRemoteTimer();

    //! Wandelt einen empfangenen Zeitstempel des Interfaces in einen lokalen Zeitstempel um
    uint64_t RemoteTimestamp2HostTimestamp(uint64_t remote_timestamp);

	//! Setzt die Firmware-Version
	void SetFirmwareVersion(int version);

	//! BidCoS-RF-Adresse des Lan-Interfaces
	int given_bidcos_address;

    //! Differenz in ms zwischen Timer des Interfaces und lokalem Timer
    uint64_t remote_timer_offset;

    //! Flag zum Beenden des Empfangsthreads
    bool exit;

    //! Mutex zum Zugriff auf das exit-Flag
	pthread_mutex_t mutex_exit;
    //! Mutex für die Verarbeitung eingehender Antworten auf Bidcos-Ebene
	pthread_mutex_t mutex_bidcos_response;
    //! Condition, die den Empfang einer Antwort auf Bidcos-Ebene anzeigt
	pthread_cond_t cond_bidcos_response;
    //! Mutex für den Zugriff auf remote_timer_offset
    pthread_mutex_t mutex_remote_timer_offset;
    //! Mutex für den Zugriff auf die AES-Schlüssel
	pthread_mutex_t mutex_keys;
    //! Mutex für die Verarbeitung eingehender Antworten auf Socket-Ebene
	pthread_mutex_t mutex_socket_response;
    //! Condition, die den Empfang einer Antwort auf Socket-Ebene anzeigt
	pthread_cond_t cond_socket_response;

    //! Handle für Empfangsthread
	pthread_t thread_rx;

    //! Zeiger auf den aktuell in SendFrame verarbeiteten Rahmen für die Zuordnung von Antworten.
	BidcosFrame* cur_tx_frame;
    //! Status für die Zuordnung von Antworten zu cur_tx_frame
	ResponseStatus cur_tx_response_status;

    //! Zeiger auf eine Nachricht, die auf Socket-Ebene auf eine Antwort wartet
    /*! 
     *  \see SendSynchronousMessage
     */
    BidcosInterfaceMessage* socket_response_message;
    //! Opcode der auf Socket-Ebene erwarteten antwort
    int socket_response_expected_command;

    //! AES-Schlüssel
    struct{
        int index;
        std::string key;
    } aes_keys[4];

    //! Wenn gesetzt, wird aktuell eine Antwort auf eine Keepalive-Nachricht erwartet
    bool keepalive_response_expected;

	//! Intervallzeit in Sekunden für die Abfrage von Debug-Dumps vom Interface
	unsigned int debug_dump_interval;

};

#endif
