/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "Dynamic.h"

namespace Dynamic {

	/*static*/ Factory* Factory::_instance = NULL;

	/*static*/ Factory& Factory::Instance()
	{
		if( !_instance )
		{
			_instance = new Factory();
		}
		return *_instance;
	}

	Factory::Factory()
	{
	}

	std::string Factory::CombineKeys( const std::type_info& baseType, const std::type_info& keyType )
	{
		std::string combinedKey = baseType.name();
		combinedKey += ".";
		combinedKey += keyType.name();
		return combinedKey;
	}

	void Factory::AddMachine( const std::type_info& baseType, const std::type_info& keyType, IMachine* machine )
	{
		_mapMachines.insert( MapType::value_type( CombineKeys( baseType, keyType ), machine ));
	}

	void* Factory::Create( const std::type_info& baseType, const std::type_info& keyType, void* key )
	{
		std::pair<MapType::iterator,MapType::iterator> range = _mapMachines.equal_range( CombineKeys( baseType, keyType ) );
		for( MapType::iterator it = range.first; it != range.second; it++ )
		{
			void* obj = it->second->Create( key );
			if( obj )return obj;
		}
		return NULL;
	}

}
