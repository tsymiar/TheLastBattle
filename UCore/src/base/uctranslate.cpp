#include "uctranslate.h"

#if UC_WITH_LIBICONV
namespace UCODE
{
	CSDTranslate::CSDTranslate()
	{
		m_hHandle = NULL;
	}

	CSDTranslate::~CSDTranslate()
	{
		if (m_hHandle)
			iconv_close(m_hHandle);
	}

	bool CSDTranslate::Init(const char *cpFromCharset,const char *cpToCharset)
	{
		m_strFromCharset = cpFromCharset;
		m_strToCharset   = cpToCharset;
		bool bResult = false;
		m_hHandle = iconv_open(cpToCharset, cpFromCharset);
		if ((iconv_t)-1 == m_hHandle) 
			printf("iconv open error!\n");
		else
			bResult = true;
		return bResult;
	}

	size_t CSDTranslate::Translate(char *src, size_t srcLen, char *desc, size_t descLen)
	{
		if ((iconv_t)-1 != m_hHandle)
		{
			char **inbuf = &src;
			char **outbuf = &desc;
			memset(desc, 0, descLen);
#ifdef __linux__
			return iconv(m_hHandle, (char**)inbuf, &srcLen, outbuf, &descLen);
#elif defined(__APPLE__)
            return iconv(m_hHandle, (char**)inbuf, &srcLen, outbuf, &descLen);
#else
			return iconv(m_hHandle, (const char**)inbuf, &srcLen, outbuf, &descLen);
#endif
		}
		return -1;
	}
	
	const CHAR *  SDGetLocaleCharset (void)
	{
		return locale_charset();
	}

	INT32 SDIconvCovert(CHAR *desc, CHAR *src, CHAR *input, size_t ilen, CHAR *output, size_t& olen)
	{
        INT32 nOutTotalLen = olen;
		CHAR **pin = &input;
		CHAR **pout = &output;
		iconv_t cd = iconv_open(desc, src);
		if (cd == (iconv_t)-1)
		{
			return -1;
		}
		memset(output, 0, olen);
#ifdef __linux__
		if (iconv(cd, (char**)pin, &ilen, pout, &olen))
#elif defined(__APPLE__)
        if (iconv(cd, (char**)pin, &ilen, pout, &olen))
#else
		if (iconv(cd, (const char**)pin, &ilen, pout, &olen))
#endif
		{
			iconv_close(cd);
			return -1;
		}
        olen = nOutTotalLen - olen;
		iconv_close(cd);
		return 0;
	}
}

#endif

