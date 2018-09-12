#ifndef UCSTRING_H
#define UCSTRING_H

#include "uctype.h"
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <vector>
//#include <ucdebug.h>
#include <stdarg.h>

#if (defined(WIN32) || defined(WIN64))

#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#else
#ifdef UNICODE
typedef std::basic_string<TCHAR> tstring;
#else
typedef std::string tstring;
#endif
#endif


#if (defined(WIN32) || defined(WIN64))
#include <tchar.h>
#define SDSYMBOL_RT         "\r\n"
#define SDFMT_I64           "%I64d"
#define WCHAR_SDFMT_I64      L"%I64d"
#define SDFMT_U64           "%I64u"
#define WCHAR_SDFMT_U64     L"%I64u"
#define SDFMT_X64           "%I64x"
#else
#include <wchar.h>
#include <wctype.h>
#define SDSYMBOL_RT         "\n"
#define SDFMT_I64           "%lld"
#define WCHAR_SDFMT_I64      L"%lld"
#define SDFMT_U64           "%llu"
#define WCHAR_SDFMT_U64     L"%llu"
#define SDFMT_64X           "%llx"
#endif

namespace UCODE
{
    /**
    * @defgroup groupstring 字符串处理
    * @brief 字符串处理不做边界检测
    * @ingroup  UCODE
    * @{
    */

    /** @see isalnum */
#define SDIsAlNum(c) (isalnum(((UCHAR)(c))))
    /** @see isalpha */
#define SDIsAlpha(c) (isalpha(((UCHAR)(c))))
    /** @see iscntrl */
#define SDIsCntrl(c) (iscntrl(((UCHAR)(c))))
    /** @see isdigit */
#define SDIsDigit(c) (isdigit(((UCHAR)(c))))
    /** @see isgraph */
#define SDISGraph(c) (isgraph(((UCHAR)(c))))
    /** @see islower*/
#define SDIsLower(c) (islower(((UCHAR)(c))))
    /** @see isascii */
#ifdef isascii
#define SDIsAscii(c) (isascii(((UCHAR)(c))))
#else
#define SDIsAscii(c) (((c) & ~0x7f)==0)
#endif
    /** @see isprint */
#define SDIsPrint(c) (isprint(((UCHAR)(c))))
    /** @see ispunct */
#define SDIsPunct(c) (ispunct(((UCHAR)(c))))
    /** @see isspace */
#define SDIsSpace(c) (isspace(((UCHAR)(c))))
    /** @see isupper */
#define SDIsUpper(c) (isupper(((UCHAR)(c))))
    /** @see isxdigit */
#define SDIsXdigit(c) (isxdigit(((UCHAR)(c))))
    /** @see tolower */
#define SDToLower(c) (tolower(((UCHAR)(c))))
    /** @see toupper */
#define SDToUpper(c) (toupper(((UCHAR)(c))))

    ///////////////////////////////////////////////////////////////////////////
#define SDSprintf sprintf
#define SDSwprintf swprintf

#if (defined(WIN32) || defined(WIN64))
#define SDSnprintf _snprintf
#else
#define SDSnprintf snprintf
#endif

///////////////////////////////////////////////////////////////////////
#define SDAtoi atoi

#if (defined(WIN32) || defined(WIN64))
#define SDAtoi64 _atoi64
#else
#define SDAtoi64 atoll
#endif

#ifdef UNICODE
#define TCHAR                 WCHAR	
#define _SDT(x)               L##x
#define _SDTStrlen            SDWcslen
#define _SDTStrnlen           SDWcsnlen
#define _SDTStrcat            SDWcscat
#define _SDTStrcpy            SDWcscpy
#define _SDTStrncpy           SDWcsncpy
#define _SDTStrcmp            SDWcscmp
#define _SDTStrncmp           SDWcsncmp
#define _SDTStrchr            SDWcschr
#define _SDTStrrchr           SDWcsrchr
#define _SDTStrcasecmp        SDWcscasecmp
#define _SDTA2T(x)            SDA2W(x).c_str()
#define _SDTT2A(x)            SDW2A(x).c_str()
#define _SDTT2Local(x)        SDW2Local(x).c_str()
#define _SDTLocal2T(x)        SDLocal2W(x).c_str()
#define _SDTUTF82W(x)         SDUTF82W(x).c_str()
#define _SDTT2UTF8(x)         SDW2UTF8(x).c_str()
#define _SDTUTF82T(x)         SDUTF82W(x).c_str()
#define _SDTSource2T(x,y)     SDSource2W(x,y).c_str()
#define _SDTTtoi		      SDWtoi
#define _SDTsnprintf		  SDsnwprintf
#define _SDTvsprintf          vswprintf
#define _SDTvsnprintf          vswprintf
#define _SDTsprintf		      swprintf
#define _SDTStrchr            SDWcschr
#define _SDTStrrchr           SDWcsrchr
#define _SDTStrtok            SDWcstok
#define _SDTStrlwr            SDWcslwr
#define _SDTStrupr            SDWcsupr
#define _SDTStrcat             SDWcscat
#define _SDTStrncat             SDWcsncat
#define _SDTStrncasecmp   SDWcsncasecmp
#define _SDTBufferToHex       SDBufferToHexW
#define _SDTIntToHex          SDIntToHexW
#define _SDTStrTrimLeft       SDStrTrimLeftW
#define _SDTStrTrimRight      SDStrTrimRightW
#define _SDTStrTrim           SDStrTrimW
#define _SDTStrtod            SDWCStrtod
#define _SDTSplitStrings      SDSplitStringsW
#define _SDTAtou64            SDWtou64
#define _SDTAtou              SDWtou
#define _SDTItoa              SDItoa
#define _SDTItoa64            SDItoa64
#define _SDTUtoa              SDUtoa
#define _SDTUtoa64            SDUtoa64
#else
#define _SDT(x)               x
#define _SDTStrlen            SDStrlen
#define _SDTStrnlen           SDStrnlen
#define _SDTStrcat            SDStrcat
#define _SDTStrcpy            SDStrcpy
#define _SDTStrncpy           SDStrncpy
#define _SDTStrcmp            SDStrcmp
#define _SDTStrncmp           SDStrncmp
#define _SDTStrchr            SDStrchr
#define _SDTStrrchr           SDStrrchr
#define _SDTStrcasecmp        SDStrcasecmp
#define _SDTA2T(x)            (x)
#define _SDTT2A(x)            (x)
#define _SDTT2Local(x)        (x)
#define _SDTLocal2T(x)        (x)
#define _SDTUTF82W(x)         (x)
#define _SDTT2UTF8(x)         SDLocal2UTF8(x).c_str()
#define _SDTUTF82T(x)         SDUTF82Local(x).c_str()
#define _SDTSource2T(x,y)     SDSource2Local(x,y).c_str()
#define _SDTTtoi		      SDAtoi
#ifdef __linux__
#define _SDTsnprintf		  snprintf
#elif defined(__APPLE__)
#define _SDTsnprintf		      snprintf
#else
#define _SDTsnprintf		  _snprintf
#endif
#define _SDTvsprintf          vsprintf
#define _SDTvsnprintf          vsnprintf
#define _SDTsprintf		      sprintf
#define _SDTStrchr            SDStrchr
#define _SDTStrrchr           SDStrrchr
#define _SDTStrtok            SDStrtok
#define _SDTStrlwr            SDStrlwr
#define _SDTStrupr            SDStrupr
#define _SDTStrcat             SDStrcat
#define _SDTStrncat             SDStrncat
#define _SDTStrncasecmp   SDStrncasecmp
#define _SDTBufferToHex       SDBufferToHexA
#define _SDTIntToHex          SDIntToHexA
#define _SDTStrTrimLeft       SDStrTrimLeftA
#define _SDTStrTrimRight      SDStrTrimRightA
#define _SDTStrTrim           SDStrTrimA
#define _SDTStrtod            SDStrtod
#define _SDTSplitStrings      SDSplitStringsA
#define _SDTAtou64            SDAtou64
#define _SDTAtou              SDAtou
#define _SDTItoa              SDItow
#define _SDTItoa64            SDItow64
#define _SDTUtoa              SDUtow
#define _SDTUtoa64            SDUtow64
#endif

    /**
    * @brief
    * 获取字符串的长度
    * @param string : 以0为结束符的字符串
    * @return : 字符串的长度
    */
    inline UINT32  SDStrlen( const CHAR* string )
    {
        return (UINT32)strlen(string);
    }

    /**
    * @brief
    * 获取Unicode字符串的长度
    * @param string : 以0为结束符的字符串
    * @return : Unicode字符串的长度
    */
    inline UINT32  SDWcslen( const WCHAR* string )
    {
        return (UINT32)wcslen(string);
    }

    /**
    * @brief
    * 获取字符串的长度
    * @param pszStr : 字符串
    * @param sizeInBytes :最大检测长度
    * @return 获取字符串的长度，如果字符串的长度超过sizeInBytes，返回sizeInBytes
    */
    inline UINT32  SDStrnlen(const CHAR* pszStr, UINT32 sizeInBytes )
    {
        return (UINT32)strnlen(pszStr, sizeInBytes);
    }

    /**
    * @brief
    * 获取Unicode字符串的长度
    * @param pszStr : Unicode字符串
    * @param sizeInBytes :最大检测长度
    * @return 获取Unicode字符串的长度，如果Unicode字符串的长度超过sizeInBytes，返回sizeInBytes
    */
    inline UINT32  SDWcsnlen(const WCHAR* pszStr, size_t sizeInBytes)
    {
        return (UINT32)wcsnlen(pszStr, sizeInBytes);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串
    * @param strSource : 以0为结束符的源字符串
    * @return 结果字符串，使用的是目标字符串空间，请预留足够的空间
    */
    inline CHAR*  SDStrcat(CHAR* strDestination, const CHAR* strSource )
    {
        return strcat(strDestination, strSource);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串
    * @param dstLen : 目标串内存的总长度,如果合并后的长度大于此长度,不会进行合并,返回空串,不修改目标串
    * @param strSource : 以0为结束符的源字符串
    * @return 结果字符串，使用的是目标字符串空间,如果合并后的长度大于此长度,不会进行合并,返回NULL,不修改目标串
    */
    inline CHAR *  SDStrSafeCat(CHAR* strDestination, UINT32 dstLen, const CHAR* strSource)
    {
        if (dstLen < strlen(strDestination) + strlen(strSource) + 1)
        {
            return NULL;
        }
        return strcat(strDestination,strSource);
    }

    /**
    * @brief
    * 在目标Unicode字符串后添加一个Unicode字符串
    * @param strDestination : 以0为结束符的目标Unicode字符串
    * @param strSource : 以0为结束符的源Unicode字符串
    * @return 结果Unicode字符串，使用的是目标Unicode字符串空间，请预留足够的空间
    */
    inline WCHAR*   SDWcscat(WCHAR*strDestination, const WCHAR *strSource )
    {
        return wcscat(strDestination, strSource);
    }

    /**
    * @brief
    * 在目标字符串后添加一个字符串
    * @param strDestination : 以0为结束符的目标字符串
    * @param strSource : 源字符串
    * @param len : 源字符串的最大添加长度
    * @return 结果字符串，使用的是目标字符串空间，请预留足够的空间
    */
    inline CHAR*  SDStrncat(CHAR *strDestination, const CHAR *strSource, UINT32 len)
    {
        return strncat(strDestination, strSource, len);
    }

    /**
    * @brief
    * 在目标Unicode字符串后添加一个Unicode字符串
    * @param strDestination : 以0为结束符的目标Unicode字符串
    * @param strSource : 源Unicode字符串
    * @param len : 源Unicode字符串的最大添加长度
    * @return 结果Unicode字符串，使用的是目标Unicode字符串空间，请预留足够的空间
    */
    inline WCHAR*  SDWcsncat(WCHAR *strDestination, const WCHAR *strSource, UINT32 len)
    {
        return wcsncat(strDestination, strSource, len);
    }

    /**
    * @brief
    * 拷贝字符串
    * @param strDestination : 以0为结束符的目标缓冲区，请保证有足够的空间
    * @param strSource : 以0为结束符的源字符串
    * @return 目标字符串
    */
    inline CHAR*  SDStrcpy( CHAR *strDestination, const CHAR *strSource )
    {
        return strcpy(strDestination, strSource);
    }

    /**
    * @brief
    * 安全拷贝字符串
    * @param strDestination : 需要拷贝字符串的目标缓冲区
    * @param numberOfElements : 目标字符串最大长度
    * @param strSource : 以0结尾的源字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回NULL
    */
    inline CHAR* SDStrSafeCpy( char *strDestination, size_t numberOfElements, const CHAR *strSource )
    {
        if (numberOfElements  < strlen(strSource) +1)
        {
            return NULL;
        }

        return strncpy(strDestination,strSource,numberOfElements);
    }

    /**
    * @brief
    * 安全拷贝字符串
    * @param strDestination : 需要拷贝字符串的目标缓冲区
    * @param numberOfElements : 目标字符串最大长度
    * @param strSource : 以0结尾的源字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回NULL
    */
    template <class T>
    inline void SDStrSafeCpy(T& Destination, const char* Source) 
    {
        // Use cast to ensure that we only allow character arrays
        (static_cast<char[sizeof(Destination)]>(Destination));

        // Copy up to the size of the buffer
        SDStrSafeCpy(Destination, Source, sizeof(Destination));
    }


    /**
    * @brief
    * 拷贝Unicode字符串
    * @param strDestination : 以0结尾的目标缓冲区，请保证有足够的空间
    * @param strSource : 以0结尾的源字符串
    * @return 目标字符串
    */
    inline WCHAR*  SDWcscpy( WCHAR *strDestination, const WCHAR *strSource )
    {
        return wcscpy(strDestination, strSource);
    }

    /**
    * @brief
    * 安全拷贝Unicode字符串
    * @param strDestination : 需要拷贝Unicode字符串的目标缓冲区
    * @param numberOfElements : 目标字Unicode符串最大长度
    * @param strSource : 以0结尾的源Unicode字符串
    * @return 返回目标串,如果目标缓冲区的长度小于源串,返回NULL
    */
    WCHAR * SDWcsSafeCpy( WCHAR *strDestination, size_t numberOfElements, const WCHAR *strSource );

    /**
    * @brief
    * 拷贝字符串
    * @param strDestination : 以0结尾的目标字符串
    * @param strSource : 以0结尾的源字符串
    * @param len : 最大拷贝长度
    * @return 返回目标字符串
    */
    inline CHAR*  SDStrncpy( CHAR *strDestination, const CHAR *strSource, UINT32 len )
    {
        return strncpy(strDestination, strSource, len);
    }

    /**
    * @brief
    * 拷贝Unicode字符串
    * @param strDestination : 以0结尾的目标Unicode字符串
    * @param strSource : 以0结尾的源Unicode字符串
    * @param len : 最大拷贝长度
    * @return 返回目标Unicode字符串
    */
    inline WCHAR*  SDWcsncpy( WCHAR *strDestination, const WCHAR *strSource, UINT32 len )
    {
        return wcsncpy(strDestination, strSource, len);
    }

    /**
    * @brief
    * 比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32  SDStrcmp( const CHAR *string1, const CHAR *string2 )
    {
        return strcmp(string1, string2);
    }

    /**
    * @brief
    * 比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32  SDWcscmp( const WCHAR *string1, const WCHAR *string2 )
    {
        return wcscmp(string1, string2);
    }

    /**
    * @brief
    * 比较字符串
    * @param string1 : 以0结束的字符串1
    * @param string2 : 以0结束的字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32  SDStrncmp( const CHAR *string1, const CHAR *string2, UINT32 count )
    {
        return strncmp( string1, string2, count );
    }

    /**
    * @brief
    * 比较Unicode字符串
    * @param string1 : 以0结束的Unicode字符串1
    * @param string2 : 以0结束的Unicode字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32  SDWcsncmp( const WCHAR *string1, const WCHAR *string2, size_t count )
    {
        return wcsncmp( string1, string2, count );
    }

    /**
    * @brief
    * 在一个字符串中查找某个字符的位置
    * @param string : 以0结束的字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的第一次出现的位置，或者没有找到返回NULL
    */
    inline CHAR*  SDStrchr( const CHAR *string, INT32 c )
    {
        return (CHAR*)strchr(string, c);
    }

    /**
    * @brief
    * 在一个Unicode字符串中查找某个字符的位置
    * @param string : 以0结束的Unicode字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的第一次出现的位置，或者没有找到返回NULL
    */
    inline WCHAR*  SDWcschr( const WCHAR *string, WCHAR c )
    {
        return (WCHAR*)wcschr(string, c);
    }

    /**
    * @brief
    * 在一个字符串中逆向查找某个字符的位置
    * @param string : 以0结束的字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的最后一次出现的位置，或者没有找到返回NULL
    */
    inline CHAR*  SDStrrchr( const CHAR *string, INT32 c )
    {
        return (CHAR*)strrchr(string ,c);
    }

    /**
    * @brief
    * 在一个Unicode字符串中逆向查找某个字符的位置
    * @param string : 以0结束的Unicode字符串
    * @param c : 需要被查找的字符字符
    * @return 指向查找的该字符的最后一次出现的位置，或者没有找到返回NULL
    */
    wchar_t*  SDWcsrchr( const wchar_t *string, wchar_t c );

    /**
    * @brief
    * 无视字符大小写，比较字符串
    * @param s1 : 以0结束的字符串1
    * @param s2 : 以0结束的字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32  SDStrcasecmp(const CHAR *s1, const CHAR *s2)
    {
#if (defined(WIN32) || defined(WIN64))
        return _stricmp(s1, s2);
#else
        return strcasecmp(s1, s2);
#endif
    }

    /**
    * @brief
    * 无视字符大小写，比较字符串
    * @param s1 : 以0结束的字符串1
    * @param s2 : 以0结束的字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    inline INT32  SDStrncasecmp(const CHAR *s1, const CHAR *s2, UINT32 count)
    {
#if (defined(WIN32) || defined(WIN64))
        return _strnicmp(s1, s2, count);
#else
        return strncasecmp(s1, s2, count);
#endif
    }

    /**
    * @brief
    * 无视字符大小写，比较Unicode字符串
    * @param s1 : 以0结束的Unicode字符串1
    * @param s2 : 以0结束的Unicode字符串2
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
     INT32  SDWcscasecmp(const wchar_t *s1, const wchar_t *s2);

    /**
    * @brief
    * 无视字符大小写，比较Unicode字符串
    * @param s1 : 以0结束的Unicode字符串1
    * @param s2 : 以0结束的Unicode字符串2
    * @param count : 最大比较长度
    * @return 比较结果
    * < 0 字符串1小于字符串2
    * 0 字符串1等于字符串2
    * > 0 字符串1大于字符串2
    */
    INT32  SDWcsncasecmp(const wchar_t *s1, const wchar_t *s2, UINT32 count);


    /**
    * @brief
    * 将某个字符串切分为多个字符串
    * @param pszStr : 第一次输入被切分的字符串，之后输入NULL
    * @param delim : 分割字符串，如“:”，“,”，“ ”等
    * @return 依次返回被切割的子字符串，如果没有新的子字符串，返回NULL
    */
    inline CHAR*  SDStrtok(CHAR *pszStr, const CHAR *delim)
    {
        return strtok(pszStr, delim);
    }

    /**
    * @brief
    * 将某个Unicode字符串切分为多个Unicode字符串
    * @param pszStr : 第一次输入被切分的Unicode字符串，之后输入NULL
    * @param delim : 分割Unicode字符串，如“:”，“,”，“ ”等
    * @return 依次返回被切割的子Unicode字符串，如果没有新的子Unicode字符串，返回NULL
    */
    inline wchar_t*  SDWcstok(wchar_t *pszStr, const wchar_t *delim);

    /**
    * @brief
    * 将字符串中的所有ANSI字符转化为小写
    * @param pszStr : 以0结尾的字符串
    * @return 被转化的字符串，使用原有字符串的空间，原有字符串将被破坏
    */
    CHAR*  SDStrlwr(CHAR* pszStr);


    /**
    * @brief
    * 将字符串中的所有ANSI字符转化为大写
    * @param pszStr : 以0结尾的字符串
    * @return 被转化的字符串，使用原有字符串的空间，原有字符串将被破坏
    */
    CHAR*  SDStrupr(CHAR* pszStr);

    /**
    * @brief
    * 将Unicode字符串中的所有字符转化为小写
    * @param pszStr : 以0结尾的Unicode字符串
    * @return 被转化的Unicode字符串，使用原有Unicode字符串的空间，原有Unicode字符串将被破坏
    */
    WCHAR*  SDWcslwr(WCHAR* pszStr);

    /**
    * @brief
    * 将Unicode字符串中的所有字符转化为大写
    * @param pszStr : 以0结尾的Unicode字符串
    * @return 被转化的Unicode字符串，使用原有Unicode字符串的空间，原有Unicode字符串将被破坏
    */
    WCHAR*  SDWcsupr(WCHAR* pszStr);

    /**
    * @brief
    * 将字符串转化为Unicode字符串
    * @param src : 以0结尾的字符串
    * @return 转化后的MultiBytes字符串
    */
	std::wstring  SDA2W(const CHAR *src);


    /**
    * @brief
    * 将Unicode字符串转化为ANSI字符串
    * @param src : 以0结尾的Unicode字符串
    * @return 转化后的ANSI字符串
    */
	std::string  SDW2A(const WCHAR *src);
  
    /**
    * @brief
    * 将Unicode字符集转化为本地操作系统设置的字符集编码
    * @param src : 以0结尾的Unicode字符串
    * @return 转化后的字符串
    */
	std::string  SDW2Local(const WCHAR *src);

    /**
    * @brief
    * 将本地操作系统设置的字符集编码转换为Unicode字符集
    * @param src : 以0结尾的字符串
    * @return 转化后的字符串
    */
	std::wstring  SDLocal2W(const CHAR *src);

    /**
    * @brief
    * 将Unicode字符集转换为UTF8编码集
    * @param src : 以0结尾的Unicode字符串
    * @return 转化后的字符串
    */
	std::string  SDW2UTF8(const WCHAR *src);

    /**
    * @brief
    * 将UTF8编码集转换为Unicode字符集
    * @param src : 以0结尾的字符串
    * @return 转化后的字符串
    */
    std::wstring  SDUTF82W(const CHAR *src);

    /**
    * @brief
    * 将UTF8编码集转换为ASCII字符集
    * @param src : 以0结尾的字符串
    * @return 转化后的字符串
    */
    std::string  SDUTF82A(const CHAR *src);

    /**
    * @brief
    * 将ASCII字符集转换为UTF8编码集
    * @param src : 以0结尾的字符串
    * @return 转化后的字符串
    */
    std::string  SDA2UTF8(const CHAR *src);
  
    /**
    * @brief
    * 将本地操作系统设置的字符集编码转换为UTF8编码集
    * @param src : 以0结尾的字符串
    * @return 转化后的字符串
    */
    std::string  SDLocal2UTF8(const CHAR *src);

    /**
    * @brief
    * 将UTF8编码集转换为本地操作系统设置的字符集编码
    * @param src : 以0结尾的字符串
    * @return 转化后的字符串
    */
    std::string  SDUTF82Local(const CHAR *src);

    /**
    * @brief
    * 将UTF8编码集转换为本地操作系统设置的字符集编码
    * @param src : 以0结尾的字符串
    * @return 转化后的字符串
    */
    std::wstring SDSource2W(const CHAR *szCodePage,const CHAR *src);

    /**
    * @brief
    * 将UTF8编码集转换为本地操作系统设置的字符集编码
    * @param src : 以0结尾的字符串
    * @return 转化后的字符串
    */
    std::string SDSource2Local(const CHAR *szCodePage,const CHAR *src);

    /**
    * @brief
    * 将UTF8编码集转换为本地操作系统设置的字符集编码
    * @param src : 以0结尾的字符串
    * @return 转化后的字符串
    */
    std::string SDUTF82Source(const CHAR *szCodePage,const CHAR *src);

	/**
	* @brief
	* 将字符串转换为32位无符号数字
	* @param pStr : 被转换的字符串
	* @return 32位无符号数字
	*/
    UINT32  SDAtou(const CHAR* pStr);

	/**
	* @brief
	* 将字符串转换为64位无符号数字
	* @param pStr : 待转化的字符串
	* @return 64位无符号数字
	*/
	UINT64  SDAtou64(const CHAR* pStr); 

	/**
	* @brief
	* 将Unicode字符串转换为64位无符号数字
	* @param pStr : 待转化的字符串
	* @return 64位无符号数字
	*/
	UINT64  SDWtou64(const WCHAR* pStr); 

    UINT32  SDWtou(const WCHAR* pStr);

    /**
	* @brief
	* 将Unicode字符串转换为32位有符号数字
	* @param _Str : 待转化的字符串
	* @return 32位有符号数字
	*/
	inline INT32 SDWtoi(const WCHAR *_Str)
	{
		return atoi(SDW2A(_Str).c_str());
	}

    /**
    * @brief
    * 将一个数字转化为字符串
    * @param pBuf : 转化后的字符存储空间
    * @param buflen : 字符存储空间的最大长度
    * @param dwNum : 将被转化的数字
    * @return 转化后字符串使用的空间长度
    */
    inline INT32  SDItoa(CHAR* pBuf, UINT32 buflen, INT32 dwNum)
    {
        return SDSnprintf(pBuf, buflen, "%d", dwNum);
    }

	/**
    * @brief
    * 将一个数字转化为Unicode字符串
    * @param pBuf : 转化后的字符存储空间
    * @param buflen : 字符存储空间的最大长度
    * @param dwNum : 将被转化的数字
    * @return 转化后字符串使用的空间长度
    */
    inline INT32  SDItow(WCHAR* pBuf, UINT32 buflen, INT32 dwNum)
    {
        return swprintf(pBuf, buflen, L"%d", dwNum);
    }


	/**
	* @brief
	* 将有64位符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dqNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	inline INT32  SDItoa64(CHAR *pBuf, UINT32 buflen, INT64 dqNum)
	{
		return SDSnprintf(pBuf, buflen, SDFMT_I64 , dqNum);
	}

	/**
	* @brief
	* 将有64位符号整型转换为Unicode字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dqNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
    inline INT32  SDItow64(WCHAR *pBuf, UINT32 buflen, INT64 dqNum)
    {
        return swprintf(pBuf, buflen,WCHAR_SDFMT_I64 , dqNum);
    }

	/**
	* @brief
	* 将无符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	inline INT32  SDUtoa(CHAR *pBuf, UINT32 buflen, UINT32 dwNum)
	{
		return SDSnprintf(pBuf, buflen, "%u", dwNum);
	}

	/**
	* @brief
	* 将无符号整型转换为Unicode字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dwNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
    inline INT32  SDUtow(WCHAR *pBuf, UINT32 buflen, UINT32 dwNum)
    {
        return swprintf(pBuf, buflen, L"%u", dwNum);
    }


	/**
	* @brief
	* 将有64位无符号整型转换为字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dqNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
	inline INT32  SDUtoa64(CHAR* pBuf,UINT32 buflen, UINT64 dqNum)
	{
		return SDSnprintf(pBuf, buflen, SDFMT_U64, dqNum);
	}
   
	/**
	* @brief
	* 将有64位无符号整型转换为Unicode字符串
	* @param pBuf : 转化后的字符存储空间
	* @param buflen : 字符存储空间的最大长度
	* @param dqNum : 将被转化的数字
	* @return 转化后字符串使用的空间长度
	*/
    inline INT32  SDUtow64(WCHAR* pBuf,UINT32 buflen, UINT64 dqNum)
    {
        return SDSwprintf(pBuf, buflen, WCHAR_SDFMT_U64, dqNum);
    }

    /**
    * @brief
    * 将字符串转化为浮点数
    * @param nptr : 将被转化的以0结尾的字符串
    * @param endptr : [输出参数]若不为NULL，则遇到无法转化的字符从这里返回其指针
    * @return 获取的浮点数
    */
    inline DOUBLE  SDStrtod(const CHAR *nptr, CHAR **endptr)
    {
        return strtod(nptr, endptr);
    }

	/**
    * @brief
    * 将Unicode字符串转化为浮点数
    * @param nptr : 将被转化的以0结尾的字符串
    * @param endptr : [输出参数]若不为NULL，则遇到无法转化的字符从这里返回其指针
    * @return 获取的浮点数
    */
    inline DOUBLE  SDWCStrtod(const WCHAR *nptr, WCHAR **endptr)
    {
        return wcstod(nptr, endptr);
    }
    
    /**
    * @brief
    * split a string将字符串分割为一个字符串的vector
    * @param sSource : 被分割的字符串
    * @param delim : 分隔符，如:“:”，“,”，“ ”等
    * @return a vector to store strings splited from sSource
    */
    std::vector<std::string>  SDSplitStringsA(const std::string &sSource, CHAR delim);
   
	/**
    * @brief
    * split a wstring将Unicode字符串分割为一个Unicode字符串的vector
    * @param sSource : 被分割的Unicode字符串
    * @param delim : 分隔符，如:“:”，“,”，“ ”等
    * @return a vector to store wstrings splited from sSource
    */
	std::vector<std::wstring>  SDSplitStringsW(const std::wstring &sSource, WCHAR delim);

    /**
    * @brief
    * 将字符串转化为大写
    * @param pszStr : 将被转化的字符串
    * @return 转化后的字符串
    */
    std::string  SDStrupr(std::string &pszStr);

    /**
    * @brief
    * 将字符串转化为小写
    * @param pszStr : 将被转化的字符串
    * @return 转化后的字符串
    */
    std::string  SDStrlwr(std::string &pszStr);

    /**
    * @brief
    * 将一个字符串置空
    * @param Destination : 需要置空的类型
    * @return VOID
    */
    template <class T>
    inline VOID  SDZeroString(T &Destination) throw()
    {
        (static_cast<CHAR[sizeof(Destination)]>(Destination));
        Destination[0] = '\0';
        Destination[sizeof(Destination)-1] = '\0';
    }


    /**
    * @brief
    * 将二进制Buffer转换为字符串,
    * @param pBuf    二进制Buffer指针
    * @param bufLen  二进制Buffer长度
    * @param pSplitter 输出的二进制数据间隔符
	* @param lineLen 源字符串分割多行的每行长度,该长度不包含分隔符的长度
    * @return 转换后的字符串
    */
    std::string  SDBufferToHexA(const CHAR *pBuf, UINT32 bufLen, const CHAR *pSplitter = "", INT32 lineLen = -1);

	/**
    * @brief
    * 将二进制Buffer转换为Unicode字符串,
    * @param pBuf    二进制Buffer指针
    * @param bufLen  二进制Buffer长度
    * @param pSplitter 输出的二进制数据间隔符
	* @param lineLen 源字符串分割多行的每行长度,该长度不包含分隔符的长度
    * @return 转换后的Unicode字符串
    */
    std::wstring  SDBufferToHexW(const WCHAR *pBuf, UINT32 bufLen, const WCHAR *pSplitter = L"", INT32 lineLen = -1);

    /**
    * @brief
    * 将32位有符号整型数字转换为二进制串,
    * @param dwNum : 32位有符号整型数字
    * @return 转换后的字符串
    */
    std::string  SDIntToHexA(INT32 dwNum);

	/**
    * @brief
    * 将32位有符号整型数字转换为二进制串,
    * @param dwNum : 32位有符号整型数字
    * @return 转换后的Unicode字符串
    */
    std::wstring  SDIntToHexW(INT32 dwNum);

    /**
    * @brief
    * 将str左边的ch去掉，原字符串将被修改
    * @param pszStr : 去掉左边的ch字符
    * @param pTrimStr : 被去掉的字符
    * @return 去掉字符pTrimStr后的字符串
    */
    std::string  SDStrTrimLeftA(std::string &pszStr, const CHAR *pTrimStr = " \r\t\n");

	/**
    * @brief
    * 将str左边的ch去掉，原Unicode字符串将被修改
    * @param pszStr : 去掉左边的ch字符
    * @param pTrimStr : 被去掉的字符
    * @return 去掉字符pTrimStr后的Unicode字符串
    */
    std::wstring  SDStrTrimLeftW(std::wstring &pszStr, const WCHAR *pTrimStr = L" \r\t\n");

    /**
    * @brief
    * 将str右边的ch去掉，原字符串将被修改
    * @param pszStr : 去掉右边的ch字符
    * @param pTrimStr : 被去掉的字符
    * @return 去掉字符pTrimStr后的字符串
    */
    std::string  SDStrTrimRightA(std::string &pszStr, const CHAR *pTrimStr = " \r\t\n");

	/**
    * @brief
    * 将str右边的ch去掉，原Unicode字符串将被修改
    * @param pszStr : 去掉右边的ch字符
    * @param pTrimStr : 被去掉的字符
    * @return 去掉字符pTrimStr后的Unicode字符串
    */
	std::wstring  SDStrTrimRightW(std::wstring &pszStr, const WCHAR *pTrimStr = L" \r\t\n");

    /**
    * @brief
    * 将str左右两边的pTrimStr去掉，原字符串将被修改
    * @param pszStr : 去掉左右两边的ch字符
    * @param pTrimStr : 被去掉的字符
    * @return 去掉字符pTrimStr后的字符串
    */
    std::string  SDStrTrimA(std::string &pszStr, const CHAR* pTrimStr = " \r\t\n");
   
	/**
    * @brief
    * 将str左右两边的pTrimStr去掉，原Unicode字符串将被修改
    * @param pszStr : 去掉左右两边的ch字符
    * @param pTrimStr : 被去掉的字符
    * @return 去掉字符pTrimStr后的Unicode字符串
    */
	std::wstring  SDStrTrimW(std::wstring &pszStr, const WCHAR* pTrimStr = L" \r\t\n");

    /** @} */


    /**
    * @brief Unicode字符串格式化
    * @param buffer : 存储格式化后的buffer
    * @param n      : Unicode字符串长度
    * @param format : 待格式化的Unicode字符串
    * @return 正值:成功转换后字符串的长度 负值:格式化失败
    */
	INT32  SDsnwprintf(WCHAR* buffer, size_t n, const WCHAR* format, ...);

    template <class T>
    inline void SDSafeSprintf(T& Destination, const CHAR *format, ...)
    {
        (static_cast<char[sizeof(Destination)]>(Destination));

        va_list args;
        va_start(args,format);
        _vsnprintf(Destination, sizeof(Destination)-1, format, args);
        va_end(args);
        Destination[sizeof(Destination)-1] = '\0';
    }

}//namespace UCODE

#endif


