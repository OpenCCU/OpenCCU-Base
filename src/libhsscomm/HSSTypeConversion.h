/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversion.h: Schnittstelle für die Klasse HSSTypeConversion.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTYPECONVERSION_H_
#define _HSSTYPECONVERSION_H_

#include "dllexport.h"


#include <XmlRpc.h>
#include "xmlParser.h"
#include "LogicalInstance.h"

//! Abstrakte Basisklasse für Konvertierungen zwischen Datentypen
/*! Wird von HSSParameter verwendet für Konvertierungen zwischen HSSLogicalType und HSSPhysicalType.
 *  Komplexere Konvertierungen werden dort durch kaskadieren von Objekten dieser Klasse abgebildet.
 */
class DLLEXPORT HSSTypeConversion  
{
public:
	//! Konvertiert einen physikalischen Wert in einen logischen Wert
	/*! Wird von einer abgeleiteten Klasse implementiert.
	 *  \param inst Zeiger auf das Geräte- oder Kanalobjekt, auf das sich die Konvertierung bezieht
	 *  \param in zu konvertierender Wert
	 *  \param out Zeiger auf die Variable, die den konvertierten Wert aufnimmt
	 *  \param event bei \c true soll aufgrund eines eingehenden Ereignisses konvertiert werden, 
	 *               bei \c false aufgrund eines Lesevorgangs
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event)=0;
	//! Konvertiert einen logischen Wert in einen physikalischen Wert
	/*! Wird von einer abgeleiteten Klasse implementiert.
	 *  \param inst Zeiger auf das Geräte- oder Kanalobjekt, auf das sich die Konvertierung bezieht
	 *  \param in zu konvertierender Wert
	 *  \param out Zeiger auf die Variable, die den konvertierten Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out)=0;
	//! Initialisierung aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node)=0;
	//! Konstruktor
	HSSTypeConversion();
	//! Destruktor
	virtual ~HSSTypeConversion();

};

#endif // _HSSTYPECONVERSION_H_
