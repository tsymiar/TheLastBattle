// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUserMgr.h"
#include "CSUser.h"
#include "CSCfgMgr.h"
#include "FBAll_Const.h"
#include <set>
#include <string>
#include "../ELogging/ELogging/SSActive.h"
#include "CSBattleMgr.h"
#include "CSKernel.h"
#include "CSGameLogMgr.h"
 
const INT32  gThread = 2;
const INT32 GUID_Devide = 1000;
namespace CentralServer{
CCSUserMgr::CCSUserMgr():m_DBCallbackQueuePool(128, 1024*10) 
	, m_UserCacheDBActiveWrapper(NULL)
	, m_CdkeyWrapper(NULL)
	, m_MaxGuid(0)
	, m_GiftCDKID(0)
{
	m_TodayTime = boost::posix_time::microsec_clock::local_time();
	m_Today = m_TodayTime.date();
	m_monthDays = boost::gregorian::gregorian_calendar::end_of_month_day(m_Today.year(),m_Today.month());
	boost::posix_time::ptime::time_duration_type timeOfDay = m_TodayTime.time_of_day();
	if (timeOfDay.hours()>=5) m_IsDailyTaskRush = true; 
	else m_IsDailyTaskRush = false;
}

CCSUserMgr::~CCSUserMgr(){

}
 
INT32 CCSUserMgr::OnHeartBeatImmediately()
{
	OnTimeUpdate();
	SynUserAskDBCallBack();
	return eNormal;
}

void CCSUserMgr::OnTimeUpdate()
{
	boost::gregorian::date oldDay = m_Today;
	m_TodayTime = boost::posix_time::microsec_clock::local_time();
	m_Today = m_TodayTime.date();
	if (m_Today.year()!=oldDay.year()) OnNewYear();
	else if (m_Today.month()!=oldDay.month()) OnNewMonth();
	else if (m_Today.day()!=oldDay.day()) OnNewDay();
	if (!m_IsDailyTaskRush && m_TodayTime.time_of_day().hours()>=5){
		for(auto it=m_cUserOnlineMap.begin();it!=m_cUserOnlineMap.end();++it){
			it->second->GetTaskMgr()->RushDailyTasks();
		}
		m_IsDailyTaskRush = true;
	}
}
void CCSUserMgr::OnNewDay()
{
	m_IsDailyTaskRush = false;
	m_monthDays = boost::gregorian::gregorian_calendar::end_of_month_day(m_Today.year(),m_Today.month());
	for (auto it=m_cUserOnlineMap.begin();it!=m_cUserOnlineMap.end();++it)
	{
		it->second->OnNewDay();
	}
}
void CCSUserMgr::OnNewMonth()
{
	m_monthDays = boost::gregorian::gregorian_calendar::end_of_month_day(m_Today.year(),m_Today.month());
	for (auto it=m_cUserOnlineMap.begin();it!=m_cUserOnlineMap.end();++it)
	{
		it->second->OnNewMonth();
	}
}
void CCSUserMgr::OnNewYear()
{

}

INT32	CCSUserMgr::PostMsgToGC_AskReturn(const SUserNetInfo &crsUserNetInfo, INT32 n32AskProtocalID, INT32 n32RetFlag){
	GSToGC::AskRet sMsg;
	sMsg.set_askid(n32AskProtocalID);
	sMsg.set_errorcode(n32RetFlag);

	return GetCSKernelInstance()->PostMsgToGC(crsUserNetInfo, sMsg, sMsg.msgid());
}

INT32	CCSUserMgr::PostMsgToGC_AskReturn(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID, INT32 n32AskProtocalID, INT32 n32RetFlag){
	GSToGC::AskRet sMsg;
	sMsg.set_askid(n32AskProtocalID);
	sMsg.set_errorcode(n32RetFlag);

	return GetCSKernelInstance()->PostMsgToGS(cpiGSInfo, n32GCNSID, sMsg, sMsg.msgid());
}

void	CCSUserMgr::ChangeUserNickName(CCSUser *pcUser, const string& nickName){
	if (nickName.empty()){
		ELOG(LOG_ERROR, "nickname is empty!");
		return;
	}
	if (!pcUser->GetNickName().empty()){
		m_cNickNameMap.erase(pcUser->GetNickName());
		m_AllNickNameSet.erase(pcUser->GetNickName());
	}
	pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_NickName, nickName);
	auto insertRes1 = m_AllNickNameSet.insert(nickName);
	Assert(insertRes1.second, "");
	auto insertRes2 = m_cNickNameMap.insert(std::make_pair(nickName, pcUser));
	Assert(insertRes2.second, "");

	UpdateUserNickNameToDB(pcUser);

	ELOG(LOG_SpecialDebug, "user szUserName:%s, nickName:%s", pcUser->GetUserDBData().szUserName.c_str(), nickName.c_str());
}

INT32	CCSUserMgr::AddUser(CCSUser *pcUser)
{
	if (NULL == pcUser){
		return eEC_NullUser;
	}

	pcUser->ResetPingTimer();

	UINT64 un64ObjIndex = pcUser->GetGUID();

	Assert(un64ObjIndex>0);
	if (pcUser->GetUserName().empty()){
		ELOG(LOG_ERROR, "UserName is empty!");
		return eEC_InvalidUserName;
	}

	auto insertRes1 = m_cUserGUIDMap.insert(std::make_pair(un64ObjIndex, pcUser));
	Assert(insertRes1.second, "插入失败！！！");

	if (!pcUser->GetNickName().empty()){
		auto insertRes3 = m_cNickNameMap.insert(std::make_pair(pcUser->GetNickName(), pcUser));
		Assert(insertRes3.second, "插入失败！！！");
	}

	int64_t timerID = GetCSKernelInstance()->AddTimer(std::bind(&CCSUser::CheckDbSaveTimer, pcUser, std::placeholders::_1, std::placeholders::_2), CCSCfgMgr::getInstance().GetCSGlobalCfg().dbSaveTimeSpace * 1000, true);
	pcUser->SetTimerID(timerID);

	return eNormal;
}

INT32 CCSUserMgr::OnUserOnline(CCSUser *pcUser, const SUserNetInfo &crsUserNetInfo)
{
	Assert(!pcUser->GetUserNetInfo().IsValid());
	Assert(m_cUserNetMap.find(crsUserNetInfo)==m_cUserNetMap.end());
	auto res1 = m_cUserNetMap.insert(std::make_pair(crsUserNetInfo, pcUser));
	Assert(res1.second);
	auto res2 = m_cUserOnlineMap.insert(std::make_pair(pcUser->GetGUID(), pcUser));
	Assert(res2.second);
	pcUser->SetUserNetInfo(crsUserNetInfo);
	ELOG(LOG_DBBUG, "Add user netinfo(%d)", crsUserNetInfo.n32GCNSID);
	return eNormal;
}

void CCSUserMgr::OnUserOffline(CCSUser *pcUser){ 
	Assert(m_cUserNetMap.find(pcUser->GetUserNetInfo())!=m_cUserNetMap.end());
	Assert(m_cUserOnlineMap.find(pcUser->GetGUID())!=m_cUserOnlineMap.end());
	m_cUserNetMap.erase(pcUser->GetUserNetInfo());
	m_cUserOnlineMap.erase(pcUser->GetGUID());
	pcUser->ClearNetInfo();
}

CCSUser*	CCSUserMgr::GetUserByNickName(const string& stUserName)
{
	auto itr = m_cNickNameMap.find(stUserName);
	if (m_cNickNameMap.end() != itr)
	{
		return itr->second; 
	}

	return NULL;
}

CCSUser*	CCSUserMgr::GetUserByNetInfo(const SUserNetInfo &crsUNI)
{
	CCSUser *pcUser = NULL;
	auto iterUser = m_cUserNetMap.find(crsUNI);
	if (m_cUserNetMap.end() != iterUser)
	{
		pcUser = iterUser->second;
	}
	return pcUser;
}

CCSUser*	CCSUserMgr::GetUserByGuid(const INT64& cpsUserGUID)
{
	auto iterGUID = m_cUserGUIDMap.find(cpsUserGUID);
	if (m_cUserGUIDMap.end() != iterGUID)
	{
		return iterGUID->second;
	}

	return NULL;
}

using namespace std::placeholders;
#define GSCallBack(realCallBack) std::bind(realCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
#define GCCallBack(realCallBack) std::bind(realCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
#define SSCallBack(realCallBack) std::bind(realCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
#define RCCallBack(realCallBack) std::bind(realCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
void CCSUserMgr::RegisterMsgHandle(SSMsgHandlerMap& m_SSMsgHandlerMap, GSMsgHandlerMap& m_GSMsgHandlerMap, GCMsgHandlerMap& m_GCMsgHandlerMap, RCMsgHandlerMap& m_RCMsgHandlerMap){
	m_GSMsgHandlerMap[GSToCS::eMsgToCSFromGS_UserOffLine] = GSCallBack(&CCSUserMgr::OnMsgFromGS_UserOffline)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskComleteUserInfo] = GCCallBack(&CCSUserMgr::OnMgrFromGC_AskCompleteUserInfo)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskLogin] = GCCallBack(&CCSUserMgr::OnMgrFromGC_AskLogin);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskBuyGoods] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskBuyGoods)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_EuipRunes] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskEqipRunes)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_UnloadRunes] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskUnloadRunes)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_ComposeRunes] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskComposeRunes)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskUserGameInfo] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskUserGameInfo)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskReconnectGame] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskReconnectGame)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskAddToSNSList] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskAddToSNSList)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskAddToSNSListByID] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskAddToSNSListByUserID)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskRemoveFromSNSList] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskRemoveFromSNSList)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskSendMsgToUser] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskSendMsgToUser)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskInviteFriendsToBattle] = GCCallBack(&CCSUserMgr::OnMgrFromGC_AskInviteFriendsToBattle)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskCanInviteFriends] = GCCallBack(&CCSUserMgr::OnMgrFromGC_AskCanInviteFriends)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_GCReplyAddFriendRequst] = GCCallBack(&CCSUserMgr::OnMsgFromGC_GCReplyAddFriendRequst)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskQueryUserByNickName] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskQueryUaserByNickName)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_GCReplyInviteToBattle] = GCCallBack(&CCSUserMgr::OnMsgFromGC_GCReplyInviteToBattle)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_UserAskGetCLReward] = GCCallBack(&CCSUserMgr::OnMsgFromGC_UserAskGetCLReward)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskOneTaskRewards] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskOneTaskRewards)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_CompCSGuideStepId] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskCompCSGuideStep)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_Notice] = GCCallBack(&CCSUserMgr::OnMsgFromGC_Notice)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskCurtNotice] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskCurtNotice)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskChangeNickName] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskChangeNickname)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskChangeheaderId] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskChangeHeaderid)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskRecoinRune] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskRecoinRune)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_GmCmd] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskNewGMCmd)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_CDKReq] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskCDKGift)

	//mail
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskGetMailGift] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskCloseMailOrGetMailGift)
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskMailInfo] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskMailInfo) 
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskGoodscfg] = GCCallBack(&CCSUserMgr::OnMsgFromGC_AskGoodsCfg)
	// SS->CS
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_AskUpdateData] = SSCallBack(&CCSUserMgr::OnMsgFromSS_AskUpdateData)
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_AskAddGold] = SSCallBack(&CCSUserMgr::OnMsgFromSS_AskAddGold)
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_AskChangeFirstWinTime] = SSCallBack(&CCSUserMgr::OnMsgFromSS_AskChangeFirstWinTime)
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_AskUpdateGuideStep] = SSCallBack(&CCSUserMgr::OnMsgFromSS_AskUpdateGuideStep)
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_AskUserGoHall] = SSCallBack(&CCSUserMgr::OnMsgFromSS_AskUserGoHall)
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_UpdateGuideBattleStep] = SSCallBack(&CCSUserMgr::OnMsgFromSS_AskUpdateBattleTaskSetps)
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_AskSSGameLogSwitch] = SSCallBack(&CCSUserMgr::OnMsgFromSS_AskGameLogSwitch);

	//RC->CS
	m_RCMsgHandlerMap[RCToCS::eMsgRC2CS_AskServerUserInfo] = RCCallBack(&CCSUserMgr::OnMgrFromRC_AskServerUserInfo)
	m_RCMsgHandlerMap[RCToCS::eMsgRC2CS_AskNoticeInfo] = RCCallBack(&CCSUserMgr::OnMgrFromRC_AskNoticeInfo)
	m_RCMsgHandlerMap[RCToCS::eMsgRC2CS_AskAddNotice] = RCCallBack(&CCSUserMgr::OnMgrFromRC_AskAddNotice)
	m_RCMsgHandlerMap[RCToCS::eMsgRC2CS_AskDeleteNotice] = RCCallBack(&CCSUserMgr::OnMgrFromRC_AskDeleteNotice)
	m_RCMsgHandlerMap[RCToCS::eMsgRC2CS_AskCreateCDKey] = RCCallBack(&CCSUserMgr::OnMgrFromRC_AskInsertCDKey)
	//mail
	m_RCMsgHandlerMap[RCToCS::eMsgRC2CS_AskAddOneMail] = RCCallBack(&CCSUserMgr::OnMgrFromRC_AskAddMail)
	//log
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_UIEvent] = GCCallBack(&CCSUserMgr::OnMsgFromGC_ReportCLLog)
}


bool CCSUserMgr::EncodeAndSendToLogicThread(google::protobuf::Message& sMsg, int n32MsgID){
	Buffer* pBuffer = m_DBCallbackQueuePool.AcquireObject();
	bool res = DBActiveWrapper::EncodeProtoMsgToBuffer(sMsg, n32MsgID, *pBuffer);
	if (!res){
		m_DBCallbackQueuePool.ReleaseObejct(pBuffer);
		ELOG(LOG_ERROR, "");
		return false;
	}
	m_DBCallbackQueue.push(pBuffer);
	return true;
}

void CCSUserMgr::GetProfileReport( wstringstream &report )
{
	report << " AllUser=" << m_cUserGUIDMap.size() 
		<< " Online=" << m_cUserOnlineMap.size()
		<< " NetUser=" << m_cUserNetMap.size();
}

DBActiveWrapper& CCSUserMgr::GetNowWorkActor(){
	auto iter = m_pUserAskDBActiveWrapperVec.begin();
	DBActiveWrapper* pMiniDBActiveWrapper = *iter;
	int n32PendingNum1 = pMiniDBActiveWrapper->GetPendingWorkNum();
	if (n32PendingNum1 == 0){
		return *pMiniDBActiveWrapper;
	}

	for (; iter != m_pUserAskDBActiveWrapperVec.end(); ++iter){
		DBActiveWrapper* pDB = *iter;
		if (pDB->GetPendingWorkNum() < pMiniDBActiveWrapper->GetPendingWorkNum()){
			pMiniDBActiveWrapper = pDB;
		}
	}

	return *pMiniDBActiveWrapper;
}

CCSUser* CCSUserMgr::GetUserByNetInfo(const ICSGSInfo *cpiGSInfo, INT32 n32GCNSID){
	SUserNetInfo lSUserNetInfo(cpiGSInfo->GetGSID(), n32GCNSID);
	auto iterUser = m_cUserNetMap.find(lSUserNetInfo);
	if (m_cUserNetMap.end() != iterUser)
	{
		return iterUser->second;
	}
	return NULL;
}

static CCSUserMgr* pInstance = nullptr;
CCSUserMgr& CCSUserMgr::GetInstance(){
	if (!pInstance){
		pInstance = new CCSUserMgr;
	}
	return *pInstance;
}

/*MysqlDWrapper::*/IDBConnector* CCSUserMgr::GetDBSource(int actorID){
	if (m_UserCacheDBActiveWrapper->GetActorID() == actorID){
		return m_UserCacheDBActiveWrapper->GetDBConnector();
	}

	if (m_CdkeyWrapper->GetActorID() == actorID){
		return m_CdkeyWrapper->GetDBConnector();
	}

	for (auto iter = m_pUserAskDBActiveWrapperVec.begin(); iter != m_pUserAskDBActiveWrapperVec.end(); ++iter){
		DBActiveWrapper* pDB = *iter;
		const int nowActorID = pDB->GetActorID();
		if (nowActorID == actorID){
			return pDB->GetDBConnector();
		}
	}

	return NULL;
}

bool CCSUserMgr::PostUserAskMsgToDBThread(google::protobuf::Message& sMsg, int n32MsgID){
	GetNowWorkActor().EncodeAndSendToDBThread(sMsg, n32MsgID);
	return true;
}

bool CCSUserMgr::PostUserCacheMsgToDBThread(INT64 guidIndex, const string& sqlStr){
	if (guidIndex < 1 || sqlStr.empty()){
		return false;
	}
	CSToDB::UpdateUser sUpdateUser;
	sUpdateUser.set_guid(guidIndex);
	sUpdateUser.set_sqlstr(sqlStr);
	m_UserCacheDBActiveWrapper->EncodeAndSendToDBThread(sUpdateUser, sUpdateUser.mgsid());
	return true;
}

CCSUser* CCSUserMgr::GetIdlePlayer(map<string,bool>& igoreMap)
{
	static std::default_random_engine generator(static_cast<long int>(time(NULL)));
	if (m_cUserOnlineMap.size()==0) return NULL;
	std::uniform_int_distribution<int> dis(0,m_cUserOnlineMap.size()-1);
	int select = dis(generator);
	auto it=m_cUserOnlineMap.begin();
	while(select-->0) { ++it; }
	CCSUser* pUser = it->second;
	if (pUser->GetNickName().size()==0) return NULL;
	if (pUser->GetUserBattleInfoEx().GetBattleState()!=eBattleState_Free) return NULL;
	if (igoreMap.find(pUser->GetNickName())==igoreMap.end()) return pUser;
	return NULL;
}

void	CCSUserMgr::UserAskUdateItem(const SUserItemInfo& info, EDBOperation optype, UINT64 user_id)
{
	stringstream operate_ss;
	switch(optype){
	case  eOperationType_Add:{

		operate_ss<<"insert gameuser_item(user_id,item_id,item_num,buy_time,end_time) values(";
		operate_ss<<user_id;
		operate_ss<<",";
		operate_ss<<info.item_id;
		operate_ss<<",";
		operate_ss<<info.item_num;
		operate_ss<<",";
		operate_ss<<info.buy_time;
		operate_ss<<",";
		operate_ss<<info.end_time;
		operate_ss<<");";
							 }
		break;
	case eOperationType_Del:{
		operate_ss<<"delete gameuser_item where user_id=";
		operate_ss<<user_id;
		operate_ss<<" and item_id=";
		operate_ss<<info.item_id;
							}
		break;
	case eOperationType_Upd:{
		operate_ss<<"update gameuser_item set item_num=";
		operate_ss<<info.item_num;
		operate_ss<<",end_tiem=";
		operate_ss<<info.end_time;
		operate_ss<<",buy_time=";
		operate_ss<<info.buy_time;
		operate_ss<<" where user_id=";
		operate_ss<<user_id;
		operate_ss<<" and item_id=";
		operate_ss<<info.item_id;
							}
		break;
	default:
		ELOG(LOG_ERROR, "Unknow DBOperation! So Give Up!");
		return;
	}

	CSToDB::AlterItem t_iteminfo;
	t_iteminfo.set_sql_str(operate_ss.str());
	ELOG(LOG_DBBUG, "增加赠送物品:%d To User:%d.", info.item_id, user_id);
	m_UserCacheDBActiveWrapper->EncodeAndSendToDBThread(t_iteminfo, t_iteminfo.mgsid());
}

void CCSUserMgr::SynHandleAllAccountCallback(Buffer* buffer){
	DBToCS::QueryAllAccount sMsg;
	if (!ParseProtoMsg(buffer->GetCurData(), buffer->GetDataLength(), sMsg)){
		ELOG(LOG_ERROR, "");
		return;
	}

	for (int i = 0; i < sMsg.account_size(); ++i){
		auto pAccount = sMsg.account(i);
		if (!pAccount.nickname().empty()){
			auto res1 = m_AllNickNameSet.insert(pAccount.nickname());
			Assert(res1.second);
		}

		UserCombineKey sUserCombineKey;
		sUserCombineKey.sdkid = pAccount.sdkid();
		sUserCombineKey.username = pAccount.user_name();

		auto res = m_AllUserName2GUIDMap.insert(std::make_pair(sUserCombineKey, pAccount.guid()));
		Assert(res.second);
		if (m_MaxGuid < pAccount.guid()){
			m_MaxGuid = pAccount.guid();
		}
	}

	m_MaxGuid /= GUID_Devide;
	ELOG(LOG_DBBUG, "Load maxguid %llu", m_MaxGuid);
}

void CCSUserMgr::UpdateUserNickNameToDB(CCSUser* pUser){
	CSToDB::ChangeNickName sChangeNickName;
	sChangeNickName.set_nickname(pUser->GetNickName());
	sChangeNickName.set_guid(pUser->GetGUID());

	m_CdkeyWrapper->EncodeAndSendToDBThread(sChangeNickName, sChangeNickName.msgid());
}

INT32 CCSUserMgr::DBAsynChangeNickNameCallBack(Buffer*& pBuffer, /*MysqlDWrapper::*/IDBConnector* pDB){
	CSToDB::ChangeNickName sChangeNickName;
	ParseProtoMsg(pBuffer->GetDataHeader(), pBuffer->GetDataLength(), sChangeNickName);
	stringstream mystream;
	mystream<<"update account_user set user_name='" << sChangeNickName.nickname() << "' where id=" << sChangeNickName.guid() << ";";
	if (pDB->ExecQuery(mystream.str()) != eNormal){
		ELOG(LOG_ERROR, "");
	}
	pDB->CloseQuery();

	return eNormal;
}

INT32 CCSUserMgr::DBAsyn_ExeSQL(Buffer*& pBuffer, /*MysqlDWrapper::*/IDBConnector* pDB){
	CSToDB::ExeSQL_Call sMsg;
	ParseProtoMsg(pBuffer->GetDataHeader(), pBuffer->GetDataLength(), sMsg);
	int res = pDB->ExecQuery(sMsg.sql());
	if (res != eNormal){
		ELOG(LOG_ERROR, "%s", pDB->GetErrorStr());
	}
	pDB->CloseQuery();
	return 0;
}

void CCSUserMgr::InsertCDKeyEvent(Buffer*& pBuffer, IDBConnector* pDB)
{
	CSToDB::CDKeyEvents sMsg;
	ParseProtoMsg(pBuffer->GetDataHeader(), pBuffer->GetDataLength(), sMsg);

	int res = pDB->ExecQuery(sMsg.sql_str());
	if (res != eNormal){
		ELOG(LOG_ERROR, "%s", pDB->GetErrorStr());
	}

	pDB->CloseQuery();
}

void CCSUserMgr::InsertCDKey(Buffer*& pBuffer, IDBConnector* pDB)
{
	CSToDB::InsertCDKeyInfo sMsg;
	ParseProtoMsg(pBuffer->GetDataHeader(), pBuffer->GetDataLength(), sMsg);

	int res = pDB->ExecQuery(sMsg.sql_str());
	if (res != eNormal){
		ELOG(LOG_ERROR, "%s", pDB->GetErrorStr());
	}

	pDB->CloseQuery();
}

void CCSUserMgr::UpdateCDKey(Buffer*& pBuffer, IDBConnector* pDB)
{
	CSToDB::UpdateCDKeyInfo sMsg;
	ParseProtoMsg(pBuffer->GetDataHeader(), pBuffer->GetDataLength(), sMsg);

	int res = pDB->ExecQuery(sMsg.sql_str());
	if (res != eNormal){
		ELOG(LOG_ERROR, "%s", pDB->GetErrorStr());
	}

	pDB->CloseQuery();
}

UINT64 CCSUserMgr::CombineGUID(){
	++m_MaxGuid; 
	return m_MaxGuid * GUID_Devide + CCSKernel::GetInstance().GetKernelCfg().unCSId;  
}

void CCSUserMgr::Initialize(){
	SDBCfg cfgGameDb = CCSCfgMgr::getInstance().GetDBCfg(eDB_GameDb);
	SDBCfg cfgCdkeyDb=CCSCfgMgr::getInstance().GetDBCfg(eDB_CdkeyDb); 
	m_UserCacheDBActiveWrapper = new DBActiveWrapper( std::bind(&CCSUserMgr::UserCacheDBAsynHandler, this, std::placeholders::_1), cfgGameDb, std::bind(&CCSUserMgr::DBAsyn_QueryWhenThreadBegin, this) );
	m_UserCacheDBActiveWrapper->Start();

	m_CdkeyWrapper = new DBActiveWrapper( std::bind(&CCSUserMgr::UserAskDBAsynHandler, this, std::placeholders::_1), cfgCdkeyDb, std::bind(&CCSUserMgr::CDKThreadBeginCallback, this) );
	m_CdkeyWrapper->Start();

	for (int i = 0; i < gThread ; i++)
	{
		DBActiveWrapper* pThreadDBWrapper(new DBActiveWrapper(std::bind(&CCSUserMgr::UserAskDBAsynHandler, this, std::placeholders::_1), cfgGameDb));
		pThreadDBWrapper->Start();
		m_pUserAskDBActiveWrapperVec.push_back(pThreadDBWrapper);
	} 
}

void CCSUserMgr::InsertNewUserToMysql(GCToCS::Login &pLogin, CCSUser * pcUser){
	if (!pcUser){
		return;
	}
	CSToDB::ExeSQL_Call sExeSQL_Call;
	stringstream sql;
	sql << "INSERT account_user(id,cs_id,sdk_id,cdkey) values(" << pcUser->GetGUID() << ","<<CCSKernel::GetInstance().GetKernelCfg().unCSId << "," << pLogin.sdk() << ",'" << pLogin.name() << "');";
	sExeSQL_Call.set_sql(sql.str());
	m_CdkeyWrapper->EncodeAndSendToDBThread(sExeSQL_Call, sExeSQL_Call.msgid());

	{
		stringstream mystream;
		mystream<<"INSERT gameuser(obj_id,sdk_id,obj_cdkey, obj_register_time) values( ";
		mystream<<pcUser->GetGUID();
		mystream<<",";
		mystream<<pLogin.sdk();
		mystream<<",'";
		mystream<<pLogin.name();
		mystream<<"',";
		mystream<<pcUser->GetUserDBData().sPODUsrDBData.tRegisteUTCMillisec;
		mystream<<");INSERT gameuser_runne(user_id)VALUES(";
		mystream<<pcUser->GetGUID(); 
		mystream<<");";

		mystream << "INSERT gameuser_guide(obj_id) values(" << pcUser->GetGUID() << ");";
		CSToDB::UpdateUser sUpdateUser;
		sUpdateUser.set_sqlstr(mystream.str());
		sUpdateUser.set_guid(pcUser->GetGUID());

		m_UserCacheDBActiveWrapper->EncodeAndSendToDBThread(sUpdateUser, sUpdateUser.mgsid());
	}
}

void CCSUserMgr::InsertCDKeySQL(string cdkey, string &sql_str)
{
	stringstream sql_ss;
	sql_ss<<"insert into cdkey_info(cdkey, event_id, use_state, server_id)value('";
	sql_ss<<cdkey;
	sql_ss<<"',";
	sql_ss<<m_GiftCDKID;
	sql_ss<<",";
	sql_ss<<0;
	sql_ss<<",";
	sql_ss<<0;
	sql_ss<<");";

	sql_str = sql_ss.str();
}

void CCSUserMgr::UpdateCDKeySQL(const string user_nam, const string cdkey, string &sql_str)
{
	stringstream sql_ss;
	sql_ss<<"update cdkey_info set use_state=";
	sql_ss<<1;
	sql_ss<<",use_time=";
	sql_ss<<time(NULL);
	sql_ss<<",server_id=";
	sql_ss<<CCSKernel::GetInstance().GetKernelCfg().unCSId;
	sql_ss<<",user_name='";
	sql_ss<<user_nam;
	sql_ss<<"' where cdkey='";
	sql_ss<<cdkey;
	sql_ss<<"';";

	sql_str = sql_ss.str();
}

void CCSUserMgr::InsertNoticeSQL(SNotice notice, string sql_str)
{
	stringstream mystream;
	mystream<<"insert into notice(platform_id, title, eflag, estate, priority, notice, star_time, end_time)value(";
	mystream<<notice.platform;
	mystream<<",'";
	mystream<<notice.title;
	mystream<<"',";
	mystream<<notice.flag;
	mystream<<",";
	mystream<<notice.state;
	mystream<<",";
	mystream<<notice.priority;
	mystream<<",'";
	mystream<<notice.msg;
	mystream<<"',";
	mystream<<notice.star_time;
	mystream<<",";
	mystream<<notice.end_time;
	mystream<<");";
}

INT32 CCSUserMgr::RemoveUser(CCSUser*& pUser){
	if (!pUser){
		return eNormal;
	}

	GetCSKernelInstance()->RemoveTimer(pUser->GetTimerID());

	pUser->CheckHeroValidTimer(time(NULL));

	DBPoster_UpdateUser(pUser);//存盘// 

	CSSGameLogMgr::GetInstance().AddGameLog(eLog_UserDiscon,pUser->GetUserDBData() );

	pUser->SaveToRedis();

	m_MailMgr.RemoveObjId(pUser->GetUserDBData().sPODUsrDBData.un64ObjIdx);

	//玩家有昵称才删除
	if (!pUser->GetNickName().empty()){
		m_cNickNameMap.erase(pUser->GetNickName());
	}
	m_cUserGUIDMap.erase(pUser->GetGUID());

	delete pUser;
}

bool CCSUserMgr::CheckIfCanRemoveUser(CCSUser*& pUser){
	if (!pUser){
		return false;
	}

	if (pUser->GetUserPlayingStatus() == eUserPlayingStatus_OffLine && pUser->GetUserBattleInfoEx().GetBattleState() == eBattleState_Free){
		RemoveUser(pUser);
		ELOG(LOG_SpecialDebug, "删除玩家!");
		return true;
	}

	return false;
}

}