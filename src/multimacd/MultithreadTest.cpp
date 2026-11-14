/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "MultithreadTest.h"
#include <iostream>
#include <iomanip>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "Sysutils.h"


/* Use 'e' as magic number */
#define MXS_AUART_IOC_MAGIC  'u'

/*
 * S means "Set" through a ptr,
 * G means "Get": reply by setting through a pointer
 */
#define MXS_AUART_IOCSPRIORITY _IOW(MXS_AUART_IOC_MAGIC,  1, uint32_t)
#define MXS_AUART_IOCGPRIORITY _IOR(MXS_AUART_IOC_MAGIC,  2, uint32_t)

MultithreadTest::MultithreadTest( const char* dev )
{
    _device = dev;
    _fdRegular = -1;
    _fdFast = -1;
    _exit = false;
    _txCounter = 0;
    _rxCounter = 0;
    _interruptCounter = 0;
    
}
        
MultithreadTest::~MultithreadTest()
{
    if( _fdRegular >= 0 )
    {
        close( _fdRegular );
    }
    if( _fdFast >= 0 )
    {
        close( _fdFast );
    }
}
        
int MultithreadTest::OpenPort()
{
	int fd = open (_device.c_str(), O_RDWR | O_NONBLOCK);
	if (fd <= 0)
	{
        perror( "open" );
		return -1;
	}
    return fd;
}

bool MultithreadTest::FlushPendingData()
{
	fd_set inFd, outFd, excFd;
	FD_ZERO(&inFd);
	FD_ZERO(&outFd);
	FD_ZERO(&excFd);

	FD_SET(_fdRegular, &inFd);
	
    int received = 0;
    while( true )
    {
        // Check for events
        int nEvents;
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 200000;
        nEvents = select(_fdRegular+1, &inFd, &outFd, &excFd, &tv);
    
        if( nEvents < 0 )
        {
            perror( "select" );
            return false;
        }
        if( nEvents == 0 )
        {
            break;
        }
        
        char buffer[64];
            
        int count = read( _fdRegular, buffer, sizeof(buffer) );
        if( count <= 0 )
        {
            perror( "receive" );
            return false;
        }
        received += count;
    }
    
    if( received )
    {
        std::cout << "Flushed " << received << " pending bytes" << std::endl;
    }
	return true;
}

bool MultithreadTest::SetPriority( int fd, unsigned int latency )
{
    if( ioctl( fd, MXS_AUART_IOCSPRIORITY, &latency ) )
    {
        perror( "ioctl" );
        return false;
    }
    return true;
}

bool MultithreadTest::Run(std::size_t byteCount)
{
    _fdRegular = OpenPort();
    if( _fdRegular < 0 )
    {
        return false;
    }
    _fdFast = OpenPort();
    if( _fdFast < 0 )
    {
        return false;
    }
    
    if( !FlushPendingData() )
    {
        return false;
    }
    
    if( !SetPriority( _fdFast, 1 ) )
    {
        return false;
    }
    
	Sysutils::ThreadSetSchedulingPriority( 0 );
    
    {
        LockGuard lock(_mutex);
        _regularTxThread = tthread::thread( &sRegularTxThreadFunction, this );
        _fastTxThread = tthread::thread( &sFastTxThreadFunction, this );
    }    
    
    
    bool success = DoRxLoop(byteCount);
    
    {
        LockGuard lock(_mutex);
		_exit = true;
        _condition.notify_all();
    }
    if( _regularTxThread.joinable() )
    {
        _regularTxThread.join();
    }
	if( _fastTxThread.joinable() )
    {
        _fastTxThread.join();
    }
    
    
    if( success )
    {
      std::cout << "Interrupts: " << _interruptCounter << std::endl << std::endl;

      std::cout << "Fast histogram:" << std::endl;
      PrintHistogram( _histogramFast );
      std::cout << std::endl;

      std::cout << "Fast histogram including wire-time:" << std::endl;
      PrintHistogram( _histogramFastTotal );
      std::cout << std::endl;

      std::cout << "Regular histogram:" << std::endl;
      PrintHistogram( _histogramRegular );
      std::cout << std::endl;

      std::cout << "Regular histogram to success:" << std::endl;
      PrintHistogram( _histogramRegularSuccess );
      std::cout << std::endl;
    }
    
    return success;
    
}

/*static*/ int MultithreadTest::FrameDuration( int bytes )
{
    return (bytes * 88873 / 1024 );
}

/*static*/ struct timespec MultithreadTest::GetTime()
{
    /* get monotonic clock time */
    struct timespec monotime;
    clock_gettime(CLOCK_MONOTONIC, &monotime);    
    return monotime;
}

/*static*/ int MultithreadTest::TimeDiff( const struct timespec& start, const struct timespec& end )
{
    int diff = int(end.tv_sec * 1000000 + end.tv_nsec / 1000);
    diff -= int(start.tv_sec * 1000000 + start.tv_nsec / 1000);
    return diff;
}

/*static*/ void MultithreadTest::PrintHistogram( const HistogramType& histogram )
{
    char buffer[32];
    int min = INT_MAX;
    int max = INT_MIN;
    int avg = 0;
    std::size_t totalCount = 0;
    for( HistogramType::const_iterator it = histogram.begin(); it != histogram.end(); it++ )
    {
        min = std::min( min, it->first );
        max = std::max( max, it->first );
        
        totalCount += it->second;
        avg += it->first * it->second;
        
        snprintf( buffer, sizeof(buffer), "  %4d: %4d", it->first, int(it->second) );
        std::string line = buffer;
        int count = std::min( int(it->second), 70 );
        line.append( count, '-' );
        std::cout << line << std::endl;
        
    }
    if( totalCount )
    {
        avg /= totalCount;
    }else{
        avg = 0;
    }
    std::cout << "  Nframes=" << totalCount
              << ", Min=" << double(min)/1000.0 << "us"
              << ", Avg=" << double(avg)/1000.0 << "us"
              << ", Max=" << double(max)/1000.0 << "us." << std::endl;
}



bool MultithreadTest::DoRxLoop( std::size_t byteCount)
{
	fd_set inFd, outFd, excFd;
	FD_ZERO(&inFd);
	FD_ZERO(&outFd);
	FD_ZERO(&excFd);

	FD_SET(_fdRegular, &inFd);
    
    char buffer[256];
    while( _rxCounter < byteCount )
    {
        // Check for events
        int nEvents;
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        nEvents = select(_fdRegular+1, &inFd, &outFd, &excFd, &tv);
    
        if( nEvents < 0 )
        {
            perror( "RX select" );
            return false;
        }
        if( nEvents == 0 )
        {
            perror( "RX select timeout");
            return false;
        }
        
        int count = read( _fdRegular, buffer, sizeof( buffer ) );
        
        if( count <= 0 )
        {
            perror( "receive" );
            return false;
        }
        
        int index = 0;
        while( index < count )
        {
            char received = buffer[index];
            char expected;
            {
                LockGuard lock(_mutex);
                std::queue<char>& queue = ((int(received) & 0xff) >=224) ? _queuePendingRxDataFast : _queuePendingRxDataRegular;
                while( (!_exit) && queue.empty() )
                {
                    if( _condition.wait_for( _mutex, tthread::chrono::milliseconds( 1000 ) ) == tthread::timeout )
                    {
                        std::ios_base::fmtflags f( std::cerr.flags() );
                        std::cerr << "Queue timeout, RX char = 0x"
                                  << std::setfill('0')
                                  << std::setw(2)
                                  << std::uppercase
                                  << std::hex
                                  << int(received)
                                  << std::nouppercase
                                  << std::dec
                                  << std::setw(0)
                                  << std::setfill(' ')
                                  << " @" << _rxCounter << std::endl;
                        std::cerr.flags( f );

                        return false;
                    }
                }
                if( _exit )
                {
                    return false;
                }
                expected = queue.front();
                queue.pop();
                if( queue.empty() )
                {
                    _condition.notify_all();
                }
            }
            
            if( received != expected )
            {
                std::ios_base::fmtflags f( std::cerr.flags() );
                std::cerr << "RX mismatch expected 0x"
                          << std::setfill('0')
                          << std::setw(2)
                          << std::uppercase
                          << std::hex
                          << (int(expected) & 0xff)
                          << std::nouppercase
                          << ", received 0x"
                          << std::uppercase
                          << (int(received) & 0xff)
                          << std::nouppercase
                          << std::dec
                          << std::setw(0)
                          << std::setfill(' ')
                          << " @" << _rxCounter << std::endl;
                std::cerr.flags( f );
                return false;
            }
            index++;
            _rxCounter++;
        }
    }
    return true;
}

void MultithreadTest::RegularTxThreadFunction()
{
    std::cout << "Regular TX thread started" << std::endl;
    unsigned int rand_state = 0;
    char buffer[224];
    for( std::size_t i=0; i<sizeof(buffer); i++ )
    {
        buffer[i] = i&0xff;
    }
    int frameCount = 0;
    while( true )
    {
        {
            LockGuard lock(_mutex);
            if( _exit )
            {
                break;
            }
        }
        int length = (rand_r(&rand_state) % sizeof(buffer)) + 1; //data size between 1 and sizeof(buffer);
        bool success = false;
        struct timespec firstAttemptStartTime = GetTime();
        while (  !success )
        {
            {
                LockGuard lock(_mutex);
                if( _exit )
                {
                    break;
                }
            }
            
            std::size_t txStartCounter;
            //printf( "Writing %d regular bytes\n", length );
            struct timespec sendStartTime = GetTime();
            int lengthSent = write( _fdRegular, buffer, length );
            //printf( "%d/%d regular bytes written\n", lengthSent, length );
            {
                LockGuard lock(_mutex);
                if( lengthSent < 0 )
                {
                    perror( "TX regular write" );
                    _exit = true;
                    _condition.notify_all();
                    break;
                }
                if( _exit )
                {
                    break;
                }
                txStartCounter = _txCounter;
                for( int i=0; i<lengthSent; i++ )
                {
                    _queuePendingRxDataRegular.push( buffer[i] );
                }
                _txCounter += lengthSent;
            }
            _condition.notify_all();
            {
                LockGuard lock(_mutex);
                while( (!_exit) && (!_queuePendingRxDataRegular.empty()) )
                {
                    if( _condition.wait_for( _mutex, tthread::chrono::milliseconds( 1000 ) ) == tthread::timeout )
                    {
                        _exit = true;
                        _condition.notify_all();
                    }
                }
            }
            struct timespec receiveEndTime = GetTime();
            int frameDuration = FrameDuration( lengthSent );
            if( lengthSent == length )
            {
                std::cout << "Success sending regular frame " << frameCount
                          << ", size=" << length
                          << " @" << txStartCounter << std::endl;

                success = true;
                int latency = TimeDiff( firstAttemptStartTime, receiveEndTime ) - frameDuration;
                latency = ((latency + 50) / 100) * 100;
                _histogramRegularSuccess[latency]++;
            } else {
                _interruptCounter++;
                std::cout << "Repeating regular frame " << frameCount
                          << ", interrupted after " << lengthSent
                          << "/" << length
                          << " @" << txStartCounter << std::endl;
                usleep(5000);
            }
            int latency = TimeDiff( sendStartTime, receiveEndTime ) - frameDuration;
            latency  = ((latency + 50) / 100) * 100;
            _histogramRegular[latency]++;
        }
        frameCount++;
    }
    std::cout << "Regular TX thread ended" << std::endl;
}

void MultithreadTest::FastTxThreadFunction()
{
    std::cout << "Fast TX thread started" << std::endl;
	Sysutils::ThreadSetSchedulingPriority( 0 );
    unsigned int rand_state = 0;
    char buffer[32];
    for( std::size_t i=0; i<sizeof(buffer); i++ )
    {
        buffer[i] = (i + 224)&0xff;
    }
    int frameCount = 0;
    while( true )
    {
        {
            LockGuard lock(_mutex);
            if( _exit )
            {
                break;
            }
        }
        int delay = (rand_r(&rand_state) % 128) + 20; //delay between 20 and 147ms;
        usleep( delay * 1000 );
        //int length = (rand_r(&rand_state) % 32) + 1; //data size between 1 and 32;
        int length = 12;
        //printf( "Writing %d fast bytes\n", length );
        struct timespec sendStartTime = GetTime();
        int lengthSent = write( _fdFast, buffer, length );
        //printf( "%d/%d fast bytes written\n", lengthSent, length );
        std::size_t txStartCounter;
        {
            LockGuard lock(_mutex);
            if( lengthSent < 0 )
            {
                perror( "TX fast write" );
                _exit = true;
            }
            if( _exit )
            {
                break;
            }
            txStartCounter = _txCounter;
            for( int i=0; i<lengthSent; i++ )
            {
                _queuePendingRxDataFast.push( buffer[i] );
            }
            _txCounter += lengthSent;
            if( lengthSent != length )
            {
                std::cout << "Error sending fast frame " << frameCount
                          << ", interrupted after " << lengthSent
                          << "/" << length
                          << " @" << txStartCounter << std::endl;
                _exit = true;
                break;
            }
        }
        _condition.notify_all();
        {
            LockGuard lock(_mutex);
            while( !_queuePendingRxDataFast.empty() )
            {
                if( _condition.wait_for( _mutex, tthread::chrono::milliseconds( 1000 ) ) == tthread::timeout )
                {
                    _exit = true;
                    _condition.notify_all();
                }
            }
        }
        struct timespec receiveEndTime = GetTime();
        int frameDuration = FrameDuration( lengthSent );
        int totalLatency = TimeDiff( sendStartTime, receiveEndTime );
        int latency = totalLatency - frameDuration;
        latency  = ((latency + 50) / 100) * 100;
        totalLatency  = ((totalLatency + 50) / 100) * 100;
        _histogramFast[latency]++;
        _histogramFastTotal[totalLatency]++;
        
        std::cout << "Success sending fast frame " << frameCount
                  << ", size=" << length
                  << " @" << txStartCounter << std::endl;
        frameCount++;
    }
    std::cout << "Fast TX thread ended" << std::endl;
}
