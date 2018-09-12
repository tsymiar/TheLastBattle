/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:stdafx.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:November 30 2010
* summary			:
*
*/

#pragma once

#ifdef _WINDOWS

#define WIN32_LEAN_AND_MEAN

#pragma warning(disable:4996)

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif  // _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#ifdef _DEBUG
//#define new DEBUG_CLIENTBLOCK
//#endif  // _DEBUG

#include <windows.h>
#include <winsock2.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Mswsock.h>
#include <process.h>

#ifndef WINVER                 
#define WINVER 0x0600          
#endif

#ifndef _WIN32_WINNT           
#define _WIN32_WINNT 0x0600     
#endif

#ifndef _WIN32_WINDOWS         
#define _WIN32_WINDOWS 0x0410 
#endif

#ifndef _WIN32_IE               
#define _WIN32_IE 0x0700        
#endif

#include	"../../CommonCPlus/CommonCPlus/WYYTools/vld.h"

#else

#include <termios.h>
#include <term.h>
#include <curses.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <memory.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <curses.h>
#include <dirent.h>
#include <execinfo.h>
#include <signal.h>
#include <exception>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dlfcn.h>

#endif

#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include  "../Include/GSKernel/GSKernel.h"
#include "../../Share/DbgLib/DbgLib.h"

using namespace std;
using namespace GateServer;

