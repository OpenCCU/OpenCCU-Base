/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
*
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "MultimacManager.h"
#include "TrafficLogger.h"

#ifndef WIN32
#include <OSCompat.h>
#include <Logger.h>
#include <fstream>
#endif

/*static*/ MultimacManager* MultimacManager::_instance = NULL;

/*static*/ MultimacManager& MultimacManager::Instance()
{
	if( !_instance )
	{
		_instance = new MultimacManager();
	}
	return *_instance;
}

MultimacManager::MultimacManager(void)
:_run(false)
{
}


MultimacManager::~MultimacManager(void)
{
}

SubsystemManager& MultimacManager::GetSubsystemManager()
{
	return _subsystemManager;
}

const PropertyMap& MultimacManager::GetConfiguration()
{
	return _configuration;
}


bool MultimacManager::Run( const PropertyMap& config )
{
	std::string coprocessorDevice = config.GetStringValue("Coprocessor Device Path");
	if( coprocessorDevice.empty() )
	{
		return false;
	}

	_configuration = config;
	_run = true;

	TrafficLogger::Instance().Configure( config.GetIntValue("Traffic Log") != 0, config.GetStringValue("Traffic Log Directory") );

	_subsystemManager.SetDownstreamFrameSink( _macController );
	_macController.SetUpstreamFrameSink( _subsystemManager );
	_fastMacResponder.SetUpstreamFrameSink( _macController );

	_fastMacResponder.Start(coprocessorDevice);
	_macController.Start(coprocessorDevice);

	_subsystemManager.Start();

	LockGuard lock( _mutex );
	bool initializationDone = false;
	while( _run )
	{
		if(initializationDone) {
			_condition.wait_for( _mutex, tthread::chrono::seconds( 5 ) );
		}
		else {
			_condition.wait_for( _mutex, tthread::chrono::seconds( 1 ) );
			if(_macController.IsInitializationDone()) {
				writeInitDoneFile(_configuration);
				initializationDone = true;
			}
		}

	}

	_subsystemManager.Stop();
	_fastMacResponder.Stop();
	_macController.Stop();

	return true;
}

bool MultimacManager::Exit()
{
	LockGuard lock( _mutex );
	if( !_run )
	{
		return false;
	}
	_run = false;
	_condition.notify_all();
	return true;
}

void MultimacManager::writeInitDoneFile(const PropertyMap& config_data) {
#ifndef WIN32
//currently only Linux OS feature
	std::string path = config_data.GetStringValue("StatusfileDir", "/var/status/");
	path = OSCompat::FixPath(path);
	OSCompat::MakeDirectory(path.c_str());
	path.append("multimacd.status");
	std::ofstream ofstream;
	ofstream.open(path.c_str());
	if(ofstream.is_open()) {
		ofstream << getpid();
		ofstream.flush();
		ofstream.close();
	}
	else {
		LOG(Logger::LOG_INFO, "Cannot write multimacd status file to %s", path.c_str());
	}
#endif
}

