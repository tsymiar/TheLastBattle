// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。
#include "CSUserMgr.h"
#include "CSUser.h"

#include "..\..\CommonCPlus\DBMgr.h"
#include "CSGameLogMgr.h"
#include "../ELogging/ELogging/SSActive.h"
#include "CSKernel.h"
#include "CSBattleMgr.h"
#include "CSDataSource.h"

using namespace std;
//using namespace MysqlDWrapper;
namespace CentralServer{

 
INT32 CCSUserMgr::SynUserAskDBCallBack(){
	while (!m_DBCallbackQueue.empty()){
		Buffer* pBuffer = NULL;
		m_DBCallbackQueue.try_pop(pBuffer);

		switch (pBuffer->m_LogLevel)
		{
		case DBToCS::eQueryUser_DBCallBack:
			SynHandleQueryUserCallback(pBuffer);
			break;
		case DBToCS::eQueryAllAccount_CallBack:
			SynHandleAllAccountCallback(pBuffer);
			break;
		case DBToCS::eMail_CallBack:
			SynHandleMailCallback(pBuffer);
			break;
		case  DBToCS::eQueryNotice_CallBack:
			DBCallBack_QueryNotice(pBuffer);
			break;
		default:
			ELOG(LOG_WARNNING, "not hv handler:%d", pBuffer->m_LogLevel);
			break;
		}

		if (pBuffer){
			m_DBCallbackQueuePool.ReleaseObejct(pBuffer);
		}
	}

	return 0;
}


void	CCSUserMgr::DBAsyn_QueryWhenThreadBegin(){    
	/*MysqlDWrapper::*/IDBConnector *piGameDBConnector = m_UserCacheDBActiveWrapper->GetDBConnector(); 
	if (NULL == piGameDBConnector){
		ELOG(LOG_ERROR,  " can not connect db!",  );
		return;
	}
 
	INT32	n32QueryRet = piGameDBConnector->ExecQuery("select MAX(mail_id) as mailid   from game_mail ;");
	if (n32QueryRet != eNormal)
	{
		ELOG(LOG_ERROR,  "--errorCode:%d,errorStr:%s", piGameDBConnector->GetErrorCode(),piGameDBConnector->GetErrorStr());
		return;
	}
	INT32 tValue = 0;
	piGameDBConnector->GetQueryFieldData("mailid", tValue);
	//服务器启动的时候 没有登录的玩家，可以这样设置
	m_MailMgr.setCurtMaxMailIdx(tValue); 
	piGameDBConnector->CloseQuery(); 

	if (tValue > 0)
	{
		DBAsyn_QueryGameMailList(piGameDBConnector,0); 
	}

	n32QueryRet = piGameDBConnector->ExecQuery("select * from notice;");
	if (n32QueryRet != eNormal)
	{
		ELOG(LOG_WARNNING,  "--errorCode:%d,errorStr:%s", piGameDBConnector->GetErrorCode(),piGameDBConnector->GetErrorStr());
		return;
	}
	tValue = piGameDBConnector->GetQueryFieldNum();

	piGameDBConnector->CloseQuery(); 
	if (tValue > 0){
		DBAsynQueryNoticeCallBack(piGameDBConnector); 
	}
}

void	CCSUserMgr::CDKThreadBeginCallback()
{
	/*MysqlDWrapper::*/IDBConnector *piGameDBConnector = m_CdkeyWrapper->GetDBConnector(); 
	if (NULL == piGameDBConnector){
		ELOG(LOG_WARNNING,  " can not connect db!",  );
		return;
	}

	stringstream sqlStr;
	sqlStr << "select id,sdk_id,cdkey,user_name from account_user where cs_id=" << GetCSKernelInstance()->GetCSID() << ";";
	INT32 n32QueryRet = piGameDBConnector->ExecQuery(sqlStr.str());
	if (n32QueryRet != eNormal)
	{
		ELOG(LOG_WARNNING,  "--errorCode:%d,errorStr:%s", piGameDBConnector->GetErrorCode(),piGameDBConnector->GetErrorStr());
		return;
	}

	int row = piGameDBConnector->GetQueryRowNum();
	DBToCS::QueryAllAccount sQueryAllAccount;
	for (INT32 i = 0; i < row; ++i){
		auto pAccount = sQueryAllAccount.add_account();

		INT64 id = 0;
		piGameDBConnector->GetQueryFieldData("id", id);
		pAccount->set_guid(id);

		string username;
		piGameDBConnector->GetQueryFieldData("cdkey", username);
		pAccount->set_user_name(username);

		string name;
		piGameDBConnector->GetQueryFieldData("user_name", name);
		pAccount->set_nickname(name);

		INT32 sdkid = 0;
		piGameDBConnector->GetQueryFieldData("sdk_id", sdkid);
		pAccount->set_sdkid(sdkid);

		piGameDBConnector->QueryNext();
	}
	if (row > 0){
		GetCSUserMgrInstance()->EncodeAndSendToLogicThread(sQueryAllAccount, sQueryAllAccount.mgsid());
	}
}

INT32 CCSUserMgr::SynHandleQueryUserCallback(Buffer* pBuffer){
	boost::shared_ptr<DBToCS::QueryUser> pMsg = ParseProtoMsgInThread<DBToCS::QueryUser>(pBuffer->GetDataHeader(), pBuffer->GetDataLength());
	if (!pMsg){
		ELOG(LOG_ERROR, "");
		return 0;
	}
	boost::shared_ptr<GCToCS::Login> pLogin = ParseProtoMsgInThread<GCToCS::Login>(pMsg->login().c_str(), pMsg->login().size());
	if (!pLogin){
		ELOG(LOG_ERROR, "");
		return 0;
	}

	SUserNetInfo netinfo(pMsg->gsid(), pMsg->gcnetid());
	if (GetUserByNetInfo(netinfo)){
		ELOG(LOG_WARNNING, "");
		return eEC_InvalidNetState;
	}

	SUserDBData sUserDBData;
	memcpy(&sUserDBData, pMsg->db().c_str(), pMsg->db().size());
	CCSUser *pcUser = GetUserByGuid(sUserDBData.sPODUsrDBData.un64ObjIdx);
	if (NULL != pcUser){
		pcUser->OnOnline(netinfo, *pLogin, false, false);
		return eNormal;
	}

	pcUser = new CCSUser;

	sUserDBData.szUserName = pLogin->name();
	sUserDBData.szUserPwd = pLogin->passwd();
	sUserDBData.szNickName = pMsg->nickname();
	sUserDBData.sPODUsrDBData.eUserPlatform = (EUserPlatform)pLogin->sdk();
	sUserDBData.szTaskData = pMsg->taskdata();
	  
	const bool bNewUser = sUserDBData.sPODUsrDBData.tRegisteUTCMillisec   < 1;
	
	pcUser->LoadDBData(sUserDBData);
	pcUser->GetUserDBData().mGuideSteps.szCSContinueGuide = pMsg->guidestr();

	for(INT32 i = 0; i < pMsg->rsinfo_size(); i++){
		pcUser->LoadUserSNSList(pMsg->rsinfo(i));
	}

	for(INT32 i = 0; i < pMsg->item_info_size(); i++){
		pcUser->AddUserItems(*pMsg->mutable_item_info(i));
	}

	for (INT32 i = 0; i < pMsg->mail_info_size(); ++i){
		m_MailMgr.updatePerMailList(pMsg->mail_info(i).mailid(),   sUserDBData.sPODUsrDBData.un64ObjIdx  ,(EMailCurtState)pMsg->mail_info(i).state() ); 
	} 

	pcUser->GetUserBattleInfoEx().mDebugName = pLogin->name();

	if (!bNewUser){		 
		TIME_MILSEC curTime = time(NULL);
		for (INT32 i = 0; i < pMsg->herocfg_size(); ++i){
			const DBToCS::HeroCfg& pHeroCfg = pMsg->herocfg(i);
			if (pHeroCfg.expiredtime() != PersistTimeAlways && pHeroCfg.expiredtime() < curTime){
				continue;
			}

			SUserHeroDBData sSUserHeroDBData;
			sSUserHeroDBData.buyTime = pHeroCfg.buytime();
			sSUserHeroDBData.endTime = pHeroCfg.expiredtime();
			sSUserHeroDBData.un32HeroID = pHeroCfg.commodityid();
			pcUser->AddHero(sSUserHeroDBData);
		}

		for (INT32 i = 0; i < pMsg->runeinfo_size(); ++i){
			const DBToCS::RuneInfo& sRuneInfo = pMsg->runeinfo(i);
			pcUser->InitRunes(sRuneInfo.bagstr(), sRuneInfo.slotstr());
		}
	}
	
	INT32 res = AddUser(pcUser);
	if (res != eNormal){
		delete pcUser;
		return res;
	}

	pcUser->OnOnline(netinfo, *pLogin, bNewUser, true);
	if (bNewUser)
	{
		stringstream mystream;
		mystream<<pLogin->name()<<LOG_SIGN<<pLogin->sdk()<<LOG_SIGN;
		mystream<<pLogin->platform()<<LOG_SIGN<<pLogin->equimentid()<<LOG_SIGN;
		mystream<<pLogin->ipaddress(); 
		CSSGameLogMgr::GetInstance().AddGameLog(eLog_Register,pcUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,mystream.str());
	}
	return eNormal;
}

INT32	CCSUserMgr::DBPoster_UpdateUser(CCSUser *pcUser)
{  
	SUserDBData &psUserDBData = pcUser->GetUserDBData();  
	pcUser->GetTaskMgr()->PackTaskData(psUserDBData.szTaskData,psUserDBData.isTaskRush);//存数据库时增加任务数据
	CCSUserDbDataMgr::UpdateUserDbData(psUserDBData, m_SaveUserStream);
	if (!m_SaveUserStream.str().empty()){
		CSToDB::UpdateUser sUpdateUser;
		sUpdateUser.set_guid(psUserDBData.sPODUsrDBData.un64ObjIdx);
		sUpdateUser.set_sqlstr(m_SaveUserStream.str());
		m_UserCacheDBActiveWrapper->EncodeAndSendToDBThread(sUpdateUser, sUpdateUser.mgsid());
	} 
	
	return eNormal;
} 

void CCSUserMgr::PostSaveCmd()
{
	ELOG(LOG_INFO,"start post save data to db....");
	for (auto it=m_cUserGUIDMap.begin();it!=m_cUserGUIDMap.end();++it){
		DBPoster_UpdateUser(it->second);
	}
	ELOG(LOG_ERROR,"only finish post save data to db, don't close me at once.");
}

}