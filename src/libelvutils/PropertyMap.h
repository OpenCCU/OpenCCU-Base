/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// PropertyMap.h: Schnittstelle für die Klasse PropertyMap.
//
//////////////////////////////////////////////////////////////////////

#ifndef _PROPERTYMAP_H_
#define _PROPERTYMAP_H_

#include "dllexport.h"
#include <map>
#include <vector>
#include <string>

class ELVUTILS_DLLEXPORT PropertyMap  
{
public:
    typedef std::vector<std::string> StringList;
	typedef std::map<std::string, std::string> Section;
	void Clear();
	const std::string GetStringValue(const std::string& key, const std::string& default_value="")const;
	int GetIntValue(const std::string& key, int default_value=0)const;
	double GetFloatValue(const std::string& key, double default_value=0.0)const;
	std::string GetBinaryValue(const std::string& key, const std::string& default_value="")const;
    void SetValue(const std::string& key, const std::string& value);
	void SetIntValue(const std::string& key, const int& value);
    void SetBinaryValue(const std::string& key, const std::string& value);
	inline void SetStringValue(const std::string& key, const std::string& value)
	{ 
		SetValue(key, value);
	};
    StringList ListSections()const;
    Section GetSection(const std::string& section)const;
    bool SetCurrentSection(const std::string& section_name);
    const std::string& GetCurrentSection()const;
	int ReadFromFile(std::string filename);
    int WriteToFile(std::string filename="");
    bool IsDirty()const;
    void SetWriteSpacesFlag(bool b);
	bool HasSection(const std::string& section_name)const;
	typedef std::map<std::string, Section> t_sectionMap;
	PropertyMap();
	virtual ~PropertyMap();
protected:
    t_sectionMap section_map;
    std::string filename;
    std::string current_section;
    bool dirty;
    bool write_spaces;
};

#endif
