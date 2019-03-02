#pragma once
#include <Windows.h>
#include "ELogging\ELogging.h"
#include <iostream>
#include <tchar.h>

// 注意：这个枚举值有2个//
enum LoggingEnum{
	LOG_INFO,
	LOG_DBBUG,
	LOG_ERROR,
	LOG_WARNNING,
	LOG_SpecialDebug,
	LOG_ASSERT,
	LOG_END
};
