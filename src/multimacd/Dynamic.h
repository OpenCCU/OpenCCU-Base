/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once

#ifdef WIN32
  #define _CRTDBG_MAP_ALLOC
  #include <stdlib.h>
  #include <crtdbg.h>
  #ifdef _DEBUG
    #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
    #endif
  #endif  // _DEBUG
#endif // WIN32

#include <map>
#include <string>
#include <typeinfo>


namespace Dynamic {
	class IMachine
	{
	public:
		virtual void* Create( void* key ) = 0;
		virtual ~IMachine(){};
	};

	class Factory
	{
	public:
		static Factory& Instance();
		void AddMachine( const std::type_info& baseType, const std::type_info& keyType, IMachine* machine );
		void* Create( const std::type_info& baseType, const std::type_info& keyType, void* key );
	private:
		Factory();
		std::string CombineKeys( const std::type_info& baseType, const std::type_info& keyType );
		static Factory* _instance;
		typedef std::multimap<std::string, IMachine*> MapType;
		MapType _mapMachines;
	};

	template <class base_t, class derived_t, class key_t>
	class DynamicMachine : IMachine
	{
	public:
		DynamicMachine(void)
		{
			Factory::Instance().AddMachine( typeid(base_t), typeid(key_t), this );
		}
		virtual ~DynamicMachine(void){}
		void* Create( void* key )
		{
			if( derived_t::CheckCreationKey( *((key_t*)key)) )return new derived_t();
		}
	};

	template <class base_t, class derived_t, class key_t>
	class StaticMachine : IMachine
	{
	public:
		StaticMachine( key_t key )
		{
			_key = key;
			Factory::Instance().AddMachine( typeid(base_t), typeid(key_t), this );
		}
		virtual ~StaticMachine(void){}
		void* Create( void* key )
		{
			if( _key == *((key_t*)key) )
			{
				derived_t* p = new derived_t();
				return p;
			}
			
			return NULL;
		}
	private:
		key_t _key;
	};

	template <class base_t>
	class Order
	{
	public:
		template <class key_t>
		static base_t* Create( key_t key )
		{
			void* obj = Factory::Instance().Create( typeid(base_t), typeid(key_t), &key );
			return dynamic_cast<base_t*>((base_t*)obj);
		}
	};

}
