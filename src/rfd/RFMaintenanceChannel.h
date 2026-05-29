/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RF_MAINTENANCE_CHANNEL_H_
#define _RF_MAINTENANCE_CHANNEL_H_

#include "RFChannel.h"
#include <XmlRpc.h>

//! Spezialisierte Kanalklasse für \c MAINTENANCE Kanäle
/*!
 *  Wird verwendet bei Angabe von \c class="maintenance" in der XML-Datei
 *  Von dieser Klasse werden interne Werte gesondert behandelt. Sie spiegelt einfach
 *  die internen Werte des Geräteobjektes. Damit kann über entsprechende Konfiguration
 *  des Kanals in der XML-Datei auf die internen Werte des Gerätes zugegriffen werden.
 */
class RFMaintenanceChannel :
	public RFChannel, public LogicalInstance::EventReceiver
{
public:
	//! Konstruktor
	RFMaintenanceChannel(void);
	//! Destruktor
	~RFMaintenanceChannel(void);
	//! Setzen eines internen Wertes
	/*!
	 *  Ruft RFDevice::SetInternalValue() auf.
	 */
	virtual bool SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event=false);
	//! Abfragen eines internen Wertes
	/*!
	 *  Ruft RFDevice::SetInternalValue() auf.
	 */
	virtual bool GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	//! Registrieren für Anderungsmitteilungen an internen Werten
	/*!
	 *  Ruft zunächst RFDevice::RegisterInternalValueEvent() auf, um dieses Objekt für
	 *  Änderungsereignisse des Geräteobjektes zu registrieren.
	 *
	 *  Danach wird LogicalInstance::RegisterInternalValueEvent() aufgerufen, damit der Aufrufer
	 *  auch bei diesem Objekt registriert ist. 
	 *
	 *  Ereignisse werden dann später von diesem Objekt als Proxy einfach durchgereicht.
	 */
	bool RegisterInternalValueEvent(const std::string& id, LogicalInstance::EventReceiver* rec);
	//! Wird aufgerufen, wenn sich am Gerät ein interner Wert geändert hat.
	/*!
	 *  Leitet den Aufruf per LogicalInstance::SendInternalValueEvent weiter.
	 */
	void OnEvent(LogicalInstance* inst, const std::string& id, XmlRpc::XmlRpcValue& val);
	//! Hilfsmethode für die dynamische Erzeugung
	/*! 
	 *  Sorgt dafür, dass ein Objekt dieser Klasse mitteln hsscomm::type_registry::create("channel_class_maintenance") erzeugt wwerden kann
	 */
	static bool CheckCreationTag(const char *tag);
	//! Dummyimplementierung, die nichts tut und \c true zurückliefert
	virtual bool CommitPendingConfig(){return true;};
	//! Speichert nur Kanalnummer und Kanaltyp
	virtual bool SaveToXml(XMLNode* node);
	//! Dummyimplementierung, die nichts tut und \c true zurückliefert
	virtual bool LoadFromXml(XMLNode& node){return true;};
	//! Dummyimplementierung, die immer \c false zurückliefert
	virtual bool IsConfigPending(){return false;};
	bool PushDefaultConfig(void);
};
#endif //_RF_MAINTENANCE_CHANNEL_H_
