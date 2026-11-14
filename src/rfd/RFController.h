/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFController.h: Schnittstelle für die Klasse RFController.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFCONTROLLER_H__7C36417F_F00D_41C0_8C42_6604B1AA3EFF__INCLUDED_)
#define AFX_RFCONTROLLER_H__7C36417F_F00D_41C0_8C42_6604B1AA3EFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include <vector>
#include <set>
#include <CommController.h>
#include "RFCommMessage.h"
#include "BidcosFrame.h"
#include "BidcosInterface.h"

class CommController;

//! Spezialisierte Kommunikationscontroller-Klasse für BidCoS-RF
/*!
 *  Die Klasse hält bestimmte in der Kommunikation nötige Daten lokal vor, damit aus dem
 *  Sende- bzw. Empfangsthread heraus schnell darauf zugegriffen werden kann:
 *  - gesendete und empfangene Telegrammzähler für die BidCoS-RF-Geräte
 *  - Kanäle, von denen eine AES-Authentifizierung verlangt werden muss
 *  - Geräte, die mit einem WAKEUP-Rahmen aufgeweckt werden müssen
 *
 *  Die Klasse kümmert sich um die Vergabe und Prüfung der BidCoS-Telegrammzähler.
 */
class RFController:public CommController, public BidcosInterface
{
public:
	//! Konstanten für die AES-Schlüsseltypen
	enum AesKeyType{
		AES_KEY_TYPE_DEFAULT=0, //!< Standardschlüssel ("ELV-Schlüssel")
		AES_KEY_TYPE_CURRENT_USER, //!< Aktiver Benutzerschlüssel
		AES_KEY_TYPE_PREVIOUS_USER, //!< Vorheriger Benutzerschlüssel
		AES_KEY_TYPE_TEMP, //!< Temporärschlüssel
	};
	//! Stellt eine zu sendende Nachricht in die Sendequeue und wartet optional bis diese gesendet wurde
	/*!
	 *  Falls die Absenderadresse der Nachricht nicht die Zentralenadresse ist, wird die Nachricht
	 *  in eine äquivalente Simulationsnachricht umgewandelt. Dann wird die Nachricht in die Sendequeue
	 *  eingereiht. Falls das "dont_delete"-Flag der Nachricht gesetzt ist, wird gewartet bis sie gesendet
	 *  wurde. Der Rückgabewert gibt dann an, ob die Sendung erfolgreich war.
	 *  Ist das "dont_delete"-Flag nicht gesetzt, wird die Nachricht vom Sendethread nach der Sendung
	 *  gelöscht. Aus diesem Grund wird dann nicht auf das Senden gewartet, sondern sofort mit \c true
	 *  zurückgekehrt.
	 */
    bool SendFrame(BidcosFrame* frame);
	//! Konstruktor
	RFController();
	//! Fügt ein Gerät gemäß BidCoS-RF-Adresse in \c map_wakeup_device ein
	/*! In \c map_wakeup_devices werden die Geräte verwaltet, die nach der nächsten 
	 *  Aussendung angesprochen werden sollen. Die Entscheidung, ein Gerät aufzuwecken, muss
	 *  innerhalb von ca. 80ms nach dem Empfang einer Nachricht von dem Gerät erfolgen. Um diese
	 *  Zeit einhalten zu können, wird direkt im Empfangsthread mittels \c map_wakeup_device
	 *  geprüft, ob ein Wakeup-Rahmen gesendet werden muss.
	 */
	bool AddDeviceWakeupRequest(int address,bool lazyConfig = false);
	//! Löscht ein Gerät gemäß BidCoS-RF-Adresse aus \c map_wakeup_device
	/*! \see AddWakeupDevice()
	 */
	bool RemoveDeviceWakeupRequest(int address);
	//! Aktiviert die AES-Authentifizierung für einen Kanal
	/*! 
	 *  Sorgt dafür, dass für vom angegebenen Kanal empfangene Nachrichten von der CCU
	 *  eine AES-Authentifizierung verlangt wird.
	 *  
	 *  In \c map_aes_devices werden die Geräte bzw. Kanäle verwaltet, von denen eine 
	 *  AES-Authentifizierung verlangt werden soll. Die Entscheidung, eine AES-Authentifizierung
	 *  zu verlangen, muss innerhalb von ca. 80ms nach dem Empfang der Nachricht von dem Gerät
	 *  erfolgen. Um diese Zeit einhalten zu können, wird dies direkt im Empfangsthread mittels 
	 *  \c map_aes_devices geprüft.
	 *  \param address BidCoS-RF-Geräteadresse
	 *  \param aes_key Index des für die Authentifizierung zu verwendenden Schlüssels
	 *  \param aes_channels Bitmaske der Kanäle, die AES-Authentifiziert werden sollen.
	 */
	bool SetAesKeyTemp(int index, const std::string& data);
	bool SetAesKeyUser(int index, const std::string& data, int last_index, const std::string& last_data);

	bool GetAesKeyIndexes(int* default_key, int* current_key, int* previous_key, int* temp_key);
	//! Destruktor
	virtual ~RFController();
    bool Init(std::map<std::string, std::string>& params);

    bool IsConnected();

protected:
	//! Prüft, ob eine empfangenen Nachricht AES-Authentifiziert werden muss
	/*!
	 *  Es wird geprüft
	 *  - ob das sendende Gerät in \c map_aes_devices eingetragen ist
	 *  - ob es sich um einen authentifizierungswürdigen Rahmen handelt:
	 *    - Schaltbefehl (Rahmentyp 0x40)
	 *    - Bedingter Schaltbefehl (Rahmentyp 0x41)
	 *    - Pegelbefehl (Rahmentyp 0x42)
	 *    - Info-Telegramm mit Statusmitteilung (Rahmentyp 0x10, Byte 9 = 0x06)
	 *  - ob das dem sendenden Kanal entsprechende Bit in AesDeviceData::channel_mask gesett ist 
	 */
	bool NeedsAES(RFCommMessage* msg, int* key_index);
	//! Factory-Methode; Erzeugt ein neues Objekt von RFCommMessage
	inline virtual CommMessage* NewMessage(){return new RFCommMessage();};
	//! Wird im Kontext des Sendethreads unmittelbar vor dem Senden aufgerufen
	/*!
	 *  Setzt den Telegrammzähler der zu sendenden Nachricht.
	 *
	 *  Sorgt über \c send_inhibit_flags und \c send_inhibit_timestamp, dass die nächste
	 *  Aussendung unterbunden wird, bis das zu sendende Paket gesendet ist oder \c SEND_INHIBIT_TIMEOUT
	 *  abgelaufen ist.
	 */
	bool CheckBeforeSend(CommMessage* msg);
	//! Führt zeitkritische Operationen mit der soeben empfangenen Nachricht durch
	/*!
	 *  Prüft mit NeedsAES(), ob eine AES-Authentifizierung verlangt werden soll.
	 *
	 *  Prüft, ob ein Wakeup-Rahmen gesendet werden soll. Bei an die CCU adressierten bidirektionalen Nachrichten
	 *  wird das Wakeup-Paket anstatt der normalen Bestätigung gesendet. Bei anderen unidirektionalen Nachrichten
	 *  wird bei ersten Versuch mit einer Wahrscheinlichkeit von 100% und ab dem zweiten Versuch mit einer
	 *  Wahrscheinlichkeit von 50% eine Wakeup-Nachricht gesendet. Über das Setzen von applikationsspezifischen Flags
	 *  an der empfangenen Nachricht wird RFDevice mitgeteilt, dass ein Wakeup-Rahmen gesendet wurde.
	 *
	 *  Setzt \c send_inhibit_flags zurück.
	 *
	 *  Prüft den BidCoS-Telegrammzähler.
	 *
	 *  Falls eine AES-Authentifizierung vom angesprochenen Gerät verlangt wurde, wird das dies mitteilende
	 *  Info-Telegramm vom ARM7 um die Empfängeradresse der aktuell gesendeten Nachricht erweitert, 
	 *  damit in RFManager::ProcessIncomingFrame() der aktuelle AES-Schlüssel dem Gerät zugeordnet
	 *  werden kann
	 */
	bool CheckAfterReceive(CommMessage* msg);
	virtual void ProcessReceivedMessage(CommMessage* msg);

	bool StartInterface(int bidcos_address);
	bool StopInterface();
private:
	//! Überträgt die BidCoS-RF-ADresse der CCU an den ARM7
	/*! \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool SendOwnAddress();
	//! Liefert die Adresse der Zentrale
	int GetAddress();

	bool ChangeAESKey(AesKeyType type, int index, const std::string& key);

	//! Konstanten für \c send_inhibit_flags
	enum{
		INHIBIT_AES=(1<<0), //!< AES-Authentifizierungszyklus ist aktiv
		INHIBIT_TX=(1<<1), //!< Es wird gerade gesendet
		INHIBIT_RX=(1<<2) //!< Es wird gerade ein Empfang bestätigt
	};
	//! Timeout für zwangsweises Zurücksetzen von \c send_inhibit_flags
	enum{
		SEND_INHIBIT_TIMEOUT=1500
	};
	//! Speichert für ein Gerät die Daten, die für das Aufwecken relevant sind
	class WakeupDeviceData{
	public:
		//! Konstruktor
		WakeupDeviceData():counter(0), telegram_counter(-1){};
		//! Zähler für Aufweckversuche
		int counter;
		//! Telegrammzähler des Aufweckversuchs. Ein nachfolgend empfangenes ACK bedeutet, das Gerät ist wach.
		int telegram_counter;
	};
	//! Typedef Map Adresse -> WakeupDeviceData für das Aufwecken von Geräten
	typedef std::map<int, WakeupDeviceData> t_map_wakeup_devices;
	//! Map Adresse -> WakeupDeviceData für das Aufwecken von Geräten
	t_map_wakeup_devices map_wakeup_devices;
	//! Mutex für Zugriff auf \c map_wakeup_devices
	pthread_mutex_t mutex_wakeup_devices;
	//! Mutex für Zugriff auf \c send_inhibit_timestamp, \c send_inhibit_flags und \c cond_send_inhibit
	pthread_mutex_t mutex_send_inhibit;
	//! Condition für die Kommunikation von Änderungen an \c send_inhibit_flags
	pthread_cond_t cond_send_inhibit;
	//! Variable für Sendesperre. Sobald ein Bit gesetzt ist, ist das Senden gesperrt. Das gesetzte Bit gibt den Grund der Sperre an.
	int send_inhibit_flags;
	//! Speichert die ID der zuletzt empfangenen Nachricht, aufgrund der \c send_inhibit_flags gesetzt ist
	/*!
	 *  Hierüber kann eine Fehlermeldung oder Infomeldung vom ARM7 zugeordnet werden und zum Zurücksetzen
	 *  von \c send_inhibit_flags führen.
	 */
	int send_inhibit_cnt_rx;
	//! Speichert die ID der zuletzt gesendeten Nachricht, aufgrund der \c send_inhibit_flags gesetzt ist
	/*!
	 *  Hierüber kann eine Fehlermeldung oder Infomeldung vom ARM7 zugeordnet werden und zum Zurücksetzen
	 *  von \c send_inhibit_flags führen.
	 */
	int send_inhibit_cnt_tx;
	//! Zeitstempel in ms. Gibt an, wann \c send_inhibit_flags gesetzt wurde.
	/*!
	 *  Wird verwendet für Timeout-Prüfung für den Fall, dass nichts vom ARM7 gemeldet wird, was zum Zurücksetzen
	 *  von \c send_inhibit_flags führt
	 */
	uint64_t send_inhibit_timestamp;
	//! BidCoS-RF-Adresse der CCU
	int bidcos_address;
    //! Map zum Zwischenspeichern der RSSI-Werte für AES-Frames
    std::map<int, int> map_aes_rssi;
    //! Gerätedatei für Kommunikation mit ARM7
    static const char* CCU_RFD_DEVICE;
};

#endif // !defined(AFX_RFINTERFACE_H__7C36417F_F00D_41C0_8C42_6604B1AA3EFF__INCLUDED_)
