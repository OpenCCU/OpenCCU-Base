/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _UTILS_H_
#define _UTILS_H_

#include "dllexport.h"

extern "C"{
#include <pthread.h>
}
#include <cstdint>
#include <string>
#include <vector>
#ifdef WIN32
#include <windows.h>
#include <io.h>
#define sleep(x) ::Sleep(x*1000)
#define usleep(x) ::Sleep(x/1000)
#define read _read
#undef max
#endif
struct timespec ELVUTILS_DLLEXPORT millis2abstime(int64_t millis);
uint64_t ELVUTILS_DLLEXPORT time_millis();
uint64_t ELVUTILS_DLLEXPORT time_difference(uint64_t t1, uint64_t t2);
void ELVUTILS_DLLEXPORT string_trim(std::string* s);
int ELVUTILS_DLLEXPORT number_running_processes();
bool ELVUTILS_DLLEXPORT operator<(const struct timespec& t1, const struct timespec& t2);

template<class T>
    inline const T& max(const T& x, const T& y)
{
	if(x<y)return y;
	else return x;
}

int         ELVUTILS_DLLEXPORT StrToInt (const std::string& s);
int         ELVUTILS_DLLEXPORT StrToInt (const char& c);
std::string ELVUTILS_DLLEXPORT IntToStr (const int& val);
double      ELVUTILS_DLLEXPORT StrToDouble (const std::string& s);
bool        ELVUTILS_DLLEXPORT StrToBool   (const std::string& s);
std::string ELVUTILS_DLLEXPORT BoolToStr (const bool& val);

std::vector<std::string> ELVUTILS_DLLEXPORT SplitString(const std::string &val, const std::string &separator);

#endif
