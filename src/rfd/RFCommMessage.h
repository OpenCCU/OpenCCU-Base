/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RF_COMM_MESSAGE_H_
#define _RF_COMM_MESSAGE_H_

#ifdef ERROR_TIMEOUT
#undef ERROR_TIMEOUT
#endif

#include "CommMessage.h"
#include "BidcosFrame.h"

//! Spezialisierte Klasse für die Verarbeitung von BidCoS-RF-Nachrichten
class RFCommMessage :
	public CommMessage
{
public:
	//! Aufzählung der Datenfelder in der Nachricht
	enum{
		//OUT-OF-BAND fields
		FIELD_TIMESTAMP		=STRUCTURED_FRAME_FIELD_INT(0, 0, 4, 0),
		FIELD_RSSI			=STRUCTURED_FRAME_FIELD_INT(4, 0, 1, 0),
	};
	//! Befehle gemäß Kommunikationsspezifikation ARM9 <-> ARM7
	enum{
		//commands to comm processor
		CMD_SEND=0x00,
		CMD_ADDRESS=0x01,
		CMD_OVERRIDE=0x03,
		CMD_SET_KEY=0x04,
		CMD_GET_KEYS=0x05,
		CMD_SEND_AT=0x06,

		//commands from comm processor
		CMD_RESPONSE=0x80,
		CMD_ERROR=0x81,
		CMD_EVENT=0x82,
		CMD_INFO=0x83,
		CMD_AES_EVENT=0x84
	};
	//! Aufzählung der Felder im Override-Befehl
	enum{
		OVERRIDE_FIELD_TYPE=0x00,
		OVERRIDE_FIELD_ID=0x01,
		OVERRIDE_FIELD_KEY=0x02
	};
	//! Flags für den Override-Befehl
	enum{
		OVERRIDE_AES=(1<<0),
		OVERRIDE_WAKEUP=(1<<1)
	};
	//! Aufzählung der Typen von Info-Nachrichten
	enum{
		INFO_TYPE_DUTY_CYCLE=0,
		INFO_TYPE_TX_COMPLETE=1,
		INFO_TYPE_RX_AUTH_REQUEST=2,
		INFO_TYPE_KEYS=3
	};
	//! Aufzählung der Felder von Info-Nachrichten
	enum{
		INFO_FIELD_MESSAGE=0,
		INFO_FIELD_DATA=1
	};
	//! Aufzählung der Typen von Fehler-Nachrichten
	enum{
		ERROR_HARDWARE=0,
		ERROR_TIMEOUT,
		ERROR_DUTY_CYCLE_FULL,
		ERROR_AUTH_FAILED,
		ERROR_ADDRESS_NOT_SET,
		ERROR_EEPROM_CRC,
		ERROR_SET_KEY_IGNORED,
		ERROR_UNKNOWN_AES_KEY
	};
    //! Aufzählung für Flags
    enum{
        FLAG_WOKENUP=1,
        FLAG_PRELIMINARY=2
    };

	//! Konstruktor
	RFCommMessage(void);
	//! Destruktor
	~RFCommMessage(void);
	//! Setzt den BidCoS-RF-Telegrammzähler
	void SetTelegramCounter(int counter);
	//! Gibt den BidCoS-RF-Telegrammzähler zurück
	int GetTelegramCounter();
	//! Gibt die BidCoS-RF-Kommunikationssteuerungsbits zurück
	int GetCtrl();
	//! Gibt die BidCoS-RF-Absenderadresse zurück
	int GetSenderAddress();
	//! Gibt die BidCoS-RF-Empfängeradresse zurück
	int GetReceiverAddress();
	//! Gibt den Rahmentyp der Nachricht zurück
	inline int GetType(){
		return GetPayloadByteData(2);
	};
	//! Gibt bei einer Antwortnachricht die Ursprungsnachricht zurück
	RFCommMessage* GetParent(){return (RFCommMessage*)parent;};
	//! Liefert zu einer Nachricht die gesammelte Antwort nach Index zurück
	inline RFCommMessage* GetResponse(unsigned int index=0){return (RFCommMessage*)CommMessage::GetResponse(index);};
	//! Liefert die Empfängergruppe zurück, an die eine empfangene Nachricht adressiert war
	/*! Mittels der Empfängergruppe kann unterschieden werden, ob eine empfangenen Nachricht
	 *  an die Zentrale, die Broadcast-Adresse oder ein anderes Gerät gesendet wurde.
	 */
	inline int GetAuthKey()
	{
		return auth_key;
	};
	//! Setzt den Index des für eine Authentifizierung verwendeten Schlüssels
	/*! Diese Methode wird von den Authentifizierungsroutinen nach erfolgreicher Authentifizierung aufgerufen.
	 */
	inline void SetAuthKey(int k)
	{
		auth_key=k;
	};
	//! Extrahiert die in der Nachricht enthaltenen BidCoS-Nutzdaten
	BidcosFrame ExtractPayload();

	//! Setzt die in der Nachricht enthaltenen BidCoS-Nutzdaten
	bool SetPayload(const BidcosFrame& payload);

    //! Fügt Flags hinzu
    void AddFlags(int flags)
    {
        this->flags |= flags;
    }

    //! Gibt die Flags zurück
    int GetFlags()
    {
        return flags;
    }

    //! Setzt den RSSI für die Nachricht
    void SetRSSI(int rssi);

    //! Ermittelt den RSSI für die Nachricht
    int GetRSSI();

protected:
	//! Verarbeitung einer eingehenden Nachricht als potentielle Antwort
	virtual bool ProcessResponse(CommMessage *m, t_state* new_state);
private:
	//! Errechnet aus einer BidcosFrame-Feldbeschreibung eine Feldbeschreibung für den Nutzdatenblock dieser Klasse
	inline uint32_t PayloadField(uint32_t f)
	{
		return f+STRUCTURED_FRAME_FIELD_INT(GetPayloadOffset(), 0, 0, 0);
	}
	inline unsigned char GetPayloadByteData(int address)
	{
		return GetByteData(address+GetPayloadOffset());
	}
	inline void SetPayloadByteData(int address, unsigned char v)
	{
		SetByteData(address+GetPayloadOffset(), v);
	}
	//! Startindex der in der Nachricht enthaltenen BidCoS-Nutzdaten
	int GetPayloadOffset();
	//! Index des für eine Authentifizierung verwendeten Schlüssels
	/*! Wird mit \c SetAuthKey() und \c GetAuthKey() ausschließlich von außen manipuliert
	 */
	int auth_key;
	int flags;
    int rssi;
    uint64_t rx_timestamp;
};
#endif //_RF_COMM_MESSAGE_H_
