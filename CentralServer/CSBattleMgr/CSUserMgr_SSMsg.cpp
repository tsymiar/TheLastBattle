// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUserMgr.h"
#include "CSUser.h"
#include "CSCfgMgr.h"

#include "CSKernel.h"
using namespace std;

namespace CentralServer{

INT32	CCSUserMgr::OnMsgFromSS_AskUpdateData(const ICSSSInfo *piSSInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<SSToCS::AskUpdateData> pAskUpdateData = ParseProtoMsg<SSToCS::AskUpdateData>(pData, n32DataLength);
	if (!pAskUpdateData){
		return 0;
	} 
	UpdateUserInfo(pAskUpdateData); 

	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromSS_AskAddGold(const ICSSSInfo *piSSInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<SSToCS::AddGold> sAddGold = ParseProtoMsg<SSToCS::AddGold>(pData, n32DataLength);
	if (!sAddGold){
		return 0;
	}

	CCSUser *pcUser = GetUserByGuid(sAddGold->guid());
	if (NULL == pcUser){
		return eEc_InvalidGUID;
	}
	pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_Gold,sAddGold->gold() );
	return eNormal;
}

INT32	CCSUserMgr::OnMsgFromSS_AskChangeFirstWinTime(const ICSSSInfo *piSSInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<SSToCS::AskChangeFirstWinTime> sAskChangeFirstWinTime = ParseProtoMsg<SSToCS::AskChangeFirstWinTime>(pData, n32DataLength);
	if (!sAskChangeFirstWinTime){
		return 0;
	}

	INT64 posUserGUID(sAskChangeFirstWinTime->guid());
	CCSUser *pcUser = GetUserByGuid(posUserGUID);
	if (NULL == pcUser){
		return eEc_InvalidGUID;
	}
	pcUser->GetUserDBData().ChangeUserDbData(eUserDBType_LastFirstWinTime,sAskChangeFirstWinTime->curtime() );
	return eNormal;
}



INT32		CCSUserMgr::OnMsgFromSS_AskUserGoHall(const ICSSSInfo *piSSInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<SSToCS::AskUserGoHall> pAskUserGoHall = ParseProtoMsg<SSToCS::AskUserGoHall>(pData, n32DataLength);
	if (!pAskUserGoHall){
		return 0;
	}

	UINT64 posUserGUID(pAskUserGoHall->guid());
	CCSUser *pcUser = GetUserByGuid(posUserGUID);
	if (NULL == pcUser){
		ELOG(LOG_ERROR,"--OnMsgFromSS_AskUserGoHall got wrong guid:%llu",posUserGUID);
		return eEC_NullMsgHandler;
	}
 	
	GSToGC::BattleStateChange sBattleStateChange;
	sBattleStateChange.set_state(GSToGC::BattleState::finish);
	pcUser->PostMsgToGC(sBattleStateChange, sBattleStateChange.msgid());

	sBattleStateChange.set_state(GSToGC::BattleState::waiting);
	pcUser->PostMsgToGC(sBattleStateChange, sBattleStateChange.msgid()); 
 
	pcUser->ResetPingTimer();
	return eNormal;
}

 INT32	CCSUserMgr::OnMsgFromSS_AskUpdateBattleTaskSetps(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength)
 {
	 boost::shared_ptr<SSToCS::UpGuideBattleStep> pBattleTaskStep = ParseProtoMsg<SSToCS::UpGuideBattleStep>(pData, n32DataLength);
	 if (!pBattleTaskStep){
		 return 0;
	 }
	 UINT64 posUserGUID(pBattleTaskStep->guid());
	 CCSUser *pcUser = GetUserByGuid(posUserGUID);
	 if (NULL == pcUser){
		 ELOG(LOG_ERROR,"--OnMsgFromSS_AskUserGoHall got wrong guid:%llu",posUserGUID);
		 return eEC_NullMsgHandler;
	 }

	DBPoster_UpdateUser(pcUser);
	return eNormal;
 }

 /************************************************************************/
 /* gamelog switch                                                                     */
 /************************************************************************/
 INT32	CCSUserMgr::OnMsgFromSS_AskGameLogSwitch(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength)
 { 
	 return eNormal;
 }

}