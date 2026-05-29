/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOSFRAME_H_
#define _BIDCOSFRAME_H_

#include <StructuredFrame.h>
#include <cstdint>

//! Spezialisierte Klasse f�r die Verarbeitung von BidCoS-RF-Nachrichten
class BidcosFrame :
	public StructuredFrame
{
public:
	//! Aufz�hlung f�r die BidCoS-RF-Kommunikationssteuerungsbits
	enum{
		CTRL_RPT_ENABLE=(1<<7), 
		CTRL_RPTED=(1<<6), 
		CTRL_BIDI=(1<<5), 
		CTRL_BURST=(1<<4),
		CTRL_RESERVED=(1<<2),
		CTRL_BCAST=(1<<2),
		CTRL_WAKEMEUP=(1<<1),
		CTRL_WAKEUP=(1<<0)
	};
	typedef enum unreach_reason_e{
		UNREACH_FALSE=0,
		UNREACH_NO_RESPONSE=1,
		UNREACH_CYCLIC_TIMEOUT=2,
		UNREACH_INTERFACE_DUTYCYCLE=3,
		UNREACH_RF_BUSY=4,
	}unreach_reason_t;
	//! Aufz�hlung der Datenfelder in der Nachricht
	enum{
		//generic fields
		FIELD_COUNTER			=STRUCTURED_FRAME_FIELD_INT(0, 0, 0, 7),
		FIELD_CTRL				=STRUCTURED_FRAME_FIELD_INT(1, 0, 1, 0),
		FIELD_RPT_ENABLE		=STRUCTURED_FRAME_FIELD_INT(1, 7, 0, 1),
		FIELD_RPTED				=STRUCTURED_FRAME_FIELD_INT(1, 6, 0, 1),
		FIELD_BIDI				=STRUCTURED_FRAME_FIELD_INT(1, 5, 0, 1),
		FIELD_BURST				=STRUCTURED_FRAME_FIELD_INT(1, 4, 0, 1),
		FIELD_BCAST				=STRUCTURED_FRAME_FIELD_INT(1, 2, 0, 1),
		FIELD_WAKEMEUP			=STRUCTURED_FRAME_FIELD_INT(1, 1, 0, 1),
		FIELD_WAKEUP			=STRUCTURED_FRAME_FIELD_INT(1, 0, 0, 1),
		FIELD_TYPE				=STRUCTURED_FRAME_FIELD_INT(2, 0, 1, 0),
		FIELD_SENDER			=STRUCTURED_FRAME_FIELD_INT(3, 0, 3, 0),
		FIELD_RECEIVER			=STRUCTURED_FRAME_FIELD_INT(6, 0, 3, 0),

		//sysinfo fields
		FIELD_SYSINFO_SWVER		=STRUCTURED_FRAME_FIELD_INT(9, 0, 1, 0),
		FIELD_SYSINFO_TYPE		=STRUCTURED_FRAME_FIELD_INT(10, 0, 2, 0),
		FIELD_SYSINFO_SERIAL	=STRUCTURED_FRAME_FIELD_STR(12, 10),
		FIELD_SYSINFO_CODE		=STRUCTURED_FRAME_FIELD_INT(22, 0, 2, 0),
		FIELD_SYSINFO_CH_A		=STRUCTURED_FRAME_FIELD_INT(24, 0, 1, 0),
		FIELD_SYSINFO_CH_B		=STRUCTURED_FRAME_FIELD_INT(25, 0, 1, 0),

		//ack fields
		FIELD_ACK_CHANNEL		=STRUCTURED_FRAME_FIELD_INT(10, 0, 1, 0),
		FIELD_ACK_STATUS		=STRUCTURED_FRAME_FIELD_INT(11, 0, 1, 0),
		FIELD_ACK_LOWBAT		=STRUCTURED_FRAME_FIELD_INT(12, 7, 0, 1),
		FIELD_ACK_DC90			=STRUCTURED_FRAME_FIELD_INT(12, 0, 0, 1),
		FIELD_ACK_CLOCK			=STRUCTURED_FRAME_FIELD_INT(12, 6, 0, 1),
		FIELD_ACK_STATE			=STRUCTURED_FRAME_FIELD_INT(12, 4, 0, 2),
		FIELD_ACK_RSSI			=STRUCTURED_FRAME_FIELD_INT(13, 0, 1, 0),

		//config fields
		FIELD_CONFIG_CHANNEL	=STRUCTURED_FRAME_FIELD_INT(9, 0, 1, 0),
		FIELD_CONFIG_COMMAND	=STRUCTURED_FRAME_FIELD_INT(10, 0, 1, 0),
		FIELD_CONFIG_PEER_A		=STRUCTURED_FRAME_FIELD_INT(11, 0, 3, 0),
		FIELD_CONFIG_PEER_CH	=STRUCTURED_FRAME_FIELD_INT(14, 0, 1, 0),
		FIELD_CONFIG_PARAM_LIST	=STRUCTURED_FRAME_FIELD_INT(15, 0, 1, 0),
		FIELD_CONFIG_PEER_CH_A	=STRUCTURED_FRAME_FIELD_INT(14, 0, 1, 0),
		FIELD_CONFIG_PEER_CH_B	=STRUCTURED_FRAME_FIELD_INT(15, 0, 1, 0),
		FIELD_CONFIG_SERIAL		=STRUCTURED_FRAME_FIELD_STR(11, 10),
		FIELD_CONFIG_PARAM_INDEX=STRUCTURED_FRAME_FIELD_INT(16, 0, 1, 0),

		//info fields
		FIELD_INFO_STATUS_CH	=STRUCTURED_FRAME_FIELD_INT(10, 0, 1, 0),
		FIELD_INFO_STATUS_ST	=STRUCTURED_FRAME_FIELD_INT(11, 0, 1, 0),
		FIELD_INFO_STATUS_LOWBAT=STRUCTURED_FRAME_FIELD_INT(12, 7, 0, 1),
		FIELD_INFO_STATUS_DC90	=STRUCTURED_FRAME_FIELD_INT(12, 0, 0, 1),
		FIELD_INFO_STATUS_CLOCK	=STRUCTURED_FRAME_FIELD_INT(12, 6, 0, 1),
		FIELD_INFO_STATUS_STATE	=STRUCTURED_FRAME_FIELD_INT(12, 4, 0, 2),
		FIELD_INFO_STATUS_RSSI	=STRUCTURED_FRAME_FIELD_INT(13, 0, 1, 0),
		FIELD_INFO_LINK_PEER_0A =STRUCTURED_FRAME_FIELD_INT(10, 0, 3, 0),
		FIELD_INFO_LINK_PEER_0CH=STRUCTURED_FRAME_FIELD_INT(13, 0, 1, 0),
		FIELD_INFO_LINK_PEER_1A =STRUCTURED_FRAME_FIELD_INT(14, 0, 3, 0),
		FIELD_INFO_LINK_PEER_1CH=STRUCTURED_FRAME_FIELD_INT(17, 0, 1, 0),
		FIELD_INFO_LINK_PEER_2A =STRUCTURED_FRAME_FIELD_INT(18, 0, 3, 0),
		FIELD_INFO_LINK_PEER_2CH=STRUCTURED_FRAME_FIELD_INT(21, 0, 1, 0),
		FIELD_INFO_LINK_PEER_3A =STRUCTURED_FRAME_FIELD_INT(22, 0, 3, 0),
		FIELD_INFO_LINK_PEER_3CH=STRUCTURED_FRAME_FIELD_INT(25, 0, 1, 0),

		FIELD_INFO_PARAM_OFFSET =STRUCTURED_FRAME_FIELD_INT(10, 0, 1, 0),

		FIELD_INFO_PARAM_CH		=STRUCTURED_FRAME_FIELD_INT(10, 0, 1, 0),
		FIELD_INFO_PARAM_PEER_A	=STRUCTURED_FRAME_FIELD_INT(11, 0, 3, 0),
		FIELD_INFO_PARAM_PEER_CH=STRUCTURED_FRAME_FIELD_INT(14, 0, 1, 0),
		FIELD_INFO_PARAM_LIST	=STRUCTURED_FRAME_FIELD_INT(15, 0, 1, 0),
		FIELD_INFO_PARAM_OFFSET2=STRUCTURED_FRAME_FIELD_INT(16, 0, 1, 0),

		//central fields
		FIELD_CENTRAL_CHANNEL	=STRUCTURED_FRAME_FIELD_INT(10, 0, 1, 0),
		FIELD_CENTRAL_LEVEL		=STRUCTURED_FRAME_FIELD_INT(11, 0, 1, 0),
		FIELD_CENTRAL_RAMPTIME	=STRUCTURED_FRAME_FIELD_INT(12, 0, 2, 0),
		FIELD_CENTRAL_ONTIME	=STRUCTURED_FRAME_FIELD_INT(14, 0, 2, 0),


		//switch fields
		FIELD_SWITCH_CHANNEL	=STRUCTURED_FRAME_FIELD_INT(9, 0, 0, 6),
		FIELD_SWITCH_DURATION	=STRUCTURED_FRAME_FIELD_INT(9, 6, 0, 1),
		FIELD_SWITCH_LOWBAT		=STRUCTURED_FRAME_FIELD_INT(9, 7, 0, 1),
		FIELD_SWITCH_COUNTER	=STRUCTURED_FRAME_FIELD_INT(10, 0, 1, 0),
		FIELD_SWITCH_LEVEL		=STRUCTURED_FRAME_FIELD_INT(11, 0, 1, 0),
		FIELD_SWITCH_COND_TIMES	=STRUCTURED_FRAME_FIELD_INT(12, 0, 1, 0),

		//weather fields
		FIELD_WEATHER_LOWBAT	=STRUCTURED_FRAME_FIELD_INT(9, 7, 0, 1),
		FIELD_WEATHER_TEMP		=STRUCTURED_FRAME_FIELD_INT(9, 0, 1, 7),
		FIELD_WEATHER_HUMIDITY	=STRUCTURED_FRAME_FIELD_INT(11, 0, 1, 0),
		FIELD_WEATHER_RAINING	=STRUCTURED_FRAME_FIELD_INT(12, 7, 0, 1),
		FIELD_WEATHER_RAINCNT	=STRUCTURED_FRAME_FIELD_INT(12, 0, 1, 7),
		FIELD_WEATHER_WINDSPEED	=STRUCTURED_FRAME_FIELD_INT(14, 0, 1, 6),
		FIELD_WEATHER_WINDDIR	=STRUCTURED_FRAME_FIELD_INT(16, 0, 1, 0),
		FIELD_WEATHER_WDIR_RANGE=STRUCTURED_FRAME_FIELD_INT(14, 6, 0, 2),
		FIELD_WEATHER_BRIGHTNESS=STRUCTURED_FRAME_FIELD_INT(18, 0, 1, 0),

		//simulation fields
		FIELD_SIM_SENDER		=STRUCTURED_FRAME_FIELD_INT(9, 0, 3, 0),
		FIELD_SIM_TYPE			=STRUCTURED_FRAME_FIELD_INT(12, 0, 1, 0),

		//aes keyexchange fields
		FIELD_CONTAINER_KEY_PART=STRUCTURED_FRAME_FIELD_INT(10, 0, 0, 1),
		FIELD_CONTAINER_KEY_IDX =STRUCTURED_FRAME_FIELD_INT(10, 1, 0, 7),
	};
	//! Aufz�hlung der Rahmentypen mit Untertypen
	enum{
		FT_SYSINFO					= 0x00,
		FT_CONFIG					= 0x01,
		FT_CONFIG_CLEAR				= STRUCTURED_FRAME_TYPE(0x01, 10, 0),
		FT_CONFIG_PEER_ADD			= STRUCTURED_FRAME_TYPE(0x01, 10, 1),
		FT_CONFIG_PEER_REMOVE		= STRUCTURED_FRAME_TYPE(0x01, 10, 2),
		FT_CONFIG_PEER_LIST_REQ		= STRUCTURED_FRAME_TYPE(0x01, 10, 3),
		FT_CONFIG_PARAM_REQ			= STRUCTURED_FRAME_TYPE(0x01, 10, 4),
		FT_CONFIG_START				= STRUCTURED_FRAME_TYPE(0x01, 10, 5),
		FT_CONFIG_END				= STRUCTURED_FRAME_TYPE(0x01, 10, 6),
		FT_CONFIG_WRITE_OFFSET		= STRUCTURED_FRAME_TYPE(0x01, 10, 7),
		FT_CONFIG_WRITE_INDEX		= STRUCTURED_FRAME_TYPE(0x01, 10, 8),
		FT_CONFIG_SERIAL_REQ		= STRUCTURED_FRAME_TYPE(0x01, 10, 9),
		FT_CONFIG_ENTER				= STRUCTURED_FRAME_TYPE(0x01, 10, 10),
		FT_CONFIG_RPTR_ADD_LINK		= STRUCTURED_FRAME_TYPE(0x01, 10, 11),
		FT_CONFIG_RPTR_DEL_LINK		= STRUCTURED_FRAME_TYPE(0x01, 10, 12),
		FT_CONFIG_RPTR_LINK_LIST_REQ= STRUCTURED_FRAME_TYPE(0x01, 10, 13),
		FT_CONFIG_STATUS_REQ		= STRUCTURED_FRAME_TYPE(0x01, 10, 14),
		FT_CONFIG_DETERMINE			= STRUCTURED_FRAME_TYPE(0x01, 10, 15),
		FT_ACK_OR_NACK				= 0x02,
		FT_ACK						= STRUCTURED_FRAME_TYPE(0x02, 9, 0x00),
		FT_ACK_STATUS				= STRUCTURED_FRAME_TYPE(0x02, 9, 0x01),
		FT_ACK_PARAM_IGNORED		= STRUCTURED_FRAME_TYPE(0x02, 9, 0x02),
		FT_ACK_PARAM_CORRECTED		= STRUCTURED_FRAME_TYPE(0x02, 9, 0x03),
		FT_ACK_W_AES_CHALLENGE		= STRUCTURED_FRAME_TYPE(0x02, 9, 0x04),
		FT_NACK						= STRUCTURED_FRAME_TYPE(0x02, 9, 0x80),
		FT_NACK_BUSY				= STRUCTURED_FRAME_TYPE(0x02, 9, 0x81),
		FT_NACK_MEMFULL				= STRUCTURED_FRAME_TYPE(0x02, 9, 0x82),
		FT_NACK_MEMFULL_PART		= STRUCTURED_FRAME_TYPE(0x02, 9, 0x83),
		FT_NACK_TARGET_INVALID		= STRUCTURED_FRAME_TYPE(0x02, 9, 0x84),
		FT_NACK_CHANNEL_INVALID		= STRUCTURED_FRAME_TYPE(0x02, 9, 0x85),
		FT_AES_SOLUTION				= 0x03,
		FT_AES_CONTAINER			= 0x04,
		FT_AES_CONTAINER_KEY		= STRUCTURED_FRAME_TYPE(0x04, 9, 1),
		FT_TESTMODE					= 0x09,
		FT_INFO						= 0x10,
		FT_INFO_SERIAL				= STRUCTURED_FRAME_TYPE(0x10, 9, 0),
		FT_INFO_PEER_LIST			= STRUCTURED_FRAME_TYPE(0x10, 9, 1),
		FT_INFO_PARAM_RESPONSE_PAIRS= STRUCTURED_FRAME_TYPE(0x10, 9, 2),
		FT_INFO_PARAM_RESPONSE_SEQ	= STRUCTURED_FRAME_TYPE(0x10, 9, 3),
		FT_INFO_PARAM_ASYNC_PAIRS	= STRUCTURED_FRAME_TYPE(0x10, 9, 4),
		FT_INFO_PARAM_ASYNC_SEQ		= STRUCTURED_FRAME_TYPE(0x10, 9, 5),
		FT_INFO_STATUS				= STRUCTURED_FRAME_TYPE(0x10, 9, 6),
		FT_INFO_ZONE_THERM			= STRUCTURED_FRAME_TYPE(0x10, 9, 7),
		FT_INFO_RPTR_LINK_LIST		= STRUCTURED_FRAME_TYPE(0x10, 9, 8),
		FT_CENTRAL_UNLOCK			= STRUCTURED_FRAME_TYPE(0x11, 9, 0),
		FT_CENTRAL_LOCK				= STRUCTURED_FRAME_TYPE(0x11, 9, 1),
		FT_CENTRAL_RAMP_START		= STRUCTURED_FRAME_TYPE(0x11, 9, 2),
		FT_CENTRAL_RAMP_STOP		= STRUCTURED_FRAME_TYPE(0x11, 9, 3),
		FT_CENTRAL_RESET			= STRUCTURED_FRAME_TYPE(0x11, 9, 4),
		FT_CENTRAL_START_BOOTLOADER = STRUCTURED_FRAME_TYPE(0x11, 9 ,0xca),
		FT_WAKEUP					= 0x12,
		FT_SIMULATION				= 0x3e,
		FT_TIME						= 0x3f,
		FT_SWITCH					= 0x40,
		FT_CONDITIONAL_SWITCH		= 0x41,
		FT_LEVEL					= 0x42,
		FT_HEATING					= 0x58,
		FT_HAZARD_SENSOR			= 0x60,
		FT_HAZARD_SENSOR_COND		= 0x61,
		FT_WEATHER					= 0x70,
		FT_WEATHER_REQUEST			= 0x7f,
		FT_FIRMWAREUPDATE			= 0xca,
		FT_WRITE_TRX_REG			= 0xcb,
	};
	//! Konstanten f�r nicht gesetzten oder unbekannten AES-Schl�ssel
	enum{
		INVALID_AUTH_KEY=-1,
		UNKNOWN_AUTH_KEY=-2
	};
	//! Konstante f�r ung�ltigen RSSI
	enum{
		INVALID_RSSI_VALUE=-0xffff
	};
	//! Konstruktor
	BidcosFrame(void);
	//! Destruktor
	~BidcosFrame(void);
	//! Setzt den BidCoS-RF-Telegrammz�hler
	void SetTelegramCounter(int counter);
	//! Gibt den BidCoS-RF-Telegrammz�hler zur�ck
	int GetTelegramCounter() const;
	//! Setzt die BidCoS-RF-Kommunikationssteuerungsbits
	void SetCtrl(int flags);
	//! Gibt die BidCoS-RF-Kommunikationssteuerungsbits zur�ck
	int GetCtrl() const;
	//! Setzt die BidCoS-RF-Absenderadresse
	void SetSenderAddress(int address);
	//! Setzt die BidCoS-RF-Empf�ngeradresse
	void SetReceiverAddress(int address);
	//! Gibt die BidCoS-RF-Absenderadresse zur�ck
	int GetSenderAddress() const;
	//! Gibt die BidCoS-RF-Empf�ngeradresse zur�ck
	int GetReceiverAddress() const;
	//! Pr�ft den Rahmentyp der Nachricht gegen den �bergebenen Rahmentypen mit eventuellem Untertypen
	int GetType() const;
	//! Wandelt eine "normale" Nachricht in eine simulierte Nachricht um
	bool TransformToSimulationMessage();
	//! Wandelt eine simulierte Nachricht in eine "normale" Nachricht um
	bool TransformFromSimulationMessage();
	//! Ermittelt, ob es sich um eine negative Best�tigung handelt
	inline bool IsNack() const
	{
		return (GetType()==FT_ACK_OR_NACK) && (GetByteData(9)&0x80);
	}
	//! Ermittelt, ob es sich um ein Gruppentelegramm (Gruppe=Team) handeln k�nnte
	/*! 
	 *  Es gibt Ger�te, die "normale" Telegramme senden, auf die diese Pr�fung ebenfalls zutrifft.
	 *  Daher muss auf jeden Fall eine zus�tzliche Pr�fung stattfinden, z.B. ob ein Team mit der
	 *  angegebenen Absenderadresse �berhaupt existiert.
	 */
	inline bool PossiblyTeamTelegram() const
	{
		return ((GetCtrl()&(CTRL_BCAST|CTRL_BIDI))==CTRL_BCAST) && (GetReceiverAddress()!=0);
	}
	//! Liefert zu einer Nachricht die gesammelte Antwort nach Index zur�ck
	BidcosFrame* GetResponse(unsigned int index=0);
	//! Liefert die Empf�ngergruppe zur�ck, an die eine empfangene Nachricht adressiert war
	/*! Mittels der Empf�ngergruppe kann unterschieden werden, ob eine empfangenen Nachricht
	 *  an die Zentrale, die Broadcast-Adresse oder ein anderes Ger�t gesendet wurde.
	 */
	virtual ReceiverType GetReceiverType() const;
	//! Liefert den Index des f�r eine Authentifizierung verwendeten Schl�ssels zur�ck
	/*! Wenn der verwendete Kommunikationskanal Authentifizierung unterst�tzt und f�r diese Nachricht
	 *  ein Authentifizierungszyklus durchlaufen wurde, kann mit dieser Funktion der Index des dabei
	 *  verwendeten Schl�ssels abgefragt werden.
	 *  \return Index des f�r die Authentifizierung verwendeten Schl�ssels. \c -1 falls nicht authentifiziert wurde.
	 */
	inline int GetAuthKey() const
	{
		return auth_key;
	}
	//! Setzt den Index des f�r eine Authentifizierung verwendeten Schl�ssels
	/*! Diese Methode wird von den Authentifizierungsroutinen nach erfolgreicher Authentifizierung aufgerufen.
	 */
	inline void SetAuthKey(int k)
	{
		auth_key=k;
	}
	//! Gibt zur�ck, ob f�r die Nachricht die AES-Authentifizierung durchlaufen wurde
	bool WasAuthenticated() const
	{
		return GetAuthKey() >= 0;
	}
	//! Liefert f�r einen Feld-Identifizierer den entsprechenden Feld-Identifizierer f�r eine Simulationsnachricht zur�ck
	static inline unsigned int SimField(unsigned int f)
	{
		return f+(4<<3);
	}

	//! Hinzuf�gen einer Antwortnachricht, falls diese "passt"
	virtual bool CheckAndAddResponse(const BidcosFrame& response);

	//! L�schen allen Antwortnachrichten
	void ClearResponses();

	//! Gibt die Anzahl der Antwortnachrichten zurück.
	unsigned int GetResponseCount() const;

	//! Werden noch mehr Antwortnachrichten erwartet?
	virtual bool CheckReceiveComplete(uint64_t* wait_time_ms);

	//! Hat der Rahmen einen g�ltigen Zeitstempel?
	bool HasTimestamp() const;

	//! Abfragen des Zeitstempels
	uint64_t GetTimestamp() const;

	//! Setzen des Zeitstempels
	void SetTimestamp(uint64_t timestamp);

	//! Bedeutet diese Nachricht, dass das sendende Ger�t jetzt wach ist?
	bool DeviceWokenup() const;

	//! Setzt das Flag device_wokenup
	void SetDeviceWokenup(bool wu);

	//! Schl�sselindex zur�cksetzen
	void ResetAuthKey();

	//! Ermittelt, ob die L�nge der Daten einem BidCoS-Frame entspricht
	bool IsValid() const;

	//! Setzt die Seriennummer des empfangenden oder sendenden Interfaces
	void SetInterfaceId(const std::string& s);

	//! Liefert die Seriennummer des empfangenden oder sendenden Interfaces
	const std::string& GetInterfaceId() const;

    //! Gibt an, ob es sich um einen vorl�ufigen Rahmen in einer laufenden Kommunikation handelt
    bool IsPreliminary() const;

    //! Setzt, ob es sich um einen vorl�ufigen Rahmen in einer laufenden Kommunikation handelt
    void SetPreliminary(bool prelim);

    //! Setzt die Empfangsfeldst�rke in dbm f�r einen empfangenen Rahmen
	void SetRSSI(int rssi);

    //! Liefert die Empfangsfeldst�rke in dbm zur�ck
    /*!
     *  \return Empfangsfeldst�rke in dbm oder INVALID_RSSI_VALUE falls keine Information �ber die
     *          Empfangsfeldst�rke vorliegt
     */
	int GetRSSI() const;
	unreach_reason_t GetUnreachReason();
	void SetUnreachReason(BidcosFrame::unreach_reason_t reason);

	/** \brief Returns true if this frame had been answered with a nack.
	 * \return True if this frame is a request frame which had been answered with a nack, otherwise false.
	 */
	bool WasNacked() const;

protected:
	//! Hinzuf�gen einer Antwortnachricht
	bool AddResponse(const BidcosFrame& response);

private:
	//! Index des f�r eine Authentifizierung verwendeten Schl�ssels
	/*! Wird mit \c SetAuthKey() und \c GetAuthKey() ausschlie�lich von au�en manipuliert
	 */
	int auth_key;
    //! Empfangsfeldst�rke in dbm. INVALID_RSSI_VALUE f�r Empfangsfeldst�rke nicht bekannt
	int rssi;
    //! Empfangszeitpunkt in ms
    /*!
     *  Bezogen auf \c time_millis()
     */
	uint64_t timestamp;
    //! Bei \c true kann mit dem Empfang dieses Rahmens davon ausgegangen werden, dass das Ger�t aufgeweckt wurde
	bool device_wokenup;
    //! Bei \c true handelt es sich um einen vorl�ufigen Rahmen
    /*!
     *  Dieser Rahmen ist Teil einer l�ngeren Kommunikation (z.B. AES). Er sollte �ber die Auswertung der Empfangsfeldst�rke
     *  hinaus nicht verwendet werden.
     */
    bool preliminary;
    //! Typedef f�r die Antwortrahmen
	typedef std::vector<BidcosFrame*> t_vec_responses;
    //! Vektor der Antwortrahmen
	t_vec_responses vec_responses;
    //! Seriennummer des Interfaces, �ber das dieser Rahmen gesendet bzw. empfangen wurde
	std::string interface_id;
	unreach_reason_t unreach_reason;
};
#endif //_BIDCOSFRAME_H_
