/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _METADATA_STORE_H_
#define _METADATA_STORE_H_

#include "dllexport.h"

#include <XmlRpc.h>

//! Diese Klasse speichert Metadaten zu Objekten als Schlüssel-Werte-Paare
/*!
 *  Der Schlüssel ist ein String, während der Wert ein XmlRpcValue ist. Objekte werden ebenfalls durch einen
 *  String bezeichnet. Sonderzeichen werden innerhalb einer Objekt-ID nicht unterschieden. "Ein-Objekt" und "Ein_Objekt"
 *  bezeichnen also das selbe Objekt.
 *  Metadaten werden gespeichert, indem in einem Dateisystem-Verzeichnis pro Objekt eine Datei angelegt wird.
 */
class DLLEXPORT MetadataStore
{
public:
    //! Konstruktor
    MetadataStore(void);
    //! Destruktor
    ~MetadataStore(void);
    //! Setzt das Verzeichnis, in dem die Metadatendateien abgelegt werden
    void SetDirectory(const char* dir);
    //! Setzt ein Metadatum zu einem Objekt
    bool Set(const char* object_id, const char* data_id, XmlRpc::XmlRpcValue& value);
    //! Liest ein Metadatum zu einem Objekt
    bool Get(const char* object_id, const char* data_id, XmlRpc::XmlRpcValue* value);
    //! Liest alle Metadaten eines Objektes und liefert diese in Form einer XmlRpc-Struct zurück
    bool GetAll(const char* object_id, XmlRpc::XmlRpcValue* value);
    //! Löscht ein Metadatum eines Objektes
    bool Delete(const char* object_id, const char* data_id);
    //! Delete all metadata for an object.
    /*! One wildcard "*" at the end of \c object_id is permitted
     */
    bool Delete(const char* object_id);
		//! Setzt flüchtige Metadaten
		bool SetVolatile(const char* data_id, XmlRpc::XmlRpcValue& value);
		//! Liest flüchtige Metadaten
		bool GetVolatile(const char* data_id, XmlRpc::XmlRpcValue* value);
		//! Löscht flüchtige Metadaten
		bool DeleteVolatile(const char* data_id);		
private:
    //! Speichert alle Metadaten zu einem Objekt in das Dateisystem
    bool Save(const char* object_id, XmlRpc::XmlRpcValue& data);
    //! Generiert den Dateinamen (ohne Verzeichnis) zu einer Objekt-ID
    std::string FilenameFromId(const char* object_id);
    //! Verzeichnispfad für die Objektdateien
    std::string directory;
		//! Flüchtige Metadaten
		XmlRpc::XmlRpcValue volatileData;
};

#endif
