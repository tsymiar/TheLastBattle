#include "uclogger.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ucstring.h>
/*snprintf*/
#if (defined(WIN32) || defined(WIN64))
#define vsnprintf _vsnprintf
#endif
using namespace UCODE;

//////////////////////////////////////////////////////////////////////////

CSDLogger::CSDLogger()
{
    m_pLogger = NULL;
    m_aszLogPrefix[0] = "";
    m_aszLogPrefix[1] = "[DBG]";
    m_aszLogPrefix[2] = "[INF]";
    m_aszLogPrefix[3] = "";
    m_aszLogPrefix[4] = "[WRN]";
    m_aszLogPrefix[5] = "";
    m_aszLogPrefix[6] = "";
    m_aszLogPrefix[7] = "";
    m_aszLogPrefix[8] = "[CRT]";
}

//////////////////////////////////////////////////////////////////////////

CSDLogger::~CSDLogger()
{
}

//////////////////////////////////////////////////////////////////////////

void UCAPI CSDLogger::SetLogger(ISDLogger* pLogger)
{
    m_pLogger = pLogger;
}

#ifdef UNICODE

void CSDLogger::_Log(UINT32 dwLevel, const WCHAR *pszFormat, va_list argptr)
{
	WCHAR szMsg[1024];
	memset(szMsg,0,sizeof(WCHAR)*1024);

	INT32 n = _SDTsnprintf(szMsg,sizeof(szMsg), _SDT("%ls "), SDA2W(m_aszLogPrefix[dwLevel]).c_str());
	vswprintf(szMsg+n, sizeof(szMsg)-n-1, pszFormat, argptr);    

	m_pLogger->LogText(szMsg);
}

void UCAPI CSDLogger::Critical(const WCHAR *format, ...)
{
	if (m_pLogger == NULL || format == NULL)
	{
		return;
	}
	va_list args;
	va_start(args,format);
	_Log(LOGLV_CRITICAL, format, args);    
	va_end(args);
}

void UCAPI CSDLogger::Info(const WCHAR *format, ...)
{
	if (m_pLogger == NULL || format == NULL)
	{
		return;
	}
	va_list args;
	va_start(args,format);
	_Log(LOGLV_INFO, format, args);
	va_end(args);
}
void UCAPI CSDLogger::Warn(const WCHAR *format, ...)
{
	if (m_pLogger == NULL || format == NULL)
	{
		return;
	}
	va_list args;
	va_start(args,format);
	_Log(LOGLV_WARN, format, args);
	va_end(args);
}
void UCAPI CSDLogger::Debug(const WCHAR *format, ...)
{
	if (m_pLogger == NULL || format == NULL)
	{
		return;
	}

	va_list args;
	va_start(args,format);
	_Log(LOGLV_DEBUG, format, args);
	va_end(args);
}

#else

void CSDLogger::_Log(UINT32 dwLevel, const char *pszFormat, va_list argptr)
{
	char szMsg[1024];
	szMsg[sizeof(szMsg)-1] = '\0';

	INT32 n = sprintf(szMsg, "%s ", m_aszLogPrefix[dwLevel]);
	vsnprintf(szMsg+n, sizeof(szMsg)-n-1, pszFormat, argptr);    

	m_pLogger->LogText(szMsg);
}
void UCAPI CSDLogger::Critical(const char *format, ...)
{
	if (m_pLogger == NULL || format == NULL)
	{
		return;
	}
	va_list args;
	va_start(args,format);
	_Log(LOGLV_CRITICAL, format, args);    
	va_end(args);
}

void UCAPI CSDLogger::Info(const char *format, ...)
{
	if (m_pLogger == NULL || format == NULL)
	{
		return;
	}
	va_list args;
	va_start(args,format);
	_Log(LOGLV_INFO, format, args);
	va_end(args);
}
void UCAPI CSDLogger::Warn(const char *format, ...)
{
	if (m_pLogger == NULL || format == NULL)
	{
		return;
	}
	va_list args;
	va_start(args,format);
	_Log(LOGLV_WARN, format, args);
	va_end(args);
}
void UCAPI CSDLogger::Debug(const char *format, ...)
{
	if (m_pLogger == NULL || format == NULL)
	{
		return;
	}

	va_list args;
	va_start(args,format);
	_Log(LOGLV_DEBUG, format, args);
	va_end(args);
}
#endif










INT32	LOGTHRD_BUF_SIZE	= 256*1024;	// 线程缓冲队列大小
UINT32	LOGTHRD_SLEEP_TIME	= 10;		// 线程空闲等待时间


