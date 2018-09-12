#ifndef SDTYPE_H
#define SDTYPE_H
/**
* @file sdtype.h
* @brief UCODE数据类型集合
*
**/
#include "ucconfig.h"
#include <string>
#include <wchar.h>
#ifdef _M_X64
#ifndef WIN64
#define WIN64
#endif
#endif

#if defined(__linux__)
#define LINUX32
#if defined(__x86_64__)
#undef  LINUX32
#define LINUX64
#endif
#endif //__linux__


#if (defined(WIN32) || defined(WIN64))
#pragma warning(disable:4996) // suppress VS 2005 deprecated function warnings
#pragma warning(disable:4786) // for string

#include <Windows.h>
#include <BaseTsd.h>
#include <tchar.h>


#define UCAPI __stdcall      

typedef HANDLE              SDHANDLE;
#define SDINVALID_HANDLE    NULL
#define SDCloseHandle(x)    CloseHandle(x)


/**
* @brief linux下数据类型统一定义
*/
#else
typedef  unsigned char      BYTE;

#ifndef FALSE
#define FALSE			    false
#endif

#ifndef TRUE
#define TRUE                true
#endif

typedef wchar_t             WCHAR;

typedef char                CHAR;

typedef unsigned char       UCHAR;

typedef signed char         INT8;

typedef unsigned char       UINT8;

typedef signed short        INT16;

typedef unsigned short      UINT16;

typedef signed int          INT32;

typedef unsigned int        UINT32;

typedef  INT32              BOOL;

typedef long				LONG;
typedef unsigned long       ULONG;		

typedef signed long long    INT64;

typedef unsigned long long  UINT64;

typedef float               FLOAT;

#ifndef VOID
typedef void                VOID;
#endif

#define SDINVALID_HANDLE    0
typedef long                SDHANDLE;

#define SDCloseHandle(x)    (x)

#ifdef UNICODE
typedef WCHAR               TCHAR;
#else
typedef CHAR                TCHAR;
#endif
#define UCAPI 
#endif //endif __linux__


#define SD_OK     0
#define SD_ERR    0xFFFFFFFF

#define SDINFINITE  0xFFFFFFFF
#ifndef NULL
typedef 0                   NULL 
#endif

#ifndef DOUBLE
typedef double              DOUBLE;
#endif

#endif





