/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * JSONObject.h
 *
 *  Created on: Jun 17, 2015
 *      Author: user
 */

#ifndef JSONOBJECT_H_
#define JSONOBJECT_H_

#include <string>
#include <map>

class JSONObject
{
public:
	JSONObject();
	virtual ~JSONObject();
	JSONObject(const std::string& jsonObjectString);

	int getInt(const std::string& key) const;
	double getDouble(const std::string& key) const;
	std::string getString(const std::string& key) const;
	bool getBool(const std::string& key) const;
	JSONObject getObj(const std::string& key) const;

	bool contains(const std::string key) const;
	bool isValid() const { return valid; }

	std::string toString() const;
private:
	std::map<std::string, std::string> members;
	bool valid;

	bool parseJSONString(const std::string& s);
	bool parseBlock(const std::string& block);
	std::string parseKey(const std::string& s);
	bool parseValue(const std::string& s, std::string& value);
	std::string getMemberValue(const std::string& key) const;
	void trim(std::string& str);
	void removeQuotationMarks(std::string& str);
	int countSubstr(const std::string& str, const std::string& cntStr);
	bool isBlockValue(const std::string& value);

};

#endif /* JSONOBJECT_H_ */
