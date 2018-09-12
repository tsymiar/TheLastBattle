#include "Logging.h"
#include <iomanip>
#include <sstream>
#include <fcntl.h>
#include <io.h>
#include "SSActive.h"

Logging g_Logging;
string LOGPreStr[LOG_END] = {"LOG_INFO", "LOG_DBBUG", "LOG_ERROR", "LOG_WARNNING"};

#ifdef _WINDOWS
// Returns the character attribute for the given color.
WORD GetColorAttribute(GLogColor color) {
	switch (color) {
	case COLOR_RED:    return FOREGROUND_RED;
	case COLOR_GREEN:  return FOREGROUND_GREEN;
	case COLOR_YELLOW: return FOREGROUND_RED | FOREGROUND_GREEN;
	default:           return 0;
	}
}
#else
// Returns the ANSI color code for the given color.
const char* GetAnsiColorCode(GLogColor color) {
	switch (color) {
	case COLOR_RED:     return "1";
	case COLOR_GREEN:   return "2";
	case COLOR_YELLOW:  return "3";
	case COLOR_DEFAULT:  return "";
	};
	return NULL; // stop warning about return type.
}

#endif  // OS_WINDOWS

GLogColor GLogColorVec[LOG_END] = {COLOR_GREEN, COLOR_DEFAULT, COLOR_RED, COLOR_YELLOW};

Logging::Logging(){
	PTIME nowTime = boost::posix_time::microsec_clock::local_time();

	INT32 n32Res = eNormal;
	for (INT32 i = LOG_INFO; i < LOG_END; ++i){
		m_File[i] = NULL;
		m_IfCanFlush[i] = FALSE;
		m_PrelushTime[i] = nowTime;
		n32Res = CreateLogFile((LoggingEnum)i);
		if (n32Res != eNormal){
			printf("Createfile(i) failed", i);
		}
	}
	
	m_pActive = Active::CreateActive(std::bind(&Logging::WriteBuffer, this, std::placeholders::_1));
}

void Logging::Write(LoggingEnum eLoggingEnum, char* msg, int msgLen){
	Buffer* pBuffer = m_pActive->GetBuffer();
	pBuffer->m_LogLevel = eLoggingEnum;
	char* curData = pBuffer->m_pMsg;
	pBuffer->m_Len = msgLen;
	memcpy(curData, msg, msgLen);

	m_pActive->Send(pBuffer);
}

void Logging::WriteBuffer(Buffer*& pBuffer){
	char* pContent = pBuffer->m_pMsg;
	pContent[pBuffer->m_Len] = '\0';

	const GLogColor color = GLogColorVec[pBuffer->m_LogLevel];
#ifdef _WINDOWS
	const HANDLE stderr_handle = GetStdHandle(STD_ERROR_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	GetConsoleScreenBufferInfo(stderr_handle, &buffer_info);
	const WORD old_color_attrs = buffer_info.wAttributes;
	fflush(stderr);
	SetConsoleTextAttribute(stderr_handle,
		GetColorAttribute(color) | FOREGROUND_INTENSITY);
	fwrite(pContent, pBuffer->m_Len+1, 1, stderr);
	fflush(stderr);
	SetConsoleTextAttribute(stderr_handle, old_color_attrs);
#else
	fprintf(stderr, "\033[0;3%sm", GetAnsiColorCode(color));
	fwrite(message, len, 1, stderr);
	fprintf(stderr, "\033[m");  // Resets the terminal to default.
#endif

	FILE* pNowFile = m_File[pBuffer->m_LogLevel];
	if (pNowFile == NULL){
		return;
	}

	std::queue<Buffer*>& bufferQueue = m_BufferQueue[pBuffer->m_LogLevel];

	if (FALSE == m_IfCanFlush[pBuffer->m_LogLevel]){
		bufferQueue.push(pBuffer);
		pBuffer = NULL;
	}
	else{
		while (FALSE == bufferQueue.empty()){
			Buffer* pPreBuffer = bufferQueue.front();
			bufferQueue.pop();

			fwrite(pPreBuffer->m_pMsg, 1, pPreBuffer->m_Len, pNowFile);
			fflush(pNowFile);

			m_pActive->ReleaseBuffer(pPreBuffer);
		}

		fwrite(pContent, 1, pBuffer->m_Len, pNowFile);
		fflush(pNowFile);

		m_IfCanFlush[pBuffer->m_LogLevel] = FALSE;
		m_PrelushTime[pBuffer->m_LogLevel] = boost::posix_time::microsec_clock::local_time();
	} 
}

void Logging::WriteWithFunLine(LoggingEnum eLoggingEnum, char* fun, int line, char* msg, ...){
	if (eLoggingEnum < LOG_INFO || eLoggingEnum > LOG_WARNNING){
		eLoggingEnum = eLoggingEnum;
	}

	char tmp[1024] = {0};
	PTIME nowTime = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration timeFidd = nowTime - m_PrelushTime[eLoggingEnum];
	if (timeFidd.total_microseconds() > 1000){
		m_IfCanFlush[eLoggingEnum] = TRUE;
	}

	sprintf(tmp, "%s.%d", boost::posix_time::to_iso_string(nowTime).c_str(), boost::this_thread::get_id());
	int curPos = strlen(tmp);

	va_list pArg = NULL;
	va_start(pArg, msg);
	vsprintf(tmp+curPos, msg, pArg);
	va_end(pArg);

	curPos = strlen(tmp);
	char end[] = "\n";
	sprintf(tmp + curPos, "%s", end);

	Write(eLoggingEnum, tmp, strlen(tmp));
}

Logging::~Logging(){
	for (INT32 i = 0; i < 4; ++i){
		if (NULL != m_File[i]){
			fclose(m_File[i]);
		}
	}

	delete m_pActive;
}


int Logging::CreateLogFile(LoggingEnum aLoggingEnum){
	string strPre;
	switch (aLoggingEnum){
	case LOG_DBBUG:
		strPre = "LOG_DBBUG";
		break;
	case LOG_INFO:
		strPre = "LOG_INFO";
		break;
	case LOG_WARNNING:
		strPre = "LOG_WARNNING";
		break;
	case LOG_ERROR:
		strPre = "LOG_ERROR";
		break;
	}
	char str[128];
	sprintf(str, "./Log/%s-%d-%s", strPre.c_str() ,boost::this_thread::get_id(), boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time()).c_str());
	string fileName(str);
	fileName += ".log";
	int fd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0664);
	if (fd == -1){
		printf("%s create(%d) file error\n", __FUNCTION__, aLoggingEnum);
		return -1;
	}

	m_File[aLoggingEnum] = fdopen(fd, "a");
	if (NULL == m_File[aLoggingEnum]){
		printf("%s open file(%d) failed\n", __FUNCTION__, aLoggingEnum);
		return -1;
	}

	return 0;
}
