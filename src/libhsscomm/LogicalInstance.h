/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LOGICAL_INSTANCE_H_
#define _LOGICAL_INSTANCE_H_

#include "dllexport.h"


#include <string>
#include <XmlRpc.h>
#include "ValueStore.h"
#include <TimerTarget.h>
#include <utils.h>
#include <map>

#ifdef ReportEvent
#undef ReportEvent
#endif

class CommMessage;
//! Abstrakte Basisklasse f�r Ger�te- und Kanalobjekte
class DLLEXPORT LogicalInstance: public ValueStore, public TimerTarget
{
public:
	//! Flag-Bits f�r die Parametrierung, was bei GetLinks() zur�ckgegeben werden soll
	enum{
		GL_FLAG_GROUP=0x01, //!< Bei Gruppen (=Tastenpaare) werden die Verkn�pfungen f�r alle Gruppenmitglieder gew�nscht
		GL_FLAG_SENDER_PARAMSET=0x02, //!< Das Verkn�pfungs-Parameterset f�r die Senderseite soll mit zur�ckgegeben werden
		GL_FLAG_RECEIVER_PARAMSET=0x04, //!< Das Verkn�pfungs-Parameterset f�r die Empf�ngerseite soll mit zur�ckgegeben werden
		GL_FLAG_SENDER_DESCRIPTION=0x08, //!< Die Kanalbeschreibung der Senderseite soll mit zur�ckgegeben werden
		GL_FLAG_RECEIVER_DESCRIPTION=0x10, //!< Die Kanalbeschreibung der Empf�ngerseite soll mit zur�ckgegeben werden
		GL_FLAG_CHECK_PEER=0x4000 //!< Es soll �berpr�ft werden, dass die Gegenseite der Verkn�pfung intakt ist
	};
	//! Flag-Bits in R�ckgabe von GetLinks()
	enum{
		LINK_FLAG_SENDER_INVALID=0x01, //!< Verkn�pfung ist auf der Senderseite nicht intakt
		LINK_FLAG_RECEIVER_INVALID=0x02, //!< Verkn�pfung ist auf der Empf�ngerseite nicht intakt
		LINK_FLAG_SENDER_UNKNOWN=0x04, //!< Ger�t auf der Senderseite der Verkn�pfung ist der CCU nicht bekannt
		LINK_FLAG_RECEIVER_UNKNOWN=0x08, //!< Ger�t auf der Empf�ngerseite der Verkn�pfung ist der CCU nicht bekannt
	};
	//!< Interface f�r Verteilung von �nderungsmitteilungen an internen Werten
	/*! Muss von Klassen implementiert werden, wenn diese �ber �nderungen an internen Werten einer LogicalInstance
	 *  informiert werden m�chten. Wird bei der Registrierung an RegisterInternalValueEvent() �bergeben.
	 */
	class DLLEXPORT EventReceiver
	{
	public:
		//! Konstruktor
		EventReceiver(){};
		//! Destruktor
		virtual ~EventReceiver(){};
		//! Wird bei Ver�nderungen an einem internen Wert, f�r den das Objekt registriert ist, aufgerufen
		/*! \param inst Ger�te- oder Kanalobjekt, an dem die �nderung aufgetreten ist
		 *  \param id Id des ver�nderten internen Wertes
		 *  \param val der neue Wert
		 */
		virtual void OnEvent(LogicalInstance* inst, const std::string& id, XmlRpc::XmlRpcValue& val)=0;
	};
	//! Konstruktor
	LogicalInstance(void);
	//! Destruktor
	virtual ~LogicalInstance(void);
	//! Gibt die aktuellen Werte eines Parametersets zur�ck
	/*! Entspricht dem XmlRpc-Aufruf \c GetParamset()
	 *  Muss von abgeleiteten Klassen implementiert werden.
	 *  \param key Schl�ssel des Parametersets (\c "MASTER", \c "VALUES" oder Adresse des Verkn�pfungspartners)
	 *  \param set Zeiger auf die Variable, die die Ids und Werte des Parametersets als XmlRpc-Struct aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetParamsetValues(const std::string& key, XmlRpc::XmlRpcValue* set)=0;
	
	virtual bool SetDefaultConfig(void) = 0;
	//! Schreibt neue Werte in ein Parameterset
	/*! Entspricht dem XmlRpc-Aufruf \c PetParamset()
	 *  Muss von abgeleiteten Klassen implementiert werden.
	 *  Es werden nur die Werte gesetzt, die in \c set enthalten sind. Alle anderen werden nicht ver�ndert.
	 *  \param key Schl�ssel des Parametersets (\c "MASTER", \c "VALUES" oder Adresse des Verkn�pfungspartners)
	 *  \param set Referenz auf die Variable, die die zu setzenden Werte des Parametersets als XmlRpc-Struct enth�lt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool PutParamsetValues(const std::string& key, XmlRpc::XmlRpcValue& set)=0;
	//! Gibt die Beschreibung eines Parametersets zur�ck
	/*! Entspricht dem XmlRpc-Aufruf \c GetParamsetDescription()
	 *  Muss von abgeleiteten Klassen implementiert werden.
	 *  \param type Typ des Parametersets (\c "MASTER", \c "VALUES" oder \c "LINK")
	 *  \param set Zeiger auf die Variable, die die Ids und Beschreibungen der einzelnen Parameter 
	 *             (siehe HSSParameter::GetDescription()) als XmlRpc-Struct aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetParamsetDescription(const std::string& type, XmlRpc::XmlRpcValue* set)=0;
	//! Gibt die Id eines Parametersets zur�ck
	/*! Entspricht dem XmlRpc-Aufruf \c GetParamsetId()
	 *  Muss von abgeleiteten Klassen implementiert werden.
	 *  Die Id eines Parametersets wird verwendet f�r die Zuordnung von Easymode-Seiten zum Parameterset.
	 *  Die Id wird aus der Ger�tebeschreibungsdatei gelesen und �ber GetParamsetId() an die Oberfl�che
	 *  durchgereicht.
	 *  \param type Typ des Parametersets (\c "MASTER", \c "VALUES" oder \c "LINK")
	 *  \param id Zeiger auf die Variable, die die Id aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetParamsetId(const std::string& type, std::string* id)=0;
	//! Automatische Ermittlung eines Parameterwertes
	/*! Entspricht dem XmlRpc-Aufruf \c DetermineParameter()
	 *  Muss von abgeleiteten Klassen implementiert werden, sofern diese das automatische Ermitteln von
	 *  Parameterwerten unterst�tzen.
	 *  Sollte durchgereicht werden zu HSSParameter::DetermineValue().
	 *  \param paramset Schl�ssel des Parametersets (\c "MASTER", \c "VALUES" oder Adresse des Verkn�pfungspartners)
	 *  \param parameter Id des zu ermittelnden Wertes
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool DetermineParameter(const std::string& paramset, const std::string& parameter){return false;};
	//! Abfrage eines einzelnen Wertes aus dem Parameterset \c "VALUES"
	/*! Entspricht dem XmlRpc-Aufruf \c GetValue()
	 *  Muss von abgeleiteten Klassen implementiert werden.
	 *  \param name Id des abzufragenden Wertes
	 *  \param val Zeiger auf die Variable, die den gelesenen Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetValue(const std::string& name, XmlRpc::XmlRpcValue* val)=0;
	//! Setzen eines einzelnen Wertes im Parameterset \c "VALUES"
	/*! Entspricht dem XmlRpc-Aufruf \c SetValue()
	 *  Muss von abgeleiteten Klassen implementiert werden.
	 *  \param name Id des zu setzenden Wertes
	 *  \param val Referenz auf die Variable, die den zu setzenden Wert enth�lt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool SetValue(const std::string& name, XmlRpc::XmlRpcValue& val)=0;
	virtual void SetValueAsDefined(const std::string& name) = 0;
	virtual void SetValueAsUndefined(const std::string& name) = 0;
	//! Setzen eines internen Wertes
	/*!  Muss von abgeleiteten Klassen implementiert werden, sofern diese interne Werte unterst�tzen.
	 *  Interne Werte sind von der Zentrale verwaltete Zustandsinformationen eines Ger�tes oder Kanals,
	 *  z.B. \c UNREACH, \c LOWBAT, \c AES. Diese Werte k�nnen in der Ger�tebeschreibungsdatei �ber 
	 *  HSSDataInterfaceInternal (&lt;physical interface="internal"&gt;) der XmlRpc-Schnittstelle bereitgestellt
	 *  werden.
	 *  \param name Id des zu setzenden Wertes
	 *  \param val Referenz auf die Variable, die den zu setzenden Wert enth�lt
	 *  \param fire_event bei \c true wird die �nderung den registrierten Objekten der Klasse EventReceiver
	 *                    mitgeteilt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event=false){return false;};
	//! Abfragen eines internen Wertes
	/*!  Muss von abgeleiteten Klassen implementiert werden, sofern diese interne Werte unterst�tzen.
	 *  Interne Werte sind von der Zentrale verwaltete Zustandsinformationen eines Ger�tes oder Kanals,
	 *  z.B. \c UNREACH, \c LOWBAT, \c AES. Diese Werte k�nnen in der Ger�tebeschreibungsdatei �ber 
	 *  HSSDataInterfaceInternal (&lt;physical interface="internal"&gt;) der XmlRpc-Schnittstelle bereitgestellt
	 *  werden.
	 *  \param name Id des abzufragenden Wertes
	 *  \param val Referenz auf die Variable, die den abgefragten Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val){return false;};
	//! �nderung eines Wertes aus dem Parameterset \c "VALUES" �ber die XmlRpc-Schnittstelle mitteilen
	/*! Ruft HSSManager::ReportEvent() auf, um die �nderung eines Wertes mitzuteilen
	 *  Muss von abgeleiteten Klassen implementiert werden, sofern diese Ereignisse f�r Werte unterst�tzen.
	 *  \param id Id des ge�nderten Wertes
	 *  \param val Referenz auf den neuen Wert
	 *  \param burst_suppression Zeit in ms. Liegt das letzte Event f�r diesen Wert l�nger als diese Zeit zur�ck,
	 *                           so wird es nicht gesendet
	 */
	virtual void ReportEvent(const std::string& id, XmlRpc::XmlRpcValue& val, uint32_t burst_suppression=0) = 0;
	//! �nderung an einem als Service-Message markierten Wert mitteilen
	/*! Muss von abgeleiteten Klassen implementiert werden, sofern diese Ereignisse f�r Werte unterst�tzen.
	 *  Ersetzt nicht den Aufruf von ReportEvent().
	 *  \param id Id des ge�nderten Wertes
	 *  \param val Referenz auf den neuen Wert
	 */
	virtual void ReportServiceMessage(const std::string& id, XmlRpc::XmlRpcValue& val){};
	//! Liefert den Index eines Kanals zur�ck. Bei Ger�ten \c -1
	/*! Muss von abgeleiteten Klassen implementiert werden, sofern es sich um Kanalobjekte handelt.
	 */
	virtual int GetIndex(){return -1;};
	//! Liste der Verkn�pfungspartner als Array von Seriennummern ermitteln
	/*! Muss von abgeleiteten Klassen implementiert werden, sofern diese direkte Verkn�pfungen unterst�tzen.
	 */
	virtual bool GetLinkPeers(std::vector<std::string>* peers){return false;};
	//! Typedef f�r die von GetLinks() zur�ckgelieferten Verkn�pfungen
	typedef std::map<std::string, XmlRpc::XmlRpcValue> link_map_t;
	//! Liefert alle direkten Verkn�pfungen zur�ck, an denen das Kanalobjekt beteiligt ist
	/*! Muss von abgeleiteten Klassen implementiert werden, sofern es sich um Kanalobjekte handelt.
	 *  \param flags Flags, die genauer spezifizieren, welche Verkn�pfungen gew�nscht sind.
	 *               - \c GL_FLAG_GROUP: Bei Gruppen (=Tastenpaare) werden die Verkn�pfungen f�r alle Gruppenmitglieder gew�nscht
	 *               - \c GL_FLAG_SENDER_PARAMSET: Das Verkn�pfungs-Parameterset f�r die Senderseite soll mit zur�ckgegeben werden
     *               - \c GL_FLAG_RECEIVER_PARAMSET: Das Verkn�pfungs-Parameterset f�r die Empf�ngerseite soll mit zur�ckgegeben werden
	 *               - \c GL_FLAG_SENDER_DESCRIPTION: Die Kanalbeschreibung der Senderseite soll mit zur�ckgegeben werden
	 *               - \c GL_FLAG_RECEIVER_DESCRIPTION: Die Kanalbeschreibung der Empf�ngerseite soll mit zur�ckgegeben werden
	 *               - \c GL_FLAG_CHECK_PEER: Es soll �berpr�ft werden, dass die Gegenseite der Verkn�pfung intakt ist
	 *  \param result Zeiger auf Map, die die Verkn�pfungen aufnimmt. Die Schl�ssel werden aus den Namen der Verkn�pfungspartner
	 *                generiert und dienen nur dazu, die Eindeutigkeit sicherzustellen. Die Werte sind XmlRpc-Structs in der Form, wie
	 *                sie an der XmlRpc-Schnittstelle ben�tigt werden.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetLinks(int flags, link_map_t* result){return false;};
	//! Liefert die Ger�te- oder Kanalseriennummer zur�ck
	/*! Muss von abgeleiteten Klassen implementiert werden.
	 */
	virtual const std::string& GetSerial()=0;
	//! F�gt einen neuen Verkn�pfungspartner hinzu
	/*! Muss von abgeleiteten Klassen implementiert werden, sofern diese direkte Verkn�pfungen unterst�tzen.
	 *  Muss f�r beide Seiten einer Verkn�pfung aufgerufen werden.
	 *  \param peer Kanalseriennummer des neuen Verkn�pfungspartners
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool AddLinkPeer(const std::string& peer){return false;};
	//! Setzt Name und Beschreibung f�r eine bestehende Verkn�pfung
	/*! Muss von abgeleiteten Klassen implementiert werden, sofern diese direkte Verkn�pfungen unterst�tzen.
	 *  Muss f�r beide Seiten einer Verkn�pfung aufgerufen werden.
	 *  \param peer Kanalseriennummer des Verkn�pfungspartners
	 *  \param name Name der Verkn�pfung
	 *  \param description Beschreibung der Verkn�pfung
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool SetLinkInfo(const std::string& peer, const std::string& name, const std::string& description){return false;};
	//! Ermittelt Namen und Beschreibung f�r eine bestehende Verkn�pfung
	/*! Muss von abgeleiteten Klassen implementiert werden, sofern diese direkte Verkn�pfungen unterst�tzen.
	 *  \param peer Kanalseriennummer des Verkn�pfungspartners
	 *  \param name Zeiger auf Variable, die den Namen der Verkn�pfung aufnimmt
	 *  \param description Zeiger auf Variable, die die Beschreibung der Verkn�pfung aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetLinkInfo(const std::string& peer, std::string* name, std::string* description){return false;};
	//! L�scht einen Verkn�pfungspartner
	/*! Muss von abgeleiteten Klassen implementiert werden, sofern diese direkte Verkn�pfungen unterst�tzen.
	 *  Muss f�r beide Seiten einer Verkn�pfung aufgerufen werden.
	 *  \param peer Kanalseriennummer des zu l�schenden Verkn�pfungspartners
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool RemoveLinkPeer(const std::string& peer){return false;};
	//! Registrierung eines Beobachters f�r die �nderung von internen Werten
	/*!
	 *  \param id Id des beobachteten Wertes
	 *  \param rec Empf�nger der �nderungsmitteilungen
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *  \see GetInternalValue()
	 *  \see SetInternalValue()
	 */
	virtual bool RegisterInternalValueEvent(const std::string& id, EventReceiver* rec);
	//! Weist ein Ger�t an, das zu einer Verkn�pfung geh�rende Parameterset (Profil) auszuf�hren
	/*! �ber diese Methode ist das Testen von Verkn�pfungseinstellungen realisiert
	 *  Muss von abgeleiteten Klassen implementiert werden, sofern diese das Testen von
	 *  Verkn�pfungseinstellungen unterst�tzen.
	 *  \param peer Kanalseriennummer des Verkn�pfungspartners
	 *  \param longpress bei \c true wird der zum langen Tastendruck geh�rende Teil des Profils aktiviert.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool ActivateLinkParamset(const std::string& peer, bool longpress){return false;};
	//! Sorgt daf�r, dass ein Wert nach Ablauf einer Zeit automatisch einen neuen Wert annimmt
	/*!
	 *  \param value_id Id des Wertes, der sich automatisch �ndern soll
	 *  \param value der neue Wert, der angenommen werden soll
	 *  \param delay Zeitverz�gerung in ms nach der dies passieren soll
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool ScheduleAutotimerEvent(const std::string& value_id, XmlRpc::XmlRpcValue& value, uint32_t delay);
	//! L�scht eine bereits eingetragene automatische Wert�nderung
	/*!
	 *  \param value_id Id des Wertes, der sich automatisch �ndern soll
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool CancelAutotimerEvent(const std::string& value_id);
protected:
	//! Konstanten f�r Timer-Cookies
	enum{
		TIMER_AUTOTIMER_EVENTS //!< Timer, der sich um per ScheduleAutotimerEvent() eingetragene Wert�nderungen k�mmert
	};
	//! Wird bei Ablauf eines Timer aufgerufen
	virtual void OnTimer(uint32_t cookie);
	//! F�hrt die anstehenden automatischen Wert�nderungen aus
	/*! Wird aus OnTimer() heraus aufgerufen.
	 *  \see ScheduleAutotimerEvent()
	 *  \see CancelAutotimerEvent()
	 */
	void ProcessAutotimerEvents();
	//! �nderungsmitteilung f�r einen internen Wert versenden.
	/*! Wird von abgeleiteten Klassen aufgerufen.
	 *  \param id Id des internen Wertes, der sich ge�ndert hat
	 *  \param val Neuer Wert des ge�nderten internen Wertes
	 */
	void SendInternalValueEvent(const std::string& id, XmlRpc::XmlRpcValue& val);
	//! �nderungsmitteilung f�r einen internen Wert versenden.
	/*! Wird von abgeleiteten Klassen aufgerufen. Fragt �ber GetInternalValue() den aktuellen Wert ab.
	 *  \param id Id des internen Wertes, der sich ge�ndert hat
	 */
	void SendInternalValueEvent(const std::string& id);
	//! Typedef f�r die registrierten Beobachter von internen Werten
	typedef std::multimap<std::string, EventReceiver*> event_receivers_t;
	//! Map f�r die registrierten Beobachter von internen Werten
	event_receivers_t event_receivers;
	//! Typedef f�r eine automatische Wert�nderung
	typedef struct{
		XmlRpc::XmlRpcValue value; //!< Neuer Wert
		uint64_t time; //!< Verz�gerung in ms
	}t_scheduled_event;
	//! Typedef f�r Map mit automatischen Wert�nderungen
	typedef std::map<std::string, t_scheduled_event> t_map_scheduled_events;
	//! Map f�r automatische Wert�nderungen
	t_map_scheduled_events map_scheduled_events;
};
#endif //_LOGICAL_INSTANCE_H_
