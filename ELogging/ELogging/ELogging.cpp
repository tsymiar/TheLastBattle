// ELogging.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ELogging.h"
#include "Logging.h"

ELOGGING_API void ELog(int eLoggingEnum, char* fun, int line, char* msg, ...){
	char tmp[1024] = {0};
	va_list pArg = NULL;
	va_start(pArg, msg);
	vsnprintf(tmp, sizeof(tmp), msg, pArg);
	va_end(pArg);
	g_Logging.WriteMsg((LoggingEnum)eLoggingEnum, fun, line, tmp);
}

ELOGGING_API void ELogDirect(int eLoggingEnum, char* fun, int line, char* msg){
	g_Logging.WriteMsg((LoggingEnum)eLoggingEnum, fun, line, msg);
}