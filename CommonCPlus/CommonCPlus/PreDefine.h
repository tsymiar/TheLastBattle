/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:PreDefine.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:March 12 2014			:November 05 2010
* summary			:
*
*/

#pragma once

#include <stdint.h>

typedef uint8_t BYTE;
typedef char CHAR;
typedef wchar_t WCHAR;
typedef float FLOAT;
typedef double DOUBLE;
typedef int8_t INT8;
typedef uint8_t UINT8;
typedef int16_t INT16;
typedef uint16_t UINT16;
typedef int32_t INT32;
typedef uint32_t UINT32;
typedef int64_t INT64;
typedef uint64_t UINT64;

typedef INT64  TIME_TICK;
typedef INT64  TIME_SECOND;
typedef INT64  TIME_MILSEC;

namespace CommonCPlus{

#define IS_X64 (sizeof(void*)==8)
#define IS_X86 (sizeof(void*)==4)

#define MAKEINT32(a, b)			((INT32)(((INT16)(((INT64)(a)) & 0xffff)) | ((INT32)((INT16)(((INT64)(b)) & 0xffff))) << 16))
#define MAKEUINT32(a, b)		((UINT32)(((UINT16)(((INT64)(a)) & 0xffff)) | ((UINT32)((UINT16)(((INT64)(b)) & 0xffff))) << 16))
#define MAKEINT64(a, b)			((INT64)(((INT32)(((INT64)(a)) & 0xffffffff)) | ((INT64)((INT32)(((INT64)(b)) & 0xffffffff))) << 32))
#define MAKEUINT64(a, b)		((UINT64)(((UINT32)(((UINT64)(a)) & 0xffffffff)) | ((UINT64)((UINT32)(((UINT64)(b)) & 0xffffffff))) << 32))

#define	HIGHINT32(a)			((INT32)(((INT64)a) >> 32))
#define	LOWINT32(a)				((INT32)(((INT64)a) & 0x00000000ffffffff))
#define	HIGHUINT32(a)			((UINT32)(((INT64)a) >> 32))
#define	LOWUINT32(a)			((UINT32)(((INT64)a) & 0x00000000ffffffff))

#define	HIGHINT16(a)			((INT16)(((INT32)a) >> 16))
#define	LOWINT16(a)				((INT16)(((INT32)a) & 0x0000ffff))
#define	HIGHUINT16(a)			((UINT16)(((INT32)a) >> 16))
#define	LOWUINT16(a)			((UINT16)(((INT32)a) & 0x0000ffff))

#define	ABS(x)					(x < 0 ? -x : x)
#define SAFEDELETE(x)			if (NULL != x){delete x; x = NULL;}
#define MINI(x,y)				(x < y ? x : y)
#define MAX(x,y)				(x > y ? x : y)

const	UINT32					c_un32MaxUINT32 = ((UINT32)~((UINT32)0));
const	INT32					c_n32MaxINT32 = ((INT32)(c_un32MaxUINT32 >> 1));
const	INT32					c_n32MiniINT32 = ((INT32)~c_n32MaxINT32);
const	UINT64					c_un64MaxUINT64 = ((UINT64)~((UINT64)0));
const	INT64					c_n64MaxINT64 =  ((INT64)(c_un64MaxUINT64 >> 1));
const	INT64					C_n64MiniINT64 = ((INT64)~c_n64MaxINT64);

typedef struct tagSVersion
{
	UINT16 un16MajorVersion;
	UINT16 un16MinorVersion;
	UINT16 un16CompatibleVersion;
	UINT16 un16BuildNumber;
public:
	tagSVersion()
	{
		un16MajorVersion = 0;
		un16MinorVersion = 1;
		un16CompatibleVersion = 0;
		un16BuildNumber = 0;
	}
	tagSVersion(UINT16 un16Major , UINT16 un16Minor , UINT16 un16Com , UINT16 un16Build)
	{
		un16MajorVersion = un16Major;
		un16MinorVersion = un16Minor;
		un16CompatibleVersion = un16Com;
		un16BuildNumber = un16Build;
	}
}SVersion , * pSVersion;

enum EModelStatus
{
	eModelStatus_Cleared = 0,
	eModelStatus_Initializing,
	eModelStatus_Initialized,
	eModelStatus_Starting,
	eModelStatus_Working,
	eModelStatus_Stoping,
};

enum ECharEncode
{
	eCharEncode_UTF8,
	eCharEncode_UNICODE,
};

}
