/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _VALUE_STORE_H_
#define _VALUE_STORE_H_

#include "dllexport.h"

#include <XmlRpc.h>
#include <map>
#include <string>
#include "xmlParser.h"


//! Diese Klasse speichert Name-Werte-Paare mit Zeitstempel
/*! Der Name ist ein String und der Wert ein XmlRpcValue.
 *  Die Klasse bietet Unterstützung für die XML-basierte Persistierung der gespeicherten Daten
 */
class DLLEXPORT ValueStore
{
public:
	//Flags
	enum{
		FLAG_VOLATILE=(1<<0) //!< Wird beim Speichern angegeben, wenn der entsprechende Wert nicht persistiert werden soll
	};
	//! Konstruktor
	/*!
	 *  \param hold_timestamps \c true aktiviert das Speichern von Zeitstempeln
	 */
	ValueStore(bool hold_timestamps=false);
	//! Destruktor
	virtual ~ValueStore(void);
	//! Abfrage eines gespeicherten Wertes mit optionaler Angabe des Maximalalters
	/*!
	 *  \param name Id des gespeicherten Wertes der abgefragt werden soll
	 *  \param val Zeiger auf die Variable, die den gelesenen Wert aufnimmt
	 *  \param age Maximal zulässiges Alter des Wertes in ms. Ist der gespeicherte Wert älter, so wird
	 *             \c false zurückgegeben. Bei \c NULL wird der Zeitstempel nicht geprüft.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetStoredValue(const std::string& name, XmlRpc::XmlRpcValue* val, uint32_t* age=NULL);
	//! Speichern bzw. Aktualisieren eines Wertes
	/*!
	 *  \param name Id des zu speichernden Wertes
	 *  \param val Referenz den zu speichernden Wert
	 *  \param flags Flags, die beeinflussen, wie der Wert gespeichert wird:
	 *               - FLAG_VOLATILE: Der Wert wird nicht persistiert
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool SetStoredValue(const std::string& name, XmlRpc::XmlRpcValue& val, int flags=0);
	//! Alter in ms eines gespeicherten Wertes ermitteln
	/*!
	 *  \param name Id des gespeicherten Wertes, dessen Zeitstempel abgefragt werden soll
	 *  \return Alter des Wertes in ms
	 */
	uint32_t GetAge(const std::string& name);
	//! Alle gespeicherten Werte löschen
	void ClearStoredValues();
	//! Als XML persistieren
	/*! Es werden nur die Werte persistiert, die das Flag \c FLAG_VOLATILE nicht gesetzt haben.
	 *  Zeitstempel werden nicht mit persistiert.
	 */
	bool SaveToXml(XMLNode* node);
	//! Aus XML einlesen
	/*! Liest Werte ein, die zuvor mit SaveToXml() persistiert wurden. Die Zeitstempel der gelesenen
	 *  Werte werden auf 10000 Sekunden in der Vergangenheit gesetzt.
	 */
	bool LoadFromXml(XMLNode& node);

	//! Gibt zurück, ob nicht-volatile Daten noch nicht gespeichert wurden.
	/*!	Rückgabe True, wenn Daten gespeichert werden müssen (und der ValueStore damit 'dirty' ist), sonst False. 
	*/
	bool IsDirty() const;
protected:
	//! Typedef für die Speicherung eines Wertes
	typedef struct{
		XmlRpc::XmlRpcValue val; //!< Zu speichernder Wert
		uint64_t timestamp; //!< Zeitstempel in ms
		int flags; //!< Flags für Persistierung
	} entry_t;
	//! Typedef für die gespeicherten Werte
	typedef std::map<std::string, entry_t> stored_values_t;
	//! Map der gespeicherten Werte
	stored_values_t stored_values;
	//! Flag, das angibt, ob Zeitstempel verwaltet werden sollen
	bool hold_timestamps;

	//! Flag, das angibt, ob nicht-volatile Änderungen gemacht wurden, die nocht nicht gespeichert wurden.
	bool dirty;
};
#endif //_VALUE_STORE_H_
