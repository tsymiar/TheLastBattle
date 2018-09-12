// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "Platform.h"

// enable this define to automatically activate the memory leak detector on application startup
#define AUTO_ENABLE_MEMLEAKDETECTOR
//#define USE_WER

#if defined(PLATFORM_WINDOWS)
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// use this define to enable the *W functions for the debugging api
#if defined(_UNICODE)
#define DBGHELP_TRANSLATE_TCHAR
#endif

#include <Windows.h>
#include <tchar.h>
#include <dbghelp.h>
#if defined(USE_WER)
#include <werapi.h>
#pragma comment(lib, "wer.lib")
#endif

#else // PLATFORM_WINDOWS

#include <malloc.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <execinfo.h>

#include "CoreDumper.h"

#endif	// PLATFORM_WINDOWS

#include <assert.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <vector>
#include <map>
#include <iomanip>
#include <iterator>
#include "Macros.h"
#include "Types.h"
