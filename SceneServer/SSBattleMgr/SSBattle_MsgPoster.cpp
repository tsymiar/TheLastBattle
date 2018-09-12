#include "stdafx.h"
#include "SSBattle.h"
#include "SSBattleMgr.h"
#include "SSGameObject.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSNPC.h"
#include "SSArea.h"

#include "SSUser.h"
#include "SSWorkThreadMgr.h"
#include "log/SSGameLogMgr.h"


namespace SceneServer{
	
INT32	CSSBattle::BroadMsgToGC(google::protobuf::Message& sMsg, int n32MsgID){
	for (INT32 i = 0; i < c_un32MaxUserInBattle; i++){
		CSSUser *piTempUser = m_asBattleUserInfo[i].piUser;
		if (NULL == piTempUser){
			continue;
		}
		CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(piTempUser->GetUserNetInfo(), sMsg, n32MsgID);
	} 
#if 0
	SendMsgToOB(sMsg, n32MsgID); 
#endif
	return eNormal;
}

INT32	CSSBattle::BroadMsgToGC_TrySelectHero(INT32 pos, INT32 heroType){
	GSToGC::TryToChooseHero sMsg;
	sMsg.set_pos(pos);
	sMsg.set_heroid(heroType);
	BroadMsgToGC(sMsg, sMsg.msgid());
	return eNormal;
}

INT32	CSSBattle::BroadMsgToGC_SelectHero(INT32 pos, INT32 heroType){
	GSToGC::HeroInfo sMsg;
	GSToGC::HeroPosInfo* pInfo = new GSToGC::HeroPosInfo;
	pInfo->set_pos(pos);
	pInfo->set_heroid(heroType);
	sMsg.set_allocated_heroposinfo(pInfo);
	BroadMsgToGC(sMsg, sMsg.msgid());
	return eNormal;
}

INT32	CSSBattle::PostMsgToGC_BattleHeroInfo(CSSUser* pcUser)
{
	GSToGC::BroadcastBattleHeroInfo sMsg;

	for (INT32 i = 0; i < c_un32MaxUserInBattle; i++){
		SBattleUserInfo& asBattleUserInfo = m_asBattleUserInfo[i];
		if (NULL == asBattleUserInfo.pcHero) continue;
		CSSHero* pHero = m_asBattleUserInfo[i].pcHero;
		GSToGC::BroadcastBattleHeroInfo::HeroInfo* pInfo = sMsg.add_heroinfo();
		pInfo->set_guid(pHero->GetObjGUID());
		pInfo->set_masterguid(m_asBattleUserInfo[i].un64Guid);
		pInfo->set_nickname(m_asBattleUserInfo[i].stNickName);
		pInfo->set_level(pHero->GetLevel());
		pInfo->set_campid(asBattleUserInfo.pcHero->GetCampID());
		pInfo->set_kills(asBattleUserInfo.pcHero->GetSSendData().un32HeroKills);
		pInfo->set_deadtimes(asBattleUserInfo.pcHero->GetSSendData().un32DeadTimes);
		pInfo->set_hp(pHero->GetCurHP());
		pInfo->set_mp(pHero->GetCurMP());
		pInfo->set_heroid(pHero->GetObjType());
		pInfo->set_headid(m_asBattleUserInfo[i].un32HeroTypeID);
		pInfo->set_assist(asBattleUserInfo.pcHero->GetAssistNum());
	}
	if (NULL != pcUser){
		BroadMsgToGC(sMsg, sMsg.msgid()); 
	}
	else{
		SendMsgToOB(sMsg, sMsg.msgid());
	}

	return eNormal;
}

INT32 CSSBattle::PostMsgToGC_BattleResult(INT32 n32WinCampID){
	//向玩家发送战斗结束
	GSToGC::BattleFinish sFinishMsg;
	for (set<CSSGameUnit*>::iterator iter = m_BuildingBaseSet.begin(); iter != m_BuildingBaseSet.end(); ++iter){
		CSSGameUnit *pcGameObject = *iter;
		if (NULL != pcGameObject && pcGameObject->GetCampID() % 2 != n32WinCampID % 2){
			sFinishMsg.set_bulidguid(pcGameObject->GetObjGUID());
			break;
		}
	}

	BroadMsgToGC(sFinishMsg, sFinishMsg.msgid());

	PostMsgToGC_BattleResultHeroInfo(n32WinCampID);

	GetBattleEndLog(n32WinCampID);
	return eNormal;
}

 
INT32	CSSBattle::PostMsgToCS_BattleEnd(INT32 n32WinCampID){
	//向CS发送详细的战斗结束统计信息
	{
		SSToCS::FinishBattle sMsg;
		sMsg.set_battleid(m_un64BattleID);
		sMsg.set_wingrpid(n32WinCampID);
		for(int i=0; i<c_un32MaxUserInBattle;i++){
			SBattleUserInfo*	pBattleUserInfo = &m_asBattleUserInfo[i];
			if(pBattleUserInfo->piUser == NULL) continue;
			if(pBattleUserInfo->pcHero == NULL) continue;
			SSToCS::FinishBattle_BattleUserData* pUserMsg = sMsg.add_batteluserdatalist();
			pUserMsg->set_guid(pBattleUserInfo->piUser->GetGUID());
			pUserMsg->set_heroguid(pBattleUserInfo->pcHero->GetObjGUID());
			pUserMsg->set_timeinsecond(m_tBattlePlayTime/1000);//不知道用来干啥的了。
			pUserMsg->set_ifend(TRUE);//暂时写死，游戏都是正常结束
			pUserMsg->set_heroid(pBattleUserInfo->un32HeroTypeID);
			pUserMsg->set_runepage(0);//符文暂时不实现
			pUserMsg->set_deadtimes(pBattleUserInfo->pcHero->GetSSendData().un32DeadTimes);
			pUserMsg->set_herokills(pBattleUserInfo->pcHero->GetSSendData().un32HeroKills);
			pUserMsg->set_destorybuilding(pBattleUserInfo->pcHero->GetSSendData().un32DestoryBuildings);
			pUserMsg->set_goldused(pBattleUserInfo->piUser->GetGoldUsed());
			pUserMsg->set_diamondused(pBattleUserInfo->piUser->GetDiamondUsed());
			pUserMsg->set_herolv(pBattleUserInfo->pcHero->GetLevel());
			pUserMsg->set_asskillnum(pBattleUserInfo->pcHero->GetSSendData().un32Assist);
			pUserMsg->set_lasthitnum(pBattleUserInfo->pcHero->GetSSendData().un32LastHit);
			pUserMsg->set_totalcp(pBattleUserInfo->pcHero->GetTotalPersonCP());
			ELOG(LOG_DBBUG, "herokills:%d.", pBattleUserInfo->asBattleResult.un32KillHero);
		}
		CSSWorkThreadMgr::GetInstance().PostMsgToCS(sMsg, sMsg.mgsid());
	}
	
	return eNormal;
}

INT32	CSSBattle::PostMsgToGC_NotifyHeroReborn(CSSUser *piUser)
{
	GSToGC::NotifyHeroReborn sMsg;
	SendMsgToSelfAndOB(sMsg, sMsg.msgid(), piUser);
	return eNormal;
}

INT32	CSSBattle::SyncMsgToGC(google::protobuf::Message& sMsg, int n32MsgID,const SGUID_KEY &guid, CSSGameUnit *pcGO, CSSUser *piSightUser){
	if (NULL == pcGO){
		return eEC_NullPointer;
	}

	if (NULL != piSightUser){
		SendMsgToSelfAndOB(sMsg, n32MsgID, piSightUser);
	}else{
		SendMsgToAllWatcher(sMsg, n32MsgID, pcGO);
	}
	return eNormal;
}

INT32	CSSBattle::SyncMsgToGC(google::protobuf::Message& sMsg, int n32MsgID,const SGUID_KEY &guid, map<CSSUser*, INT32> &rcUserSightMap){
	if (0 < rcUserSightMap.size()){
		std::map<CSSUser*, INT32>::iterator iterUserSight = rcUserSightMap.begin();
		while (rcUserSightMap.end() != iterUserSight){
			CSSUser *piTempUser = iterUserSight->first;
			CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(piTempUser->GetUserNetInfo(), sMsg, n32MsgID);
			++iterUserSight; 
		}
	}

	SendMsgToOB(sMsg, n32MsgID);

	return eNormal;
}

INT32	CSSBattle::SyncSpeedMsgToGC(  CSSGameUnit *pcGO, std::vector<CSSGameUnit*> & vecObject  ){
	if (NULL == pcGO){
		return eEC_NullPointer;
	}
	if (vecObject.size() < 1){
		return eEc_NoObjList;
	}

	GSToGC::CurAtkSpeed sMsg;
	std::vector<CSSGameUnit*>::const_iterator itr = vecObject.begin();
	for (; itr != vecObject.end(); itr++){  
		CSSGameUnit * gunit = *itr;
		if ( gunit != NULL){
			GSToGC::CurAtkSpeed::SpeedInfo* pSpeedInfo = sMsg.add_info();
			pSpeedInfo->set_guid(gunit->GetObjGUID());
			pSpeedInfo->set_speed(gunit->GetFPData(eEffectCate_AttackSpeed)); 
		} 
	} 

 
	SendMsgToAllWatcher(sMsg, sMsg.msgid());
	return eNormal;
}

INT32 CSSBattle::PostMsgToGC_NotifyReconnectInfo(CSSUser *piUser){
	GSToGC::NotifyReconnectInfo sMsg;
	sMsg.set_battlestate(m_eBattleState);
	for(INT32 i = 0; i < c_un32MaxUserInBattle; i++){
		if (!m_asBattleUserInfo[i].IfNoGameUnit()){
			GSToGC::NotifyReconnectInfo::ReconnectInfo* pReconnectInfo = sMsg.add_reconnectinfo();
			pReconnectInfo->set_pos(m_asBattleUserInfo[i].un16Pos);
			pReconnectInfo->set_guid(m_asBattleUserInfo[i].un64Guid);
			pReconnectInfo->set_nickname(m_asBattleUserInfo[i].stNickName.c_str());
			pReconnectInfo->set_heroid(m_asBattleUserInfo[i].un32HeroTypeID);
			pReconnectInfo->set_ifselected(m_asBattleUserInfo[i].bIfHeroChoosed);
		}
	}
	CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(piUser->GetUserNetInfo(), sMsg, sMsg.msgid());
	return eNormal;
}

INT32 CSSBattle::PostMsgToGC_EnterBattleState(CSSUser* pcUser, int64_t clientTime)
{
	INT32 pos = GetUserSeatPos(pcUser);
	if (pos==0) return eEC_UserDonotInTheBattle;
	BOOLEAN isReconnect = m_asBattleUserInfo[pos-1].bIfReconnect;
	if (isReconnect==TRUE) PostMsgToGC_NotifyReconnectInfo(pcUser);
	GSToGC::HeroList sHeroMsg;
	int64_t serverTime = GetUTCMiliSecond()+CSSWorkThreadMgr::GetInstance().GetUICInitSpanTime();
	sHeroMsg.set_timediff(serverTime-clientTime);
	for (auto iter = pcUser->GetCanUseHeroList()->begin(); 
		iter != pcUser->GetCanUseHeroList()->end(); iter++){
			sHeroMsg.add_heroid(*iter);
	}
	CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(pcUser->GetUserNetInfo(), sHeroMsg, sHeroMsg.msgid());
	// now set state change.
	GSToGC::BattleStateChange sMsg;
	sMsg.set_state(m_eBattleState);
	INT32 time = 0;
	if(eSSBS_SelectHero == m_eBattleState) time = c_tSelectHeroTime/1000;
	else if(eSSBS_SelectRune == m_eBattleState) time = c_tSelectRuneTime/1000;
	else if(eSSBS_Loading == m_eBattleState) time = c_tLoadTime/1000;
	sMsg.set_statetimeleft(time-(GetUTCMiliSecond()-GetBattleStateStartTime())/1000);
	ELOG(LOG_SpecialDebug,"玩家(%s)确认加入战场(%d),当前战场状态(%d)",pcUser->GetNickName().c_str(),m_un64BattleID,m_eBattleState);
	CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(pcUser->GetUserNetInfo(), sMsg, sMsg.msgid());

	switch(m_eBattleState)
	{
	case eSSBS_SelectHero:
	case eSSBS_SelectRune:
		{
			for(int i=0;i<c_un32MaxUserInBattle; i++){
				if(m_asBattleUserInfo[i].IfNoGameUnit() || m_asBattleUserInfo[i].un32HeroTypeID == 0) continue;
				if(m_asBattleUserInfo[i].bIfHeroChoosed){//英雄已选定//
					GSToGC::HeroInfo sMsg;
					GSToGC::HeroPosInfo* pInfo = new GSToGC::HeroPosInfo;
					pInfo->set_pos(m_asBattleUserInfo[i].un16Pos);
					pInfo->set_heroid(m_asBattleUserInfo[i].un32HeroTypeID);
					sMsg.set_allocated_heroposinfo(pInfo);
					CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(pcUser->GetUserNetInfo(), sMsg, sMsg.msgid());
				}
				else{//英雄尚未选定//
					GSToGC::TryToChooseHero sMsg;
					sMsg.set_pos(m_asBattleUserInfo[i].un16Pos);
					sMsg.set_heroid(m_asBattleUserInfo[i].un32HeroTypeID);
					CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(pcUser->GetUserNetInfo(), sMsg, sMsg.msgid());
				}
			}
		}
		break;
	case eSSBS_Loading:
		break;
	case eSSBS_Playing:
		{
			CSSHero* pHero = m_asBattleUserInfo[pos-1].pcHero;
			m_asBattleUserInfo[pos-1].offLineTime = 0;
			if(pHero == NULL) break;
			//将AI切换到普通的玩家AI
			if(pHero->IfAI()){
				pHero->SetIfAI(FALSE);
				pHero->ResetAI();
			}
		}
		break;
	default:
		ELOG(LOG_ERROR,"sorry, i can't let you enter battle in this state.");
		break;
	}
	return eNormal;
}
 

 
 void	CSSBattle::PostMsgToGC_BattleResultHeroInfo(INT32 n32WinCampID)
 {   
	 GSToGC::BroadcastBatteleRes sBroadcastBatteleRes;
	 for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		 if (m_asBattleUserInfo[i].IfNoGameUnit()){
			 continue;
		 }
		 GSToGC::BroadcastBatteleRes::ResInfo* pInfo = sBroadcastBatteleRes.add_resinfo();
		 pInfo->set_nickname(m_asBattleUserInfo[i].stNickName);
		 pInfo->set_camgpid(m_asBattleUserInfo[i].un16GrpID);
		
		 if (m_asBattleUserInfo[i].pcHero){
			 pInfo->set_curlevel(m_asBattleUserInfo[i].pcHero->GetLevel());
			 pInfo->set_objguid(m_asBattleUserInfo[i].pcHero->GetObjGUID());
			 pInfo->set_killtimes(m_asBattleUserInfo[i].pcHero->GetSSendData().un32HeroKills);
			 pInfo->set_deadtimes(m_asBattleUserInfo[i].pcHero->GetSSendData().un32DeadTimes);
			 pInfo->set_asstimes(m_asBattleUserInfo[i].pcHero->GetSSendData().un32Assist);
			 pInfo->set_totalcp(m_asBattleUserInfo[i].pcHero->GetTotalPersonCP());
			 pInfo->set_lasthit(m_asBattleUserInfo[i].pcHero->GetSSendData().un32LastHit);
			 pInfo->set_heroid(m_asBattleUserInfo[i].pcHero->GetHeroID()); 
		 }
	 }

	 for (UINT32 i = 0; i < c_un32MaxUserInBattle; i++){
		 if (m_asBattleUserInfo[i].piUser == NULL){
			 continue;
		 }
		 sBroadcastBatteleRes.set_ifwin(n32WinCampID==m_asBattleUserInfo[i].un16GrpID);
		 CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(m_asBattleUserInfo[i].piUser->GetUserNetInfo(), sBroadcastBatteleRes, sBroadcastBatteleRes.msgid());
	 } 
 }
 
}