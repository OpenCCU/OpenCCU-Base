/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * PThreadCondition.h
 *
 *  Created on: Jan 19, 2016
 *      Author: user
 */

#ifndef PTHREADCONDITION_H_
#define PTHREADCONDITION_H_

#include <pthread.h>
#include <stdint.h>

class PThreadCondition
{
public:
	PThreadCondition();
	virtual ~PThreadCondition();

	//True: was signaled, false timeout
	bool wait(const uint32_t msTimeout);
	void signal();

private:
	pthread_cond_t condition;
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutexAttr;
	volatile bool predicate;
};

#endif /* PTHREADCONDITION_H_ */
