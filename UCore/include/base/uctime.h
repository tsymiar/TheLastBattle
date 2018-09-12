#ifndef UCTIME_H
#define UCTIME_H

#include "uctype.h"

#include <ctime>
#include <string>

#ifndef tstring
#if (defined(WIN32) || defined(WIN64))

#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#else
#ifdef UNICODE
typedef std::basic_string<TCHAR> tstring;
#else
typedef std::string tstring;
#endif
#endif
#endif

namespace UCODE
{
    /**
    * @defgroup grouptime 时间操作
    * @ingroup  UCODE
    * @{
    */
    class CSDDateTime;

    /**
    * @brief 获取代表当前日期时间(调用此函数的此时此刻)的CSDDateTime
    * @return 代表当前日期时间(调用此函数的此时此刻)的CSDDateTime
    */

    CSDDateTime SDNow();

    /**
    * @brief 日期时间类,代表1970年午夜到2037年之间的某一个时间
    *
    */
    class  CSDDateTime
    {
    public:
        /**
        * @brief 构造一个CSDDateTime类,构造出来的时间为1970-1-1午夜
		* @param t : 设置的日期时间
        */
        CSDDateTime(time_t t = 0);


        /**
        * @brief 获取本类所代表的日期时间的time_t值
        * @return 返回的time_t值
        */
        time_t GetTimeValue();

        /**
        * @brief 设置本类所代表的日期时间
        * @param t 设置的日期时间
        * @return VOID
        */
        VOID SetTimeValue(time_t t);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的秒数
        * @param datetime 一个日期时间
        * @return 差距的秒数
        */
        INT64 operator - (const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的秒数
        * @param datetime 一个日期时间
        * @return 差距的秒数
        */
        INT64 DiffSecond(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的分钟数,差距少于1分钟的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的分钟数
        */
        INT64 DiffMinute(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的小时数,差距少于1小时的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的小时数
        */
        INT64 DiffHour(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的天数,差距少于1天的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的天数
        */
        INT64 DiffDay(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的星期数,差距少于1星期的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的星期数
        */
        INT64 DiffWeek(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的月份,差距少于1个月的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的星期数
        */
        INT32 DiffMonth(const CSDDateTime &datetime);

        /**
        * @brief 获取本类所代表的日期时间与datetime所代表的日期时间差距的年数,差距少于1年的不计算在内
        * @param datetime 一个日期时间
        * @return 差距的年数
        */
        INT32 DiffYear(const CSDDateTime &datetime);

        /**
        * @brief 增加本类所代表的日期时间的年数
        * @param year 增加的年数
        * @return 新的日期时间
        */
        CSDDateTime & IncYear(UINT32 year = 1);

        /**
        * @brief 减少本类所代表的日期时间的年数
        * @param year 减少的年数
        * @return 新的日期时间
        */
        CSDDateTime & DecYear(UINT32 year = 1);

        /**
        * @brief 增加本类所代表的日期时间的月数
        * @param month 增加的月数
        * @return 新的日期时间
        */
        CSDDateTime & IncMonth(UINT32 month = 1);

        /**
        * @brief 减少本类所代表的日期时间的月数
        * @param month 减少的月数
        * @return 新的日期时间
        */
        CSDDateTime & DecMonth(UINT32 month = 1);

        /**
        * @brief 增加本类所代表的日期时间的天数
        * @param day 增加的天数
        * @return 新的日期时间
        */
        CSDDateTime & IncDay(UINT32 day = 1);

        /**
        * @brief 减少本类所代表的日期时间的天数
        * @param day 减少的天数
        * @return 新的日期时间
        */
        CSDDateTime & DecDay(UINT32 day = 1);

        /**
        * @brief 增加本类所代表的日期时间的小时数
        * @param hour 增加的小时数
        * @return 新的日期时间
        */
        CSDDateTime & IncHour(UINT32 hour = 1);

        /**
        * @brief 减少本类所代表的日期时间的小时数
        * @param hour 减少的小时数
        * @return 新的日期时间
        */
        CSDDateTime & DecHour(UINT32 hour = 1);

        /**
        * @brief 增加本类所代表的日期时间的分钟数
        * @param minute 增加的分钟数
        * @return 新的日期时间
        */
        CSDDateTime & IncMinute(UINT32 minute = 1);

        /**
        * @brief 减少本类所代表的日期时间的分钟数
        * @param minute 减少的分钟数
        * @return 新的日期时间
        */
        CSDDateTime & DecMinute(UINT32 minute = 1);

        /**
        * @brief 增加本类所代表的日期时间的秒数
        * @param second 增加的秒数
        * @return 新的日期时间
        */
        CSDDateTime & IncSecond(UINT32 second = 1);

        /**
        * @brief 减少本类所代表的日期时间的秒数
        * @param second 减少的秒数
        * @return 新的日期时间
        */
        CSDDateTime & DecSecond(UINT32 second = 1);

        /**
        * @brief 增加本类所代表的日期时间的星期数
        * @param week 增加的星期数
        * @return 新的日期时间
        */
        CSDDateTime & IncWeek(UINT32 week = 1);

        /**
        * @brief 减少本类所代表的日期时间的星期数
        * @param week 减少的星期数
        * @return 新的日期时间
        */
        CSDDateTime & DecWeek(UINT32 week = 1);



        /**
        * @brief 设置本类所代表的日期时间
        * @param year 设置的年份[1970-2037]
        * @param month 设定的此年的哪个月[1-12]
        * @param day 设定的此月的第几天[1-31]
        * @param hours 设定的此天的第几小时[0-23]
        * @param minutes 设定的此小时的第几分钟[0-59]
        * @param seconds 设定的此分钟的第几秒[0-59]
        * @return 是否设置成功,false为没有设置成功
        */
        BOOL SetDateTime(UINT32 year, UINT32 month, UINT32 day, UINT32 hours, UINT32 minutes, UINT32 seconds);

        /**
        * @brief 设置本类所代表的日期,时间不变
        * @param year 设置的年份[1970-2037]
        * @param month 设定的此年的哪个月[1-12]
        * @param day 设定的此月的第几天[1-31]
        * @return 是否设置成功,false为没有设置成功
        */
        BOOL SetDate(UINT32 year, UINT32 month, UINT32 day);

        /**
        * @brief 设置本类所代表的时间,日期不变
        * @param hours 设定的此天的第几小时[0-23]
        * @param minutes 设定的此小时的第几分钟[0-59]
        * @param seconds 设定的此分钟的第几秒[0-59]
        * @return 是否设置成功,false为没有设置成功
        */
        BOOL SetTime(UINT32 hours, UINT32 minutes, UINT32 seconds);

        /**
        * @brief 获取本类所代表的年份
        * @return 获取的年份
        */
        UINT32 GetYear() const;

        /**
        * @brief 获取本类所代表的日期时间所在的年份的第几个月
        * @return 获取的月份
        */
        UINT32 GetMonth() const;

        /**
        * @brief 获取本类所代表的日期时间所在的月份的第几天
        * @return 获取的天数
        */
        UINT32 GetDay() const;

        /**
        * @brief 获取本类所代表的日期时间所在的天的第几个小时
        * @return 获取的小时数
        */
        UINT32 GetHour() const;

        /**
        * @brief 获取本类所代表的日期时间所在的小时的分钟数
        * @return 获取的分钟数
        */
        UINT32 GetMinute() const;

        /**
        * @brief 设置本类所代表的日期时间所在的分钟的秒数
        * @return 获取的秒数
        */
        UINT32 GetSecond() const;

		/**
        * @brief 设置本类所代表的日期时间所在的星期数
        * @return 获取的星期数
		*		0为星期天Sunday
		*		1为星期一Monday
		*		... ...
		*		6为星期六Saturday
        */
		UINT32 GetWeek() const;

        /**
        * @brief 将SDDateTime按某种格式进行格式化：YYYY-mm-dd hh:mm:ss
        * @param pszFormat : 时间格式
        * @return 格式化的字符串
        */
        tstring ToString(const TCHAR * pszFormat);

		/**
        * @brief 将SDDateTime按某种格式进行格式化(无参表示默认第一种格式)YYYY-mm-dd hh:mm:ss
        * @return 格式化的字符串
        */
        tstring ToString();

    private:
        /**
        * @brief 不允许操作+
        */
        CSDDateTime & operator + (const CSDDateTime & datetime);

        /**
        * @brief 检查日期是否出处范围
        * @param year : 年份
        * @param month: 月份
        * @param day  : 日
        * @return TRUE:合法日期 FALSE:不合法日期
        */
        BOOL CheckDate(UINT32 year, UINT32 month, UINT32 day);

        /**
        * @brief 检查时间是否出处范围
        * @param hours   : 小时
        * @param minutes : 分钟
        * @param seconds : 秒
        * @return TRUE:合法时间 FALSE:不合法时间
        */
        BOOL CheckTime(UINT32 hours, UINT32 minutes, UINT32 seconds);

        tm m_time;
    };



    /**
    * @brief 检测此年是否为闰年
    * @param year 检测的年
    * @return 检测的结构,TRUE为是闰年,FALSE不为闰年
    */
    BOOL SDIsLeapYear(UINT32 year);

    /**
    * @brief
    * 计算机系统启动到目前为止所经历的时间（此函数有效时间为系统启动后49.7天内）
    * @return 计算机系统启动到目前为止所经历的时间，如果该值不可获得，返回－1
    */
    UINT32  SDGetTickCount();

    /**
    * @brief
    * 挂起当前线程的执行直到milliseconds毫秒后
    * @param milliseconds : 指示睡眠的时间
    * @return VOID
    */
    VOID  SDSleep(UINT32 milliseconds);

    /**
    * @brief
    * 获取当前的微妙时间
    * @return 当前的微妙时间
    */
    UINT64  SDTimeMicroSec();

    /**
    * @brief
    * 获取当前的毫秒时间
    * @return 当前的毫秒时间
    */
    UINT64  SDTimeMilliSec();

    /**
    * @brief
    * 获取当前的秒时间
    * @return 当前的秒时间
    */
    UINT64  SDTimeSecs();

    /**
    * @brief 将SDDateTime按某种格式进行格式化(无参表示默认第一种格式)
    * @param pszFormat : 时间格式
    * @param t         : 日期时间对象
    * @return 格式化的字符串
    */
    tstring SDTimeToString(const TCHAR* pszFormat,CSDDateTime t);

	/**
    * @brief 将SDDateTime按某种格式进行格式化(无格式说明表示默认第一种格式)
    * @param t         : 日期时间对象
    * @return 格式化的字符串
    */
    tstring SDTimeToString(CSDDateTime t);

    /**
    * @brief 将日期时间字符串转换为CSDDateTime
    * @param strDateTime : 日期时间字符串
    * @param t           : 转换后的DateTime对象
    * @return TRUE:转换成功 FALSE:转换失败
    */
    BOOL    SDTimeFromString(tstring strDateTime,CSDDateTime& t);

}//

#endif

