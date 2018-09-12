
//#if WIN32
#include "stdafx.h"
//#endif
#include "CSGameLogMgr.h"
#include <stdarg.h>
#include "CSKernel.h"  
#include "CommLog.h" 

using namespace CentralServer;

	CSSGameLogMgr::CSSGameLogMgr(){ }
	CSSGameLogMgr::~CSSGameLogMgr(){ }   

	void CSSGameLogMgr::AddGameLog(EGameLog elog, std::string & log)
	{  
		ToLog::WriteLog sWriteLog;
		sWriteLog.set_log_type(elog );
		sWriteLog.set_log_str(log); 
		CentralServer::GetCSKernelInstance()->PostMsgToLogServer(sWriteLog, sWriteLog.msgid()); 
	}   

	void	CSSGameLogMgr::AddGameLog(EGameLog eLog, std::string& cdkey ,GCToCS::CurtUIEvent&  pMsg ) 
	{ 
		stringstream mystream;
		GetGameLogHead(  eLog,  mystream); 

		mystream<< cdkey<<LOG_SIGN;
		mystream<< pMsg.eventlist_size() << LOG_SIGN;
		for (int i=0; i < pMsg.eventlist_size(); i++ )
		{
			mystream<< pMsg.eventlist(i).uiidx() << LOG_SIGN;
			mystream<< pMsg.eventlist(i).eventnum() << LOG_SIGN;
		}
		mystream<<"');";
		AddGameLog(eLog,mystream.str()); 
	} 

	void	CSSGameLogMgr::AddGameLog(EGameLog eLog,const std::string& cdkey , INT32 param,INT32 param2,INT32 param3 )
	{
		stringstream mystream;
		GetGameLogHead(  eLog,  mystream); 

		mystream<< cdkey<<LOG_SIGN;
		if (param > 0)
		{
			mystream<<param<<LOG_SIGN; 
		} 
		if (param2 > 0)
		{
			mystream<<param2<<LOG_SIGN;
		}
		if (param3 > 0)
		{
			mystream<<param3<<LOG_SIGN;
		}
		mystream<<"');";
		AddGameLog(eLog,mystream.str()); 
	}

	 
	void	CSSGameLogMgr::AddGameLog(EGameLog eLog, const SUserDBData &db )
	{
		stringstream mystream;
		GetGameLogHead(  eLog,  mystream); 

		mystream<<db.szUserName<<LOG_SIGN<<(INT32)db.sPODUsrDBData.un8UserLv<<LOG_SIGN<<db.sPODUsrDBData.un16VipLv;
		mystream<<"');";
		AddGameLog(eLog,mystream.str()); 
	}

	void	CSSGameLogMgr::AddGameLog(EGameLog eLog, const std::string& cdkey ,GCToCS::ComposeRunes& sMsg,INT32  idx)
	{
		stringstream mystream;
		GetGameLogHead(  eLog,  mystream); 

		mystream<<cdkey<<LOG_SIGN;
		mystream<< sMsg.runesid_size()<<LOG_SIGN;
		for (int i=0; i< sMsg.runesid_size(); i++)
		{
			mystream<<sMsg.runesid(i)<<LOG_SIGN;
		}
		mystream<<idx<<LOG_SIGN;
		mystream<<"');";
		AddGameLog(eLog,mystream.str()); 
	}

	void	CSSGameLogMgr::AddGameLog(EGameLog eLog, INT64 param, const std::string& logStr)
	{
		stringstream mystream;
		GetGameLogHead(  eLog,  mystream); 

		mystream<<param<<LOG_SIGN;
		mystream<<logStr;
		mystream<<"');";
		AddGameLog(eLog,mystream.str()); 
	}

	void	CSSGameLogMgr::AddGameLog(EGameLog eLog, INT64 param,INT64 param2 ,INT64 param3)
	{
		stringstream mystream;
		GetGameLogHead(  eLog,  mystream); 

		mystream<<param<<LOG_SIGN;
		mystream<<param2;
		if (param3 > 0)
		{
			mystream<<LOG_SIGN<<param3;
		}
		mystream <<"');";
		AddGameLog(eLog,mystream.str()); 
	}
 
 