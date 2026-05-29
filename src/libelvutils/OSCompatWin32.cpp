/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifdef WIN32

#include "OSCompat.h"
#include <string>
#include <io.h>
#include "windows.h"
#include <shlobj.h>

/*static*/ const char OSCompat::PATH_SEPARATOR='\\';

/*static*/ void OSCompat::RaiseThreadPriority()
{
}

/*static*/ bool OSCompat::MakeDirectory(const char* s)
{
    if(CreateDirectory(s, NULL) != 0){
        return true;
    }else if(GetLastError()==ERROR_PATH_NOT_FOUND){
        //recursively create the base directories
        std::string base_dir=s;
        std::string::size_type pos = base_dir.rfind(PATH_SEPARATOR);
        if(pos != std::string::npos){
            base_dir.erase(pos);
            if( MakeDirectory(base_dir.c_str()) ){
                return CreateDirectory(s, NULL) != 0;
            }else{
                return false;
            }
        } else {
            return false;
        }
    }else{
        return false;
    }
}

/*static*/ std::string OSCompat::ResolvePathConstants(const std::string& s)
{
    std::string path=s;
    std::string::size_type left;
    std::string::size_type right=0;
    while(true){
        left=path.find("${", right);
        if(left == std::string::npos)break;
        right=path.find("}", left+2);
        if(right == std::string::npos)break;
        std::string constant=path.substr(left+2, right-left-2);
        std::string value;
        char buffer[MAX_PATH];
        if(constant == "Bindir"){
            //Directory where application is installed
            GetModuleFileName(NULL, buffer, sizeof(buffer));
            value=buffer;
            std::string::size_type pos=value.find_last_of('\\');
            value.erase(pos);
        }else if(constant == "CommonAppData"){
            //Application data for all users. typically "C:\Dokumente und Einstellungen\All Users\Anwendungsdaten"
            if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, buffer)))value=buffer;
        }else if(constant == "LocalAppData"){
            //Local application data for a single user. typically "C:\Dokumente und Einstellungen\user\Lokale Einstellungen\Anwendungsdaten"
            if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, buffer)))value=buffer;
        }else if(constant == "AppData"){
            //Roaming application data for a single user. typically "C:\Dokumente und Einstellungen\user\Anwendungsdaten"
            if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, buffer)))value=buffer;
        }else if(constant == "Home"){
            //MyDocuments for a single user. typically "C:\Dokumente und Einstellungen\user\Eigene Dateien"
            if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE, NULL, 0, buffer)))value=buffer;
        }
        path.replace(left, right-left+1, value);
        right=left+value.size();
    }
    return path;
}

/*static*/ std::string OSCompat::FixPath(const std::string& s)
{
    std::string path;
    if( ( (s.size() > 0) && ( (s[0]=='\\') || (s[0]=='/') ) ) || ( (s.size() >= 2) && (s[1]==':') ) || ( (s.size() >= 2) && (s[0]=='$') && (s[1] == '{') ) ){
        //use an absolute path or a path starting with a path variable unmodified
        path=s;
    }else{
        path = "${Bindir}\\" + s;
        //modify relative path into absolute path
        char buffer[MAX_PATH];
        GetModuleFileName(NULL, buffer, sizeof(buffer));
        path=buffer;
        std::string::size_type pos=path.find_last_of('\\');
        path.erase(pos);
        path=path+PATH_SEPARATOR+s;
    }

    std::string fixed_path;
    fixed_path.reserve(path.size());
    std::string::size_type left=0;
    std::string::size_type right=0;
    while(left!=std::string::npos)
    {
        right=path.find_first_of("/\\", left);
        std::string component=path.substr(left, right-left);
        if(component == ".."){
            std::string::size_type pos = fixed_path.rfind( PATH_SEPARATOR );
            fixed_path=fixed_path.substr(0, pos);
        }else if(component != "."){
            if(fixed_path.size())fixed_path+=PATH_SEPARATOR;
            fixed_path+=ResolvePathConstants(component);
        }
        if(right==std::string::npos)break;
        left=path.find_first_not_of("/\\", right);
    }
    return fixed_path;
}

OSCompat::DirectoryLister::DirectoryLister(const char* path, int flags)
{
    this->flags=flags;
    this->path=path;
    private_data=NULL;
}

OSCompat::DirectoryLister::~DirectoryLister()
{
    if(private_data){
        intptr_t* p_handle=(intptr_t*)private_data;
        _findclose(*p_handle);
        delete p_handle;
    }
}

std::string OSCompat::DirectoryLister::NextEntry()
{
   	struct _finddata_t fd;
    if(!private_data){
    	std::string s_path=path;
	    if( s_path.size() && s_path[s_path.size()-1]!=PATH_SEPARATOR )s_path+=PATH_SEPARATOR;
	    std::string pattern=s_path+"*";
        intptr_t* p_handle=new intptr_t;
        *p_handle=_findfirst(pattern.c_str(), &fd);
        if(*p_handle<0){
            delete p_handle;
            return "";
        }
        private_data=p_handle;
    }else{
        intptr_t* p_handle=(intptr_t*)private_data;
        if(!p_handle)return "";
        if(_findnext(*p_handle, &fd)!=0){
            return "";
        }
    }
    if((flags & FLAG_LIST_DIRS)==0 && (fd.attrib & _A_SUBDIR)!=0)return NextEntry();
    if((flags & FLAG_LIST_FILES)==0 && (fd.attrib & _A_SUBDIR)==0)return NextEntry();
    return fd.name;
}

#endif
