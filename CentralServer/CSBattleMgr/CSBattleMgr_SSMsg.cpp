// CSBattleMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSBattle.h"
#include "CSBattleMgr.h"

#include "CSUser.h"
#include "CSKernel.h"

namespace CentralServer{

INT32 CCSBattleMgr::OnMsgFromSS_AllBattleInfo(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength)
{
	boost::shared_ptr<SSToCS::SyncAllBattleInfo> pRecvMsg = ParseProtoMsg<SSToCS::SyncAllBattleInfo>(pData, n32DataLength);
	BattleMap theBattles;
	// 1.find all battle on this cs.
	for (BattleMap::iterator it=m_cAllBattleMap.begin();it!=m_cAllBattleMap.end();++it)
	{
		CCSBattle* pBattle = it->second;
		if (pBattle->GetSSID() == piSSInfo->GetSSID())
		{//相同的SS
			theBattles.insert(make_pair(it->first,it->second));
		}
	}
	// 2.erase the battle still in ss.
	int battle_size = pRecvMsg->playbattle_size();
	for (int i=0;i<battle_size;++i)
	{
		UINT64 battleId = pRecvMsg->playbattle(i);
		theBattles.erase(battleId);
	}
	// 3.cant' find the battle now, set it finish.
	for (auto it = theBattles.begin(); it != theBattles.end(); ++it)
	{
		it->second->OnFinish(NULL);
		m_cAllBattleMap.erase(it->first);
		delete it->second;
	}
	// 4.reset max battle id, here check it had more than 1000
	if (pRecvMsg->maxbattleid()>m_un64MaxBattleID)
	{
		m_un64MaxBattleID = pRecvMsg->maxbattleid();
	}

	piSSInfo->SetSSNetState(eSNS_Free);

	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromSS_CreateBattleResponse(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength)
{
	boost::shared_ptr<SSToCS::CreateBattleResponse> pCreateBattleResponse = ParseProtoMsg<SSToCS::CreateBattleResponse>(pData, n32DataLength);
	if (!pCreateBattleResponse){
		return eNormal;
	}
	CCSBattle* pBattle = GetBattle(pCreateBattleResponse->battleid());
	if (pBattle==NULL){
		Assert(false);
		return eNormal;
	}
	if (pCreateBattleResponse->rst()){
		pBattle->OnCreate();
	}
	else{
		pBattle->Abort();
		m_cAllBattleMap.erase(pBattle->GetBattleId());
		delete pBattle;
		pBattle = NULL;
	}
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromSS_FinishedBattle(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength)
{
	boost::shared_ptr<SSToCS::FinishBattle> pFinishBattle= ParseProtoMsg<SSToCS::FinishBattle>(pData, n32DataLength);
	if (!pFinishBattle){
		return eNormal;
	}
	CCSBattle* pBattle = GetBattle(pFinishBattle->battleid());
	if (pBattle==NULL){
		Assert(false);
		return eNormal;
	}
	pBattle->OnFinish(pFinishBattle.get());
	m_cAllBattleMap.erase(pBattle->GetBattleId());
	delete pBattle;
	pBattle = NULL;
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromSS_AskAllCfgList(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength)
{
	PostMsgToSS_NotifyGameLogSwitch(piSSInfo);

	PostMsgToSS_NotifyNPCCfgList(piSSInfo);
	PostMsgToSS_NotifyHeroCfgList(piSSInfo);
	PostMsgToSS_NotifyShopCfgList(piSSInfo);
	PostMsgToSS_NotifyGoodsCfgList(piSSInfo);
	PostMsgToSS_NotifyRobotAICfgList(piSSInfo);
	PostMsgToSS_NotifyRobotAIPathCfgList(piSSInfo);
	PostMsgToSS_NotifyNewSkillCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelAccountCfgCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelEmitCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelRangeCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelLeadingCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelBufCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelSummonCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelMoveCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelSwitchCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelPurificationCfgList(piSSInfo);
	PostMsgToSS_NotifySkillModelLinkCfgList(piSSInfo);
	PostMsgToSS_NotifyPassitiveSkillCfgList(piSSInfo);
	PostMsgToSS_NotifyPassitiveEffectBloodSeekCfgList(piSSInfo);
	PostMsgToSS_NotifyPassitiveEffectReboundCfgList(piSSInfo);
	PostMsgToSS_NotifyPassitiveEffectReliveCfgList(piSSInfo); 
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromSS_RepostGotMsg(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<SSToCS::ReportGotSomeMsg> pMsg = ParseProtoMsg<SSToCS::ReportGotSomeMsg>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	return eNormal;
}

}