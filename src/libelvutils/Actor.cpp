/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <iostream>
#include "Actor.h"
#include "Logger.h"

using namespace std;

#define DEFAULT_STACK_SIZE (512 * 1024)

namespace EQ3
{
    Actor::Actor():m_thread(0)
    {
        //m_messages = new list<ActorMessage*>();
        m_isRunning = false;
        m_conditionMet = false;

        pthread_mutex_init(&m_access, NULL);
        pthread_cond_init(&m_newMessage_cond,NULL);
    }

    Actor::~Actor()
    {
    	//LOG(Logger::LOG_DEBUG, "Actor::~Actor()");
        Stop();
        pthread_mutex_destroy(&m_access);
        pthread_cond_destroy(&m_newMessage_cond);
    }

    void Actor::Start()
    {
        pthread_attr_t attributes;
        pthread_attr_init(&attributes);
        pthread_attr_setstacksize(&attributes, DEFAULT_STACK_SIZE);
        pthread_create(&m_thread, &attributes, Actor::ThreadFunction, (void*) this);
        pthread_attr_destroy(&attributes);
    }

    void Actor::Run()
    {
        pthread_mutex_lock(&m_access);
        m_isRunning = true;
        while (m_isRunning)
        {
        	while(!m_conditionMet)
        	{//just wait when no events pending
        		pthread_cond_wait(&m_newMessage_cond,&m_access);
        	}
        	m_conditionMet = false;
        	if(m_isRunning) {
				list<ActorMessage*> messages(m_messages);
				//m_messages = new list<ActorMessage*>();
				m_messages.clear();
				pthread_mutex_unlock(&m_access);
				Handle(messages);
				Delete(&messages);
        	}
        	else {
        		break;
        	}
            pthread_mutex_lock(&m_access);
        }
        Delete(&m_messages);
        pthread_mutex_unlock(&m_access);
        LOG(Logger::LOG_DEBUG, "Actor::Run(): returning");
    }

    void Actor::Handle(list<ActorMessage*> const& messages)
    {
    }



    Actor& Actor::operator<<(ActorMessage const& message)
    {
        pthread_mutex_lock(&m_access);
        if (m_isRunning)
        {
            if(m_messages.size() >= 200) { //Remove oldest message to avoid collecting messages until memory exhaust.
                LOG(Logger::LOG_WARNING, "Actor: Maximung message queue size reached. Dropping oldest message.");
                list<ActorMessage*>::iterator itFirst = m_messages.begin();
                delete *itFirst;
                m_messages.erase(itFirst);
            }
            m_messages.push_back(message.Clone());
            m_conditionMet = true; // flag for waiting events
            pthread_cond_signal(&m_newMessage_cond);
        }
        pthread_mutex_unlock(&m_access);
        return *this;
    }

    void Actor::Stop()
    {
    	//LOG(Logger::LOG_DEBUG, "Actor::Stop()");
        pthread_mutex_lock(&m_access);
        if (m_isRunning)
        {
            m_isRunning = false;
			m_conditionMet = true; // flag for waiting events
            pthread_cond_signal(&m_newMessage_cond);
            //LOG(Logger::LOG_DEBUG, "Actor::Stop(): Emitted signal");
        }
        pthread_mutex_unlock(&m_access);

        void* status;
        LOG(Logger::LOG_ALL, "Actor::Stop(): Waiting for thread joining");
        pthread_join(m_thread, &status);
        LOG(Logger::LOG_ALL, "Actor::Stop(): Thread joined");

    }

    void* Actor::ThreadFunction(void *arg)
    {
        Actor* actor = (Actor*) arg;

        try
        {
            actor->Run();
        }
        catch (std::exception &ex)
        {
            // do something
        }

        return NULL;
    }

    void Actor::Delete(list<ActorMessage*> *messages)
    {
        list<ActorMessage*>::iterator it;
        for(it = messages->begin(); it != messages->end(); it++)
        {
            ActorMessage* message = *it;
            if(message != NULL) {
            	delete message;
            }
        }
        messages->clear();
        //delete messages;
    }


}
