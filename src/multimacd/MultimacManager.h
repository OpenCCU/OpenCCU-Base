/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "FastMacResponder.h"
#include "MacController.h"
#include "SubsystemManager.h"
#include "Enums.h"
#include "Subsystem.h"
#include "tinythread.h"
#include <PropertyMap.h>
#include <map>

class MultimacManager
{
public:
    static MultimacManager& Instance();

	~MultimacManager(void);
	bool Run( const PropertyMap& config );
	bool Exit();
	SubsystemManager& GetSubsystemManager();
	const PropertyMap& GetConfiguration();
private:
	MultimacManager(void);
	FastMacResponder _fastMacResponder;
	MacController _macController;
	SubsystemManager _subsystemManager;
	typedef std::map<MacSubsystemType, Subsystem*> MapSubsystems;
	MapSubsystems _mapSubsystems;
    static MultimacManager* _instance;
	PropertyMap _configuration;


	tthread::mutex _mutex;
	tthread::condition_variable _condition;
	typedef tthread::lock_guard<tthread::mutex> LockGuard;

	bool _run;

	void writeInitDoneFile(const PropertyMap& config_data);
};

