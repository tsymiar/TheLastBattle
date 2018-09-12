//#pragma once
//
//#include <string>
//#include <map>
//#include <vector>
//using namespace std;
//
////#define USE_SCREEN_LOG
//
//#ifdef _UNICODE
//typedef wstring tstring;
//#else
//typedef string tstring;
//#endif
//
//enum LOG_LEVEL
//{
//	LOG_None	= 1<<0,
//	LOG_Info	= 1<<1,
//	LOG_Warn	= 1<<2,
//	LOG_Error	= 1<<3,
//	LOG_Debug	= 1<<4,
//	LOG_DebugByLuoxin	= 1<<5,//自定义日志
//};
//
//class CLogger
//{
//public:
//	CLogger(int level);
//	~CLogger();
//
//public:
//	void Log(LOG_LEVEL loglevel, const TCHAR *file, const TCHAR *format, ...);
//	void Error(const TCHAR *file, const TCHAR *format, ...);
//	void Info(const TCHAR *file, const TCHAR *format, ...);
//	void Warn(const TCHAR *file, const TCHAR *format, ...);
//	void Debug(const TCHAR *file, const TCHAR *format, ...);
//	void DebugByLuoxin(const TCHAR *file, const TCHAR *format, ...);
//
//private:
//	void _Log(LOG_LEVEL loglevel, const TCHAR *file, const TCHAR *text);
//
//private:
//	bool mPrintf;
//	int mLogLevel;
//	tstring mLogDir;
//	map<int,tstring> mLogTitle;
//};
//
//extern CLogger gLogger;
//
//#ifdef USE_SCREEN_LOG
//#undef ELOG
//#define ELOG(x, msg, ...) \
//	do \
//{\
//	USES_CONVERSION;\
//	gLogger.DebugByLuoxin(_T("msg"), A2W(msg), ## __VA_ARGS__); \
//} while (0);
//#endif
