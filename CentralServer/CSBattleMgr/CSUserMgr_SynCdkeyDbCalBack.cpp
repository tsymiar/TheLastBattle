#include "stdafx.h" 
#include "CSUserMgr.h"
#include "CSUser.h"  
#include "../ELogging/ELogging/SSActive.h" 
#include "CSKernel.h"


//using namespace MysqlDWrapper;
namespace CentralServer
{  	 
	void	CCSUserMgr::DBAsynQueryUserCallBack(Buffer*& pBuffer, /*MysqlDWrapper::*/IDBConnector* pDB)
	{
		boost::shared_ptr<CSToDB::QueryUserReq> pMsg = ParseProtoMsgWithNewMemory<CSToDB::QueryUserReq>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());
		if (!pMsg.get()){ 
			return;
		} 

		boost::shared_ptr<GCToCS::Login> pLogin = ParseProtoMsgWithNewMemory<GCToCS::Login>(pMsg->logininfo().c_str(), pMsg->logininfo().size());
		if (!pLogin.get()){ 
			return;
		}
		
		SUserDBData sUserDBData; 
		sUserDBData.sPODUsrDBData.un64ObjIdx = pMsg->objid();
		sUserDBData.sPODUsrDBData.eUserPlatform = (EUserPlatform)pLogin->sdk();

		DBToCS::QueryUser sQueryUser; 
		DBAsyn_QueryUser(sUserDBData, sQueryUser, pDB);

		sQueryUser.set_db((char*)&sUserDBData.sPODUsrDBData, sizeof(sUserDBData.sPODUsrDBData));
		sQueryUser.set_login(pMsg->logininfo());
		sQueryUser.set_gcnetid(pMsg->gcnetid());
		sQueryUser.set_gsid(pMsg->gsid());
		sQueryUser.set_nickname(sUserDBData.szNickName);
		GetCSUserMgrInstance()->EncodeAndSendToLogicThread(sQueryUser, sQueryUser.msgid()); 
	}

	void	CCSUserMgr::DBAsynAlterSNSList(Buffer*& pBuffer, /*MysqlDWrapper::*/IDBConnector* pDB)
	{
		boost::shared_ptr<CSToDB::AlterSNSList> pMsg = ParseProtoMsgWithNewMemory<CSToDB::AlterSNSList>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());
		if (!pMsg.get()){
			ELOG(LOG_ERROR, "");
			return;
		}

		AlterUserSNSList(pDB, pMsg->user_id(), pMsg->related_id(), (ERelationShip)pMsg->related(), (EDBOperation)pMsg->opration());
	}

	INT32	CCSUserMgr::DBAsynInsertNoticeCall(Buffer*& pBuffer, /*MysqlDWrapper::*/IDBConnector* pDB)
	{
		if (NULL == pDB){
			return eEC_NullPointer;
		}

		boost::shared_ptr<CSToDB::InsertNotice> sMsp = ParseProtoMsgWithNewMemory<CSToDB::InsertNotice>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());;
		if (!sMsp){
			return eEC_NullMsg;
		}

		INT32 n32QueryRet = pDB->ExecQuery(sMsp->sql_str());
		if (n32QueryRet != eNormal)
		{
			ELOG(LOG_WARNNING,  "--errorCode:%d,errorStr:%s.", pDB->GetErrorCode(),pDB->GetErrorStr());
			return n32QueryRet;
		} 
		return eNormal;
	}

	INT32	CCSUserMgr::DBAsynQueryNoticeCallBack(/*MysqlDWrapper::*/IDBConnector* pDB)
	{
		if (NULL == pDB){
			return eEC_NullPointer;
		}

		stringstream mystream;
		mystream<<"select * from notice;";

		INT32 	n32QueryRet = pDB->ExecQuery(mystream.str());
		if (n32QueryRet != eNormal)
		{
			ELOG(LOG_WARNNING,  "--errorCode:%d,errorStr:%s.", pDB->GetErrorCode(),pDB->GetErrorStr());
			return n32QueryRet;
		} 

		int row = pDB->GetQueryRowNum();

		UINT32 temp_un32 = 0;
		UINT64 temp_un64 = 0;
		string temp_str = "";

		DBToCS::QueryNotice notice;
		for (INT32 i = 0; i < row; i ++){
			DBToCS::QueryNotice_Notice *info = notice.add_notice_info();
			pDB->GetQueryFieldData("id", temp_un32);
			info->set_id(temp_un32);

			pDB->GetQueryFieldData("platform_id", temp_un32);
			info->set_platform((EUserPlatform)temp_un32);

			pDB->GetQueryFieldData("title", temp_str);
			info->set_title(temp_str);

			pDB->GetQueryFieldData("eflag", temp_un32);
			info->set_eflag((ENoticeFlag)temp_un32);

			pDB->GetQueryFieldData("estate", temp_un32);
			info->set_estate((ENoticeState)temp_un32);

			pDB->GetQueryFieldData("priority", temp_un32);
			info->set_priority(temp_un32);

			pDB->GetQueryFieldData("notice", temp_str);
			info->set_notice(temp_str);

			pDB->GetQueryFieldData("star_time", temp_un64);
			info->set_star_time(temp_un64);

			pDB->GetQueryFieldData("end_time", temp_un64);
			info->set_end_time (temp_un64);

		
			pDB->QueryNext();
		}
		GetCSUserMgrInstance()->EncodeAndSendToLogicThread(notice, notice.mgsid() ); 
		return eNormal;
	}

// 	INT32	CCSUserMgr::DBAsynDelNoticeCallBack(Buffer*& pBuffer, /*MysqlDWrapper::*/IDBConnector* pDB)
// 	{
// 		if (NULL == pDB){
// 			return eEC_NullPointer;
// 		}
// 
// 		boost::shared_ptr<CSToDB::DeleteNotice> sMsp = ParseProtoMsgWithNewMemory<CSToDB::DeleteNotice>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());
// 		if (!sMsp){
// 			return eEC_NullMsg;
// 		}
// 
// 		stringstream mystream;
// 		mystream<<"delete * from notice where id=";
// 		mystream<<sMsp->id();
// 		mystream<<";";
// 
// 		INT32 	n32QueryRet = pDB->ExecQuery(mystream.str());
// 		if (n32QueryRet != eNormal)
// 		{
// 			ELOG(LOG_WARNNING,  "--errorCode:%d,errorStr:%s.", pDB->GetErrorCode(),pDB->GetErrorStr());
// 			return n32QueryRet;
// 		} 
// 
// 		return eNormal;
// 	}

	INT32	CCSUserMgr::DBCallBack_QueryNotice(Buffer*& pBuffer)
	{
		boost::shared_ptr<DBToCS::QueryNotice> pMsg = ParseProtoMsgWithNewMemory<DBToCS::QueryNotice>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());

		for (INT32 i = 0; i < pMsg->notice_info_size(); i++)
		{
			SNotice t_notice;
			t_notice.id = pMsg->notice_info(i).id();
			t_notice.platform = (EUserPlatform)pMsg->notice_info(i).platform();
			t_notice.title = pMsg->notice_info(i).title();
			t_notice.flag = (ENoticeFlag)pMsg->notice_info(i).eflag();
			t_notice.state = (ENoticeState)pMsg->notice_info(i).estate();
			t_notice.priority = pMsg->notice_info(i).priority();
			t_notice.msg = pMsg->notice_info(i).notice();
			t_notice.star_time = pMsg->notice_info(i).star_time();
			t_notice.end_time = pMsg->notice_info(i).end_time();

			AddNotice(t_notice);
		}
		return eNormal;
	}
	
}