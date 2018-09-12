/*******************************************************************\
| Types.h
|--------------------------------------------------------------------
| CREATED:		2006/11/15/
| AUTHOR:		Martin Fleisz
|--------------------------------------------------------------------
| DESCRIPTION:
| 
| Definition of basic types used by CoreLib
\********************************************************************/
#ifndef DBGLIB_TYPES_H
#define DBGLIB_TYPES_H

#include "Platform.h"

namespace DbgLib
{

//*****************************************************************************
//** data type definitions
//*****************************************************************************

// in MSVC we have no C99 stdint.h file ...
// #if defined(_MSC_VER)
// /*
// 
// typedef __int8           int8_t;
// typedef unsigned __int8  uint8_t;
// 
// typedef __int16          int16_t;
// typedef unsigned __int16 uint16_t;
// 
// typedef __int32          int32_t;
// typedef unsigned __int32 uint32_t;
// 
// typedef __int64          int64_t;
// typedef unsigned __int64 uint64_t;
// */
// 
// #else
// 
// // define to have the C99 limit macros available when compiling C++ code
// #ifndef __STDC_LIMIT_MACROS
// 	#define __STDC_LIMIT_MACROS
// #endif

#include <stdint.h>
//#endif

// explicit numerical types with size
typedef int8_t		int8;
typedef uint8_t		uint8;

typedef int16_t		int16;
typedef uint16_t	uint16;

typedef int32_t		int32;
typedef uint32_t	uint32;

typedef int64_t		int64;
typedef uint64_t	uint64;

// byte type
typedef uint8 byte;

// 32/64 bit platform dependent types
#if defined(_M_IX86) || defined(__i386__)  || defined(__i386)
	typedef int32 sx;
	typedef uint32 ux;
	#define digits_intx (8)
#else
	typedef int64 sx;
	typedef uint64 ux;
	#define digits_intx (16)
#endif

// character typedef
#if defined(_UNICODE)
typedef wchar_t			tchar;
#else
typedef char			tchar;
#endif

// STL typedefs for unicode/ansi builds
#if defined (UNICODE) || defined (_UNICODE)

typedef std::wstring         tstring;

typedef std::wstringbuf      tstringbuf;
typedef std::wstringstream   tstringstream;
typedef std::wostringstream  tostringstream;
typedef std::wistringstream  tistringstream;

typedef std::wstreambuf      tstreambuf;

typedef std::wistream        tistream;
typedef std::wiostream       tiostream;

typedef std::wostream        tostream;

typedef std::wfilebuf        tfilebuf;
typedef std::wfstream        tfstream;
typedef std::wifstream       tifstream;
typedef std::wofstream       tofstream;

typedef std::wios            tios;

#else // defined UNICODE || defined _UNICODE

typedef std::string          tstring;

typedef std::stringbuf       tstringbuf;
typedef std::stringstream    tstringstream;
typedef std::ostringstream   tostringstream;
typedef std::istringstream   tistringstream;

typedef std::streambuf       tstreambuf;

typedef std::istream         tistream;
typedef std::iostream        tiostream;

typedef std::ostream         tostream;

typedef std::filebuf         tfilebuf;
typedef std::fstream         tfstream;
typedef std::ifstream        tifstream;
typedef std::ofstream        tofstream;

typedef std::ios             tios;

#endif // defined UNICODE || defined _UNICODE

#if defined(PLATFORM_WINDOWS)
typedef HMODULE ModuleHandle;
#else
typedef void* ModuleHandle;	// dummy define for Unix platforms
#endif

} // namespace DbgLib

#endif // DBGLIB_TYPES_H
