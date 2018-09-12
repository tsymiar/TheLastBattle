// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <sstream>
#include "CSUserMgr.h"
#include "CSUser.h"

#include "..\..\CommonCPlus\DBMgr.h"
#include "boost/algorithm/string.hpp"
#include "boost/spirit.hpp"
#include "CSKernel.h"

namespace CentralServer{

void CCSUserMgr::UserCacheDBAsynHandler(Buffer*& pBuffer){
	auto pDB = GetDBSource(pBuffer->m_ActorID);
	if (NULL == pDB){
		ELOG(LOG_ERROR, "db is null");
		return;
	}
	switch (pBuffer->m_LogLevel){
	case CSToDB::eUpdateUser_DBCallBack:
		DBAsynUpdateUserCallback(pBuffer, pDB);
		break;
	case CSToDB::eAlterSNSList_DBCall:
		DBAsynAlterSNSList(pBuffer,pDB);
		break;
	case CSToDB::eAlterItem_DBCall:
		DBAsyAlterItemCallBack(pBuffer, pDB);
		break;
	case CSToDB::eInsertNotice_DBCall:
		DBAsynInsertNoticeCall(pBuffer, pDB);
		break;
	}
}

void CCSUserMgr::DBAsynUpdateUserCallback(Buffer*& pBuffer, /*MysqlDWrapper::*/IDBConnector* piGameDBConnector){ 
	if (NULL == piGameDBConnector){
		ELOG(LOG_WARNNING, "  NULL == piGameDBConnector." );
		return;
	}
	
	boost::shared_ptr<CSToDB::UpdateUser> pMsg = ParseProtoMsgWithNewMemory<CSToDB::UpdateUser>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());
	if (!pMsg.get()){ 
		return;
	} 

	stringstream mystream; 
	INT32 n32Update = piGameDBConnector->ExecQuery( "begin;set autocommit=0;");
	if (n32Update != eNormal)
	{
		ELOG(LOG_WARNNING,  "error:begin;%llu, crrorcode:%d, msg:%s!",pMsg->guid(), piGameDBConnector->GetErrorCode(), piGameDBConnector->GetErrorStr());
		return ;
	} 
	 
	n32Update = piGameDBConnector->ExecQuery( pMsg->sqlstr());
	if (n32Update != eNormal)
	{
		ELOG(LOG_WARNNING,  "error:begin;%llu, crrorcode:%d, msg:%s!",pMsg->guid(), piGameDBConnector->GetErrorCode(), piGameDBConnector->GetErrorStr());
		return ;
	}  

	  n32Update = piGameDBConnector->ExecQuery("commit;");
	if (n32Update != eNormal)
	{
		ELOG(LOG_WARNNING,  "error:begin;%llu, crrorcode:%d, msg:%s!",pMsg->guid(), piGameDBConnector->GetErrorCode(), piGameDBConnector->GetErrorStr());
		return ;
	}   

	ELOG(LOG_DBBUG, "Update User %llu   to db", pMsg->guid() );
}

}