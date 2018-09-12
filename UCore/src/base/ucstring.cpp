#include "ucstring.h"
#include <algorithm>
#include <string.h>
#include "uctranslate.h"
namespace UCODE
{
#if (defined(WIN32) || defined(WIN64))
     #define UNICODE_CHARSET      "UCS-2LE"
#else
     #define UNICODE_CHARSET      "UCS-4LE"
#endif
   
    #define ASCII_CHARSET             "ASCII"
    #define UTF8_CHARSET             "UTF-8"
    #define SDPAGE                          4096

    CHAR*  SDStrlwr(CHAR *str)
    {
    #if (defined(WIN32) || defined(WIN64))
        return strlwr(str);
    #else
        std::transform(str, str + SDStrlen(str),
                       str, ::tolower);
        return str;
    #endif
    }

    CHAR*  SDStrupr(CHAR *str)
    {
    #if (defined(WIN32) || defined(WIN64))
        return strupr(str);
    #else
        std::transform(str, str + SDStrlen(str),
                       str, ::toupper);
        return str;
    #endif
    }



    std::vector<std::string>  SDSplitStringsA(const std::string& sSource, CHAR delim)
    {
        std::string::size_type pos1, pos2;
        std::vector<std::string> strParams;

        pos2 = 0;
        while (pos2 != std::string::npos)
        {
            pos1 = sSource.find_first_not_of(delim, pos2);
            if (pos1 == std::string::npos)
                break;
            pos2 = sSource.find_first_of(delim, pos1 + 1);
            if (pos2 == std::string::npos)
            {
                if (pos1 != sSource.size())
                    strParams.push_back(sSource.substr(pos1));
                break;
            }
            strParams.push_back(sSource.substr(pos1, pos2 - pos1));
        }
        return strParams;
    }

    std::string   SDStrupr(std::string& str)
    {
        return SDStrupr(const_cast<CHAR*>(str.c_str()));
    }

    std::string   SDStrlwr(std::string& str)
    {
        return SDStrlwr(const_cast<CHAR*>(str.c_str()));
    }

    std::string  SDBufferToHexA(const CHAR *pBuf, UINT32 bufLen, const CHAR *pSplitter, INT32 lineLen)
    {
	    static unsigned char szHex[] = "0123456789abcdef";

	    char* p = (char*)pBuf;
	    std::string strResult;
	    for (unsigned int i = 0; i < bufLen; ++i)
	    {
		    strResult += szHex[(p[i] >> 4) & 0xF];
		    strResult += szHex[p[i] & 0xF];
		    if (pSplitter != NULL && *pSplitter && i < bufLen - 1)
		    {
			    strResult.append(pSplitter);
		    }
		    if (lineLen > 0  && (i + 1) % lineLen == 0)
            {
                strResult += "\n";
            }
	    }
	    return strResult;
    }

    std::string  SDIntToHexA(INT32 num)
    {
        CHAR szBuf[16] = {0};
        SDSnprintf(szBuf, sizeof(szBuf), "0x%x",num);
        return szBuf;
    }

    std::string   SDStrTrimLeftA(std::string & str, const CHAR *trimStr )
    {
        for (std::string::iterator itr = str.begin(); itr != str.end(); ++itr)
        {
            if (strchr(trimStr,*itr) == NULL)
            {
                str.erase(str.begin(), itr);
                break;
            }
        }
        return str;
    }

    std::string   SDStrTrimRightA(std::string & str, const CHAR* trimStr )
    {
        for (std::string::reverse_iterator itr = str.rbegin(); itr != str.rend(); ++itr)
        {
            if (strchr(trimStr,*itr) == NULL)
            {
                str.erase(itr.base(), str.end());
                break;
            }
        }
        return str;
    }

    std::string   SDStrTrimA(std::string & str, const CHAR* pTrimStr)
    {
        SDStrTrimRightA(str, pTrimStr);
        return SDStrTrimLeftA(str, pTrimStr);
    }

    UINT32  SDAtou(const CHAR* pStr)
    {
        UINT32 result = 0;
        const CHAR * pos = pStr;
        while (*pos != 0)
        {
            if (SDIsDigit(*pos) || *pos == '+')
            {
                UINT32 d = *pos - '0';
                result = result * 10 + d;
            }
            else
            {
                return 0;
            }
            pos ++;
        }
        return result;
    }

    UINT64  SDAtou64(const CHAR* pStr)
    {
	    UINT64 result = 0;
	    const CHAR * pos = pStr;
	    while (*pos != 0 )
	    {
		    if (SDIsDigit(*pos) || *pos == '+')
		    {
			    UINT64 d = *pos - '0';
			    result = result * 10 + d;
		    }
		    else
		    {
			    return 0;
		    }
		    pos ++;
	    }
	    return result;
    }

    std::string  SDW2A(const WCHAR *src)
    {
	    CHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
	    SDIconvCovert(ASCII_CHARSET,UNICODE_CHARSET,(CHAR*)src,wcslen(src)*sizeof(WCHAR),szDesc,nSize);
	    std::string strResult = szDesc;
	    return strResult;
    }
    std::string  SDW2Local(const WCHAR *src)
    {
        CHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        CHAR* cpCharSet = (CHAR*)SDGetLocaleCharset();
        SDIconvCovert(cpCharSet,UNICODE_CHARSET,(CHAR*)src,wcslen(src)*sizeof(WCHAR),szDesc,nSize);
        std::string strResult = szDesc;
        return strResult;
    } 

    std::wstring  SDLocal2W(const CHAR *src)
    {
        WCHAR szDesc [SDPAGE*10] = {0};
        CHAR* cpCharSet = (CHAR*)SDGetLocaleCharset();
        size_t nSize = SDPAGE*10;
        SDIconvCovert(UNICODE_CHARSET,cpCharSet,(CHAR*)src,strlen(src),(CHAR*)szDesc,nSize);
        std::wstring strResult = szDesc;
        return strResult;
    } 

    std::wstring  SDA2W(const CHAR *src)
    {
	    WCHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
	    SDIconvCovert(UNICODE_CHARSET,ASCII_CHARSET,(CHAR*)src,strlen(src),(CHAR*)szDesc,nSize);
	    return szDesc;
    }

    std::string  SDW2UTF8(const WCHAR *src)
    {
        CHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        SDIconvCovert(UTF8_CHARSET,UNICODE_CHARSET,(CHAR*)src,wcslen(src)*sizeof(WCHAR),szDesc,nSize);
        std::string strResult = szDesc;
        return strResult;
    }

    std::wstring  SDUTF82W(const CHAR *src)
    {
        WCHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        SDIconvCovert(UNICODE_CHARSET,UTF8_CHARSET,(CHAR*)src,SDStrlen(src),(CHAR*)szDesc,nSize);
        std::wstring strResult = szDesc;
        return strResult;
    }

    std::wstring  SDSource2W(const CHAR *szCodePage,const CHAR *src)
    {
        WCHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        SDIconvCovert(UNICODE_CHARSET,(CHAR*)szCodePage,(CHAR*)src,SDStrlen(src),(CHAR*)szDesc,nSize);
        std::wstring strResult = szDesc;
        return strResult;
    }
    std::string  SDSource2Local(const CHAR *szCodePage,const CHAR *src)
    {
        CHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        CHAR* cpCharSet = (CHAR*)SDGetLocaleCharset();
        SDIconvCovert(cpCharSet,(CHAR*)szCodePage,(CHAR*)src,SDStrlen(src),(CHAR*)szDesc,nSize);
        std::string strResult = szDesc;
        return strResult;
    }
    std::string  SDUTF82Source(const CHAR *szCodePage,const CHAR *src)
    {
        CHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        CHAR* cpCharSet = (CHAR*)SDGetLocaleCharset();
        SDIconvCovert((CHAR*)szCodePage,UTF8_CHARSET,(CHAR*)src,SDStrlen(src),(CHAR*)szDesc,nSize);
        std::string strResult = szDesc;
        return strResult;
    }

    std::string  SDUTF82A(const CHAR *src)
    {
        CHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        SDIconvCovert(ASCII_CHARSET,UTF8_CHARSET,(CHAR*)src,SDStrlen(src),(CHAR*)szDesc,nSize);
        std::string strResult = szDesc;
        return strResult;
    }

    std::string  SDA2UTF8(const CHAR *src)
    {
        CHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        SDIconvCovert(UTF8_CHARSET,ASCII_CHARSET,(CHAR*)src,SDStrlen(src),(CHAR*)szDesc,nSize);
        std::string strResult = szDesc;
        return strResult;
    }

    std::string  SDLocal2UTF8(const CHAR *src)
    {
        CHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        CHAR* cpCharSet = (CHAR*)SDGetLocaleCharset();
        SDIconvCovert(UTF8_CHARSET,cpCharSet,(CHAR*)src,SDStrlen(src),(CHAR*)szDesc,nSize);
        std::string strResult = szDesc;
        return strResult;
    }

    std::string  SDUTF82Local(const CHAR *src)
    {
        CHAR szDesc [SDPAGE*10] = {0};
        size_t nSize = SDPAGE*10;
        CHAR* cpCharSet = (CHAR*)SDGetLocaleCharset();
        SDIconvCovert(cpCharSet,UTF8_CHARSET,(CHAR*)src,SDStrlen(src),(CHAR*)szDesc,nSize);
        std::string strResult = szDesc;
        return strResult;
    }
}



