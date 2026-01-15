/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485LogicalTypeAddress.h: Schnittstelle für die Klasse HS485LogicalTypeAddress.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HS485LOGICALTYPEADDRESS_H__D1407B50_88CD_4E0C_8CAF_0C3452688B29__INCLUDED_)
#define AFX_HS485LOGICALTYPEADDRESS_H__D1407B50_88CD_4E0C_8CAF_0C3452688B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <HSSLogicalType.h>

//! Logischer Typ für Geräte- oder Kanaladressen. Nach aussen erscheint hier die Seriennummer.
class HS485LogicalTypeAddress : public HSSLogicalType  
{
public:
	virtual bool GetDescription(XmlRpc::XmlRpcValue* val);
	virtual bool EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue* val, operation_t op);
	static bool CheckCreationTag(const char* tag);
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	HS485LogicalTypeAddress();
	virtual ~HS485LogicalTypeAddress();
	XmlRpc::XmlRpcValue GetDefault();
};

#endif // !defined(AFX_HS485LOGICALTYPEADDRESS_H__D1407B50_88CD_4E0C_8CAF_0C3452688B29__INCLUDED_)
