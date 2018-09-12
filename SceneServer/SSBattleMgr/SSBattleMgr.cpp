// SSBattleMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include "SSBattle.h"
#include "SSBattleMgr.h"

#include "SSNPC.h"
#include "SSHero.h"
 
#include "SSBattle_Script.h"
#include "SSUser.h"

#include "SSWorkThreadMgr.h"
#include "SSProfileStatics.h"
#include "log/SSGameLogMgr.h"
 
using namespace std;

namespace SceneServer{

CSSBattleMgr::CSSBattleMgr()
	:m_un64MaxBattleID(1)
	,m_pMsgUser(NULL)
	,m_pMsgBattle(NULL)  
{
	Initialize();
}

CSSBattleMgr::~CSSBattleMgr(){
	Uninitialize();
}

INT32	CSSBattleMgr::Initialize(){
	//CSToSS
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_CreateBattle] = (PFCSMsgHandler)(&CSSBattleMgr::OnMsgFromCS_CreateBattle);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_OneUserOnline] = (PFCSMsgHandler)(&CSSBattleMgr::OnMsgFromCS_UserOnline);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_OneUserOffline] = (PFCSMsgHandler)(&CSSBattleMgr::OnMsgFromCS_UserOffline);
	//GCToSS
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskEnterBattle] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskEnterBattle);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskTrySelectHero] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskTrySelectHero);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskSelectHero] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskSeleteHero);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskSelectRunePage] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskSeleteRunePage);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_ReportLoadBattleComplete] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_LoadBattleComplete);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskMoveDir] = (PFGCMsgHandler)&CSSBattleMgr::OnMsgFromGC_AskMoveDir;
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskStopMove] = static_cast<PFGCMsgHandler>(&CSSBattleMgr::OnMsgFromGC_AskStopMove);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskBuyGoods] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskBuyGoods);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskUseGoods] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskUseGoods);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskSellGoods] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskSellGoods);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskMoveGoods] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskMoveGoods);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskUseSkill] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskUseSkill); 
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskLockTarget] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskLockTarget); 
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_ReportAltarStrawSolder] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_ReportAltarStrawSolder);	 
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskAbsorbMonster] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskAbsorbMonster);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskGasExplosion] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskGasExplosion);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskBuyRebornHero] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskBuyReborn);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskAutoAttack] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskAutoAttack);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_GMCmd] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_GMCmd);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_StepId] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_CompGuideStep);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskBornNPC] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskBornNPC);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskMovePos] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskMoveTar);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskAlwaysAutoAtk] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskAlwaysAutoAtk);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskHerosBattleInfo] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskHerosBattleInfo);
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskHeroAttributesInfo] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskHeroAttributes);
	m_asGCMsgHandler[GCToSS::eMsgToSSFromGS_AskQuickBattleEnd] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskQuickBattleEnd);
	//新手引导
	m_asGCMsgHandler[GCToSS::eMsgToGSToSSFromGC_AskGuideStepComp] = (PFGCMsgHandler)(&CSSBattleMgr::OnMsgFromGC_AskSSGuideStepComp);
	return eNormal;
}

INT32	CSSBattleMgr::Uninitialize(){
	map<UINT64, CSSUser*>::iterator iterUser = m_cUserGUIDMap.begin();
	while (m_cUserGUIDMap.end() != iterUser)
	{
		CSSUser *pcTempUser = iterUser->second;
		delete pcTempUser;
		++iterUser;
	}

	m_cUserGUIDMap.clear();

	UINT32 un32PlayingBattleNum = (UINT32)m_cAllBattleMap.size();
	map<UINT64, CSSBattle*>::iterator iterBattle = m_cAllBattleMap.begin();
	while (m_cAllBattleMap.end() != iterBattle){
		CSSBattle *pcTempBattle = iterBattle->second;
		delete pcTempBattle;
		pcTempBattle = NULL;
		++iterBattle;
	}
	m_cAllBattleMap.clear();

	ELOG(LOG_INFO, "%d battles cleared!", un32PlayingBattleNum);
	return eNormal;
}

void	CSSBattleMgr::OnBattleHeartBeat(int64_t tUTCMilsec, int64_t tTickSpan){
	m_tHeartBeartUTCMilsec = tUTCMilsec;
	auto iterPlayingBattle = m_cAllBattleMap.begin();
	while (m_cAllBattleMap.end() != iterPlayingBattle){
		CSSBattle *pcTempBattle = iterPlayingBattle->second;
		pcTempBattle->OnHeartBeat(tUTCMilsec, tTickSpan);
		if (eSSBS_Finished == pcTempBattle->GetBattleState()){
			iterPlayingBattle = m_cAllBattleMap.erase(iterPlayingBattle);
			delete pcTempBattle;
			pcTempBattle = NULL;
		}
		else{
			++iterPlayingBattle;
			CSSGameLogMgr::GetInstance().RefreshLog(pcTempBattle->GetBattleMapId(),pcTempBattle->GetBattleID(),pcTempBattle->GetBattleStateStartTime(),tUTCMilsec);
		}
		
	} 
}

void CSSBattleMgr::OnEvent(UINT32 event_type,PVOID params)
{
	switch (event_type)
	{
	case EVT_SS2CS_SyncAllGameInfo:
		PostMsgToCS_SyncAllBattleInfo();
		break;
	}
}

CSSBattle*	CSSBattleMgr::GetBattle(UINT64 un64BattleID){
	CSSBattle* pcBattle = NULL;

	map<UINT64, CSSBattle*>::iterator iterBattle = m_cAllBattleMap.find(un64BattleID);
	if (m_cAllBattleMap.end() != iterBattle){
		pcBattle = iterBattle->second;
	}

	return pcBattle;
}

INT32	CSSBattleMgr::PostMsgToGC_AskReturn(CSSUser *piUser, INT32 n32AskProtocalID, INT32 n32RetFlag){
	if (NULL == piUser){
		return eEC_NullUser;
	}

	GSToGC::AskRet sMsg;
	sMsg.set_askid(n32AskProtocalID);
	sMsg.set_errorcode(n32RetFlag);
	return CSSWorkThreadMgr::GetInstance().PostMsgToGCLIULU(piUser->GetUserNetInfo(), sMsg, sMsg.msgid());
}

TIME_MILSEC CSSBattleMgr::GetHeartBeatUTCMilsec(){
	return m_tHeartBeartUTCMilsec;
}

INT32 CSSBattleMgr::OnMsgFromGS_ReportGCMsg(ISSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pMsg, int n32MsgLength, int n32MsgID){
	if (n32MsgID < GCToSS::eMsgToGSToSSFromGC_Begin || n32MsgID >= GCToSS::eMsgToGSToSSFromGC_End){
		ELOG(LOG_WARNNING, "GS %d at NSIdx %d, GC protocalID %d, size %d error.", piGSInfo->GetGSID(), piGSInfo->GetNSID(), n32MsgID, n32MsgLength);
		return eNormal;
	}
	OnMsgFromGC(piGSInfo, n32GCNSID, pMsg, n32MsgLength, n32MsgID);
	return eNormal;
}

}