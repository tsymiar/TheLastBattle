// CSBattleMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSBattle.h"
#include "CSBattleMgr.h"
#include "CSCfgMgr.h"
#include "CSWordFilter.h"
#include "CSUser.h"
#include "CSUserMgr.h"
#include "CSKernel.h"
  
#include "CSGameLogMgr.h"

using namespace std;

namespace CentralServer{

CCSBattleMgr::CCSBattleMgr(){
	m_un64MaxBattleID = 0;
	m_pMatchMgr = new CCSMatchMgr();
}

CCSBattleMgr::~CCSBattleMgr(){
	Uninitialize();
}

INT32	CCSBattleMgr::Initialize(){
	GetCSKernelInstance()->AddTimer(std::bind(&CCSMatchMgr::Update, m_pMatchMgr, std::placeholders::_1, std::placeholders::_2), c_matcherDelay, true);
	return eNormal;
}

INT32	CCSBattleMgr::Uninitialize(){
	BattleMap::iterator iterBattle = m_cAllBattleMap.begin();
	while (m_cAllBattleMap.end() != iterBattle){
		CCSBattle *pcTempBattle = iterBattle->second;
		delete pcTempBattle;
		++iterBattle;
	}
	m_cAllBattleMap.clear();
	CCSCfgMgr::getInstance().Uninitialize();
	ELOG(LOG_DBBUG, "success!");
	return eNormal;
}

void CCSBattleMgr::GetProfileReport(wstringstream &report)
{
	report << " room=" << m_BattleRoomList.size() << " battle=" << m_cAllBattleMap.size();
}

CCSBattle*	CCSBattleMgr::GetBattle(UINT64 un64BattleID){
	BattleMap::iterator iterBattle = m_cAllBattleMap.find(un64BattleID);
	if (m_cAllBattleMap.end() != iterBattle){
		return iterBattle->second;
	}
	return NULL;
}

string sBattleStateStrArr[] ={
	"eBS_Free",
	"eBS_Waiting",
	"eBS_SelectHero",
	"eBS_Show",
	"eBS_Loading",
	"eBS_Playing",
	"eBS_Finished",
};

INT32	CCSBattleMgr::PostMsgToBattleSS(UINT64 un64BattleID, google::protobuf::Message& sMsg, int n32MsgID){
	CCSBattle *pcBattle = GetBattle(un64BattleID);
	if (NULL == pcBattle){
		return eEC_InvalidBattleID;
	}
	return pcBattle->PostMsgToBattleSS(sMsg, n32MsgID);
}

INT32	CCSBattleMgr::SyncMsgForRemoveBattle(UINT64 un64BattleID)
{
	CCSBattle *pcBattle = GetBattle(un64BattleID); 
	if(pcBattle){
		m_cAllBattleMap.erase(un64BattleID);
		delete pcBattle;
	} 
	return eNormal;
}

template <class Msg, class T>
void FillStrOfCfg(Msg& pMsg, T& cfg){
	std::string* pCfg = pMsg.add_cfg();
	pCfg->resize(sizeof(cfg));
	memcpy((char*)pCfg->data(), &cfg, sizeof(cfg));
}

INT32	CCSBattleMgr::PostMsgToSS_NotifyNPCCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyNPCCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyNPCCfgList);
	map<INT32, SNPCCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cNPCCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cNPCCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32	CCSBattleMgr::PostMsgToSS_NotifyHeroCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyHeroCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyHeroCfgList);
	map<INT32, SHeroCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cHeroCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cHeroCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}


INT32 CCSBattleMgr::PostMsgToSS_NotifyShopCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyShopCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyShopCfgList);
	map<INT32,SShopCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cShopCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cShopCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}


INT32 CCSBattleMgr::PostMsgToSS_NotifyGoodsCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyGoodsCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyGoodsCfgList);
	map<INT32,SGoodsCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cGoodsCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cGoodsCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32 CCSBattleMgr::PostMsgToSS_NotifyRobotAICfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyRobotAICfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyRobotAICfgList);
	for(auto iter = CCSCfgMgr::getInstance().m_cRobotAICfg.begin(); iter != CCSCfgMgr::getInstance().m_cRobotAICfg.end(); iter++){
		FillStrOfCfg(sMsg, *iter);
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32 CCSBattleMgr::PostMsgToSS_NotifyRobotAIPathCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyRobotAIPathCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyRobotAIPathCfgList);
	map<INT32,SRobotAIPathCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cRobotAIPathCfg.begin();
	while (CCSCfgMgr::getInstance().m_cRobotAIPathCfg.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32 CCSBattleMgr::PostMsgToSS_NotifyNewSkillCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyNewSkillCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyNewSkillCfgList);
	map<UINT32, SSNewSkillCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cNewSkillCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cNewSkillCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32 CCSBattleMgr::PostMsgToSS_NotifySkillModelAccountCfgCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelAccountCfgCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelAccountCfgCfgList);
	map<UINT32, SSkillModelAccountCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSkillModelAccountCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSkillModelAccountCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32 CCSBattleMgr::PostMsgToSS_NotifySkillModelLeadingCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelLeadingCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelLeadingCfgList);
	map<UINT32, SSkillModelLeadingCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSkillModelLeadingCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSkillModelLeadingCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32 CCSBattleMgr::PostMsgToSS_NotifySkillModelEmitCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelEmitCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelEmitCfgList);
	map<UINT32, SSkillModelEmitCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSkillModelEmitCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSkillModelEmitCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32	CCSBattleMgr::PostMsgToSS_NotifySkillModelRangeCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelRangeCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelRangeCfgList);
	map<UINT32, SSkillModelRangeCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSkillModelRangeCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSkillModelRangeCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}
 
INT32	CCSBattleMgr::PostMsgToSS_NotifySkillModelBufCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelBufCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelBufCfgList);
	map<UINT32, SSkillModelBufCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSkillModelBufCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSkillModelBufCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32		CCSBattleMgr::PostMsgToSS_NotifySkillModelSummonCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelSummonCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelSummonCfgList);
	map<UINT32, SSkillModuleSummonCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSSkillModuleSummonCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSSkillModuleSummonCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32		CCSBattleMgr::PostMsgToSS_NotifySkillModelMoveCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelMoveCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelMoveCfgList);
	map<UINT32, SSkillModelMoveCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSkillModuleMoveCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSkillModuleMoveCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32		CCSBattleMgr::PostMsgToSS_NotifySkillModelSwitchCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelSwitchCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelSwitchCfgList);
	map<UINT32, SSkillModelSwitchCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSkillModuleSwitchCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSkillModuleSwitchCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32		CCSBattleMgr::PostMsgToSS_NotifySkillModelPurificationCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelPurificationCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelPurificationCfgList);
	map<UINT32, SSkillModelPurificationCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSkillModulePurificationCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSkillModulePurificationCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32		CCSBattleMgr::PostMsgToSS_NotifySkillModelLinkCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifySkillModelLinkCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifySkillModelLinkCfgList);
	map<UINT32, SSkillModelLinkCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cSkillModuleLinkCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cSkillModuleLinkCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32		CCSBattleMgr::PostMsgToSS_NotifyPassitiveSkillCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyPassitiveSkillCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyPassitiveSkillCfgList);
	map<UINT32, SSPassiveSkilllCfg>::iterator iterCfg = CCSCfgMgr::getInstance().m_cPassitiveSkillCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cPassitiveSkillCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32		CCSBattleMgr::PostMsgToSS_NotifyPassitiveEffectBloodSeekCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyPassitiveEffectBloodSeekCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyPassitiveEffectBloodSeekCfgList);
	map<UINT32, SSPassiveEffectCfg_BloodSeek>::iterator iterCfg = CCSCfgMgr::getInstance().m_cPassitiveEffectBloodSeekCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cPassitiveEffectBloodSeekCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32		CCSBattleMgr::PostMsgToSS_NotifyPassitiveEffectReboundCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyPassitiveEffectReboundCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyPassitiveEffectReboundCfgList);
	map<UINT32, SSPassiveEffectCfg_Rebound>::iterator iterCfg = CCSCfgMgr::getInstance().m_cPassitiveEffectReboundCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cPassitiveEffectReboundCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

INT32		CCSBattleMgr::PostMsgToSS_NotifyPassitiveEffectReliveCfgList(ICSSSInfo *piSSInfo){
	CSToSS::NotifyPassitiveEffectReliveCfgList sMsg;
	sMsg.set_msgid(CSToSS::eMsgToSSFromCS_NotifyPassitiveEffectReliveCfgList);
	map<UINT32, SSPassiveEffectCfg_Relive>::iterator iterCfg = CCSCfgMgr::getInstance().m_cPassitiveEffectReliveCfgMap.begin();
	while (CCSCfgMgr::getInstance().m_cPassitiveEffectReliveCfgMap.end() != iterCfg){
		FillStrOfCfg(sMsg, iterCfg->second);
		++iterCfg;
	}

	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}
bool CCSBattleMgr::CheckWordFilter(char* pStr){
	return -1 == CCSWordFilter::GetWordFilterInstance().Check(pStr);
}

void	CCSBattleMgr::RegisterMsgHandle(SSMsgHandlerMap& m_SSMsgHandlerMap, GSMsgHandlerMap& m_GSMsgHandlerMap, GCMsgHandlerMap& m_GCMsgHandlerMap, RCMsgHandlerMap& m_RCMsgHandlerMap)
{
#define PARAMS_3 this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
#define PARAMS_4 this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
	//////////////////////////////////////////////////////////////////////////
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_AllBattleInfo] = std::bind(&CCSBattleMgr::OnMsgFromSS_AllBattleInfo, PARAMS_3);
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_CreateBattleResponse] = std::bind(&CCSBattleMgr::OnMsgFromSS_CreateBattleResponse, PARAMS_3);
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_FinishBattle] = std::bind(&CCSBattleMgr::OnMsgFromSS_FinishedBattle, PARAMS_3);
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_AskAllCfgList] = std::bind(&CCSBattleMgr::OnMsgFromSS_AskAllCfgList, PARAMS_3);
	m_SSMsgHandlerMap[SSToCS::eMsgToCSFromSS_ReportGotSomeMsg] = std::bind(&CCSBattleMgr::OnMsgFromSS_RepostGotMsg, PARAMS_3);
	//////////////////////////////////////////////////////////////////////////
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskCreateMatchTeam] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskCreateMatchTeam, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskAddMatchTeam] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskAddMatchTeam, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskRemoveMatchTeam] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskRemoveMatchTeam, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskInviteJoinMatchTeam] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskInviteJoinMatchTeam, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskStartMatch] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskStartMatch, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskStopMatch] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskStopMatch, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_RequestMatchTeamList] = std::bind(&CCSBattleMgr::OnMsgFromGC_QuestMatchTeamList, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_NotifyOneMatchNeedOneRet] = std::bind(&CCSBattleMgr::OnMsgFromGC_NotifyOneMatchNeedOneRet, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskLeaveRoom] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskLeaveRoom, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskChangeRoomSeat] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskChangeRoomSeat, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskReadyRoom] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskReadyRoom, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskStartRoom] =  std::bind(&CCSBattleMgr::OnMsgFromGC_AskStartRoom, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskCancelRoom] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskCancelRoom, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskChatInRoom] =  std::bind(&CCSBattleMgr::OnMsgFromGC_AskChatInRoom, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskCreateRoom] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskCreateRoom, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskAddRoom] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskAddRoom, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskRoomList] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskRoomList, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskReEnterRoom] = std::bind(&CCSBattleMgr::OnMsgFromGC_AskReEnterRoom, PARAMS_4);
	m_GCMsgHandlerMap[GCToCS::eMsgToGSToCSFromGC_AskGuideBattle] =  std::bind(&CCSBattleMgr::OnMsgFromGC_AskGuideBattle, PARAMS_4);
	//////////////////////////////////////////////////////////////////////////
	m_RCMsgHandlerMap[RCToCS::eMsgRC2CS_AskServerInfo] = std::bind(&CCSBattleMgr::OnMsgFromRC_AskServerInfo, PARAMS_3);
}

INT32 CCSBattleMgr::NotifyBattleSSUserIsOnline(CCSUser *piUser, bool isOnline){
	if (NULL == piUser){
		return eEC_NullUser;
	}

	CCSBattle *pcBattle = GetBattle(piUser->GetUserBattleInfoEx().GetBattleID());
	if (NULL == pcBattle){
		return eEC_NullBattle;
	}

	return pcBattle->PostMsgToSS_NotifyUserIsOnline(piUser, isOnline);
}

static CCSBattleMgr* pInstance = nullptr;
CCSBattleMgr& CCSBattleMgr::GetInstance(){
	if (nullptr == pInstance){
		pInstance = new CCSBattleMgr;
	}
	return *pInstance;
}

void CCSBattleMgr::OnBattleMached( EBattleMatchType type,UINT32 mapId,map<UINT32,vector<CCSMatchTeam*>>& teamList,map<UINT32,UINT32>* aiRobots )
{
	stringstream mystream;  

	CCSUserListMap userListMap;
	CCSMatchTeam* pTeam = NULL;
	IMatchPlayer* pPlayer = NULL;
	auto it=teamList.begin();
	for (;it!=teamList.end();++it){
		for (auto it2=it->second.begin();it2!=it->second.end();++it2){
			pTeam = (*it2);
			pTeam->Switch(false);
			for (auto itplayer=pTeam->mVPlayer.begin();itplayer!=pTeam->mVPlayer.end();++itplayer){
				pPlayer = (*itplayer);
				userListMap[it->first].AddElement(pPlayer->m_pUser);

				//log
				if (pPlayer->m_pUser)
				{
					mystream << pPlayer->m_pUser->GetUserDBData().sPODUsrDBData.un64ObjIdx << LOG_SIGN_FIRST; 
				} 
			}
		}
	}
	OnBattleMached(userListMap,aiRobots,mapId,type);

	//log
	{   
		mystream << LOG_SIGN << type << LOG_SIGN;
		mystream << ( (pTeam !=NULL) ? pTeam->mTeamId : 0)  << LOG_SIGN;
		mystream <<  mapId << LOG_SIGN;
		mystream << ( (pTeam !=NULL) ? (time(NULL) - pTeam->mStartTime) : 0) << LOG_SIGN; 
		CSSGameLogMgr::GetInstance().AddGameLog(eLog_MatchEnd,type,mystream.str()); 
	} 
}

INT32 CCSBattleMgr::OnMsgFromGC_AskCreateMatchTeam( ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength )
{
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskCreateMatchTeam> pMsg = ParseProtoMsg<GCToCS::AskCreateMatchTeam>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	if (pMsg->matchtype()>=eMatchType_Total) return eEC_ParseProtoError;
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Free) return eEC_JustInBattle;
	SMapLogicCfg* pLogicCfg = CCSCfgMgr::getInstance().GetMapLogicCfg(pMsg->mapid());
	if (pLogicCfg==NULL) return eEC_InvalidMapID;
	CCSMatchTeam* pTeam = m_pMatchMgr->UserCreateTeam(piUser->GetMatchPlayer(),(EBattleMatchType)pMsg->matchtype(),pMsg->mapid());
	if (pTeam==NULL) return eEC_NullBattle;

	//log
	CSSGameLogMgr::GetInstance().AddGameLog(eLog_CreateMatch,piUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,pMsg->matchtype(),pMsg->mapid());

	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskAddMatchTeam( ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength )
{
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskAddMatchTeam> pMsg = ParseProtoMsg<GCToCS::AskAddMatchTeam>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Free){
		if (!m_pMatchMgr->UserRemoveTeam(piUser->GetMatchPlayer())) return eEC_AddMatchTeamError;
		if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Free) return eEC_AddMatchTeamError;
	}
	INT32 errorCode = eEC_AddMatchTeamError;
	CCSUser* pLeader = GetCSUserMgrInstance()->GetUserByNickName(pMsg->friendsnickname());
	if (pLeader!=NULL){
		if (m_pMatchMgr->UserAddTeam(piUser->GetMatchPlayer(),pLeader->GetMatchPlayer())){
			errorCode = eNormal;
		}
	}
	if (errorCode!=eNormal){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskAddMatchTeam, eEC_AddMatchTeamError);
	}
	return errorCode;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskRemoveMatchTeam( ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength )
{
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskRemoveMatchTeam> pMsg = ParseProtoMsg<GCToCS::AskRemoveMatchTeam>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	bool bRet = m_pMatchMgr->UserRemoveTeam(piUser->GetMatchPlayer());
	if (!bRet) return eEC_NullBattle;
	 
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskInviteJoinMatchTeam( ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength )
{
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskInviteJoinMatchTeam> pMsg = ParseProtoMsg<GCToCS::AskInviteJoinMatchTeam>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Match) return eEC_JustInBattle;
	CCSUser* pFriend = GetCSUserMgrInstance()->GetUserByNickName(pMsg->friendsnickname());
	if (pFriend==NULL) return eEC_JustInBattle;
	if (pFriend->GetUserBattleInfoEx().GetBattleType()!=eBattleState_Free){
		if (pFriend->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Match) return eEC_JustInBattle;
		else if (m_pMatchMgr->IsUserTeamInMatch(pFriend->GetMatchPlayer())) return eEC_JustInBattle;
	}
	GSToGC::NotifyMatchInviteJoin sendmsg;
	sendmsg.set_nickname(piUser->GetNickName());
	pFriend->PostMsgToGC(sendmsg,sendmsg.msgid());
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskStartMatch( ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength )
{
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskStartMatch> pMsg = ParseProtoMsg<GCToCS::AskStartMatch>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Match) return eEC_JustInBattle;
	if (piUser->GetUserBattleInfoEx().GetBattleState()!=eBattleState_Wait) return eEC_JustInBattle;
	INT32 nRet = m_pMatchMgr->TeamStartMatch(piUser->GetMatchPlayer());
	if (nRet!=eNormal){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskStartMatch, nRet);
	}
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskStopMatch( ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength )
{
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskStopMatch> pMsg = ParseProtoMsg<GCToCS::AskStopMatch>(pData, n32DataLength);
	if (!pMsg) return eEC_ParseProtoError;
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Match) return eEC_JustInBattle;
	bool bRet = m_pMatchMgr->TeamStopMatch(piUser->GetMatchPlayer());
	if (!bRet) return eEC_NullBattle;
	//log
	CSSGameLogMgr::GetInstance().AddGameLog(eLog_LogoutMatch,piUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,0);
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_QuestMatchTeamList( ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength )
{
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	bool bRet = GetBattleMgrInstance()->GetMatcher()->PushDataOnlinePlayer(piUser->GetMatchPlayer());
	if (!bRet){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_RequestMatchTeamList, eEC_UnKnownError);
		return eEC_UnKnownError;
	}
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_NotifyOneMatchNeedOneRet( ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength )
{
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::NotifyOneMatchNeedOneRet> pMsgRet = ParseProtoMsg<GCToCS::NotifyOneMatchNeedOneRet>(pData, n32DataLength);
	if (!pMsgRet ||
		!GetBattleMgrInstance()->GetMatcher()->TeamAddInvitePlayer(
		piUser->GetMatchPlayer(),pMsgRet->fightid(),pMsgRet->mapid(),pMsgRet->isaccept())){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_NotifyOneMatchNeedOneRet, eEC_MatchLinkInvalid);
	}
	return eNormal;
}


INT32		CCSBattleMgr::PostMsgToSS_NotifyGameLogSwitch(ICSSSInfo *piSSInfo)
{
	CSToSS::SSGameFuncSwitch sMsg;
	sMsg.set_log_witch( CCSCfgMgr::getInstance().GetCSGlobalCfg().dbSSLogSwitch);
	sMsg.set_gm_witch( CCSCfgMgr::getInstance().GetCSGlobalCfg().sSGMCmdSwitch);  
	sMsg.set_solderwitch(CCSCfgMgr::getInstance().GetCSGlobalCfg().ssSolderBornSwitch);
	sMsg.set_monsterwitch(CCSCfgMgr::getInstance().GetCSGlobalCfg().ssMonsterBornSwitch);
	sMsg.set_altarsolderwitch(CCSCfgMgr::getInstance().GetCSGlobalCfg().ssAltarSolderSwitch);
	sMsg.set_logrefshtime(CCSCfgMgr::getInstance().GetCSGlobalCfg().logRefreshTime);
	ELOG(LOG_INFO,"-------cs send switch to ss ok------------",);
	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
}

}