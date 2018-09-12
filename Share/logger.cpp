//#include "../stdafx.h"
//#include "logger.h"
//
//#include <time.h>
//#include <sys\timeb.h>
//#include <tchar.h>
//
//CLogger gLogger(LOG_Info|LOG_Warn|LOG_Error|LOG_Debug|LOG_DebugByLuoxin);
//
//CLogger::CLogger(int level)
//{
//	mPrintf = true;
//	mLogLevel = level;
//	TCHAR tempDir[MAX_PATH];
//	GetModuleFileName(NULL,tempDir,MAX_PATH);
//	TCHAR* p = _tcschr(tempDir,_T('\\'));
//	if (p==NULL) mLogDir = _T("C:\\Log\\");
//	else { *p = _T('\0'); mLogDir = tempDir; mLogDir += _T("\\Log\\"); }
//	CreateDirectory(mLogDir.c_str(),NULL);
//	mLogTitle[LOG_None] = _T("[NON]");
//	mLogTitle[LOG_Info] = _T("[INF]");
//	mLogTitle[LOG_Warn] = _T("[WRN]");
//	mLogTitle[LOG_Error] = _T("[ERR]");
//	mLogTitle[LOG_Debug] = _T("[DBG]");
//	mLogTitle[LOG_DebugByLuoxin] = _T("[DBGLX]");
//}
//
//CLogger::~CLogger()
//{
//}
//
//void CLogger::_Log(LOG_LEVEL loglevel, const TCHAR *file, const TCHAR *text)
//{
//	tstring& logtitle = mLogTitle[loglevel];
//	if (logtitle.empty()) logtitle = _T("Unknown");
//	struct timeb sTmb;
//	struct tm sTm;
//	ftime(&sTmb);
//	localtime_s(&sTm,&sTmb.time);
//	TCHAR text_time[1024];
//	_stprintf_s(text_time,1024,_T("%02d:%02d:%02d %s %s\n"),
//		sTm.tm_hour,sTm.tm_min,sTm.tm_sec,logtitle.c_str(),text);
//	tstring strFile = mLogDir + file + _T(".log");
//	FILE* fp = NULL;
//	if (ERROR_SUCCESS!=_wfopen_s(&fp,strFile.c_str(),_T("a+"))) return;
//	fputws(text_time,fp);
//	fclose(fp);
//	if (mPrintf) wprintf(text_time);
//}
//
//void CLogger::Log(LOG_LEVEL loglevel, const TCHAR *file, const TCHAR *format, ...)
//{
//	if (mLogLevel&loglevel)
//	{
//		va_list args;
//		TCHAR text[1024];
//		va_start(args,format);
//		_vsnwprintf_s(text,_countof(text),_TRUNCATE,format,args);
//		va_end(args);
//		_Log(loglevel,file,text);
//	}
//}
//
//void CLogger::Error(const TCHAR *file, const TCHAR *format, ...)
//{
//	if (mLogLevel&LOG_Error)
//	{
//		va_list args;
//		TCHAR text[1024];
//		va_start(args,format);
//		_vsnwprintf_s(text,_countof(text),_TRUNCATE,format,args);
//		va_end(args);
//		_Log(LOG_Error,file,text);
//	}
//}
//
//void CLogger::Info(const TCHAR *file, const TCHAR *format, ...)
//{
//	if (mLogLevel&LOG_Info)
//	{
//		va_list args;
//		TCHAR text[1024];
//		va_start(args,format);
//		_vsnwprintf_s(text,_countof(text),_TRUNCATE,format,args);
//		va_end(args);
//		_Log(LOG_Info,file,text);
//	}
//}
//
//void CLogger::Warn(const TCHAR *file, const TCHAR *format, ...)
//{
//	if (mLogLevel&LOG_Warn)
//	{
//		va_list args;
//		TCHAR text[1024];
//		va_start(args,format);
//		_vsnwprintf_s(text,_countof(text),_TRUNCATE,format,args);
//		va_end(args);
//		_Log(LOG_Warn,file,text);
//	}
//}
//
//void CLogger::Debug(const TCHAR *file, const TCHAR *format, ...)
//{
//	if (mLogLevel&LOG_Debug)
//	{
//		va_list args;
//		TCHAR text[1024];
//		va_start(args,format);
//		_vsnwprintf_s(text,_countof(text),_TRUNCATE,format,args);
//		va_end(args);
//		_Log(LOG_Debug,file,text);
//	}
//}
//
//void CLogger::DebugByLuoxin(const TCHAR *file, const TCHAR *format, ...)
//{
//	if (mLogLevel&LOG_DebugByLuoxin)
//	{
//		va_list args;
//		TCHAR text[1024];
//		va_start(args,format);
//		_vsnwprintf_s(text,_countof(text),_TRUNCATE,format,args);
//		va_end(args);
//		_Log(LOG_DebugByLuoxin,file,text);
//	}
//}
