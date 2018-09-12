#include "ucstring.h"
namespace UCODE
{
    INT32  SDsnwprintf(wchar_t* buffer, size_t n, const wchar_t* format, ...)
    {
        int retval;
        va_list argptr;
        va_start( argptr, format );
        retval = vswprintf( buffer, n, format, argptr );
        va_end( argptr );
        return retval;
    } 

    wchar_t*  SDWcsupr(WCHAR *str)
    {
#if (defined(WIN32) || defined(WIN64))
        return wcsupr(str);
#else
        std::transform(str, str + SDWcslen(str),
            str, ::towupper);
        return str;
#endif
    }

    wchar_t*  SDWcslwr(WCHAR *str)
    {
#if (defined(WIN32) || defined(WIN64))
        return wcslwr(str);
#else
        std::transform(str, str + SDWcslen(str),
            str, ::tolower);
        return str;
#endif
    }
    
    wchar_t*  SDWcstok(wchar_t *pszStr, const wchar_t *delim)
    {
#if (defined(WIN32) || defined(WIN64))
        return wcstok(pszStr, delim);
#else
        return wcstok(pszStr, delim, NULL);
#endif
    }

    INT32  SDWcsncasecmp(const wchar_t *s1, const wchar_t *s2, UINT32 count)
    {
#if (defined(WIN32) || defined(WIN64))
        return _wcsnicmp(s1, s2, count);
#else
        return wcsncasecmp(s1, s2, count);
#endif
    }

    INT32  SDWcscasecmp(const wchar_t *s1, const wchar_t *s2)
    {
#if (defined(WIN32) || defined(WIN64))
        return _wcsicmp(s1, s2);
#else
        return wcscasecmp(s1, s2);
#endif
    }

    wchar_t*  SDWcsrchr( const wchar_t *string, wchar_t c )
    {
        return (wchar_t*)wcsrchr(string, c);
    }

    wchar_t * SDWcsSafeCpy( wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource )
    {
        if (numberOfElements  < wcslen(strSource) +1)
        {
            return NULL;
        }
        return wcsncpy(strDestination,strSource,numberOfElements);
    }


    std::wstring  SDBufferToHexW(const WCHAR *pBuf, UINT32 bufLen, const WCHAR *pSplitter, INT32 lineLen)
    {
        static WCHAR szHex[] = L"0123456789abcdef";

        WCHAR* p = (WCHAR*)pBuf;
        std::wstring strResult;
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
                strResult += L"\n";
            }
        }
        return strResult;
    }

    std::wstring  SDIntToHexW(INT32 num)
    {
        WCHAR szBuf[16] = {0};
        swprintf(szBuf, sizeof(szBuf), L"0x%x",num);
        return szBuf;
    }

    std::wstring   SDStrTrimLeftW(std::wstring & str, const WCHAR *trimStr )
    {
        for (std::wstring::iterator itr = str.begin(); itr != str.end(); ++itr)
        {
            if (SDWcschr(trimStr,*itr) == NULL)
            {
                str.erase(str.begin(), itr);
                break;
            }
        }
        return str;
    }

    std::wstring   SDStrTrimRightW(std::wstring & str, const WCHAR* trimStr )
    {
        for (std::wstring::reverse_iterator itr = str.rbegin(); itr != str.rend(); ++itr)
        {
            if (SDWcschr(trimStr,*itr) == NULL)
            {
                str.erase(itr.base(), str.end());
                break;
            }
        }
        return str;
    }

    std::wstring   SDStrTrimW(std::wstring & str, const WCHAR* pTrimStr)
    {
        SDStrTrimRightW(str, pTrimStr);
        return SDStrTrimLeftW(str, pTrimStr);
    }

    std::vector<std::wstring>  SDSplitStringsW(const std::wstring& sSource, WCHAR delim)
    {
        std::wstring::size_type pos1, pos2;
        std::vector<std::wstring> strParams;

        pos2 = 0;
        while (pos2 != std::wstring::npos)
        {
            pos1 = sSource.find_first_not_of(delim, pos2);
            if (pos1 == std::wstring::npos)
                break;
            pos2 = sSource.find_first_of(delim, pos1 + 1);
            if (pos2 == std::wstring::npos)
            {
                if (pos1 != sSource.size())
                    strParams.push_back(sSource.substr(pos1));
                break;
            }
            strParams.push_back(sSource.substr(pos1, pos2 - pos1));
        }
        return strParams;
    }

    UINT64  SDWtou64(const WCHAR* pStr)
    {
        UINT64 result = 0;
        const WCHAR * pos = pStr;
        while (*pos != 0 )
        {
            if (iswdigit(*pos) || *pos == L'+')
            {
                UINT64 d = *pos - L'0';
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


    UINT32  SDWtou(const WCHAR* pStr)
    {
        UINT32 result = 0;
        const WCHAR * pos = pStr;
        while (*pos != 0)
        {
            if (iswdigit(*pos) || *pos == L'+')
            {
                UINT32 d = *pos - L'0';
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


};

