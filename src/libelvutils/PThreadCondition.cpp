/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * PThreadCondition.cpp
 *
 *  Created on: Jan 19, 2016
 *      Author: user
 */

#include "PThreadCondition.h"
#include "errno.h"
#include "utils.h"

PThreadCondition::PThreadCondition()
: predicate(false)
{
	pthread_cond_init(&condition, NULL);
	pthread_mutexattr_init(&mutexAttr);
	pthread_mutex_init(&mutex, &mutexAttr);
}

PThreadCondition::~PThreadCondition()
{
	pthread_cond_destroy(&condition);
	pthread_mutex_destroy(&mutex);
	pthread_mutexattr_destroy(&mutexAttr);
}

void PThreadCondition::signal() {
	pthread_mutex_lock(&mutex);
	predicate = true;
	pthread_cond_signal(&condition);
	pthread_mutex_unlock(&mutex);
}

bool PThreadCondition::wait(uint32_t msTimeout) {
	timespec ts;
	ts = millis2abstime(msTimeout);
	int rc = 0;
	pthread_mutex_lock(&mutex);
	while( (!predicate) &&  rc == 0) {
		rc = pthread_cond_timedwait(&condition, &mutex, &ts);
	}
	predicate = false;
	pthread_mutex_unlock(&mutex);
	return rc != ETIMEDOUT;
}
