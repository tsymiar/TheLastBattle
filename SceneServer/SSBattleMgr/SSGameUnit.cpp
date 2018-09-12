#include "stdafx.h"
#include "SSGameUnit.h"
#include "SSNPC.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSEffectMgr.h"
#include "SSPassiveSkillMgr.h"

#include "SSUser.h"
#include "SSBattle_Script.h"
#include "SSBattleMgr.h"

#include "SSWorkThreadMgr.h"
#include <iostream>
#include "log/SSGameLogMgr.h"
 

namespace SceneServer{

CSSGameUnit::CSSGameUnit(UINT64 guid, EGameObjectCamp sCamp, UINT32 objectID, UINT64 sMasterGUID, CSSBattle* pBattle, CSSBattleMgr* pMgr):CSSGameObject(guid, sCamp, objectID, sMasterGUID, pBattle, pMgr)
	, m_n32LastRecHP(0)
	, m_n32LastShouldRecHP(0)
	, m_n32LastRecMP(0)
	, m_n32LastShouldRecMP(0)
	, m_tRecoverTickCounter(0)
	, m_tLastRecoverTime(0)
	, m_moveHolder(NULL)
	, m_MoveEffectID(0) 
	, m_n32FastArrayID(0){
		m_sFpMgr.SetMaster(this);
}

CSSGameUnit::~CSSGameUnit(void){
	for (INT32 i = EPassiveSkillTriggerType_None; i < EPassiveSkillTriggerType_Max; ++i){
		for(INT* uIDIter = m_mPassiveSkillMap[i].Begin(); uIDIter != m_mPassiveSkillMap[i].End(); uIDIter = m_mPassiveSkillMap[i].Next()){
			GetCurBattle()->GetPassiveSkillMgr()->ExpiredPassiveSkill(*uIDIter);
		}
		m_mPassiveSkillMap[i].Clear();
	}
	for (auto iter = m_BufArr.Begin(); iter != m_BufArr.End(); iter = m_BufArr.Next()){
		CSSSkillEffect* pEffect = GetCurBattle()->GetEffectMgr()->GetEffect(*iter);
		if (NULL != pEffect){
			pEffect->SetExpired();
		}
	}
}

INT32	CSSGameUnit::OnGameUnitHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if (IsDead()){
		CheckDeadState();
	}

	//检查状态
	
	if (0 < GetFPData(eEffectCate_Dizziness) && eOAS_PassiveState > GetGOActionStateInfo().eOAS && eOAS_Free != GetGOActionStateInfo().eOAS){
		BeginAction_Free(true);
	}

	CheckRecover(tTickSpan);
	if(m_pAI != NULL) m_pAI->HeartBeat(tUTCMilsec, tTickSpan);
	CheckSyncMoveState();
	OnPassitiveSkillHeartBeat(tUTCMilsec, tTickSpan);
	m_sFpMgr.OnHeartBeat(tUTCMilsec,tTickSpan);
	return eNormal;
}

INT32	CSSGameUnit::CheckRecover(TIME_TICK tTickSpan){
	if (IsDead()){
		return eNormal;
	}

	m_tRecoverTickCounter += tTickSpan;

	if (m_tRecoverTickCounter - m_tLastRecoverTime < 1000){
		return eNormal;
	}
	if (0 < GetFPData(eEffectCate_HPRecoverRate) && GetFPData(eEffectCate_MaxHP) > GetCurHP()){	
		INT32 n32WouldRec = GetFPData(eEffectCate_HPRecoverRate) * (m_tRecoverTickCounter - m_tLastRecoverTime) / 1000.0f;	
		INT32 n32ThisTimeRecove = n32WouldRec + m_n32LastShouldRecHP - m_n32LastRecHP * 1000;
		INT32 n32RealRecover = (INT32) (n32ThisTimeRecove / 1000.0f);

		m_n32LastShouldRecHP = n32ThisTimeRecove;
		m_n32LastRecHP = n32RealRecover;
		if (IfHero()){
			INT32 tspan = m_tRecoverTickCounter - m_tLastRecoverTime;
			if (! TryCastToHero()->GetHeroBornPos().IsZero()){
				if (GetCurPos().CanWatch(500, TryCastToHero()->GetHeroBornPos())){
					INT32 n32AddHP = GetFPData(eEffectCate_MaxHP) * 20 / 100.0f;
					n32RealRecover += n32AddHP;
				}
			}
		}

		if (0 < n32RealRecover){
			ChangeCurHP(this,GSToGC::HPMPChangeReason::RecoverHurt,n32RealRecover);
		}
	}
	if (GetCurMP() < GetFPData(eEffectCate_MaxMP) && 0 < GetFPData(eEffectCate_MPRecoverRate)){
		INT32 n32WouldRec = GetFPData(eEffectCate_MPRecoverRate) * (m_tRecoverTickCounter - m_tLastRecoverTime) / 1000.0f;
		INT32 n32ThisTimeRecove = n32WouldRec + m_n32LastShouldRecMP - m_n32LastRecMP * 1000.0f;
		INT32 n32RealRecover = (INT32) (n32ThisTimeRecove / 1000.0f);

		m_n32LastShouldRecMP = n32ThisTimeRecove;
		m_n32LastRecMP = n32RealRecover;
		if (IfHero()){
			if (!TryCastToHero()->GetHeroBornPos().IsZero()){
				if (GetCurPos().CanWatch(500, TryCastToHero()->GetHeroBornPos())){
					INT32 n32AddMP = GetFPData(eEffectCate_MaxMP) * 20 / 100.0f;
					n32RealRecover += n32AddMP;
				}
			}
		}
		if (0 < n32RealRecover){
			ChangeCurMP(this,GSToGC::HPMPChangeReason::RecoverHurt,n32RealRecover);	
		}
	}
	m_tLastRecoverTime = m_tRecoverTickCounter;

	return eNormal;
}

INT32	CSSGameUnit::FullHP(){
	return ChangeCurHP(this,GSToGC::HPMPChangeReason::RecoverHurt,GetFPData(eEffectCate_MaxHP) - GetFPData(eEffectCate_CurHP));
}

INT32	CSSGameUnit::FullMP(){
	return ChangeCurMP(this,GSToGC::HPMPChangeReason::RecoverHurt,GetFPData(eEffectCate_MaxMP) - GetFPData(eEffectCate_CurMP));
}

INT32	CSSGameUnit::ApplyHurt(CSSGameUnit *pcEnemyGO, INT32 n32HurtValue, EEffectCate hurtCate, INT32 &hurtValue, bool bIfNormalAtk , bool ifBlast ){
	if (IsDead() ||0 >= GetFPData(eEffectCate_CurHP)){
		return eEC_InvalidTargetActionState;
	}
	//计算基础护甲
	float fDef = 0;
	float fDefPassNum = 0;
	float fDefPassPercent = 0;
	float fDmgReduceNum = 0;
	float fDmgReducePercent = 0;
	if(hurtCate == eEffectCate_PhyHurt){
		fDef = GetFPData(eEffectCate_PhyDefense);
		fDefPassNum = GetFPData(eEffectCate_PhyPassNum);
		fDefPassPercent = GetFPData(eEffectCate_PhyPassPercent);
		fDmgReduceNum = GetFPData(eEffectCate_PhyDmgReduceNum);
		fDmgReducePercent = GetFPData(eEffectCate_PhyDmgReduce);
	}
	else if(hurtCate == eEffectCate_MagicHurt){
		fDef = GetFPData(eEffectCate_MagicDefense);
		fDefPassNum = GetFPData(eEffectCate_MagicPassNum);
		fDefPassPercent = GetFPData(eEffectCate_MagicPassPercent);
		fDmgReduceNum = GetFPData(eEffectCate_MagicDmgReduceNum);
		fDmgReducePercent = GetFPData(eEffectCate_MagicDmgReduce);
	}
	else{
		fDmgReduceNum = GetFPData(eEffectCate_TrueDmgReduceNum);
		fDmgReducePercent = GetFPData(eEffectCate_TrueDmgReduce);
	}
	//计算护甲穿透
	if(fDef >= 0){
		fDef = fDef * (1.0f - fDefPassPercent/1000.0f) - fDefPassNum;
	}
	else{
		fDef = fDef * (1.0f + fDefPassPercent/1000.0f) - fDefPassNum;
	}

	//计算护甲减伤
	if(fDef >= 0){
		fDef = 1.0f + fDef /100.0f;
	}
	else{
		fDef = (100 - fDef) / (100 - fDef*2);
	}
	

	//根据护甲、减伤，计算实际伤害值
	INT32 finalHurtValue = (n32HurtValue / fDef - fDmgReduceNum);
	if(finalHurtValue <= 0) finalHurtValue = 0;
	else finalHurtValue *= (1.0f - fDmgReducePercent/1000.0f);

	//如果没有造成伤害，则返回
	if(finalHurtValue <= 0) return eEC_NullPointer;

	if (NULL != pcEnemyGO){
		pcEnemyGO->SetCurAtkGUID(GetObjGUID());
		this->SetCurEnemyGUID(pcEnemyGO->GetObjGUID());
	}
	//如果是暴击伤害，需要发送消息给客户端
	if (TRUE == ifBlast){
		GSToGC::NotifyBlastHurt sNotifyBlastHurt;
		sNotifyBlastHurt.set_guid(GetObjGUID());
		sNotifyBlastHurt.set_blasthp(finalHurtValue);
		GetCurBattle()->BroadMsgToGC(sNotifyBlastHurt, sNotifyBlastHurt.msgid());
	}
 
	//改变生命值
	GSToGC::HPMPChangeReason eHPCR = GSToGC::HPMPChangeReason::NormalHurt;
	if(TRUE == bIfNormalAtk){
		if( TRUE == ifBlast){
			eHPCR = GSToGC::HPMPChangeReason::BloastHurt;
		}
		else{
			eHPCR = GSToGC::HPMPChangeReason::NormalHurt;
		}
	}
	else{
		eHPCR = GSToGC::HPMPChangeReason::SkillHurt;
	}
	hurtValue = finalHurtValue;
	ChangeCurHP(pcEnemyGO, eHPCR, -finalHurtValue);

	//添加被伤害被动
	PassiveSkillArr& passiveVec = GetPassiveSkillVec(EPassiveSkillTriggerType_Hurted);
	for (INT32* pEle = passiveVec.Begin(); pEle != passiveVec.End(); pEle = passiveVec.Next()){
		GetCurBattle()->GetPassiveSkillMgr()->Trigger(*pEle,(CSSGameUnit*)pcEnemyGO,EPassiveSkillTriggerType_Hurted,n32HurtValue);
	}
	//添加对敌人造成了伤害被动
	if(pcEnemyGO){
		if (eHPCR == GSToGC::HPMPChangeReason::NormalHurt || eHPCR == GSToGC::HPMPChangeReason::BloastHurt){
			pcEnemyGO->OnPassitiveSkillCalled(EPassiveSkillTriggerType_NormalAttackHurt, this, n32HurtValue);
		}
		else if (eHPCR == GSToGC::HPMPChangeReason::SkillHurt){
			pcEnemyGO->OnPassitiveSkillCalled(EPassiveSkillTriggerType_SkillHurt,this,n32HurtValue);
		}
	}   
	return eNormal;
}


INT32	CSSGameUnit::ChangeCurMP(CSSGameUnit *pcReasonGO, GSToGC::HPMPChangeReason eMPCR, INT32 n32ChangeValue){
	m_sFpMgr.ChangeMP(n32ChangeValue,eMPCR);
	return eNormal;
}
 
INT32	CSSGameUnit::BeginAction_Controled(bool bIfNotifyGC, CSSGameUnit *pcReasonGO){
	m_sCurOASI.eOAS = eOAS_Controlled;
	m_sCurOASI.tASUTCMilsec = m_pCSSBattleMgr->GetHeartBeatUTCMilsec();
	GetCurBattle()->SyncState(this);
	return eNormal;
}

INT32	CSSGameUnit::EndAction_Controled(bool bIfNotifyGC, CSSGameUnit *pcReasonGO){
	if(m_sCurOASI.eOAS == eOAS_Controlled){
		BeginAction_Free(bIfNotifyGC);
		m_sCurOASI.tASUTCMilsec = m_pCSSBattleMgr->GetHeartBeatUTCMilsec();
	}
	return eNormal;
}

INT32	CSSGameUnit::BeginAction_Reliving(bool bIfNotifyGC, CSSGameUnit *pcReasonGO){
	m_sCurOASI.eOAS = eOAS_Reliving;
	m_sCurOASI.tASUTCMilsec = m_pCSSBattleMgr->GetHeartBeatUTCMilsec();
	if (bIfNotifyGC){
		GetCurBattle()->SyncState(this);
	}
	return eNormal;
}

INT32	CSSGameUnit::EndAction_Reliving(bool bIfNotifyGC, CSSGameUnit *pcReasonGO){
	if(m_sCurOASI.eOAS == eOAS_Reliving){
		BeginAction_Free(bIfNotifyGC);
		m_sCurOASI.tASUTCMilsec = m_pCSSBattleMgr->GetHeartBeatUTCMilsec();
	}
	return eNormal;
}

EHateLevel	CSSGameUnit::GetHateLevel(CSSGameUnit& sGO){
	CSSGameUnit *pBeAtkOurTeamGO = sGO.GetCurBattle()->GetGameUnitByGUID(sGO.GetCurAtkGUID());

	if (IfNPC_Building()){
		if (NULL != pBeAtkOurTeamGO){
			if (pBeAtkOurTeamGO->IfHero()){
				if (sGO.IfHero()){
					return eHateLevel_First;
				}

				if (sGO.IfNPC()){
					return eHateLevel_Second;
				}
			}
		}

		if (sGO.IfNPC()){
			return eHateLevel_Third;
		}

		if (sGO.IfHero()){
			return eHateLevel_Fourth;
		}
	}
	else{
		if (NULL != pBeAtkOurTeamGO){
			if (pBeAtkOurTeamGO->IfHero()){
				if (sGO.IfHero()){
					return eHateLevel_First;
				}
					
				if (sGO.IfSolider()){
					return eHateLevel_Second;
				}
			}
			
			/*
			if (pBeAtkOurTeamGO->IfSolider()){
				if (sGO.IfSolider()){
					return eHateLevel_Third;
				}

				if (sGO.IfNPC_Building()){
					return eHateLevel_Fourth;
				}

				if (sGO.IfHero()){
					return eHateLevel_Fifth;
				}
			}
			*/
		}

		if (sGO.IfSolider() || sGO.IfWildNPC()){
			return eHateLevel_Sixth;
		}

		if (sGO.IfNPC_Building()){
			return eHateLevel_Seventh;
		}

		if (sGO.IfHero()){
			return eHateLevel_Eighth;
		}
	}

	return eHateLevel_End;
}

INT32 CSSGameUnit::ChangeFPData(EEffectCate eEffectCate, INT32 n32ChangeNum, INT32 n32Persent, bool ifAdd, INT32 n32UniqueID){
	if (0 == n32ChangeNum && 0 == n32Persent){
		return eNormal;
	}

	if(ifAdd){
		if(n32ChangeNum != 0){
			m_sFpMgr.AddBaseValue(eEffectCate,n32ChangeNum,n32UniqueID);
		}
		if(n32Persent != 0){
			m_sFpMgr.AddPercentValue(eEffectCate,n32Persent,n32UniqueID);
		}
	}
	else{
		if(n32ChangeNum != 0){
			m_sFpMgr.RemoveBaseValue(eEffectCate,n32ChangeNum,n32UniqueID);
		}
		if(n32Persent != 0){
			m_sFpMgr.RemovePercentValue(eEffectCate,n32Persent,n32UniqueID);
		}
	}

	return eNormal;
}

bool CSSGameUnit::CanAddBuf(){
	return m_BufArr.GetCurSize() < m_BufArr.GetMaxSize();
}

INT32 CSSGameUnit::GetFPData(EEffectCate eEffectCate){
	return m_sFpMgr.GetValue(eEffectCate);
}

INT32 CSSGameUnit::AddBuf(UINT32 un32BufTypeId){
	if (un32BufTypeId == 0){
		return eNormal;
	}
	m_BufArr.AddElement(un32BufTypeId);
	return eNormal;
}

INT32 CSSGameUnit::RemoveBuf(INT32 n32BufProjectId){
	if (0 == n32BufProjectId){
		return eNormal;
	}
	m_BufArr.RemoveElement(n32BufProjectId);
	return eNormal;
}

INT8	CSSGameUnit::GetHeroKillState(CSSGameUnit * pcReasonGO){
	if( pcReasonGO && pcReasonGO->IfHero()) {
		CSSHero *pcHero = (CSSHero *)pcReasonGO;
		pcHero->SetHeroKillState();
		return  pcHero->GetHeroKillState(); 
	} 

	if (pcReasonGO && pcReasonGO->IfNPC()){
		CSSNPC *pNPC = (CSSNPC*)pcReasonGO;
		if (NULL == pNPC){
			return eNormalKill;
		}
		if(pcReasonGO->GetFatherHeroGUID() > 0){
			CSSHero *pcHero = (CSSHero*)pcReasonGO->GetCurBattle()->GetGameUnitByGUID(pcReasonGO->GetFatherHeroGUID()); 
			pcHero->SetHeroKillState();
			return  pcHero->GetHeroKillState(); 
		}
	}
	return eNormalKill; 
}

INT32	CSSGameUnit::NotifyKillHeroMsgToGC(SGUID_KEY& killerGUID, CSSHero* pOtherHero){
	if (NULL == pOtherHero){
		return eNormal;
	}

	pOtherHero->AddSSendData_HeroKills();

	GSToGC::HeroKills sHeroKills;
	sHeroKills.set_guid(killerGUID);
	sHeroKills.set_kills(pOtherHero->GetSSendData().un32HeroKills);
	ELOG(LOG_SpecialDebug, "User Kills:%d.", pOtherHero->GetSSendData().un32HeroKills);
	pOtherHero->GetCurBattle()->BroadMsgToGC(sHeroKills, sHeroKills.msgid());	

	//log 
	if(pOtherHero && !pOtherHero->IfAI()&& !pOtherHero->GetCurBattle()->IsGuideBattle()){
		stringstream mystream; 
		mystream<< ((GetUTCMiliSecond() - pOtherHero->GetCurBattle()->GetBattleStateStartTime()) /1000) <<LOG_SIGN;
		mystream << ( (pOtherHero->GetUser() != NULL) ? pOtherHero->GetUser()->GetGUID() : 0 )<<LOG_SIGN; //0表示ai
		mystream << pOtherHero->GetObjType()<<LOG_SIGN;
		mystream <<pOtherHero->GetLevel()<<LOG_SIGN;
		mystream <<pOtherHero->GetSSendData().un32HeroKills;
		CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleHeroMounKill,pOtherHero->GetCurBattle()->GetBattleID(),pOtherHero->GetCurBattle()->GetBattleMapId(), mystream.str());
	} 
	return eNormal;
}

INT32	CSSGameUnit::AddPassiveSkill(EPassiveSkillTriggerType eType, INT32 n32UniqueId){
	m_mPassiveSkillMap[eType].AddElement(n32UniqueId);
	return eNormal;
}
 
INT32	CSSGameUnit::RemovePassiveSkill(EPassiveSkillTriggerType eType, INT32 n32UniqueId){
	m_mPassiveSkillMap[eType].RemoveElement(n32UniqueId);
	return eNormal;
}

PassiveSkillArr& CSSGameUnit::GetPassiveSkillVec(EPassiveSkillTriggerType eType){
	return m_mPassiveSkillMap[eType];
}
void CSSGameUnit::OnPassitiveSkillHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	OnPassitiveSkillCalled(EPassiveSkillTriggerType_HeartBeat,this);
}

void CSSGameUnit::OnPassitiveSkillCalled(EPassiveSkillTriggerType type, CSSGameUnit* pTarget, INT32 para1, INT32 para2, INT32 para3){
	PassiveSkillArr& passiveVecBefore = GetPassiveSkillVec(type);
	for (INT32* pEle = passiveVecBefore.Begin(); pEle != passiveVecBefore.End(); pEle = passiveVecBefore.Next()){
		GetCurBattle()->GetPassiveSkillMgr()->Trigger(*pEle, pTarget, type, para1, para2, para3);
	}
}
 

INT32 CSSGameUnit::GetNowMoveSped(){
	if(GetFPData(eEffectCate_Dizziness) > 0 
		|| GetFPData(eEffectCate_Restrain) > 0){
			return 0;
	}
	else{
		return GetFPData(eEffectCate_MoveSpeed);
	}
}
 
 
INT32	CSSGameUnit::CheckSummonGUNEndLife(ESummonFunc esf,CSSGameUnit * pcSelfG)
{  
	if (!IfNPC())
	{
		return eEC_InvalidObjTypeID;
	}
	CSSNPC *pNPC = (CSSNPC*)this;
	if ( pNPC && pNPC->GetNPCCfg() && ((pNPC->GetNPCCfg()->eNPCCateChild >= eNPCChild_Summon ) && (pNPC->GetNPCCfg()->eNPCCateChild < eNPCChild_Summon_End))  )
	{	 
		INT32 nLift  = pNPC->GetObjEndLife() - m_pCSSBattleMgr->GetHeartBeatUTCMilsec();
		if (nLift <= 0)
		{
			//pNPC->SetExpire(TRUE);
			pNPC->BeginAction_Dead(false,this);
			return eEC_TimeExpire; 
		}
		if (pcSelfG && (esf == eSummonFunc_One))
		{
			pNPC->SendSummonLifeMsg(esf, pcSelfG);  
		}  
	} 
	return eNormal;
}
 

google::protobuf::Message* CSSGameUnit::ConstructObjStateMsg(INT32& n32MsgID,bool spot){
	const SGOActionStateInfo &crsOASI = GetGOActionStateInfo();
	switch (crsOASI.eOAS){
	case eOAS_Free:
	case eOAS_Controlled:
		{
			GSToGC::FreeState* pMsg = new GSToGC::FreeState;
			pMsg->set_objguid(GetObjGUID());
			GSToGC::Pos* pos = new GSToGC::Pos;
			CSSBattle::SetPos(crsOASI.cPos, *pos);
			GSToGC::Dir* pDir = new GSToGC::Dir;
			CSSBattle::SetDir(crsOASI.cDir, *pDir);
			pMsg->set_allocated_pos(pos);
			pMsg->set_allocated_dir(pDir);
			n32MsgID = pMsg->msgid();
			return pMsg;
		}
		break;
	case eOAS_Running:
		{
			GSToGC::RunningState* pMsg = new GSToGC::RunningState;
			pMsg->set_objguid(GetObjGUID());
			GSToGC::Pos* pos = new GSToGC::Pos;
			CSSBattle::SetPos(crsOASI.cPos, *pos);
			GSToGC::Dir* pDir = new GSToGC::Dir;
			CSSBattle::SetDir(crsOASI.cDir, *pDir);
			pMsg->set_allocated_pos(pos);
			pMsg->set_allocated_dir(pDir);
			pMsg->set_movespeed(GetNowMoveSped());
			n32MsgID = pMsg->msgid();
			return pMsg;
		}
		break;
	case eOAS_PreparingSkill:
		{
			GSToGC::PrepareSkillState* pMsg = new GSToGC::PrepareSkillState;
			pMsg->set_objguid(GetObjGUID());
			GSToGC::Pos* pos = new GSToGC::Pos;
			CSSBattle::SetPos(crsOASI.cPos, *pos);
			GSToGC::Dir* pDir = new GSToGC::Dir;
			CSSBattle::SetDir(crsOASI.cDir, *pDir);
			pMsg->set_allocated_pos(pos);
			pMsg->set_allocated_dir(pDir);
			pMsg->set_targuid(crsOASI.sSkillTarGUID);
			pMsg->set_speed(GetNowMoveSped());
			pMsg->set_skillid(crsOASI.un32SkillID);
			n32MsgID = pMsg->msgid();
			return pMsg;
		}
		break;
	case eOAS_ReleasingSkill:
		{
			GSToGC::ReleasingSkillState* pMsg = new GSToGC::ReleasingSkillState;
			pMsg->set_objguid(GetObjGUID());
			GSToGC::Pos* pos = new GSToGC::Pos;
			CSSBattle::SetPos(crsOASI.cPos, *pos);
			GSToGC::Dir* pDir = new GSToGC::Dir;
			CSSBattle::SetDir(crsOASI.cDir, *pDir);
			pMsg->set_allocated_pos(pos);
			pMsg->set_allocated_dir(pDir);
			pMsg->set_targuid(crsOASI.sSkillTarGUID);
			n32MsgID = pMsg->msgid();
			pMsg->set_skillid(crsOASI.un32SkillID);
			return pMsg;
		}
		break;
	case eOAS_UsingSkill:
		{
			GSToGC::UsingSkillState* pMsg = new GSToGC::UsingSkillState;
			pMsg->set_objguid(GetObjGUID());
			GSToGC::Pos* pos = new GSToGC::Pos;
			CSSBattle::SetPos(crsOASI.cPos, *pos);
			GSToGC::Dir* pDir = new GSToGC::Dir;
			CSSBattle::SetDir(crsOASI.cDir, *pDir);
			pMsg->set_allocated_pos(pos);
			pMsg->set_allocated_dir(pDir);
			pMsg->set_targuid(crsOASI.sSkillTarGUID);
			n32MsgID = pMsg->msgid();
			pMsg->set_skillid(crsOASI.un32SkillID);
			return pMsg;
		}
		break;
	case eOAS_LastingSkill:
		{
			GSToGC::LastingSkillState* pMsg = new GSToGC::LastingSkillState;
			pMsg->set_objguid(GetObjGUID());
			GSToGC::Pos* pos = new GSToGC::Pos;
			CSSBattle::SetPos(crsOASI.cPos, *pos);
			GSToGC::Dir* pDir = new GSToGC::Dir;
			CSSBattle::SetDir(crsOASI.cDir, *pDir);
			pMsg->set_allocated_pos(pos);
			pMsg->set_allocated_dir(pDir);
			pMsg->set_targuid(crsOASI.sSkillTarGUID);
			n32MsgID = pMsg->msgid();
			pMsg->set_skillid(crsOASI.un32SkillID);
			return pMsg;
		}
		break;
	case eOAS_Reliving:
		{
			GSToGC::NotifyGameObjectReliveState* pMsg = new GSToGC::NotifyGameObjectReliveState;
			pMsg->set_guid(GetObjGUID());
			GSToGC::Pos* pos = new GSToGC::Pos;
			CSSBattle::SetPos(crsOASI.cPos, *pos);
			GSToGC::Dir* pDir = new GSToGC::Dir;
			CSSBattle::SetDir(crsOASI.cDir, *pDir);
			pMsg->set_allocated_pos(pos);
			pMsg->set_allocated_dir(pDir);
			n32MsgID = pMsg->msgid();
			return pMsg;
		}
		break;
	case eOAS_Dead:
		{
			GSToGC::DeadState* pMsg = new GSToGC::DeadState;
			pMsg->set_objguid(GetObjGUID());
			GSToGC::Pos* pos = new GSToGC::Pos;
			CSSBattle::SetPos(crsOASI.cPos, *pos);
			GSToGC::Dir* pDir = new GSToGC::Dir;
			CSSBattle::SetDir(crsOASI.cDir, *pDir);
			pMsg->set_allocated_pos(pos);
			pMsg->set_allocated_dir(pDir);
			pMsg->set_spot(spot);
			n32MsgID = pMsg->msgid();
			return pMsg;
		}
		break;
	}
	return NULL;
}

INT32 CSSGameUnit::BeginAction_Move(const CVector3D &crsDir, bool bIfNotifyGC){
	SetGOActionState(eOAS_Running);
	SetGOActionState_Dir(crsDir);
	m_sCurOASI.fDistMoved = 0; 
	if (bIfNotifyGC){
		CSSBattle *pcCurBattle = GetCurBattle();
		if (NULL != pcCurBattle){
			pcCurBattle->SyncState(this);
		}
	}
	return eNormal;
}

INT32 CSSGameUnit::BeginAction_Free(bool bIfNotifyGC){
	CSSBattle *pcCurBattle = GetCurBattle();
	if (eOAS_Free != m_sCurOASI.eOAS){
		SetGOActionState(eOAS_Free);
		if (bIfNotifyGC && NULL != pcCurBattle){
			pcCurBattle->SyncState(this);
		}
	}
	return eNormal;
}

INT32 CSSGameUnit::BeginAction_PrepareSkill(CSSSkill* pSkill, const CVector3D &crsDir, bool bIfNotifyGC){
	if (NULL == pSkill){
		return eEC_NullSkillData;
	}
	if(eOAS_PreparingSkill != GetGOActionStateInfo().eOAS){
		SetGOActionState(eOAS_PreparingSkill);
		SetGOActionState_SkillID(pSkill->cpsCfg->un32SkillID);
		SetGOActionState_SkillTarGUID(pSkill->pcTarGU==NULL? 0 : pSkill->pcTarGU->GetObjGUID());
		SetGOActionState_Dir(crsDir);
		GetCurBattle()->SyncState(this);
	}
	return eNormal;
}

INT32 CSSGameUnit::BeginAction_ReleaseSkill(CSSSkill* pSkill, const CVector3D &crsDir,bool bIfNotifyGC){
	if (NULL == pSkill){
		ELOG(LOG_WARNNING, "NULL == pSkill");
		return eEC_NullSkillData;
	}
	//if(eOAS_ReleasingSkill != GetGOActionStateInfo().eOAS){
		SetGOActionState(eOAS_ReleasingSkill);
		SetGOActionState_SkillID(pSkill->cpsCfg->un32SkillID);
		SetGOActionState_SkillTarGUID(pSkill->pcTarGU==NULL? 0:pSkill->pcTarGU->GetObjGUID());
		SetGOActionState_Dir(crsDir);
		GetCurBattle()->SyncState(this);
	//}
	return eNormal;
}

INT32 CSSGameUnit::BeginAction_UsingSkill(CSSSkill* pSkill, const CVector3D &crsDir,bool bIfNotifyGC){
	if (NULL == pSkill){
		ELOG(LOG_WARNNING, "NULL == pSkill");
		return eEC_NullSkillData;
	}
	if(eOAS_UsingSkill != GetGOActionStateInfo().eOAS){
		SetGOActionState(eOAS_UsingSkill);
		SetGOActionState_SkillID(pSkill->cpsCfg->un32SkillID);
		SetGOActionState_SkillTarGUID(pSkill->pcTarGU==NULL? 0:pSkill->pcTarGU->GetObjGUID());
		SetGOActionState_Dir(crsDir);
		GetCurBattle()->SyncState(this);
	}
	return eNormal;
}

INT32 CSSGameUnit::BeginAction_LastingSkill(CSSSkill* pSkill, const CVector3D &crsDir,bool bIfNotifyGC){
	if (NULL == pSkill){
		ELOG(LOG_WARNNING, "NULL == pSkill");
		return eEC_NullSkillData;
	}
	if(eOAS_LastingSkill != GetGOActionStateInfo().eOAS){
		SetGOActionState(eOAS_LastingSkill);
		SetGOActionState_SkillID(pSkill->cpsCfg->un32SkillID);
		SetGOActionState_SkillTarGUID(pSkill->pcTarGU==NULL? 0:pSkill->pcTarGU->GetObjGUID());
		SetGOActionState_Dir(crsDir);
		GetCurBattle()->SyncState(this);
	}
	return eNormal;
}

INT32 CSSGameUnit::CheckSyncMoveState(){
	const SGOActionStateInfo &crsOASI = GetGOActionStateInfo();
	TIME_MILSEC tCurUTCMilsec = m_pCSSBattleMgr->GetHeartBeatUTCMilsec();
	if (tCurUTCMilsec < m_tLastSyncMoveStateMilsec){
		m_tLastSyncMoveStateMilsec = tCurUTCMilsec;
	}
	TIME_TICK tSyncTickSpan = tCurUTCMilsec - m_tLastSyncMoveStateMilsec;
	if (5000 <= tSyncTickSpan && eOAS_Running == crsOASI.eOAS){
		GetCurBattle()->SyncState(this);
		m_tLastSyncMoveStateMilsec = tCurUTCMilsec;
	}

	return eNormal;
}

INT32 CSSGameUnit::CheckBeginAction_Free(bool bSyncToGC){
	if (eOAS_PassiveState <= GetGOActionStateInfo().eOAS){
		return eEC_MoveBlocked;
	}
	BeginAction_Free(bSyncToGC);
	return eNormal;
}

void CSSGameUnit::RemoveMoveEffect(UINT32 un32MoveID){
	if (m_MoveEffectID == un32MoveID && GetCurBattle()){
		GetCurBattle()->GetEffectMgr()->RemoveEffect(m_MoveEffectID);
		m_MoveEffectID = 0;
	}
}

void CSSGameUnit::SetMoveEffect(UINT32 un32MoveID, bool bIfActiveMove){
	if (m_MoveEffectID != 0){
		GetCurBattle()->GetEffectMgr()->RemoveEffect(m_MoveEffectID);
		m_MoveEffectID = un32MoveID;
		m_bIfActiveMove = bIfActiveMove;
	}
}

INT32 CSSGameUnit::DoChangeHP(INT32 n32ChangeValue, GSToGC::HPMPChangeReason eHPCR, CSSGameUnit *pcReasonGO,CSSGameUnit *deadGO){

	if (n32ChangeValue==0){
		return eEC_NullPointer;
	}

	INT32 oldHP = GetFPData(eEffectCate_CurHP);
	m_sFpMgr.ChangeHP(n32ChangeValue,eHPCR);
	CSSBattle *pcCurBattle = GetCurBattle();//Load会进来,没进场景//
	if (NULL == pcCurBattle){
		return eEC_NullPointer;
	}

	if (NULL != pcReasonGO && 0 == GetFPData(eEffectCate_CurHP)){
		INT32 hpChanged = GetFPData(eEffectCate_CurHP) - oldHP;
		if(hpChanged < 0){
			GetCurBattle()->AskStopMoveObjectAll(this);
			OnPassitiveSkillCalled(EPassiveSkillTriggerType_Die, pcReasonGO);
			pcReasonGO->OnPassitiveSkillCalled(EPassiveSkillTriggerType_TargetDie, this); 
	 
			if(TRUE == IfCanBeTarget()){
				BeginAction_Dead(true, pcReasonGO);
				return eEC_GUDead;
			}
		}
	}

	return eNormal;
}
/************************************************************************/
/* about sight func                                                        */
/************************************************************************/

float CSSGameUnit::GetSightX()
{
	return GetCurPos().m_fX;
}
float CSSGameUnit::GetSightY()
{
	return GetCurPos().m_fZ;
}

INT32	CSSGameUnit::SendMsgToWatchersByType(ESysSightType est, google::protobuf::Message& sMsg, int n32MsgID, vector<CSSUser*>& pUserVec, TIME_MILSEC dealyTime){
	switch(est){
	case eSightType_SelfAndOB:
	case eSightType_All: 
		GetCurBattle()->SendMsgToWatchers(sMsg, n32MsgID, pUserVec, dealyTime); 
		break;
	case eSightType_OB:
		break;
	}
	return eNormal;
}

void CSSGameUnit::SendAppearMsg(std::vector<CSSUser*>& cTempUserMap, UINT32 objTypeID){
	GSToGC::GOAppear sAppear;
	GSToGC::GOAppear_AppearInfo* pInfo = sAppear.add_info();
	pInfo->set_objguid(GetObjGUID());
	pInfo->set_masterguid(GetMasterGUID());
	pInfo->set_camp(GetCampID());
	GSToGC::Dir* dir = new GSToGC::Dir;
	CSSBattle::SetDir(GetCurDir(), *dir);
	pInfo->set_allocated_dir(dir);
	GSToGC::Pos* pos = new GSToGC::Pos;
	CSSBattle::SetPos(GetCurPos(), *pos);
	pInfo->set_allocated_pos(pos);
	pInfo->set_hp(GetCurHP());
	pInfo->set_mp(GetCurMP());
	pInfo->set_obj_type_id(objTypeID);

	SendMsgToWatchersByType(eSightType_All, sAppear, sAppear.msgid(), cTempUserMap);
#if 0
	if (this->IfNPC())
	{
		//hold NPC ICO
		((CSSNPC*)this)->CheckGUnitNPCICO(eAltarBornNPCType_Add, cTempUserMap); 
	} 
#endif
}

void CSSGameUnit::SendHPMPMsg(std::vector<CSSUser*>& cTempUserMap){
	GSToGC::NotifyHPInfo sNotifyHPInfo;
	GSToGC::NotifyHPInfo_HPInfo* pInfo = sNotifyHPInfo.add_hpinfo();
	pInfo->set_guid(GetObjGUID());
	pInfo->set_curhp(GetFPData(eEffectCate_CurHP));
	pInfo->set_maxhp(GetFPData(eEffectCate_MaxHP));
	SendMsgToWatchersByType(eSightType_All, sNotifyHPInfo, sNotifyHPInfo.msgid(), cTempUserMap);

	GSToGC::NotifyMPInfo sNotifyMPInfo;
	GSToGC::NotifyMPInfo_MPInfo* pMpInfo = sNotifyMPInfo.add_mpinfo();
	pMpInfo->set_curmp(GetFPData(eEffectCate_CurMP));
	pMpInfo->set_maxmp(GetFPData(eEffectCate_MaxMP));
	pMpInfo->set_guid(GetObjGUID());
	SendMsgToWatchersByType(eSightType_All, sNotifyMPInfo, sNotifyMPInfo.msgid(), cTempUserMap);
}
 
  
 
/************************************************************************/
/* about move func                                                      */
/************************************************************************/

void CSSGameUnit::OnMoveBlock(){
	if (eOAS_PassiveState > GetGOActionStateInfo().eOAS){
		CheckBeginAction_Free(TRUE);
	}
	if(m_moveHolder != NULL){
		m_moveHolder->OnStopMove();
	}
	m_pAI->OnMoveBlock();
}

void CSSGameUnit::OnStartMove(ColVector dir){ 
	CVector3D cDir; 
	cDir.m_fX = dir.x; cDir.m_fZ = dir.y; 
	if (eOAS_PassiveState > GetGOActionStateInfo().eOAS) BeginAction_Move(cDir,TRUE);
	OnPassitiveSkillCalled(EPassiveSkillTriggerType_Move, this);
}


void CSSGameUnit::SendDisppearMsg(std::vector<CSSUser*>& cTempUserMap){
	GSToGC::DisappearInfo sMsg;
	sMsg.add_guid(GetObjGUID());

	INT32 delaytime = 0;
	if (GetGOActionStateInfo().eOAS == eOAS_Dead){
		delaytime = 3000;
	}
	SendMsgToWatchersByType(eSightType_All, sMsg, sMsg.msgid(), cTempUserMap, delaytime);
 
	if (this->IfNPC())
	{
		//hold NPC ICO
		((CSSNPC*)this)->CheckGUnitNPCICO(eAltarBornNPCType_Del, cTempUserMap); 
	} 
 
}

void CSSGameUnit::SetBattleMgr(CSSBattleMgr* pMgr){
	m_pCSSBattleMgr = pMgr;
}

string  EGOActionStateStr[] = {
	"eOAS_Free",
	"eOAS_Running",
	"eOAS_PreparingSkill",
	"eOAS_ReleasingSkill",
	"eOAS_PassiveState",
	"eOAS_Controlled",
	"eOAS_Reliving",
	"eOAS_Dead",
	"eOAS_End",
};
string CSSGameUnit::GetDebugString(){
	stringstream debugStrem;
	debugStrem << "GUID:" << m_sObjGUID << "; state:" << EGOActionStateStr[m_sCurOASI.eOAS].c_str();
	return debugStrem.str();
}

CSSNPC* CSSGameUnit::TryCastToNPC(){
	if (IfNPC()){
		return (CSSNPC *)this;
	}
	return NULL;
}

CSSHero* CSSGameUnit::TryCastToHero(){
	if (IfHero()){
		return (CSSHero*)this;
	}
	return NULL;
}

//npc log:如果是输出方是英雄，记录输出值
void	CSSGameUnit::CelObjHurtLog(CSSGameUnit *pcEnemyGO,INT32 skillId, INT32 n32HurtValue,  EEffectCate ec)
{ 
	CSSHero  *pHero = NULL;
	if ( pcEnemyGO && IfNPC() && pcEnemyGO->IfHero()  )
	{ 
		 pHero = (CSSHero*)pcEnemyGO;
		if ( pHero->IfAI())
		{
			return;
		}
		if (eEffectCate_MagicHurt == ec) //魔法 || eHPCR == GSToGC::HPMPChangeReason::BloastHurt   )
		{
			pHero->GetSSendData().un32SkillMagicSendHurt += n32HurtValue > 0 ? n32HurtValue : -n32HurtValue; 
		} 
		else if (eEffectCate_PhyHurt == ec)//物理
		{
			pHero->GetSSendData().un32SkillPhySendHurt += n32HurtValue > 0 ? n32HurtValue : -n32HurtValue; 
		}
		else 
		{
			pHero->GetSSendData().un32RealSendHurt += n32HurtValue > 0 ? n32HurtValue : -n32HurtValue; 
		}
		pHero->GetSSendData().un32SendHurt += n32HurtValue > 0 ? n32HurtValue : -n32HurtValue; 
	} 
	else if (this->IfHero() )
	{
		 pHero = (CSSHero*)this;
		if (pHero->IfAI())
		{
			return;
		}
		if (eEffectCate_MagicHurt == ec) //魔法 || eHPCR == GSToGC::HPMPChangeReason::BloastHurt   )
		{
			pHero->GetSSendData().un32RevSkillMagicHurt += n32HurtValue > 0 ? n32HurtValue : -n32HurtValue; 
		} 
		else if (eEffectCate_PhyHurt == ec)//物理
		{
			pHero->GetSSendData().un32RevSkillPhyHurt += n32HurtValue > 0 ? n32HurtValue : -n32HurtValue; 
		}
		else 
		{
			pHero->GetSSendData().un32RevRealHurt += n32HurtValue > 0 ? n32HurtValue : -n32HurtValue; 
		}
		pHero->GetSSendData().un32RevHurt += n32HurtValue > 0 ? n32HurtValue : -n32HurtValue; 
	}

	if (pHero && !pHero->IfAI() )
	{
		CSSGameLogMgr::GetInstance().AddSkillHurt(this->GetCurBattle()->GetBattleMapId(),this->GetCurBattleID(),pHero->GetUser()->GetGUID(),pcEnemyGO->GetObjType(),skillId,n32HurtValue,ec);
	}
}
void		CSSGameUnit::CelObjDeadLog(CSSGameUnit *pcEnemyGO)
{
	if (!pcEnemyGO->IfHero())
	{
		return;
	}
	CSSHero *pHero = (CSSHero*)pcEnemyGO;
	if (pHero && pHero->IfAI())
	{
		return;
	}
	if (IfNPC())
	{
		CSSNPC *pNPC = (CSSNPC*)this;
		if (pNPC->GetBelongPoint() > 0)
		{
			pHero->GetSSendData().un32KillMonster += 1; 
		}
		else
		{
			pHero->GetSSendData().un32KillSolder += 1; 
		}
	}
}

bool CSSGameUnit::IfInReleaseSkillRange(CSSGameUnit* sTarget, const SSNewSkillCfg* pCfg,float addedDist){
	if(NULL == pCfg){
		return FALSE;
	}
	if (sTarget){
		if(pCfg->bIfNomalAttack){
			//return GetCurPos().CanWatch(pCfg->fReleaseDist + sTarget->GetObjCollideRadius() + addedDist, sTarget->GetCurPos());
			return GetCurPos().CanWatch(GetFPData(eEffectCAte_AttackDist) + sTarget->GetObjCollideRadius() + addedDist, sTarget->GetCurPos());
		}
		else{
			return GetCurPos().CanWatch(pCfg->fReleaseDist + addedDist, sTarget->GetCurPos());
		}
	}

	return TRUE;
}
}