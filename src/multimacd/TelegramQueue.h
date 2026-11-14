/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include <queue>
#include "tinythread.h"

/// <summary>
/// Generic thread safe queue implementation for queueing different types of telegrams.
/// </summary>

template <typename T>
class TelegramQueue
{
public:

	/// <summary>
	/// Constructor.
	/// </summary>
	///
	/// <param name="maxEntries">
	/// (optional) the maximum number of entries. Default = 64.
	/// </param>

	TelegramQueue(unsigned int maxEntries = 64)
	{
		_maxEntries = maxEntries;
		_signalled = false;
	}

	/// <summary>
	/// Destructor.
	/// </summary>

	virtual ~TelegramQueue(void){};

	/// <summary>
	/// Adds an element to the queue.
	/// </summary>
	///
	/// <param name="element">
	/// The element to add.
	/// </param>
	///
	/// <returns>
	/// true if it succeeds, false if it fails.
	/// </returns>

	bool Add(T element)
	{
		LockGuard lock(_mutex);
		if( _queue.size() >= _maxEntries )return false;
		_queue.push( element );
		_condition.notify_all();
		return true;
	}

	/// <summary>
	/// Gets the next element from the queue.
	/// </summary>
	///
	/// <param name="timeout">
	/// The timeout in ms to wait until a new element arrives.
	/// </param>
	/// <param name="remove">
	/// (optional) If true, the element is removed from the queue.
	/// </param>
	///
	/// <returns>
	/// The next element.
	/// </returns>

	T GetNextElement( uint32_t timeout, bool remove = true )
	{
		LockGuard lock(_mutex);
		if( _queue.empty() && !_signalled)
		{
			_condition.wait_for( _mutex, tthread::chrono::milliseconds( timeout ) );
		}
		_signalled = false;
		if( _queue.empty() )return T(0);
		T element = _queue.front();
		if( remove )_queue.pop();
		return element;
	}

	/// <summary>
	/// Gets the number of queued elements.
	/// </summary>
	///
	/// <returns>
	/// The number of elements.
	/// </returns>

	unsigned int GetNumberOfElements()
	{
		LockGuard lock(_mutex);
		return _queue.size();
	}

	/// <summary>
	/// Signals this object causing all wait operations to end prematurely.
	/// </summary>

	void Signal()
	{
		LockGuard lock(_mutex);
		_signalled = true;
		_condition.notify_all();
	}
private:
	std::queue<T> _queue;

	tthread::condition_variable _condition;
	tthread::mutex _mutex;
	typedef tthread::lock_guard<tthread::mutex> LockGuard;
	unsigned int _maxEntries;
	bool _signalled;
};
