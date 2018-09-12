// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。
#include "stdafx.h"
#include "CSUserMgr.h"
#include "CSCfgMgr.h"
#include "DBMgr.h"
#include "../ELogging/ELogging/SSActive.h"
#include "CSKernel.h"
#include "FBAll_Const.h"


using namespace std;
//using namespace MysqlDWrapper;
namespace CentralServer{  
	bool CCSUserMgr::AddNotice(const SNotice& notice)
{
	UINT64 temp_date = time(NULL);
	INT64 temp = temp_date - notice.end_time;
	if (temp > 0) //公告过期判断
	{
		return false;
	}
	gNoticeVec.push_back(notice);
	return true;
}
	
	UINT64 CCSUserMgr::GetCurDateTime()
{
	time_t curtime=time(0); 
	tm tim =*localtime(&curtime); 
	int day,mon,year,hour,min,sec; 
	day=tim.tm_mday;
	mon=tim.tm_mon;
	year=tim.tm_year;
	hour=tim.tm_hour;
	min = tim.tm_min;
	sec = tim.tm_sec;
	year += 1900;
	mon += 1;

	stringstream t_ss;
	t_ss<<year;
	//月、日、时、分、秒不足两位的补0
	if (mon < 10){
		t_ss<<0;
	}
	t_ss<<mon;
	if (day < 10){
		t_ss<<0;
	}
	t_ss<<day;
	if (hour<10){
		t_ss<<0;
	}
	t_ss<<hour;

	//转string
	string t_sec;
	t_ss>>t_sec;

	//转Uint64
	stringstream ss;
	ss<<t_sec;

	UINT64 un64_t;
	ss>>un64_t;

	return un64_t;
}

	UINT64 CCSUserMgr::GetCDKeySurplusSec(INT64 &dateTime, UINT64 expire_time)
	{
		INT64 temp_time = expire_time - GetCurDateTime();
		INT64 temp_n64 = 0;

		dateTime += temp_time * 3600;
		temp_n64 = temp_time / 100;

		if (temp_n64 != 0)
		{
			dateTime += temp_time * 3600 * 24;
		}

		return eNormal;
	}
	
}
