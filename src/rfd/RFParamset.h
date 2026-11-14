/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFParamset.h: Schnittstelle f�r die Klasse RFParamset.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFPARAMSET_H__0815F594_247F_4C45_86DA_B8898697C969__INCLUDED_)
#define AFX_RFPARAMSET_H__0815F594_247F_4C45_86DA_B8898697C969__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <XmlRpc.h>
#include <HSSParamset.h>

#include "xmlParser.h"
#include <HSSParameter.h>
#include "BidcosFrame.h"
#include "RFLogicalInstance.h"

class RFDeviceDescription;
class RFChannel;

//! Zusammenfassung einer Menge von Objekten der Klasse HSSParameter f�r BidCoS-RF

class RFParamset: public HSSParamset
{
public:
	// Virtuelle Methoden aus HSSParameterset:
	bool Get(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue *set, int mode = 0);
	bool Put(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue& set);
	//! Ruft die SetDefaultConfig Methode der HSSParameterset Klasse auf
	bool SetDefaultConfig(RFLogicalInstance *inst);
	virtual bool InitFromXml(XMLNode &node, XMLNode& root_node);
	bool IsLinkset(){return is_linkset;};

	//! Startet die selbstst�ndige Ermittlung eines Parameterwertes
	/*! 
	 *  Entspricht dem XmlRpc-Aufruf \c DetermineParameter()
	 *  \param inst Ger�te- oder Kanalobjekt, auf das sich die Abfrage bezieht
	 *  \param peer Verkn�pfungspartner auf den sich die Abfrage bezieht [Nur bei \c LINKS relevant]
	 *  \param value_id id des zu ermittelnden Parameters
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool Determine(RFLogicalInstance* inst, const std::string& peer, const std::string& value_id);
	//! Setzt die erzwungenen Werte f�r das Parameterset
	/*! Setzt die aus der XML-Datei gelesenen und in HSSParamset::enforced_values gespeicherten Werte
	 *  Zus�tzlich werden falls relevant die vom Verkn�pfungspartner per
	 *  RFChannelDescription::GetLinkEnforcements() spezifizierten Parameter gesetzt.
	 *  \param inst Ger�te- oder Kanalobjekt, auf das sich die Operation bezieht
	 *  \param peer Verkn�pfungspartner auf den sich die Operation bezieht [Nur bei \c LINKS relevant]
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool SetEnforcedValues(RFLogicalInstance* inst, RFChannel* peer=NULL);
	//! Setzt die Standardwerte f�r das Parameterset
	/*! Setzt die aus der XML-Datei gelesenen und in vec_default_values gespeicherten Werte
	 *  \param inst Ger�te- oder Kanalobjekt, auf das sich die Operation bezieht
	 *  \param peer Verkn�pfungspartner auf den sich die Operation bezieht [Nur bei \c LINKS relevant]
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool SetDefaultValues(RFLogicalInstance* inst, RFChannel* peer);
	//! Aufl�sung von symbolischen erzwungenen oder Standardwerten
	/*! F�r erzwungene Werte und Standardwerte kann eine zur Laufzeit aufgel�ste symbolische
	 *  Notation verwendet werden, um Werte zu setzen, die erst zur Laufzeit bekannt sind.
	 *  Siehe https://svn.elv.lan/wiki/index.php/CCU_xml_runtime_values
	 *  \param config_value aufzul�sender Wert
	 *  \param inst Ger�te- oder Kanalobjekt, auf das sich die Operation bezieht
	 *  \param peer Verkn�pfungspartner auf den sich die Operation bezieht [Nur bei \c LINKS relevant]
	 *  \return \c aufgel�ster Wert
	 */
	XmlRpc::XmlRpcValue ResolveConfigValue(XmlRpc::XmlRpcValue& config_value, RFLogicalInstance* inst, RFLogicalInstance* peer);
	//! Konstruktor
	RFParamset();
	//! Destruktor
	virtual ~RFParamset();
	bool IsCompatible(RFParamset *newParamset);


protected:
	//! Vektor der Standardwerte
	std::vector<enforced_values_t> vec_default_values;
	//! Gibt an, ob es sich um ein Verkn�pfungs-Parameterset handelt
	bool is_linkset;

};

#endif // !defined(AFX_RFPARAMSET_H__0815F594_247F_4C45_86DA_B8898697C969__INCLUDED_)
