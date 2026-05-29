/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// PropertyMap.cpp: Implementierung der Klasse PropertyMap.
#ifdef WIN32
#pragma warning(disable:4786)
#endif
#include "PropertyMap.h"
#include <fstream>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

#include "Logger.h"

// Konstruktion/Destruktion
PropertyMap::PropertyMap()
{
    dirty=false;
    write_spaces=true;
}
PropertyMap::~PropertyMap()
{
}

void PropertyMap::SetWriteSpacesFlag(bool b)
{
    write_spaces = b;
}

int PropertyMap::WriteToFile(std::string filename)
{
    std::string delimiter=write_spaces?" = ":"=";
    if(filename.empty())filename=this->filename;
    std::ofstream os(filename.c_str());
    if(os.good()){
    
        for(t_sectionMap::iterator sec_it=section_map.begin();sec_it!=section_map.end();sec_it++){
            const std::string& section=sec_it->first;
            if(!section.empty())os<<"["<<section<<"]"<<std::endl;
            for(Section::iterator key_it=sec_it->second.begin();key_it!=sec_it->second.end();key_it++){
                const std::string& key=key_it->first;
                const std::string& value=key_it->second;
                os<<key<<delimiter<<value<<std::endl;
            }
            os<<std::endl;
        }
        dirty=false;
        return 1;
    }else{
    	LOG(Logger::LOG_ERROR, "Property file \"%s\" could not be written", filename.c_str());
	    return -1;
    }
}
int PropertyMap::ReadFromFile(std::string filename)
{
	if(filename.empty())return -1;
    this->filename=filename;
	std::ifstream is(filename.c_str());
    std::string section;
    current_section.clear();
	if(is.good()){
		char line[256];
		while(!is.eof()){
			is.getline(line, 256);
			char* left=line;
			char* end=line+strlen(line);
            // remove trailing line end characters
            while( (end>left) && (*(end-1)<' ') )end--;
            *end=0;
			char* right=strchr(line, '=');
			if(!right)right=end;
			*right=0;
			std::string key=left;
			string_trim(&key);
			if(key.empty() || key[0]=='#')continue;
            if(key[0]=='['){
                section=line+1;
                string_trim(&section);
                if(section.size() && section[0]=='[')section=section.substr(1);
                if(section.size() && section[section.size()-1]==']')section=section.substr(0, section.size()-1);
                continue;
            }
			left=right+1;
			right=strchr(left, '#');
			if(!right)right=end;
			*right=0;
			std::string value=left;
			string_trim(&value);
        	section_map[section][key]=value;
		}
        dirty=false;
		return 1;
	}else{
		//LOG(Logger::LOG_DEBUG, "Property file \"%s\" not found", filename.c_str());
		return -1;
	}
}
int PropertyMap::GetIntValue(const std::string &key, int default_value)const
{
	const std::string& s=GetStringValue(key, "");
	if(s.empty())return default_value;
	return strtol(s.c_str(), NULL, 0);
}
double PropertyMap::GetFloatValue(const std::string &key, double default_value)const
{
	const std::string& s=GetStringValue(key, "");
	if(s.empty())return default_value;
	return atof(s.c_str());
}
const std::string PropertyMap::GetStringValue(const std::string &key, const std::string &default_value)const
{
    t_sectionMap::const_iterator it=section_map.find(current_section);
    if(it==section_map.end()){
		//LOG(Logger::LOG_DEBUG, "Warning: section \"%s\" not defined", current_section.c_str());
		return default_value;
    }
	Section::const_iterator it2=it->second.find(key);
	if(it2==it->second.end()){
		//LOG(Logger::LOG_DEBUG, "Warning: property \"%s\" not defined", key.c_str());
		return default_value;
	}
	return it2->second;
}
void PropertyMap::SetValue(const std::string& key, const std::string& value)
{
    if(section_map[current_section][key]!=value){
        section_map[current_section][key]=value;
        dirty=true;
    }
}
void PropertyMap::SetIntValue(const std::string& key, const int& value)
{
	std::string sVal = IntToStr(value);
	SetValue(key, sVal);
}
bool PropertyMap::HasSection(const std::string& section_name)const
{
	return section_map.find( section_name ) != section_map.end();
}

bool PropertyMap::IsDirty()const
{
    return dirty;
}

void PropertyMap::Clear()
{
	section_map.clear();
}

PropertyMap::StringList PropertyMap::ListSections()const
{
    StringList l;
    for(t_sectionMap::const_iterator it=section_map.begin();it!=section_map.end();it++)l.push_back(it->first);
    return l;
}

PropertyMap::Section PropertyMap::GetSection(const std::string& section)const
{
    t_sectionMap::const_iterator it=section_map.find(section);
    if(it==section_map.end())return Section();
    else return it->second;
}

bool PropertyMap::SetCurrentSection(const std::string& section_name)
{
    current_section=section_name;
    return true;
}

const std::string& PropertyMap::GetCurrentSection()const
{
    return current_section;
}

std::string PropertyMap::GetBinaryValue(const std::string &key, const std::string &default_value)const
{
    std::string hex_value=GetStringValue(key, "*");
    if(hex_value=="*")return default_value;
    std::string binary_value;
    binary_value.reserve(hex_value.size()/2);
    for(unsigned int i=0;i<hex_value.size();i+=2)
    {
        binary_value.append(1, (char)strtol(hex_value.substr(i, 2).c_str(), NULL, 16));
    }
    return binary_value;
}

void PropertyMap::SetBinaryValue(const std::string& key, const std::string& value)
{
    std::string hex_value;
    char buf[4];
    for(unsigned int i=0;i<value.size();i++)
    {
        snprintf(buf, sizeof(buf), "%02X", ((int)value[i])&0xff);
        hex_value+=buf;
    }
    SetValue(key, hex_value);
}
