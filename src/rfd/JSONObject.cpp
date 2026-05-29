/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * JSONObject.cpp
 *
 *  Created on: Jun 17, 2015
 *      Author: user
 */

#include "JSONObject.h"
#include <stdlib.h>

JSONObject::JSONObject()
: valid(true)
{

}

JSONObject::~JSONObject()
{
	// TODO Auto-generated destructor stub
}

JSONObject::JSONObject(const std::string& jsonObjectString)
{
	valid = parseJSONString(jsonObjectString);
}

bool JSONObject::contains(const std::string key) const
{
	return (members.find(key) != members.end());
}

int JSONObject::getInt(const std::string& key) const
{
	const std::string s = getMemberValue(key);
	if (s.empty())
	{
		return 0;
	}
	else
	{
		return atoi(s.c_str());
	}
}

double JSONObject::getDouble(const std::string& key) const
{
	const std::string s = getMemberValue(key);
	if (s.empty())
	{
		return double(0);
	}
	else
	{
		return strtod(s.c_str(), NULL);
	}
}

bool JSONObject::getBool(const std::string& key) const
{
	const std::string s = getMemberValue(key);
	if(s.empty()) {
		return false;
	}
	else {
		const char c = s.at(0);
		if(isdigit(c)) {
			return (c != '0');
		}
		if(c=='t' || c =='T') {
			return true;
		}
		else {
			return false;
		}
	}
}

std::string JSONObject::getString(const std::string& key) const
{
	return getMemberValue(key);
}

bool JSONObject::parseJSONString(const std::string& s)
{
	if(s.empty()) {//empty strings result in empty objects
		return true;
	}
	//find first tag
	std::string::size_type bStart, bEnd;

	bStart = s.find("{");
	if(bStart == std::string::npos) {
		bStart = s.find("[");
		if(bStart == std::string::npos) {
			return false;
		}
	}
	bEnd = s.rfind("}");
	if(bEnd == std::string::npos) {
		bEnd = s.rfind("]");
		if(bEnd == std::string::npos) {
			return false;
		}
	}
	return parseBlock(s.substr(bStart, bEnd-bStart+1));
}

bool JSONObject::parseBlock(const std::string& block)
{
	std::string s(block);
	trim(s);
	if(s.length() < 2) {
		return false;
	}
	s = s.substr(1, s.length()-2);
	trim(s);
	if(s.empty()) {
		return true;
	}
	//check how many key, value pairs are there
	const int kvPairCount = countSubstr(s, ":");
	for(int i = 0; i < kvPairCount; i++) {
		std::string key = parseKey(s);
		if(key.empty()) {
			return false;
		}
		std::string value;
		parseValue(s, value);
		std::pair<std::string, std::string> pair;
		pair.first = key;
		pair.second = value;
		members.insert(pair);
		if(isBlockValue(value)) {
			break;
		}
		else {
			s = s.substr(s.find(":")+1);//remove key
			std::string::size_type keyvalueEnd = s.find(value);//remove value
			if(keyvalueEnd != std::string::npos) {
				keyvalueEnd += (value.length()+1);
				if(keyvalueEnd < s.length()-1) {
					s = s.substr(keyvalueEnd);
					trim(s);
					if(s.at(0) == ',') {
						s.erase(0, 1);
					}
					trim(s);
					continue;
				}
			}
			return false;
		}
	}
	return true;
}

std::string JSONObject::parseKey(const std::string& s) {
	if(s.empty()) {
		return "";
	}
	std::string::size_type index = s.find(":");
	if(index == std::string::npos) {
		return "";
	}
	else {
		std::string key = s.substr(0, index);
		trim(key);
		removeQuotationMarks(key);
		return key;
	}
}

bool JSONObject::parseValue(const std::string& s, std::string& value)
{
	value.clear();
	std::string val(s);
	//get value part
	std::string::size_type idx = val.find(":");
	if(idx+1 < val.length()) {//extract part after first :
		val = val.substr(idx+1);
		trim(val);
		if(val.empty()) {
			return true;
		}
	}
	else {//empty value
		return true;
	}

	if(val.at(0) == '{') {
		trim(val);
		if(val.at(0)=='"' || val.at(0) == '\'') {
			removeQuotationMarks(val);
		}
		value.assign(val);
		return true;
	}
	else if(val.at(0)=='[') {
		std::string::size_type end = val.find("]");
		if(end == std::string::npos) {
			return false;
		}
		val = val.substr(0, end);
		value.assign(val);
		return true;
	}
	else {
		trim(val);
		if(val.at(0) == '"' || val.at(0) == '\'') {//string type... search until second "
			char searchChar = '"';
			if(val.at(0) == '\'') {
				searchChar = '\'';
			}
			std::string::size_type idx = val.find(searchChar, 1);
			if(idx == std::string::npos || (idx+1) > val.length()) {
				return false;
			}
			val = val.substr(0, idx+1);
			trim(val);
			removeQuotationMarks(val);
			value.assign(val);
			return true;
		}
		else {
			//parse until , or end of string
			std::string::size_type idx = val.find(",");
			if(idx != std::string::npos) {
				val = val.substr(0, idx);
				trim(val);
				value.assign(val);
				return true;
			}
			else {
				trim(val);
				value.assign(val);
				return true;
			}
		}
	}
}

bool JSONObject::isBlockValue(const std::string& value)
{
	std::string s(value);
	trim(s);
	if(s.empty()) {
		return false;
	}
	if(s.at(0) == '{' && s.at(s.length()-1) == '}') {
		return true;
	}
	else {
		return false;
	}
}

JSONObject JSONObject::getObj(const std::string& key) const
{
	std::string s = getMemberValue(key);
	return JSONObject(s);
}

std::string JSONObject::getMemberValue(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator iter = members.find(key);
	if (iter == members.end())
	{
		return "";
	}
	else
	{
		return (*iter).second;
	}
}

void JSONObject::trim(std::string& str)
{
	while(!str.empty()) {
		if(isspace(str.at(0))) {
			str.erase(0, 1);
		}
		else {
			break;
		}
	}
	while(!str.empty()) {
		if(isspace(str.at(str.size()-1))) {
			str.erase(str.size()-1, 1);
		}
		else {
			break;
		}
	}
}

std::string JSONObject::toString() const
{
	std::string s;
	std::map<std::string, std::string>::const_iterator iter;
	for(iter = members.begin() ;  iter != members.end() ; ++iter) {
		s.append((*iter).first);
		s.append(" : ");
		s.append((*iter).second);
		s.append("\n");
	}
	return s;
}

void JSONObject::removeQuotationMarks(std::string& str)
{
	if(str.length() < 2) {
		return;
	}
	std::string::size_type iA = str.find("\"");
	std::string::size_type iB = str.rfind("\"");
	if(iB <= iA) {
		iA = str.find("'");
		iB = str.rfind("'");
		if(iB <= iA) {
			return;
		}
	}
	str = str.substr(iA+1, iB-iA-1);
	trim(str);
}

int JSONObject::countSubstr(const std::string& str, const std::string& cntStr)
{
	std::string::size_type i = 0;
	unsigned int cnt = 0;
	const unsigned int maxIndex = str.length() - 1;
	while(i < maxIndex && i != std::string::npos) {
		i = str.find(cntStr, i);
		if(i != std::string::npos) {
			cnt++;
			i++;
		}
		else {
			break;
		}

	}
	return cnt;
}


