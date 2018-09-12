#pragma once
#include <boost/date_time/posix_time/posix_time.hpp> 
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/atomic/atomic.hpp"
#include <queue>
#include <string>

class Active;
class Buffer;

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

enum GLogColor {
	COLOR_DEFAULT,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
};

class Logging
{
public:
	Logging();
	~Logging();
	void	WriteMsg(LoggingEnum eLoggingEnum, char* fun, int line, char* msg);
	void	WriteBuffer(Buffer*& pBuffer);

private:
	int		CreateLogFile(LoggingEnum aLoggingEnum);
	void	Write(LoggingEnum eLoggingEnum, char* msg, int msgLen);
	void	ConvertTimeString(std::string& Str);
private:
	boost::atomic<FILE*>	m_File[LOG_END];
	Active*					m_pActive;
	typedef boost::posix_time::ptime PTIME;
	boost::atomic<bool>		m_IfCanFlush[LOG_END];
	boost::atomic<PTIME>	m_PrelushTime[LOG_END];
	Buffer*					m_BufferQueue[LOG_END];
	std::string				m_AppPreStr;
	int					m_n32IfConsoleDump;
	int					m_n32IfFileDump;
	int					m_n32BufferDumpToFileNum;
	int					m_n32BufferDumpToFileMaxTime;
	int					m_n32MaxAllPending;
	int					m_IfDumpInfo;
	int					m_IfDumpDebug;
	int					m_IfDumpSpecialDebug;
	int					m_IfDumpError;
	int					m_IfMBoxAssert;
	int					m_IfDumpWarning;
	int					m_IfReuseCurDayLogging;
	char				m_AppPath[256];
	int					m_BufferMaxLength;
	int					m_n32MaxBufferSize;
};

extern Logging g_Logging;
