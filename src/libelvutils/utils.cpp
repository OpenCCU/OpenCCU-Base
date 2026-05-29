/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "utils.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "Logger.h"
#include <stdio.h>  //fuer snprintf
#include <stdlib.h> //atof, atoi
#include <string.h>

#define NSEC_PER_SEC      1000000000L
#define NSEC_PER_MILLISEC 1000000L
#define MSEC_PER_SEC      1000L

struct timespec millis2abstime(int64_t millis)
{
  struct timespec currSysTime;

  // get current system time
  clock_gettime(CLOCK_REALTIME, &currSysTime);

  // add millis to currSysTime
  currSysTime.tv_sec += (millis / MSEC_PER_SEC);
  currSysTime.tv_nsec += (millis % MSEC_PER_SEC) * NSEC_PER_MILLISEC;

  if (currSysTime.tv_nsec >= NSEC_PER_SEC) {
    currSysTime.tv_sec++;
    currSysTime.tv_nsec -= NSEC_PER_SEC;
  } else if (currSysTime.tv_nsec < 0) {
    currSysTime.tv_sec--;
    currSysTime.tv_nsec += NSEC_PER_SEC;
  }

  return currSysTime;
}
bool operator<(const struct timespec& t1, const struct timespec& t2)
{
	if(t1.tv_sec<t2.tv_sec)return true;
	if(t1.tv_sec>t2.tv_sec)return false;
	return t1.tv_nsec<t2.tv_nsec;
}
uint64_t time_millis()
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return now.tv_sec * MSEC_PER_SEC + now.tv_nsec / NSEC_PER_MILLISEC;
}
uint64_t time_difference(uint64_t t1, uint64_t t2)
{
	int64_t diff=static_cast<int64_t>(t1)-static_cast<int64_t>(t2);
	if(diff<0)diff *= -1;
	return diff;
}
void string_trim(std::string* s)
{
	std::string::size_type pos=s->find_first_not_of( ' ');
	if(pos)s->erase(0, pos);
	pos=s->find_last_not_of(' ');
	if(pos!=std::string::npos)s->erase(pos+1);
}
int number_running_processes()
{
#ifndef WIN32
	char buffer[32];
	int fd = open("/proc/loadavg", O_RDONLY | O_NOCTTY );
	if(fd<0){
		perror("open /proc/loadavg");
		return -1;
	}
	int count=read(fd, buffer, 31);
	close(fd);
	buffer[count]=0;
	char* field=strtok(buffer, " ");
	int fieldIndex=0;
	while(field){
		if(fieldIndex==3){
			return atoi(field);
		}
		if(++fieldIndex > 3)break;
	}
	LOG(Logger::LOG_ERROR, "Error getting number of threads from /proc/loadavg");
	return 1;
#else
	return 6;
#endif
}
//------------------------------------------------------------------------
int StrToInt(const std::string& s)
{
	return atoi(s.c_str());
}
//------------------------------------------------------------------------
int StrToInt(const char& c)
{
	return c - 48;
}
//------------------------------------------------------------------------
std::string IntToStr(const int& val)
{
	char buffer[11];
	snprintf(buffer, sizeof(buffer), "%d", val);
	return std::string(buffer);
}
//------------------------------------------------------------------------
double StrToDouble (const std::string& s)
{
	return atof(s.c_str());
}
//------------------------------------------------------------------------
bool StrToBool (const std::string& s)
{
	return s == "true";
}
//------------------------------------------------------------------------
std::string BoolToStr (const bool& val)
{
	return val ? "true" : "false";
}
//------------------------------------------------------------------------
std::vector<std::string> SplitString(const std::string &val, const std::string &separator)
{
	std::string::size_type pos = 0;
	std::string value = val;
	std::string token;
	std::vector<std::string> v;
	

	while (value.length() > 0)
	{
		pos = value.find(separator, 0);

		if (pos == std::string::npos)
		{
			token = value;
			        value = "";
		}
		else
		{
			token = value.substr(0,    pos); 
			value = value.substr(pos+1);//+1 überspringt das separator-Zeichen
		}

		v.push_back(token);
	}

	return v;
}
//------------------------------------------------------------------------
