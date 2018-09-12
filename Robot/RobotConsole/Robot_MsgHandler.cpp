#include "stdafx.h"
#include "Robot.h"
#include "RobotMgr.h"
#include "Hero.h"
#include "RobotNPC.h"
#include "CfgMgr.h"
#include "Player.h"

namespace ReBot
{
static std::string gBattleStateStr[] ={	"eBS_Free", "eBS_Waiting",
	"eBS_SelectHero", "eBS_ShowHero",
	"eBS_Loading", "eBS_Playing",
	"eBS_ShowResult", "eBS_Finished"};

bool	CCClient::HandleMsgFalter(INT32 n32NSID, const char* pMsg, int n32MsgLength, int n32MsgID)
{
	switch(n32MsgID)
	{
	case (int)LSToGC::MsgID::eMsgToGCFromLS_NotifyLoginResult://ls登录返回失败（失败重新走ls登录）
		OnMsg_NotifySdkLoginResult(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)LSToGC::MsgID::eMsgToGCFromLS_NotifyServerBSAddr://ls登录返回bs列表（开始连接bs）
		OnMsg_NotifyServerAddr(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)BSToGC::MsgID::eMsgToGCFromBS_OneClinetLoginCheckRet://bs登录返回是否成功（失败重新走ls登录）
		OnMsg_OneClinetLoginCheckRet(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)BSToGC::MsgID::eMsgToGCFromBS_AskGateAddressRet://bs登录成功并分配了gs（开始连接gs，未收到玩家基本信息重新走ls登录，若收到了则断线重连）
		OnMsg_NotifyGateServerInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyUserBaseInfo://gs登录成功返回了用户信息
		OnMsgFromGS_NotifyUserBaseInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromCS_NotifyIsOnSS:
		OnMsgFromGS_IsOnSS(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyMatchTeamBaseInfo:
		OnMsg_NotifyTeamBaseInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyMatchTeamPlayerInfo:
		OnMsg_NotifyTeamPlayerInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	default:
		return false;
		break;
	}
	return true;
}

INT32	CCClient::HandleMsg(INT32 n32NSID, const char* pMsg, int n32MsgLength, int n32MsgID)
{
	if (HandleMsgFalter(n32NSID,pMsg,n32MsgLength,n32MsgID)) return eNormal;
	switch(n32MsgID){
	case (int)GSToGC::MsgID::eMsgToGCFromGS_GCAskPingRet:
		OnMsgFromGS_GCAskPingRet(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_GCAskRet:
		OnMsgFromGS_GCAskRetError(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyHeroList:
		OnMsgFromGS_NotifyHeroList(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyBattleBaseInfo:
		OnMsgFromGS_NotifyBattleBaseInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyBattleSeatPosInfo:
		OnMsgFromGS_NotifyBattleSeatPosInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyBattleStateChange:
		OnMsgFromGS_NotifyBattleStateChange(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyCurBattleChange:
		OnMsgFromGS_NotifyCurBattleChange(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGameObjectAppear:
		OnMsgFromGS_NotifyGamgObjectAppear(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGameObjectDisappear:
		OnMsgFromGS_NotifyGameObjectDisappear(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGameObjectFreeState:
		OnMsgFromGS_NotifyGameObjectFreeState(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGameObjectRunState:
		OnMsgFromGS_NotifyGameObjectRunState(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyBattleHeroInfo:
		OnMsgFromGS_NotifyBattleHeroInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyTryToChooseHero:
		OnMsgFromGS_NotifyBattleTryHeroInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_BroadBuildingDestroyByWho:
		OnMsgFromGS_NotifyBroadBuildingDestroyByWho(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyCurCP:
		OnMsgFromGS_NotifyCurCP(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifySkillInfo:
		OnMsgFromGS_NotifySkillInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyFightPropertyInfo:
		OnMsgFromGS_NotifyFightPropertyInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyHPInfo:
		OnMsgFromGS_NotifyHPInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyMPInfo:
		OnMsgFromGS_NotifyMPInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyHeroLevelInfo:
		OnMsgFromGS_NotifyLevelInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyFuryValue:
		OnMsgFromGS_NotifyFury(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyLeaveBattleSuccess:
		OnMsg_NotifyLeaveBattleSuccess();
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGameObjectPrepareSkillState:
		OnMsgFromGS_NotifyGameObjectPrepareSkillState(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGameObjectReleaseSkillState:
		OnMsgFromGS_NotifyGameObjectReleaseSkillState(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGameObjectDeadState:
		OnMsgFromGS_NotifyGameObjectDeadState(pMsg,n32MsgLength,n32MsgID);
		break;
		//case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGoodsInf:
		//OnMsg_NotifyGoodsInfo(pMsg,n32MsgLength,n32MsgID);
		//break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyHeroInfo:
		OnMsg_NotifyHeroInfo(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyPassitiveSkillLoad:
		OnMsg_NotifyPassitiveSkillLoad(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyPassitiveSkillUnload:
		OnMsg_NotifyPassitiveSkillUnLoad(pMsg,n32MsgLength,n32MsgID);
		break;
		//没有函数体
	case GSToGC::MsgID::eMsgToGCFromGS_NotifySkillModelEmit:
	case GSToGC::MsgID::eMsgToGCFromGS_NotifySkillModelHitTarget:
	case GSToGC::MsgID::eMsgToGCFromGS_NotifySkillModelRange:
	case GSToGC::MsgID::eMsgToGCFromGS_NotifySkillModelRangeEnd:
	case GSToGC::MsgID::eMsgToGCFromGS_NotifyUserSNSList:
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyHPChange:
		OnMsg_NotifyHPChange(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyMPChange:
		OnMsg_NotifyMPChange(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyExpInfo:
		OnMsg_NotifyExp(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyHeroRebornTimes:
		OnMsg_NotifyHeroRebornTimes(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyCurDeadTimes:
		OnMsg_NotifyCurDeadTimes(pMsg,n32MsgLength,n32MsgID);
		break;
		//没有函数体
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGameObjectSkillCD:
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyAbsorbMonsterResult:
		OnMsg_NotifyAbsorbMonsterResult(pMsg,n32MsgLength,n32MsgID);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_NotifyGoodsInf:
		OnMsg_NotifyGoodsInfo(pMsg,n32MsgLength);
		break;
	case (int)GSToGC::MsgID::eMsgToGCFromGS_AskRoomListRet:
		OnMsg_NotifyRoomListRet(pMsg,n32MsgLength,n32MsgID);
		break;
	default:
		//ELOG(LOG_WARNNING, "Not Handler error No:%d", n32MsgID);
		break;
	}
	return eNormal;
}
//message 1
INT32	CCClient::OnMsgFromGS_GCAskPingRet(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::PingRet> msg(new GSToGC::PingRet);
	msg->ParseFromArray(pMsg,n32MsgLength);
	return eNormal;
}


INT32 CCClient::OnMsg_NotifyAbsorbMonsterResult(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::AbsorbRes> msg(new GSToGC::AbsorbRes);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->guid());
	if (msg->res()){
		CClientGameObject* pGO = GetGOByGUID(sGUID);
		if (NULL != pGO && pGO->IsHero()){
			CRebotHero* pHero = (CRebotHero*)pGO;
			pHero->UpdateAbsorb(msg->absorb1(), msg->absorb2());
		}
	}
	return 0;
}

INT32 CCClient::OnMsgFromGS_NotifyFury(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::FuryVal> msg(new GSToGC::FuryVal);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->guid());
	INT32 n32Fury = msg->fury();

	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO && pGO->IsHero()){
		CRebotHero* pHero = (CRebotHero*)pGO;
		pHero->UpdateFury(n32Fury);
	}
	return eNormal;
}

INT32 CCClient::OnMsg_NotifyCurDeadTimes(const char* pMsg,int n32MsgLength,int n32MsgID){
	return eNormal;
}


INT32 CCClient::OnMsg_NotifyGoodsInfo(const char* pMsg,int n32MsgLength){
	std::auto_ptr<GSToGC::NotifyGoodsInfo> msg(new GSToGC::NotifyGoodsInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);

	SGUID_KEY sGUID(msg->guid());
	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO && pGO == m_pPlayer){
		for (int i = 0; i < msg->info_size(); ++i){
			m_pPlayer->UpdateGoodsState(*msg->mutable_info(i));
		}
	}

	return 0;
}

INT32 CCClient::OnMsg_NotifyHeroRebornTimes(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::RebornTimes> msg(new GSToGC::RebornTimes);
	msg->ParseFromArray(pMsg,n32MsgLength);

	SGUID_KEY sGUID(msg->masterguid());
	INT64 sendTime = msg->sendtimes();
	INT64 n64Gold = msg->gold();
	INT16 n16SurplusRebornTimes = msg->remain_times();
	INT32 n32RebornTime = msg->reborn_time();

	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO && pGO->IsHero()){
		CRebotHero* pHero = (CRebotHero*)pGO;
	}

	return eNormal;
}

INT32 CCClient::OnMsg_NotifyPassitiveSkillLoad(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::NotifyPassitiveSkillLoad> msg(new GSToGC::NotifyPassitiveSkillLoad);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->guid());
	INT32 n32SkillID = msg->skillid();

	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO){
	}

	return eNormal;
}


INT32 CCClient::OnMsg_NotifyHPChange(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::HPChange> msg(new GSToGC::HPChange);
	msg->ParseFromArray(pMsg,n32MsgLength);

	SGUID_KEY sGUID(msg->guid());
	INT32 n32CurHP = msg->hp();

	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO){
		pGO->UpdateHP(n32CurHP);
	}

	return eNormal;
}


INT32 CCClient::OnMsg_NotifyMPChange(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::MpChange> msg(new GSToGC::MpChange);
	msg->ParseFromArray(pMsg,n32MsgLength);

	SGUID_KEY sGUID(msg->guid());
	INT32 n32CurMP = msg->mp();

	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO){
		pGO->UpdateMP(n32CurMP);
	}

	return eNormal;
}

INT32 CCClient::OnMsg_NotifyPassitiveSkillUnLoad(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::NotifyPassitiveSkillUnLoad> msg(new GSToGC::NotifyPassitiveSkillUnLoad);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->guid());
	INT32 n32SkillID = msg->skillid();

	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO){

	}

	return eNormal;
}


INT32 CCClient::OnMsg_NotifyHeroInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::NotifyHeroInfo> msg(new GSToGC::NotifyHeroInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);

	SGUID_KEY sGUID(msg->guid());
	INT32 n32Fury = msg->fury();
	UINT32 un32AbosorbID1 = msg->absorb1();
	UINT32 un32AbosorbID2 = msg->absorb2();

	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (pGO != NULL && pGO->IfHero()){
		CRebotHero* pHero = (CRebotHero*)pGO;
		pHero->UpdateFury(n32Fury);
		pHero->UpdateAbsorb(un32AbosorbID1, un32AbosorbID2);
	}

	return eNormal;
}

INT32 CCClient::OnMsg_NotifyGoodsInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::NotifyGoodsInfo> msg(new GSToGC::NotifyGoodsInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	for (int i = 0; i <msg->info_size();i++)
	{
		const GSToGC::NotifyGoodsInfo::GoodsInfo& info = msg->info(i);
		UINT8 un8Pos = info.pos();
		UINT8 un8IsComposed = info.ifcomposed()?1:0;//其实没用上 
		INT32 n32Num = info.num();
		UINT32 un32TypeId = info.tyepid();
		INT32 n32State = info.state();
		UINT32 un32StateTime = info.statetime();
		m_pPlayer->UpdateGoods(un8Pos, n32Num, un32TypeId, n32State, un32StateTime);
	}

	return eNormal;
}

INT32 CCClient::OnMsgFromGS_NotifySkillInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::NotifySkillInfo> msg(new GSToGC::NotifySkillInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->guid());
	UINT8 un8SkillSlotID = msg->skillslot();
	UINT32 un32StateTime = msg->time();
	GOMap::iterator iter = m_pClientGameObjectMap.find(sGUID);
	if (iter != m_pClientGameObjectMap.end()){
		iter->second->UpdateSkillState(un8SkillSlotID, 0, un32StateTime);
	}
	return eNormal;
}

//未调用
INT32 CCClient::OnMsg_NotifySkillID(const char* pMsg,int n32MsgLength,int n32MsgID){
	//BYTE *pbDataPos = (BYTE*)(psMsgHeader + 1);
	//SGUID_KEY sGUID = *(SGUID_KEY*)pbDataPos;
	//pbDataPos += sizeof(sGUID);
	//UINT32 un32SkillID = *(UINT32*)pbDataPos;
	//pbDataPos += sizeof(UINT32);
	//UINT32 un32CDTime = *(UINT32*)pbDataPos;

	//GOMap::iterator iter = m_pClientGameObjectMap.find(sGUID);
	//if (iter != m_pClientGameObjectMap.end()){
	//	iter->second->UpdateSkill(un32SkillID, un32CDTime);
	//}
	//return eNormal;
}

INT32 CCClient::OnMsgFromGS_NotifyGameObjectDeadState(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::DeadState> msg(new GSToGC::DeadState);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGameObjGuid(msg->objguid());

	CClientGameObject* pGO = GetGOByGUID(sGameObjGuid);
	if (NULL != pGO && pGO != m_pPlayer){
		m_pClientGameObjectMap.erase(sGameObjGuid);
		delete pGO;
	}

	return eNormal;
}

INT32 CCClient::OnMsgFromGS_NotifyLevelInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::LevelInfo> msg(new GSToGC::LevelInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->guid());
	UINT32 un32Level = msg->level();

	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO && pGO->IsHero()){
		CRebotHero* pHero = (CRebotHero*)pGO;
		pHero->UpdateLvel(un32Level);
	}

	return eNormal;
}

INT32 CCClient::OnMsgFromGS_NotifyHPInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::NotifyHPInfo> msg(new GSToGC::NotifyHPInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	for (int i = 0; i < msg->hpinfo_size(); i++){
		const GSToGC::NotifyHPInfo::HPInfo& info = msg->hpinfo(i);
		SGUID_KEY sGUID(info.guid());
		INT32 n32CurHP = info.curhp();
		INT32 n32MaxHP = info.maxhp();
		CClientGameObject* pGO = GetGOByGUID(sGUID);
		if (NULL != pGO){
			pGO->UpdateHP(n32CurHP);
			pGO->UpdateMaxHP(n32MaxHP);
		}
	}

	return eNormal;
}

INT32 CCClient::OnMsgFromGS_NotifyMPInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::NotifyMPInfo> msg(new GSToGC::NotifyMPInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	for (int i = 0; i < msg->mpinfo_size(); i++){
		const GSToGC::NotifyMPInfo::MPInfo& info = msg->mpinfo(i);
		SGUID_KEY sGUID(info.guid());
		INT32 n32CurMP = info.curmp();
		INT32 n32MaxMP = info.maxmp();
		CClientGameObject* pGO = GetGOByGUID(sGUID);
		if (NULL != pGO){
			pGO->UpdateMP(n32CurMP);
			pGO->UpdateMaxMP(n32MaxMP);
		}
	}
	return eNormal;
}

INT32 CCClient::OnMsgFromGS_NotifyFightPropertyInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::FPInfo> msg(new GSToGC::FPInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->guid());
	SFightProperty sFP;
	sFP.n32PhyAttPower = msg->info(0);
	sFP.n32MagicAttPower =msg->info(1);
	sFP.n32PhyDefAbility =msg->info(2);
	sFP.n32MagicDefAbility=msg->info(3);
	sFP.n32MoveSpeed=msg->info(4);
	sFP.n32AttackSpeed=msg->info(5);
	sFP.n32AttackDist=msg->info(6);
	sFP.n32MaxHP=msg->info(7);
	sFP.n32MaxMP=msg->info(8);
	sFP.n32HPRecover=msg->info(9);
	sFP.n32MPRecover=msg->info(10);
	sFP.n32ReliveSecond=msg->info(11);
	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO){
		pGO->UpdateFP(sFP);
	}

	return eNormal;
}

// 该函数全是执行错误的返回代码
INT32 CCClient::OnMsgFromGS_GCAskRetError(const char* pMsg,int n32MsgLength,int n32MsgID)
{
	std::auto_ptr<GSToGC::AskRet> msg(new GSToGC::AskRet);
	msg->ParseFromArray(pMsg,n32MsgLength);
	m_LastErrorCode = msg->errorcode();//最后一次错误代码
	switch(msg->askid()){
	case GSToGC::eMsgToGCFromGS_GCAskPingRet:
		OnMsg_NotifyPing(pMsg,n32MsgLength,n32MsgID);
		break;
	case GCToCS::eMsgToGSToCSFromGC_AskComleteUserInfo:
		if (m_eAskState==RS_AskCompleteUserInfo) m_eAskState = RS_AskCompleteUserInfoFailed;
		break;
	case GCToSS::eMsgToGSToSSFromGC_AskLockTarget:
		if (m_pPlayer) m_pPlayer->ClearLockedTar();
		break;
	case GCToCS::eMsgToGSToCSFromGC_AskReEnterRoom:
		if (m_eAskState==RS_AskReEnterRoom) m_eAskState = RS_AskReEnterRoomFailed;
		break;
	case GCToCS::eMsgToGSToCSFromGC_RequestMatchTeamList:
		if (m_eAskState==RS_AskMatchTeamList) m_eAskState = RS_AskMatchTeamListFailed;
		break;
	case GCToSS::eMsgToGSToSSFromGC_AskTrySelectHero:
		if (m_eAskState==RS_AskTryToSelectHero) m_eAskState = RS_AskTryToSelectHeroFailed;
		break;
	case GCToSS::eMsgToGSToSSFromGC_AskSelectHero:
		if (m_eAskState==RS_AskSelectHero) m_eAskState = RS_AskSelectHeroFailed;
		break;
	case GCToCS::eMsgToGSToCSFromGC_AskStartRoom:
		if (m_eAskState==RS_AskStartRoom) m_eAskState = RS_AskStartRoomFailed;
		break;
	case GCToSS::eMsgToGSToSSFromGC_AskEnterBattle:
		if (m_eAskState==RS_AskEnterBattle) m_eAskState = RS_AskEnterBattleFailed;
		break;
	case GCToCS::eMsgToGSToCSFromGC_AskAddRoom:
		if (m_eAskState==RS_AskAddRoom) m_eAskState = RS_AskRoomListFailed;
		break;
	case GCToCS::eMsgToGSToCSFromGC_AskCreateMatchTeam:
		if (m_eAskState==RS_AskCreateMatchTeam) m_eAskState = RS_AskCreateMatchTeamFailed;
		break;
	case GCToCS::eMsgToGSToCSFromGC_AskStartMatch:
		if (m_eAskState==RS_AskStartMatch) m_eAskState = RS_AskStartMatchFailed;
	default:
		//ELOG(LOG_ERROR, "n32AskProtocalID:%d, ErrorCode:%d", n32MsgID, m_LastErrorCode);
		break;
	}
	return eNormal;
}

INT32 CCClient::OnMsgFromGS_IsOnSS(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::NotifyIsOnSS> msg(new GSToGC::NotifyIsOnSS);
	msg->ParseFromArray(pMsg,n32MsgLength);
	m_n32SSID = msg->ssid();
	return eNormal;
}

//message 7
INT32	CCClient::OnMsgFromGS_NotifyHeroList(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::HeroList> msg(new GSToGC::HeroList);
	msg->ParseFromArray(pMsg,n32MsgLength);
	m_HeroVec.clear();
	for(int i=0;i<msg->heroid_size();i++)
	{
		m_HeroVec.push_back(msg->heroid(i));
	}
	//ELOG(LOG_DBBUG, "heronum: %d", m_HeroVec.size());
	return eNormal;
}

INT32 CCClient::OnMsg_NotifyLeaveBattleSuccess(){
	return eNormal;
}

//未调用
INT32 CCClient::OnMsg_NotifyPing(const char* pMsg,int n32MsgLength,int n32MsgID){
	return eNormal;
}

// 收到该消息，代表你在战场中了。
INT32 CCClient::OnMsgFromGS_NotifyBattleBaseInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::BattleBaseInfo> msg(new GSToGC::BattleBaseInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	m_MapID = msg->mapid();
	m_un64BattleID = msg->battleid();
	Assert(m_eServerBattleState==eSSBS_Invalid);
	bool isReconnect = msg->ifreconnect();//你是否以前掉线过...
	return eNormal;
}

// 收到该消息，服务器战场状态改变了。
INT32	CCClient::OnMsgFromGS_NotifyBattleStateChange(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::BattleStateChange> msg(new GSToGC::BattleStateChange);
	msg->ParseFromArray(pMsg,n32MsgLength);
	m_eServerBattleState = (ESSBattleState)msg->state();
	m_ServerStartTime = gRobotMgr->GetTickCount();
	return eNormal;
}

// 收到该消息，代表你可以设置房间了。
INT32	CCClient::OnMsgFromGS_NotifyBattleSeatPosInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::BattleSeatPosInfo> msg(new GSToGC::BattleSeatPosInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	for(int i=0;i<msg->posinfo_size();i++){
		const GSToGC::BattleSeatPosInfo::PosInfo& info = msg->posinfo(i);
		INT8 un8Seat = info.pos();
		SGUID_KEY UserGuid = info.guid();
		Assert(un8Seat<=6);
		if (UserGuid==0) { m_RoomCounter.erase(un8Seat); continue; }
		RobotTimes& pRobotTime = m_RoomCounter[un8Seat];
		pRobotTime.Seat = un8Seat;
		pRobotTime.UserGuid = UserGuid;
		pRobotTime.AppearTimes = gRobotMgr->GetTickCount();
		pRobotTime.IfMaster = info.ifmaster();
		pRobotTime.IfReady = info.ifready();
		pRobotTime.HeadId = info.headid();
		pRobotTime.HeroId = 0;
		pRobotTime.TryHeroId = 0;
	}
	return eNormal;
}

//message 11
INT32	CCClient::OnMsgFromGS_NotifyCurBattleChange(const char* pMsg,int n32MsgLength,int n32MsgID)
{
	std::auto_ptr<GSToGC::CurBattleChange> msg(new GSToGC::CurBattleChange);
	msg->ParseFromArray(pMsg,n32MsgLength);

	//ELOG(LOG_DBBUG, "m_n32SSID:%d, m_un64BattleID:%llu, m_MapID:%d", msg->ssid(), msg->battleid(), msg->mapid());
	return eNormal;
}
//message 12
INT32	CCClient::OnMsgFromGS_NotifyGamgObjectAppear(const char* pMsg,int n32MsgLength,int n32MsgID){
	return eNormal;
	std::auto_ptr<GSToGC::GOAppear> msg(new GSToGC::GOAppear);
	msg->ParseFromArray(pMsg,n32MsgLength);
	for(int i=0;i<msg->info_size();i++)
	{
		const GSToGC::GOAppear::AppearInfo& info = msg->info(i);
		SGUID_KEY sMasterGUID;
		sMasterGUID = info.masterguid();
		SGUID_KEY sGameObjGUID;
		sGameObjGUID = info.objguid();
		INT32 n32CampID = info.camp();
		CVector3D curPos(m_pRobotMgr->PosToVec(info.pos()));
		CVector3D curDir(m_pRobotMgr->DirToVec(info.dir()));
		UINT32 un32TypeID = 0;//sGameObjGUID.GetGUIDType();
		CClientGameObject* pGO = NULL;
		if (m_pClientGameObjectMap.find(sGameObjGUID) == m_pClientGameObjectMap.end()){
			if (true/*CFunction::IfTypeNPC(sGameObjGUID)*/){
				CRobotNPC* pNPC = new CRobotNPC();
				pNPC->LoadNPCCfg(CCfgMgr::getInstance().GetNPCCfg(un32TypeID));
				pGO = pNPC;
			}
			else if (true/*CFunction::IfTypeHero(sGameObjGUID)*/){
				CRebotHero* pHero = NULL;
				if (sMasterGUID == m_UserGUID){
					ELOG(LOG_INFO, "Robot(%u) EnterBattle!", m_un32RobotID);
					m_pPlayer = new CRobotPlayer();
					m_pPlayer->SetRobot(this);
					pHero = m_pPlayer;
				}
				else{
					pHero = new CRebotHero();
				}
				pGO = pHero;
				pHero->LoadHeroCfg(CCfgMgr::getInstance().GetHeroCfg(un32TypeID));
			}
			else{
				//ELOG(LOG_ERROR, "unknow type:%llu", sGameObjGUID);
				break;
			}
		}

		if (NULL == pGO){
			continue;
		}

		pGO->SetObjectGUID(sGameObjGUID);
		pGO->SetCamp((EGameObjectCamp)n32CampID);
		pGO->SetMasterGUID(sMasterGUID);
		pGO->UpdatePos(curPos);
		pGO->UpdateDir(curDir);
		m_pClientGameObjectMap[sGameObjGUID] = pGO;
	}

	return eNormal;
}
//message 14
INT32	CCClient::OnMsgFromGS_NotifyGameObjectFreeState(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::FreeState> msg(new GSToGC::FreeState);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->objguid());
	CVector3D curPos(m_pRobotMgr->PosToVec(msg->pos()));
	CVector3D curDir(m_pRobotMgr->DirToVec(msg->dir()));
	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO){
		pGO->UpdateState(eOAS_Free, curPos, curDir);
	}
	return eNormal;
}

INT32 CCClient::OnMsgFromGS_NotifyGameObjectDisappear(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::DisappearInfo> msg(new GSToGC::DisappearInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	for (int i = 0 ; i<msg->guid_size();i++){
		SGUID_KEY sGUID(msg->guid(i));
		CClientGameObject* pGO = GetGOByGUID(sGUID);
		if (NULL != pGO && pGO != m_pPlayer){
			m_pClientGameObjectMap.erase(sGUID);
			delete pGO;
		}
	}
	return eNormal;
}

INT32 CCClient::OnMsgFromGS_NotifyGameObjectRunState(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::RunningState> msg(new GSToGC::RunningState);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->objguid());
	CVector3D curPos(m_pRobotMgr->PosToVec(msg->pos()));
	CVector3D curDir(m_pRobotMgr->DirToVec(msg->dir()));
	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (NULL != pGO){
		pGO->UpdateState(eOAS_Running, curPos, curDir);
		pGO->UpdateMoveSpeed(msg->movespeed());
	}
	return eNormal;
}
//message 18
 INT32	CCClient::OnMsgFromGS_NotifyGameObjectPrepareSkillState(const char* pMsg,int n32MsgLength,int n32MsgID)
 {
	 std::auto_ptr<GSToGC::PrepareSkillState> msg(new GSToGC::PrepareSkillState);
	 msg->ParseFromArray(pMsg,n32MsgLength);

	 SGUID_KEY sGameObjGuid(msg->objguid());
	 UINT32 un32SkillID = msg->skillid();
	 CVector3D curPos = m_pRobotMgr->PosToVec(msg->pos());
	 CVector3D curDir = m_pRobotMgr->DirToVec(msg->dir());
	 SGUID_KEY sTarGUID(msg->targuid());

	 CClientGameObject* pGO = GetGOByGUID(sGameObjGuid);
	 if (NULL != pGO){
		 pGO->UpdateState(eOAS_PreparingSkill, curPos, curDir);
	 }

	 return eNormal;
 }
//message 19
INT32	CCClient::OnMsgFromGS_NotifyGameObjectReleaseSkillState(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::ReleasingSkillState> msg(new GSToGC::ReleasingSkillState);
	msg->ParseFromArray(pMsg,n32MsgLength);

	SGUID_KEY sGameObjGuid(msg->objguid());
	UINT32 un32SkillID = msg->skillid();
	CVector3D curPos = m_pRobotMgr->PosToVec(msg->pos());
	CVector3D curDir = m_pRobotMgr->DirToVec(msg->dir());
	SGUID_KEY sTarGUID(msg->targuid());

	CClientGameObject* pGO = GetGOByGUID(sGameObjGuid);
	if (NULL != pGO){
		pGO->UpdateState(eOAS_ReleasingSkill, curPos, curDir);
	}

	return eNormal;
}

// 收到欲选择英雄列表
INT32	CCClient::OnMsgFromGS_NotifyBattleTryHeroInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	if (m_eAskState==RS_AskTryToSelectHero) m_eAskState = RS_AskTryToSelectHeroFailed;
	std::auto_ptr<GSToGC::TryToChooseHero> msg(new GSToGC::TryToChooseHero);
	msg->ParseFromArray(pMsg,n32MsgLength);	
	UINT8 seatPos = msg->pos();	
	UINT32 heroId = msg->heroid();
	auto it = m_RoomCounter.find(seatPos);
	if (it!=m_RoomCounter.end()) it->second.TryHeroId = heroId;
	if (m_eAskState==RS_AskTryToSelectHeroFailed) m_eAskState = RS_AskTryToSelectHeroOk;
	return eNormal;
}

// 收到已选择英雄列表
INT32	CCClient::OnMsgFromGS_NotifyBattleHeroInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	if (m_eAskState==RS_AskSelectHero) m_eAskState = RS_AskSelectHeroFailed;
	std::auto_ptr<GSToGC::HeroInfo> msg(new GSToGC::HeroInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);	
	UINT8 seatPos = msg->heroposinfo().pos();	
	UINT32 heroId = msg->heroposinfo().heroid();
	auto it = m_RoomCounter.find(seatPos);
	if (it!=m_RoomCounter.end()) it->second.HeroId = heroId;
	if (m_eAskState==RS_AskSelectHeroFailed) m_eAskState = RS_AskSelectHeroOk;
	return eNormal;
}

INT32 CCClient::OnMsg_NotifyExp(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::Exp> msg(new GSToGC::Exp);
	msg->ParseFromArray(pMsg,n32MsgLength);

	SGUID_KEY sGUID(msg->guid());
	INT32 n32Exp = msg->exp();

	CClientGameObject* pGO = GetGOByGUID(sGUID);
	if (pGO && pGO->IsHero()){
		CRebotHero* pHero = (CRebotHero*)pGO;
		pHero->UpdateExp(n32Exp);
	}

	return eNormal;
}


INT32 CCClient::OnMsgFromGS_NotifyBroadBuildingDestroyByWho(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::BroadcastBuildingDestory> msg(new GSToGC::BroadcastBuildingDestory);
	msg->ParseFromArray(pMsg,n32MsgLength);
	SGUID_KEY sGUID(msg->buildingguid());

	GOMap::iterator iter = m_pClientGameObjectMap.find(sGUID);
	if (m_pClientGameObjectMap.end() == iter){
		return eNormal;
	}

	delete iter->second;
	m_pClientGameObjectMap.erase(iter);

	return eNormal;
}

INT32 CCClient::OnMsgFromGS_NotifyCurCP(const char* pMsg,int n32MsgLength,int n32MsgID){
	std::auto_ptr<GSToGC::CurCP> msg(new GSToGC::CurCP);
	msg->ParseFromArray(pMsg,n32MsgLength);


	SGUID_KEY sGUID(msg->targetguid());
	
	INT32 n32PersonCP = msg->person_cp();
	INT32 n32TeamCP = msg->team_cp();
	GOMap::iterator iter = m_pClientGameObjectMap.find(sGUID);
	if (iter != m_pClientGameObjectMap.end()){
		CClientGameObject* pGO = iter->second;
		pGO->UpdateCP(n32PersonCP, n32TeamCP);
	}
	return eNormal;
}

//////////////////////////////////////////////////////////////////////////

INT32 CCClient::OnMsg_NotifySdkLoginResult( const char* pMsg,int n32MsgLength,int n32MsgID )
{
	Assert(mLoginState==RLS_LsAskLogin);
	mLoginState = RLS_LsAskLoginError;
	std::auto_ptr<LSToGC::LoginResult> msg(new LSToGC::LoginResult);
	msg->ParseFromArray(pMsg,n32MsgLength);
	ELOG(LOG_ERROR,"ls check false robotid:%u reson:%d", m_un32RobotID, msg->result());
	// 登录失败要断开连接重新登录
	return eNormal;
}

INT32 CCClient::OnMsg_NotifyServerAddr( const char* pMsg,int n32MsgLength,int n32MsgID )// 获得服务器列表
{
	Assert(mLoginState==RLS_LsAskLogin);
	mLoginState = RLS_LsHadGetBsAddress;
	NetworkClose(eServerLink_LS);
	return eNormal;
}

INT32 CCClient::OnMsg_OneClinetLoginCheckRet( const char* pMsg,int n32MsgLength,int n32MsgID )//gate server link result
{
	ELOG(LOG_SpecialDebug,"user(%s)(%s)(%u) ask login ret",m_szUserName.c_str(),m_szUserPwd.c_str(),m_n32NSID);
	Assert(mLoginState==RLS_BsAskLogin);
	mLoginState = RLS_BsAskLoginRet;
	std::auto_ptr<BSToGC::ClinetLoginCheckRet> msg(new BSToGC::ClinetLoginCheckRet);
	msg->ParseFromArray(pMsg,n32MsgLength);
	if(msg->login_success() != 1){//登录bs失败//
		NetworkClose(eServerLink_BS);
	}
	return eNormal;
}

INT32 CCClient::OnMsg_NotifyGateServerInfo( const char* pMsg,int n32MsgLength,int n32MsgID )//获得gate server信息
{
	Assert(mLoginState==RLS_BsAskLoginRet);
	mLoginState = RLS_BsHadGetGsAddress;
	std::auto_ptr<BSToGC::AskGateAddressRet> msg(new BSToGC::AskGateAddressRet);
	msg->ParseFromArray(pMsg,n32MsgLength);
	m_szUserNameRet = msg->user_name();
	m_sToken = msg->token();
	m_gateIp = msg->ip();
	m_gatePort = msg->port();
	NetworkClose(eServerLink_BS);
	return eNormal;
}

INT32	CCClient::OnMsgFromGS_NotifyUserBaseInfo(const char* pMsg,int n32MsgLength,int n32MsgID){
	ELOG(LOG_SpecialDebug,"robot(%u) OnMsgFromGS_NotifyUserBaseInfo",m_un32RobotID);
	Assert(mLoginState==RLS_GsAskLogin || mLoginState==RLS_GsHadGetBaseInfo);
	mLoginState = RLS_GsHadGetBaseInfo;
	std::auto_ptr<GSToGC::UserBaseInfo> msg(new GSToGC::UserBaseInfo);
	msg->ParseFromArray(pMsg,n32MsgLength);
	m_UserGUID = msg->guid();
	if (m_eAskState==RS_AskNone || m_eAskState==RS_AskCompleteUserInfo){
		bool isReged = (msg->nickname().size()>0 && msg->headid()>0);
		m_eAskState = isReged?RS_AskCompleteUserInfoOk:RS_AskCompleteUserInfoFailed;
	}
	return eNormal;
}

}
