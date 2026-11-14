/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <string>
#include <queue>
#include <map>
#include "tinythread.h"
#include <stdint.h>

class MultithreadTest
{
    public:
        MultithreadTest( const char* dev );
        ~MultithreadTest();
        
        bool Run( std::size_t byteCount);
    private:
        typedef std::map<int, uint32_t> HistogramType;
        int OpenPort();
        bool FlushPendingData();
        bool SetPriority( int fd, unsigned int latency );
    
		bool DoRxLoop(std::size_t byteCount);
		void RegularTxThreadFunction();
		static void sRegularTxThreadFunction(void* arg)
        {
            reinterpret_cast<MultithreadTest*>(arg)->RegularTxThreadFunction();
        }

		void FastTxThreadFunction();
		static void sFastTxThreadFunction(void* arg)
        {
            reinterpret_cast<MultithreadTest*>(arg)->FastTxThreadFunction();
        }
        
        static int FrameDuration( int bytes );
        static struct timespec GetTime();
        static int TimeDiff( const struct timespec& start, const struct timespec& end );
        static void PrintHistogram( const HistogramType& histogram );

        std::string _device;
    
        int _fdRegular;
        int _fdFast;
    
		tthread::mutex _mutex;
		tthread::condition_variable _condition;
		typedef tthread::lock_guard<tthread::mutex> LockGuard;

		tthread::thread _regularTxThread;
		tthread::thread _fastTxThread;
        
		bool _exit;
        std::size_t _txCounter;
        std::size_t _rxCounter;
        std::size_t _interruptCounter;
    
        std::queue<char> _queuePendingRxDataRegular;
        std::queue<char> _queuePendingRxDataFast;
        
        HistogramType _histogramFast;
        HistogramType _histogramFastTotal;
        HistogramType _histogramRegular;
        HistogramType _histogramRegularSuccess;
        
};
