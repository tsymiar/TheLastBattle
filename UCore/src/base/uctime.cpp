#include "uctime.h"
#include "ucstring.h"
#include <cassert>
#include <sstream>
#include <iomanip>


#if !(defined(WIN32) || defined(WIN64))
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#endif

namespace UCODE
{
    CSDDateTime  SDNow()
    {
        return time(NULL);
    }

    CSDDateTime::CSDDateTime(time_t t)
    {
        m_time = *localtime(&t);
    }

    time_t CSDDateTime::GetTimeValue()
    {
        return mktime(&m_time);
    }

    INT64 CSDDateTime::operator - (const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return (INT64)difftime(mktime(&m_time), mktime(&t_tm));
    }

    INT64 CSDDateTime::DiffSecond(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return (INT64)difftime(mktime(&m_time), mktime(&t_tm));
    }

    INT64 CSDDateTime::DiffMinute(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return ((INT64)difftime(mktime(&m_time), mktime(&t_tm)))/60;
    }

    INT64 CSDDateTime::DiffHour(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return ((INT64)difftime(mktime(&m_time), mktime(&t_tm)))/(60*60);
    }

    INT64 CSDDateTime::DiffDay(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return ((INT64)difftime(mktime(&m_time), mktime(&t_tm)))/(60*60*24);
    }

    INT64 CSDDateTime::DiffWeek(const CSDDateTime & datetime)
    {
        tm t_tm = datetime.m_time;
        return ((INT64)difftime(mktime(&m_time), mktime(&t_tm)))/(60*60*24*7);
    }

    INT32 CSDDateTime::DiffMonth(const CSDDateTime & datetime)
    {
        return (m_time.tm_year - datetime.m_time.tm_year) * 12 + (m_time.tm_mon - datetime.m_time.tm_mon);
    }

    INT32 CSDDateTime::DiffYear(const CSDDateTime & datetime)
    {
        return m_time.tm_year - datetime.m_time.tm_year;
    }

    CSDDateTime & CSDDateTime::IncYear(UINT32 year)
    {
        m_time.tm_year += year;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecYear(UINT32 year)
    {
        m_time.tm_year -= year;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncMonth(UINT32 month)
    {
        m_time.tm_mon += month;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecMonth(UINT32 month)
    {
        m_time.tm_mon -= month;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncDay(UINT32 day)
    {
        m_time.tm_mday += day;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecDay(UINT32 day)
    {
        m_time.tm_mday -= day;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncHour(UINT32 hour)
    {
        m_time.tm_hour += hour;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecHour(UINT32 hour)
    {
        m_time.tm_hour -= hour;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncMinute(UINT32 minute)
    {
        m_time.tm_min += minute;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecMinute(UINT32 minute)
    {
        m_time.tm_min -= minute;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncSecond(UINT32 second)
    {
        m_time.tm_sec += second;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecSecond(UINT32 second)
    {
        m_time.tm_sec -= second;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::IncWeek(UINT32 week)
    {
        m_time.tm_mday += 7 * week;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    CSDDateTime & CSDDateTime::DecWeek(UINT32 week)
    {
        m_time.tm_mday -= 7 * week;
		m_time.tm_isdst = -1;
        mktime(&m_time);
        return *this;
    }

    void CSDDateTime::SetTimeValue(time_t t)
    {
        m_time = *localtime(&t);
    }

    BOOL CSDDateTime::SetDateTime(UINT32 year, UINT32 month, UINT32 day, UINT32 hours, UINT32 minutes, UINT32 seconds)
    {
        if(CheckDate(year, month, day) && CheckTime(hours, minutes, seconds))
        {
            m_time.tm_year = year - 1900;
            m_time.tm_mon = month - 1;
            m_time.tm_mday = day;

            m_time.tm_hour = hours;
            m_time.tm_min = minutes;
            m_time.tm_sec = seconds;
			m_time.tm_isdst = -1;
            mktime(&m_time);
            return TRUE;
        }
        return FALSE;
    }

    BOOL CSDDateTime::SetDate(UINT32 year, UINT32 month, UINT32 day)
    {
        if(CheckDate(year, month, day))
        {
            m_time.tm_year = year - 1900;
            m_time.tm_mon = month - 1;
            m_time.tm_mday = day;
			m_time.tm_isdst = -1;
            mktime(&m_time);
            return TRUE;
        }
        return FALSE;
    }

    BOOL CSDDateTime::SetTime(UINT32 hours, UINT32 minutes, UINT32 seconds)
    {
        if(CheckTime(hours, minutes, seconds))
        {
            m_time.tm_hour = hours;
            m_time.tm_min = minutes;
            m_time.tm_sec = seconds;
			m_time.tm_isdst = -1;
            mktime(&m_time);
            return TRUE;
        }
        return FALSE;
    }

    UINT32 CSDDateTime::GetYear() const
    {
        return m_time.tm_year + 1900;
    }

    UINT32 CSDDateTime::GetMonth() const
    {
        return m_time.tm_mon + 1;
    }

    UINT32 CSDDateTime::GetDay() const
    {
        return m_time.tm_mday;
    }

    UINT32 CSDDateTime::GetHour()  const 
    {
        return m_time.tm_hour;
    }

    UINT32 CSDDateTime::GetMinute() const
    {
        return m_time.tm_min;
    }

    UINT32 CSDDateTime::GetSecond() const
    {
        return m_time.tm_sec;
    }

	UINT32 CSDDateTime::GetWeek() const
	{
		return m_time.tm_wday;
	}

    BOOL CSDDateTime::CheckDate(UINT32 year, UINT32 month, UINT32 day)
    {
        if(year < 1970 || year >= 2037)
        {
            return FALSE;
        }
        if(month < 1 || month > 12)
        {
            return FALSE;
        }
        if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
        {
            if(day < 1 || day > 31)
            {
                return FALSE;
            }
        }
        else if(month == 4 || month == 6 || month == 9 || month == 11)
        {
            if(day < 1 || day > 30)
            {
                return FALSE;
            }
        }
        else if(month == 2)
        {
            if(SDIsLeapYear(year))
            {
                if(day < 1 || day > 29)
                {
                    return FALSE;
                }
            }
            else
            {
                if(day < 1 || day > 28)
                {
                    return FALSE;
                }
            }
        }
        return TRUE;
    }

    BOOL CSDDateTime::CheckTime(UINT32 hours, UINT32 minutes, UINT32 seconds)
    {
        if(hours < 0 || hours >= 24)
        {
            return FALSE;
        }
        if(minutes < 0 || minutes >= 60)
        {
            return FALSE;
        }
        if(seconds < 0 || seconds >= 60)
        {
            return FALSE;
        }
        return TRUE;
    }

     tstring CSDDateTime::ToString(const TCHAR * pszFormat)
     {
         TCHAR temp[20];
         if(_SDTStrcmp(_SDT("YYYY-mm-dd hh:mm:ss"),pszFormat)==0)
         {
             _SDTsnprintf(temp, 20, _SDT("%4d-%02d-%02d %02d:%02d:%02d"), GetYear(),GetMonth(),GetDay(),GetHour(),GetMinute(),GetSecond());
         }
         else if(_SDTStrcmp(_SDT("YYYY-mm-dd"),pszFormat)==0)
         {
             _SDTsnprintf(temp, 20,_SDT("%4d-%02d-%02d"), GetYear(),GetMonth(),GetDay());
         }
         else if(_SDTStrcmp(_SDT("hh:mm:ss"),pszFormat)==0)
         {
             _SDTsnprintf(temp, 20,_SDT("%02d:%02d:%02d"),GetHour(),GetMinute(),GetSecond());
         }
         return temp;
     }


     tstring CSDDateTime::ToString()
     {
         TCHAR temp[20];
         _SDTsnprintf(temp, 20,_SDT("%4d-%02d-%02d %02d:%02d:%02d"), GetYear(),GetMonth(),GetDay(),GetHour(),GetMinute(),GetSecond());
         return temp;
     }

/////////////////////////////////////////////////////////////////////////////////////////////////////

    BOOL SDIsLeapYear(UINT32 year)
    {
        if ( year%400==0 || year%100!=0 && year%4==0 )
        {
            return TRUE;
        }
        return FALSE;
    }

#define EPOCH_1970_1601 116444736000000000ULL

    static UINT64  SDGetSystemTime()
    {
        assert( sizeof(UINT64) == 64/8 );
        UINT64 time=0;
#if (defined(WIN32) || defined(WIN64))
        FILETIME ft;
        GetSystemTimeAsFileTime( &ft);
        ULARGE_INTEGER li;
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        time = (li.QuadPart - EPOCH_1970_1601) /10;
#else
        struct timeval now;
        gettimeofday( &now , NULL );
        //assert( now );
        time = now.tv_sec;
        time = time*1000000;
        time += now.tv_usec;
#endif
        return time;
    }

    void  SDSleep(UINT32 milliseconds)
    {
#if (defined(WIN32) || defined(WIN64))
        Sleep((DWORD)milliseconds);
#else
        usleep(milliseconds * 1000);
#endif
    }

    UINT64  SDTimeMicroSec()
    {
        return SDGetSystemTime();
    }

    UINT64  SDTimeMilliSec()
    {
        return SDGetSystemTime()/1000LL;
    }

    UINT64  SDTimeSecs()
    {
        return SDGetSystemTime()/1000000LL;
    }

#define SD_CLOCK_PER_SEC 1000;
    /**
    * @brief
    * 计算机系统启动到目前为止所经历的时间（此函数有效时间为系统启动后49.7天内）
    * @return 计算机系统启动到目前为止所经历的时间，如果该值不可获得，返回－1
    */
    UINT32  SDGetTickCount()
    {
#if (defined(WIN32) || defined(WIN64))
        return GetTickCount();
#else
    struct timeval cateq_v;
    struct timezone cateq_z;

    gettimeofday(&cateq_v, &cateq_z);

    return (UINT32)(1000.0 * (double)(cateq_v.tv_sec) + (double)(cateq_v.tv_usec) / 1000.0);

#endif //
    }

    tstring SDTimeToString(const TCHAR * pszFormat,CSDDateTime t)
    {
        TCHAR temp[20];
        if(_SDTStrcmp(_SDT("YYYY-mm-dd hh:mm:ss"),pszFormat)==0)
        {
            _SDTsnprintf(temp, 20, _SDT("%4d-%02d-%02d %02d:%02d:%02d"), t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond());
        }
        else if(_SDTStrcmp(_SDT("YYYY-mm-dd"),pszFormat)==0)
        {
            _SDTsnprintf(temp, 20, _SDT("%4d-%02d-%02d"), t.GetYear(),t.GetMonth(),t.GetDay());
        }
        else if(_SDTStrcmp(_SDT("hh:mm:ss"),pszFormat)==0)
        {
            _SDTsnprintf(temp, 20, _SDT("%02d:%02d:%02d"),t.GetHour(),t.GetMinute(),t.GetSecond());
        }
        return temp;
    }

    tstring SDTimeToString(CSDDateTime t)
    {
        TCHAR temp[20];
        _SDTsnprintf(temp, 20,_SDT("%4d-%02d-%02d %02d:%02d:%02d"), t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond());
        return temp;
    }

    BOOL SDTimeFromString(tstring strDateTime,CSDDateTime& t)
    {
        if( NULL!=_SDTStrchr(strDateTime.c_str(),_SDT('-')) && NULL!=_SDTStrchr(strDateTime.c_str(),_SDT(':')) )
        {
            std::vector<tstring> oSpliteStrVec = _SDTSplitStrings(strDateTime,_SDT(' '));
            if(oSpliteStrVec.size()>1)
            {
                std::vector<tstring> oSpliteStrVec1 = _SDTSplitStrings(oSpliteStrVec[0],_SDT('-'));
                std::vector<tstring> oSpliteStrVec2 = _SDTSplitStrings(oSpliteStrVec[1],_SDT(':'));
                if(oSpliteStrVec1.size()>2 && oSpliteStrVec2.size()>2)
                {
                    t.SetDateTime(_SDTAtou(oSpliteStrVec1[0].c_str()),_SDTAtou(oSpliteStrVec1[1].c_str()),_SDTAtou(oSpliteStrVec1[2].c_str()),_SDTAtou(oSpliteStrVec2[0].c_str()),_SDTAtou(oSpliteStrVec2[1].c_str()),_SDTAtou(oSpliteStrVec2[2].c_str()));
                    return TRUE;
                }
            }
        }
        else if( NULL!=_SDTStrchr(strDateTime.c_str(),_SDT('-')) && NULL==_SDTStrchr(strDateTime.c_str(),_SDT(':')) )
        {
            std::vector<tstring> oSpliteStrVec = _SDTSplitStrings(strDateTime,_SDT('-'));
            if(oSpliteStrVec.size()>2)
            {
                t.SetDate(_SDTAtou(oSpliteStrVec[0].c_str()),_SDTAtou(oSpliteStrVec[1].c_str()),_SDTAtou(oSpliteStrVec[2].c_str()));
                return TRUE;
            }
        }
        else if( NULL==_SDTStrchr(strDateTime.c_str(),_SDT('-')) && NULL!=_SDTStrchr(strDateTime.c_str(),_SDT(':')) )
        {
            std::vector<tstring> oSpliteStrVec = _SDTSplitStrings(strDateTime,_SDT(':'));
            if(oSpliteStrVec.size()>2)
            {
                t.SetTime(_SDTAtou(oSpliteStrVec[0].c_str()),_SDTAtou(oSpliteStrVec[1].c_str()),_SDTAtou(oSpliteStrVec[2].c_str()));
                return TRUE;
            }
        }
        return FALSE;
    }

}

