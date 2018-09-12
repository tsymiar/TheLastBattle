#include "Logging.h"
#include <iomanip>
#include <sstream>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <Windows.h>
#include "SSActive.h"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include "tinyxml.h" 
#include "tinystr.h"
#include <sys/types.h>
#include <sys/stat.h>

Logging g_Logging;

#ifdef _WINDOWS
// Returns the character attribute for the given color.
WORD GetColorAttribute(GLogColor color) {
	switch (color) {
	case COLOR_RED:    return FOREGROUND_RED;
	case COLOR_GREEN:  return FOREGROUND_GREEN;
	case COLOR_YELLOW: return FOREGROUND_RED | FOREGROUND_GREEN;
	case COLOR_BLUE: return FOREGROUND_BLUE | FOREGROUND_RED;
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

GLogColor GLogColorVec[LOG_END] = {COLOR_GREEN, COLOR_DEFAULT, COLOR_RED, COLOR_YELLOW, COLOR_BLUE, COLOR_RED};

Logging::Logging():m_n32IfConsoleDump(0)
	, m_n32IfFileDump(0)
	, m_n32BufferDumpToFileNum(0)
	, m_n32BufferDumpToFileMaxTime(0)
	, m_IfDumpInfo(0)
	, m_IfDumpDebug(0)
	, m_IfDumpSpecialDebug(0)
	, m_IfDumpError(0)
	, m_IfMBoxAssert(0)
	, m_IfDumpWarning(0)
	, m_IfReuseCurDayLogging(0)
	, m_BufferMaxLength(512)
	, m_n32MaxBufferSize(250){
	::GetModuleFileNameA(0, m_AppPath, 256);  
	setlocale(LC_CTYPE, "");
	m_AppPreStr = m_AppPath;
	m_AppPreStr = m_AppPreStr.substr(0, m_AppPreStr.rfind("\\"));
	const char* filepath = "eloggingSetting.xml";
	m_AppPreStr += "//eloggingSetting.xml";
	TiXmlDocument doc(m_AppPreStr.c_str());
	for (int i = 0; i < LOG_END; ++i){
		m_BufferQueue[i] = new Buffer(1024*10);
	}

	bool loadOkay = doc.LoadFile();
	if (!loadOkay){
		printf("load eloggingSetting.xml failed for %s\n", doc.ErrorDesc());
	}
	else{
		TiXmlElement* m_pXMLEle = doc.RootElement();
		TiXmlElement* ConsoleDumpEle = m_pXMLEle->FirstChildElement("ConsoleDump");
		TiXmlElement* RobotNumEle = m_pXMLEle->FirstChildElement("FileDump");
		TiXmlElement* BufferDumpToFileMaxTimeEle = m_pXMLEle->FirstChildElement("BufferDumpToFileMaxTime");
		TiXmlElement* BufferDumpToFileNumEle = m_pXMLEle->FirstChildElement("BufferDumpToFileNum");

		m_n32IfConsoleDump = atoi(ConsoleDumpEle->GetText());

		m_IfReuseCurDayLogging = atoi(m_pXMLEle->FirstChildElement("IfReuseCurDayLogging")->GetText());

		m_n32IfFileDump = atoi(RobotNumEle->GetText());
		if (m_n32IfFileDump != 0){
			m_n32BufferDumpToFileNum = atoi(BufferDumpToFileNumEle->GetText());
			m_n32BufferDumpToFileMaxTime = atoi(BufferDumpToFileMaxTimeEle->GetText());
		}

		m_IfDumpInfo = atoi(m_pXMLEle->FirstChildElement("IfDumpInfo")->GetText());
		if (!m_IfDumpInfo){
			delete m_BufferQueue[LOG_INFO];
			m_BufferQueue[LOG_INFO] = nullptr;
		}

		m_IfDumpDebug = atoi(m_pXMLEle->FirstChildElement("IfDumpDebug")->GetText());
		if (!m_IfDumpDebug){
			delete m_BufferQueue[LOG_DBBUG];
			m_BufferQueue[LOG_DBBUG] = nullptr;
		}

		m_IfDumpSpecialDebug = atoi(m_pXMLEle->FirstChildElement("IfDumpSpecialDebug")->GetText());
		if (!m_IfDumpSpecialDebug){
			delete m_BufferQueue[LOG_SpecialDebug];
			m_BufferQueue[LOG_SpecialDebug] = nullptr;
		}

		m_IfDumpError = atoi(m_pXMLEle->FirstChildElement("IfDumpError")->GetText());
		if (!m_IfDumpError){
			delete m_BufferQueue[LOG_ERROR];
			m_BufferQueue[LOG_ERROR] = nullptr;
		}

		m_IfDumpWarning = atoi(m_pXMLEle->FirstChildElement("IfDumpWarning")->GetText());
		if (!m_IfDumpWarning){
			delete m_BufferQueue[LOG_WARNNING];
			m_BufferQueue[LOG_WARNNING] = nullptr;
		}

		m_IfMBoxAssert = atoi(m_pXMLEle->FirstChildElement("IfMsgBoxAssert")->GetText());
	}

	if (m_n32IfFileDump != 0){
		m_AppPreStr = m_AppPath;
		m_AppPreStr = m_AppPreStr.substr(m_AppPreStr.rfind("\\") + 1);
		m_AppPreStr = m_AppPreStr.substr(0, m_AppPreStr.size()-4);

		PTIME nowTime = boost::posix_time::microsec_clock::local_time();
		namespace bf = boost::filesystem;

		string curWorkStr = m_AppPath;
		curWorkStr = curWorkStr.substr(0, curWorkStr.rfind("\\")) + "\\Log";
		if (!bf::exists(curWorkStr)){
			boost::filesystem::create_directory(curWorkStr);
		}

		int n32Res = 0;
		for (int i = LOG_INFO; i < LOG_END; ++i){
			m_File[i] = NULL;
			m_IfCanFlush[i] = false;
			m_PrelushTime[i] = nowTime;
			n32Res = CreateLogFile((LoggingEnum)i);
			if (n32Res != 0){
				printf("Createfile(i) failed", i);
			}
		}
	}
#undef min
	m_n32BufferDumpToFileNum = std::min(m_n32BufferDumpToFileNum, m_n32MaxBufferSize);
	m_n32MaxAllPending = m_n32BufferDumpToFileNum;
	if (m_n32MaxAllPending == 0){
		m_n32MaxAllPending = 1024;
	}
	if (m_n32IfFileDump != 0 || m_n32IfConsoleDump != 0){
		m_pActive = Active::CreateActive(std::bind(&Logging::WriteBuffer, this, std::placeholders::_1), 16, m_BufferMaxLength);
		m_pActive->Start();
	}
	setlocale(LC_CTYPE, "");
}

void Logging::Write(LoggingEnum eLoggingEnum, char* msg, int msgLen){
	Buffer* pBuffer = m_pActive->GetBuffer();
	if (!pBuffer){
		return;
	}
	pBuffer->m_LogLevel = eLoggingEnum;
	pBuffer->append(msg, msgLen);

	m_pActive->Send(pBuffer);
}

void Logging::WriteBuffer(Buffer*& pBuffer){
	PTIME nowTime = boost::posix_time::microsec_clock::local_time();
	string DateStr = boost::posix_time::to_iso_string(nowTime);
	ConvertTimeString(DateStr);
	memcpy(pBuffer->GetDataHeader(), DateStr.c_str(), 19);
	boost::posix_time::time_duration timeFidd = nowTime - m_PrelushTime[pBuffer->m_LogLevel];
	if (timeFidd.total_microseconds() > 1000){
		m_IfCanFlush[pBuffer->m_LogLevel] = TRUE;
	}

	if (m_n32IfConsoleDump != 0){
		const GLogColor color = GLogColorVec[pBuffer->m_LogLevel];
#ifdef _WINDOWS
		const HANDLE stderr_handle = GetStdHandle(STD_ERROR_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO buffer_info;
		GetConsoleScreenBufferInfo(stderr_handle, &buffer_info);
		const WORD old_color_attrs = buffer_info.wAttributes;
		fflush(stderr);
		SetConsoleTextAttribute(stderr_handle,
			GetColorAttribute(color) | FOREGROUND_INTENSITY);

		fwrite(pBuffer->GetDataHeader(), 1, pBuffer->GetDataLength(), stderr);
		fflush(stderr);
		SetConsoleTextAttribute(stderr_handle, old_color_attrs);
#else
		fprintf(stderr, "\033[0;3%sm", GetAnsiColorCode(color));
		fwrite(message, len, 1, stderr);
		fprintf(stderr, "\033[m");  // Resets the terminal to default.
#endif
	}

	if (m_n32IfFileDump != 0){
		FILE* pNowFile = m_File[pBuffer->m_LogLevel];
		if (pNowFile == NULL){
			return;
		}

		Buffer* pDumpBuffer = m_BufferQueue[pBuffer->m_LogLevel];
		bool ifDumpSucces = pDumpBuffer->GetDataLength() >= pDumpBuffer->GetMaxLength();
		if (ifDumpSucces && FALSE == m_IfCanFlush[pBuffer->m_LogLevel] && pDumpBuffer->GetDataLength() < m_n32BufferDumpToFileNum){
			pDumpBuffer->append(pBuffer->GetDataHeader(), pBuffer->GetDataLength());
		}
		else{
			if (pDumpBuffer->GetDataLength() > 0){
				fwrite(pDumpBuffer->GetDataHeader(), 1, pDumpBuffer->GetDataLength(), pNowFile);
				pDumpBuffer->Clear();
			}

			fwrite(pBuffer->GetDataHeader(), 1, pBuffer->GetDataLength(), pNowFile);
			fflush(pNowFile);
			m_IfCanFlush[pBuffer->m_LogLevel] = FALSE;
			m_PrelushTime[pBuffer->m_LogLevel] = boost::posix_time::microsec_clock::local_time();
		} 
	}

	m_pActive->ReleaseBuffer(pBuffer);
}

void Logging::WriteMsg(LoggingEnum eLoggingEnum, char* fun, int line, char* msg){
	if (m_n32IfConsoleDump == 0 && m_n32IfFileDump == 0){
		return;
	}

	if (eLoggingEnum < LOG_INFO || eLoggingEnum >= LOG_END){
		return;
	}

	if (eLoggingEnum == LOG_INFO){
		if (m_IfDumpInfo == 0){
			return;
		}
	}
	else if (eLoggingEnum == LOG_DBBUG){
		if (m_IfDumpDebug == 0){
			return;
		}
	}
	else if (eLoggingEnum == LOG_SpecialDebug){
		if (m_IfDumpSpecialDebug == 0){
			return;
		}
	}
	else if (eLoggingEnum == LOG_ERROR){
		if (m_IfDumpError == 0){
			return;
		}
	}
	else if (eLoggingEnum == LOG_ASSERT){
		if (m_IfDumpError == 0){
			return;
		}
	}
	else if (eLoggingEnum == LOG_WARNNING){
		if (m_IfDumpWarning == 0){
			return;
		}
	}

	Buffer* pBuffer = m_pActive->GetBuffer();
	if (!pBuffer){
		return;
	}
	pBuffer->m_LogLevel = eLoggingEnum;
	char tmp[1024] = {0};
	_snprintf(tmp, sizeof(tmp), "%d %s:%d %s\n", GetCurrentThreadId(), fun, line, msg);
	pBuffer->append("                                          ", 20);
	pBuffer->append(tmp);
	m_pActive->Send(pBuffer);

	if (eLoggingEnum == LOG_ASSERT){
		if (m_IfMBoxAssert){
			MessageBoxA(NULL,tmp,"Error",MB_OK);
		}
	}
}

Logging::~Logging(){
	for (INT32 i = 0; i < LOG_END; ++i){
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
		strPre = "DBBUG";
		break;
	case LOG_INFO:
		strPre = "INFO";
		break;
	case LOG_WARNNING:
		strPre = "WARNNING";
		break;
	case LOG_ERROR:
		strPre = "ERROR";
		break;
	case LOG_SpecialDebug:
		strPre = "SpecialDebug";
		break;
	case LOG_ASSERT:
		strPre = "ASSERT";
		break;
	}
	char str[128];

	int fd = -1;
	string curWorkStr = m_AppPath;
	curWorkStr = curWorkStr.substr(0, curWorkStr.rfind("\\"));

	if (0 == m_IfReuseCurDayLogging){
		_snprintf(str, sizeof(str), "%s/Log/%s-%s-%d-%s.log", curWorkStr.c_str(), m_AppPreStr.c_str(), strPre.c_str() ,boost::this_thread::get_id(), boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time()).c_str());
		fd = open(str, O_WRONLY | O_CREAT | O_EXCL, 0664);
		if (fd == -1){
			printf("%s create(%d) file error\n", __FUNCTION__, aLoggingEnum);
			return -1;
		}
	}
	else{
		std::string strTime = boost::gregorian::to_iso_string(boost::gregorian::day_clock::local_day());  
		_snprintf(str, sizeof(str), "%s/Log/%s-%s-%s.log", curWorkStr.c_str(), m_AppPreStr.c_str(), strPre.c_str() , strTime.c_str());
		fd = open(str, O_WRONLY | O_CREAT , _S_IREAD | _S_IWRITE | _S_IEXEC);
		if (fd == -1){
			DWORD errorMessageID = ::GetLastError();

			LPSTR messageBuffer = nullptr;
			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

			std::string message(messageBuffer, size);

			//Free the buffer.
			LocalFree(messageBuffer);

			printf("%s create(%d) file error %s\n", __FUNCTION__, aLoggingEnum, message.c_str());
			return -1;
		}
	}

	m_File[aLoggingEnum] = fdopen(fd, "a");
	if (NULL == m_File[aLoggingEnum]){
		printf("%s open file(%d) failed\n", __FUNCTION__, aLoggingEnum);
		return -1;
	}

	return 0;
}

void Logging::ConvertTimeString(std::string& strTime){
	strTime.erase(0, 4);
	int pos = strTime.find('T');
	strTime.replace(pos,1,std::string("-"));  
	strTime.replace(pos + 3,0,std::string(":"));  
	strTime.replace(pos + 6,0,std::string(":")); 
	strTime.replace(pos-2,0,std::string("-"));
}
