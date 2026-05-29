/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _ACTOR_H_
#define _ACTOR_H_

#include <pthread.h>
#include <list>
#include "ActorMessage.h"
#include "dllexport.h"

using namespace std;

namespace EQ3
{
	class ELVUTILS_DLLEXPORT Actor
	{
	public:
		Actor();
		virtual ~Actor();
		void Start();
		Actor& operator<<(ActorMessage const& message);		
	protected:
		void Stop();
		virtual void Handle(list<ActorMessage*> const& messages);
	private:
		void Run();
		void Delete(list<ActorMessage*> *messages);
		static void* ThreadFunction(void* arg); 
		volatile bool m_isRunning;
		volatile bool m_conditionMet; //Flag to signal when new events occurs while old events transfered via XmlRpc
		//list<ActorMessage*> *m_messages; <-- dangerous because we don't have a custom copy constructor and assignment operator!!!
		list<ActorMessage*> m_messages;
		pthread_t m_thread;
		pthread_cond_t m_newMessage_cond;
		pthread_mutex_t m_access;
	};
}

#endif
