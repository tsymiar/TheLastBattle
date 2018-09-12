#include "SSAI_Hero.h"
#include "SSGameUnit.h"
#include "SSBattle.h"
#include "SSSkill.h"
#include "SSNPC.h"
#include "SSHero.h"
#include "SSCfgMgr.h"
#include "SSUser.h"
#include "log/SSGameLogMgr.h"

namespace SceneServer{

CSSAI_Hero::CSSAI_Hero(CSSGameUnit* pGU):CSSAI(pGU), m_ifAutoAttack(FALSE)
, m_pNowSkill(NULL)
, m_pWantUseSkill(NULL)
, m_nextSkill(NULL)
, m_ifAbsorb(FALSE)
, m_ifMoveDir(FALSE)
, m_pcAbsorbNPC(NULL)
, m_ifStandAttack(FALSE)
{
	if (m_pcMasterGU){
		m_pcHeroGU = m_pcMasterGU->TryCastToHero();
	}
	m_bIfFliter = TRUE;
}

CSSAI_Hero::~CSSAI_Hero(){
	m_pcHeroGU = NULL;
	m_pNowSkill = NULL;
	m_pWantUseSkill = NULL;
}

INT32 CSSAI_Hero::AskMoveDir(const CVector3D &crcDir){
	//检查是否是不能操作状态
	if(TRUE == IfPassitiveState()){
		return eEC_AbsentOrderPriority;
	}
	//尝试停止所有技能
	if(FALSE == StopAllSkill()){
		if(m_pNowSkill == NULL || FALSE == m_pNowSkill->IfMasterMoveAble()){
			return eEC_AbsentOrderPriority;
		}
	}
	//停止自动攻击
	m_ifAutoAttack = FALSE;
	StopAttack();
	//停止吸附
	TryCancleAbsorb();
	//请求移动
	if(FALSE == m_pcHeroGU->GetCurBattle()->AskStartMoveDir(m_pcHeroGU, crcDir)){
		m_pcHeroGU->BeginAction_Free(TRUE);
		return eEC_AbsentOrderPriority;
	}
	//停止站立攻击
	m_ifStandAttack = FALSE;
	m_ifMoveDir = TRUE;
	m_ifMoving = TRUE;
	return eNormal;
}

INT32 CSSAI_Hero::AskMoveTar(const CVector3D &crcPos){
	//检查是否是不能操作状态
	if(TRUE == IfPassitiveState()){
		return eEC_AbsentOrderPriority;
	}
	//尝试停止所有技能
	if(FALSE == StopAllSkill()){
		if(m_pNowSkill == NULL || FALSE == m_pNowSkill->IfMasterMoveAble()){
			return eEC_AbsentOrderPriority;
		}
	}
	//停止自动攻击
	m_ifAutoAttack = FALSE;
	//停止自动追击施放技能
	StopWantUseSkill();
	//停止自动攻击
	StopAttack();
	//停止吸附
	TryCancleAbsorb();
	//停止手动移动
	m_ifMoveDir = FALSE;
	//请求移动
	if(FALSE == m_pcHeroGU->GetCurBattle()->AskStartMoveToTar(m_pcHeroGU,crcPos,TRUE,TRUE)){
		m_pcHeroGU->BeginAction_Free(TRUE);
		return eEC_AbsentOrderPriority;
	}
	//停止站立攻击
	m_ifStandAttack = FALSE;
	m_ifMoving = TRUE;
	return eNormal;
}

INT32 CSSAI_Hero::AskStopMove(){
	//检查是否是不能操作状态
	if(TRUE == IfPassitiveState()){
		return eEC_AbsentOrderPriority;
	}
	m_pcHeroGU->GetCurBattle()->AskStopMoveObjectDir(m_pcHeroGU);
	m_ifMoveDir = FALSE;

	return eNormal;
}

INT32 CSSAI_Hero::AskStartAutoAttack(){
	//检查是否不能控制中
	if(TRUE == IfPassitiveState()){
		return eNormal;
	}
	//如果已经是自动攻击了，直接返回
	if(m_ifAutoAttack){
		return eNormal;
	}
	//检查是否在手动移动中。如果是，拒绝启动自动攻击
	if(TRUE == m_ifMoveDir){
		return eNormal;
	}
	//检查是否正在吸附中。如果是，拒绝启动自动攻击
	if(m_ifAbsorb){
		return eNormal;
	}
	//检查是否被缴械
	if (m_pcMasterGU->GetFPData(eEffectCate_Restrain) > 0){
		return eNormal;
	}
	//检查其他不能攻击的情况
	INT32 rst = CheckAttackTarget();
	if(eNormal != rst){
		return rst;
	}
	//尝试停止所有技能
	if(FALSE == StopAllSkill()){
		return eEC_AbsentOrderPriority;
	}
	
	//停止手动移动
	m_ifMoveDir = FALSE;
	//停止站立攻击
	m_ifStandAttack = FALSE;
	//初始化数据
	m_ifAutoAttack = TRUE;
	m_eAttackState = eAttackState_Pursue;
	m_tLastCheckMoveTarTime = 0;

	//开始进行自动攻击
	AttackHeartBeat(GetUTCMiliSecond(),0);

	return eNormal;
}

INT32 CSSAI_Hero::AskUseSkill(UINT32 un32SkillID){
	//检查是否是不能操作状态
	if(TRUE == IfPassitiveState()){
		return eEC_AbsentOrderPriority;
	}
	//检查是否被沉默了
	if(m_pcHeroGU->GetFPData(eEffectCate_Silence) > 0){
		return eEC_UseSkillFailForSilenced;
	}
	//检查是否在吸附中，如果是，不能使用技能
	if(m_ifAbsorb){
		return eEC_AbsentOrderPriority;
	}
	//获取并检查技能是否存在
	CSSSkill* pSkill = m_pcHeroGU->GetSkillByID(un32SkillID);
	if(NULL == pSkill){
		return eEC_CanNotFindTheSkill;
	}

	//检查技能是否符合可用条件
	INT32 rst = pSkill->IfSkillUsable();
	if(eNormal != rst){
		return rst;
	}
	//检查是否是正在运行的技能
	if(m_pNowSkill != NULL && m_pNowSkill->cpsCfg->un32SkillID == pSkill->cpsCfg->un32SkillID){
		return eEC_AbsentOrderPriority;
	}
	//首先判断是否在施放技能中。如果在施放技能中，则后面的技能都进行记录，等待施放
	if(IfUsingSkill() && (TRUE == m_pNowSkill->IfSkillBeforeHit() || m_pNowSkill->GetSkillState() == eSkillState_Using)){
		//前摇和引导状态，需要将当前技能记忆下来
		m_nextSkill = pSkill;
		return eEC_AbsentOrderPriority;
	}
	else{
		//先检测技能是否够距离施放	
		rst = pSkill->IfSkillUsableWithNowTarget();
		if(eEC_AbsentSkillDistance == rst){
			//如果技能射程不够，则启动自动追踪释放技能功能

			//停止自动攻击
			m_ifAutoAttack = FALSE;
			//停止手动移动
			m_ifMoveDir = FALSE;
			//停止站立攻击
			m_ifStandAttack = FALSE;
			//设置属性
			m_pWantUseSkill = pSkill;
			m_pcMoveTarPos = m_pWantUseSkill->pcTarGU->GetCurPos();
			m_tLastCheckMoveTarTime = GetUTCMiliSecond();
			MoveToTar(m_pcMoveTarPos,FALSE,m_tLastCheckMoveTarTime);
			return eNormal;
		}
		else if(eNormal != rst){
			return rst;
		}
		else{
			//检测是否是瞬发技能。如果是瞬发技能，则不需要打断当前的位移
			if(FALSE == pSkill->IfImpactSkill() || 
				(pSkill->cpsCfg->n32SkillLastTime > 0 && IfMoving() == FALSE && m_ifAutoAttack == FALSE && m_pAttackSkill->ifRunning == FALSE)
				){
				//如果是非瞬发技能，则停止相关的自动攻击，位移等
				//停止自动攻击
				CancleAttack();
				//停止移动
				m_ifMoveDir = FALSE;
				m_pcHeroGU->GetCurBattle()->AskStopMoveObjectAll(m_pcHeroGU);
				//开始使用技能，停止其他技能
				StopAllSkill();
				//需要有心跳，保存技能
				m_pNowSkill = pSkill;
				//停止自动攻击和站立攻击
				if(pSkill->IfHasPreTime() == FALSE){
					//没有前摇的非瞬发技能(现阶段就是引导技能)才需要停止自动攻击
					m_ifAutoAttack = FALSE;
				}
				m_ifStandAttack = FALSE;
			}
			//开始使用技能
			pSkill->Start();
			return rst;
		}
	}
	
}

INT32 CSSAI_Hero::AskAbsorbMonster(INT32 n32ToRemoveIndex){
	//检查是否是不能操作状态
	if(TRUE == IfPassitiveState()){
		return eEC_AbsentOrderPriority;
	}
	//检查吸附对象合法性
	CSSGameUnit* pTempGU = m_pcHeroGU->GetCurBattle()->GetGameUnitByGUID(m_pcHeroGU->GetLockedTargetGUID());
	if (NULL == pTempGU || FALSE == pTempGU->IfWildNPC() || pTempGU->IsDead()){
		return eEC_NULLNPC;
	}
	m_pcAbsorbNPC = pTempGU->TryCastToNPC();
	//检查是否可以吸附
	if (FALSE == m_pcAbsorbNPC->TryCastToNPC()->GetNPCCfg()->bIfCanControl){
		return eEC_AbsorbMonsterFailForMonsterCannotBeConstrol;
	}
	//检查敌我关系
	if (false == CFunction::IfEnemy(m_pcAbsorbNPC->GetCampID(),m_pcHeroGU->GetCampID())){
		return eEc_InvalidAbsorbTar;
	}
	//检查吸附等级
	if (m_pcHeroGU->GetLevel() < c_un32HeroMinAbsorbLevel){
		return eEC_AbsorbMonsterFailForLackLevel;
	}
	//检查吸附格子的合法性
	if (n32ToRemoveIndex > 1 || n32ToRemoveIndex < -1){
		return eEC_NullHero;
	}
	//检查是否已经吸附了重复的野怪
	for (INT32 i = 0; i < c_n32MaxAbsorbSkillNum; ++i){
		if (m_pcAbsorbNPC->TryCastToNPC()->GetNPCCfg()->eOT == m_pcHeroGU->GetAbsorbMonsterID(i)){
			return eEC_AbsorbMonsterFailForHasSameSkillID;
		}
	}
	//检查金钱
	if (m_pcHeroGU->GetCP(eCPCP_Personnal) < GetAbsorbCost(m_pcAbsorbNPC->TryCastToNPC())){
		return eEC_AbsorbMonsterFailForLackCP;
	}
	//尝试停止所有技能
	if(FALSE == StopAllSkill()){
		return eEC_AbsentOrderPriority;
	}
	//停止自动攻击
	if(m_ifAutoAttack){
		CancleAttack();
	}
	//停止手动移动
	m_ifMoveDir = FALSE;
	m_pcHeroGU->GetCurBattle()->AskStopMoveObjectDir(m_pcHeroGU);
	m_pcHeroGU->GetCurBattle()->AskStopMoveObjectTar(m_pcHeroGU);
	//初始化数据
	m_n32ToRemoveAbsorbIndex = n32ToRemoveIndex;
	m_pcHeroGU->BeginAction_Free(TRUE);
	m_tStartAbsorbTime = GetUTCMiliSecond();
	m_ifAbsorb = TRUE;
	//停止吸附对象的AI
	m_pcAbsorbNPC->TryCastToNPC()->AskStopAI();
	//发送开始吸附消息给客户端
	GSToGC::AbsorbBegin sAbsorbBegin;
	sAbsorbBegin.set_guid(m_pcHeroGU->GetObjGUID());
	sAbsorbBegin.set_monsterguid(m_pcAbsorbNPC->GetObjGUID());
	m_pcHeroGU->GetCurBattle()->SyncMsgToGC(sAbsorbBegin, sAbsorbBegin.msgid(), m_pcHeroGU->GetObjGUID(), m_pcHeroGU, NULL);
	 
	return eNormal;
}

INT32	CSSAI_Hero::AskGasExpersion(){
	//检查是否是不能操作状态
	if(TRUE == IfPassitiveState()){
		return eEC_AbsentOrderPriority;
	}
	//检查是否被沉默了
	if(m_pcHeroGU->GetFPData(eEffectCate_Silence) > 0){
		return eEC_StateCanNotUseGas;
	}

	INT32 rst = m_pcHeroGU->GasExpersion();
	if(eNormal == rst){
		//爆气成功，需要打断技能寻路
		StopWantUseSkill(TRUE);
	}
	//log
	if(!m_pcHeroGU->IfAI() && !m_pcHeroGU->GetCurBattle()->IsGuideBattle()){
		stringstream mystream;
		mystream << ( ( GetUTCMiliSecond() - m_pcHeroGU->GetCurBattle()->GetBattleStateStartTime() ) / 1000) << LOG_SIGN;
		mystream << m_pcHeroGU->GetUser()->GetGUID();
		CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleExpersion,m_pcHeroGU->GetCurBattleID(),m_pcHeroGU->GetCurBattle()->GetBattleMapId(),mystream.str());
	}
	return rst;
}

void	CSSAI_Hero::TryCancleAbsorb(){
	if(m_ifAbsorb){
		m_pcHeroGU->SynAbsobInfoToGC(0, 0, 0);
		if(m_pcAbsorbNPC != NULL){
			m_pcAbsorbNPC->TryCastToNPC()->AskResumeAI();
			m_pcAbsorbNPC = NULL;
		}
		m_ifAbsorb = FALSE;
	}
}

INT32	CSSAI_Hero::GetAbsorbCost(CSSNPC *sspNPC){
	if (0 < sspNPC->GetBelongAltar()){
		return sspNPC->GetNPCCfg()->n32ConsumeCP * 2;	
	}
	return sspNPC->GetNPCCfg()->n32ConsumeCP;
}

INT32 CSSAI_Hero::AbsorbHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	//检查状态
	if (eOAS_Free != m_pcHeroGU->GetGOActionStateInfo().eOAS){
		return eEC_AbsorbMonsterFailForHeroDead;
	}
	if (m_pcHeroGU->GetFPData(eEffectCate_Dizziness)){
		return eEC_AbsorbMonsterFailForDizziness;
	}
	//检查是否切换了对象
	if (m_pcAbsorbNPC == NULL || m_pcHeroGU->GetLockedTargetGUID() != m_pcAbsorbNPC->GetObjGUID()){
		return eEC_AbsorbMonsterFailForDiffNPC;
	}
	//对象状态
	if (m_pcAbsorbNPC->IsDead()){
		return eEC_AbsorbMonsterFailForMonsterDead;
	}
	//检查吸附时间是否到了
	if (tUTCMilsec - m_tStartAbsorbTime >= c_n32AbsorbTimeout){
		//检查金钱
		if (m_pcHeroGU->GetCP(eCPCP_Personnal) < GetAbsorbCost(m_pcAbsorbNPC->TryCastToNPC())){
			return eEC_AbsorbMonsterFailForLackCP;
		}
		//获得吸附目标对应的技能
		const SSNewSkillCfg* cpsCfg = (SSNewSkillCfg*)(CSSCfgMgr::GetSSCfgInstance().GetSkillFromOriSkillAndNewLevel(m_pcAbsorbNPC->TryCastToNPC()->GetNPCCfg()->aun32SkillList[1] , m_pcHeroGU->GetLevel()));
		if (NULL == cpsCfg){
			return eEC_NULLCfg;
		}
		//计算需要移除的格子
		INT32	newAbsorbSkillIndex = -1;
		if (0 == m_pcHeroGU->GetAbsorbMonsterID(0)){
			m_n32ToRemoveAbsorbIndex = -1;
			newAbsorbSkillIndex = 0;
		}
		else if (0 == m_pcHeroGU->GetAbsorbMonsterID(1)){
			if (m_pcHeroGU->GetLevel() < c_un32HeroMidAbsorbLevel){
				//英雄等级不够吸附第二个怪，则移除第一个怪
				m_n32ToRemoveAbsorbIndex = 0;
			}
			else{
				m_n32ToRemoveAbsorbIndex = -1;
			}
			newAbsorbSkillIndex = 1;
		}
		else if(-1 == m_n32ToRemoveAbsorbIndex){
			m_n32ToRemoveAbsorbIndex = 1;
			newAbsorbSkillIndex = m_n32ToRemoveAbsorbIndex;
		}

		//移除占位的格子
		if (-1 != m_n32ToRemoveAbsorbIndex){
			UINT32 un32MosterID = m_pcHeroGU->GetAbsorbMonsterID(m_n32ToRemoveAbsorbIndex);
			if (0 != un32MosterID){
				m_pcHeroGU->AskRemoveAbsorbSkill(un32MosterID);
			}
			newAbsorbSkillIndex = m_n32ToRemoveAbsorbIndex;
		}

		m_pcHeroGU->ChangeCP(eCPCP_Personnal, -GetAbsorbCost(m_pcAbsorbNPC->TryCastToNPC()), false);
		m_pcHeroGU->ChangeFPInAbsorb(*m_pcAbsorbNPC->TryCastToNPC()->GetNPCCfg(), TRUE);
		m_pcAbsorbNPC->SetExpire(TRUE);

		AbsorbRuntimeInfo* pChangAbsorbRuntimeInfo = m_pcHeroGU->GetAbsorbMonsterSkillInfo(newAbsorbSkillIndex);
		pChangAbsorbRuntimeInfo->Clear();
		pChangAbsorbRuntimeInfo->sNPCCfg = (SNPCCfg*)m_pcAbsorbNPC->TryCastToNPC()->GetNPCCfg();
		pChangAbsorbRuntimeInfo->pSkill->Clear();
		pChangAbsorbRuntimeInfo->pSkill->cpsCfg = cpsCfg;

		pChangAbsorbRuntimeInfo->pSkill->pcMasterGU = m_pcMasterGU;
		pChangAbsorbRuntimeInfo->pSkill->eSkillState = eSkillState_Free;
		pChangAbsorbRuntimeInfo->pSkill->tStateMilsec = 0;


		//发送吸附成功消息给客户端
		UINT32 n32MonsterIDArr[2] = {0};
		for (INT32 i = 0; i < 2; ++i){
			n32MonsterIDArr[i] = m_pcHeroGU->GetAbsorbMonsterID(i);
		}
		m_pcHeroGU->SynAbsobInfoToGC(1, n32MonsterIDArr[0], n32MonsterIDArr[1]);

		m_pcHeroGU->GetCurBattle()->DoWildMonsterDead(  m_pcAbsorbNPC->TryCastToNPC());			 

		m_ifAbsorb = FALSE;

		//log
		if(!m_pcHeroGU->IfAI() && m_pcHeroGU->GetUser()&& !m_pcHeroGU->GetCurBattle()->IsGuideBattle())
		{
			stringstream mystream;
			mystream <<((GetUTCMiliSecond() - m_pcHeroGU->GetCurBattle()->GetBattleStateStartTime()) / 1000)<<LOG_SIGN;
			mystream <<  m_pcHeroGU->GetUser()->GetGUID()   <<LOG_SIGN;
			mystream <<m_pcHeroGU->GetObjType()<<LOG_SIGN;
			mystream <<m_pcHeroGU->GetLevel()<<LOG_SIGN;
			mystream <<m_pcAbsorbNPC->GetObjType();
			CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleStrawMonster,m_pcHeroGU->GetCurBattle()->GetBattleID(),m_pcHeroGU->GetCurBattle()->GetBattleMapId(),mystream.str());
		} 
	}

	return eNormal;
}

INT32	CSSAI_Hero::CheckAttackTarget(){
	if(m_pcHeroGU->GetLockedTargetGUID() < 1 || NULL == m_pcHeroGU->GetCurBattle()->GetGameUnitByGUID(m_pcHeroGU->GetLockedTargetGUID())){
		return eEC_NotEnemy;
	}
	SetAttackGU(m_pcHeroGU->GetCurBattle()->GetGameUnitByGUID(m_pcHeroGU->GetLockedTargetGUID()));
	//检查目标合法性
	if(FALSE == CFunction::IfEnemy(m_pcMasterGU->GetCampID(), GetAttackGU()->GetCampID())){
		SetAttackGU(NULL);
		return eEC_NotEnemy;
	}
	return eNormal;
}

INT32 CSSAI_Hero::WantUseSkillHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if (m_pcMasterGU->GetFPData(eEffectCate_Silence) > 0){
		//如果被沉默了，需要终止技能寻路
		StopWantUseSkill(TRUE);
		return eNormal;
	}
	//检查是否进入施放距离
	if (eNormal == m_pWantUseSkill->IfSkillUsableWithNowTarget()){
		//进入施法距离了，停止自动追击，开始施放技能
		m_pcMasterGU->GetCurBattle()->AskStopMoveObjectTar(m_pcMasterGU);
		INT32 rst = TryUseSkillWithAnyType(m_pWantUseSkill);
		StopWantUseSkill();
		return rst;
	}
	//检查是否目标不符合施放要求了
	if(m_pWantUseSkill->pcTarGU == NULL || m_pWantUseSkill->pcTarGU->IsDead()){
		StopWantUseSkill(TRUE);
		return eNormal;
	}
	//如果没有进入施法距离，则继续追击
	if(FALSE == m_ifMoving || (tUTCMilsec - m_tLastCheckMoveTarTime > 500 && m_pWantUseSkill->pcTarGU->GetCurPos() != m_pcMoveTarPos ) ){
		m_pcMoveTarPos = m_pWantUseSkill->pcTarGU->GetCurPos();
		m_tLastCheckMoveTarTime = tUTCMilsec;
		MoveToTar(m_pcMoveTarPos,FALSE,m_tLastCheckMoveTarTime);
	}

	return eNormal;
}

void CSSAI_Hero::StopWantUseSkill(BOOLEAN ifStopMove){
	if(m_pWantUseSkill != NULL){
		m_pWantUseSkill = NULL;
		if(ifStopMove && m_ifMoving){
			m_pcHeroGU->GetCurBattle()->AskStopMoveObjectTar(m_pcHeroGU);
		}
	}
	m_nextSkill = NULL;
}

bool CSSAI_Hero::StopAllSkill(bool bIfForceStop){
	if(NULL != m_pNowSkill){
		if(FALSE == m_pNowSkill->TryCancle()){
			return FALSE;
		}
		m_pNowSkill = NULL;
	}
	StopWantUseSkill(TRUE);
	m_nextSkill = NULL;
	return TRUE;
}

INT32 CSSAI_Hero::UseSkillHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	INT32 rst = m_pNowSkill->HeartBeat(tUTCMilsec, tTickSpan);
	if(eNormal != rst){
		if(eSkillState_End != rst){
			m_pNowSkill->TryCancle();
		}
		m_pNowSkill = NULL;
		TryFree();
		//如果技能结束了，检查并调用下一个预备技能
		if(m_nextSkill != NULL){
			TryUseSkillWithAnyType(m_nextSkill);
		}
	}
	return eNormal;
}

INT32 CSSAI_Hero::TryUseSkillWithAnyType(CSSSkill* pSkill){
	//检查是否物品技能
	map<CSSSkill*,UINT8>* pGoodsSkillMap = m_pcHeroGU->GetGoodsSkillMap();
	auto iter = pGoodsSkillMap->find(pSkill);
	INT32 rst = eNormal;
	if(iter != pGoodsSkillMap->end()){
		rst = m_pcHeroGU->AskUseGoods(iter->second);
	}
	else{
		rst =  AskUseSkill(pSkill->cpsCfg->un32SkillID);
	}
	return rst;
}

INT32 CSSAI_Hero::HeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	//检查是否是不能操作状态a
	if(TRUE == IfPassitiveState()){
		//如果不可控状态下，清理所有移动和操作状态
		//停止吸附
		TryCancleAbsorb();
		StopAllSkill(TRUE);
		//停止自动攻击
		CancleAttack();
		StopWantUseSkill(TRUE);
		m_nextSkill = NULL;
		m_ifAutoAttack = FALSE;
		m_ifMoveDir = FALSE;
		if(m_ifMoving){
			m_pcHeroGU->GetCurBattle()->AskStopMoveObjectDir(m_pcHeroGU);
			m_pcHeroGU->GetCurBattle()->AskStopMoveObjectTar(m_pcHeroGU);
		}
		return eEC_AbsentOrderPriority;
	}
	/*
		英雄的心跳可以做三种事情：吸附，使用技能，自动攻击
		这三件事情是相互独立，不能并存的
	*/

	if(m_ifAbsorb){
		if(eNormal != AbsorbHeartBeat(tUTCMilsec,tTickSpan)){
			TryCancleAbsorb();
		}
	}
	//自动追击施放技能qwqq
	else if(m_pWantUseSkill != NULL){
		WantUseSkillHeartBeat(tUTCMilsec,tTickSpan);
	}
	//施放技能心跳
	else if(m_pNowSkill != NULL){
		UseSkillHeartBeat(tUTCMilsec,tTickSpan);
	}
	//自动攻击心跳。当有技能在运行的时候，不运行自动攻击
	else if(m_ifAutoAttack){
		//检查目标。当锁定目标被切换的时候，要切换攻击目标。
		if(GetAttackGU() == NULL || GetAttackGU()->GetObjGUID() != m_pcHeroGU->GetLockedTargetGUID()){
			INT32 rst = CheckAttackTarget();
			if(eNormal != rst){
				CancleAttack();
				m_ifAutoAttack = FALSE;
				TryFree();
				return rst;
			}
			//初始化数据
			m_eAttackState = eAttackState_Pursue;
			m_tLastCheckMoveTarTime = 0;
		}
		//如果目标死亡了或者是在复活中
		else if(GetAttackGU()->IsDead() || GetAttackGU()->GetGOActionStateInfo().eOAS == eOAS_Reliving){
			TryFree();
			CancleAttack();
			return eNormal;
		}
		AttackHeartBeat(tUTCMilsec,tTickSpan);
	}
	//什么都没做，那么就检查是否有可以主动攻击的目标
	else if(FALSE == m_ifMoving){
		DoStandNormalAttack(tUTCMilsec,tTickSpan);
	}

	return eNormal;
}

void	CSSAI_Hero::DoStandNormalAttack(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	//判断是否有锁定目标
	if(m_pcHeroGU->GetLockedTargetGUID() > 0){
		CSSGameUnit* pAttackTar = m_pcHeroGU->GetCurBattle()->GetGameUnitByGUID(m_pcHeroGU->GetLockedTargetGUID());
		//是否正在普通攻击中
		if(m_pAttackSkill->ifRunning){
			if(pAttackTar == NULL || pAttackTar->IsDead() || pAttackTar->GetGOActionStateInfo().eOAS == eOAS_Reliving){
				m_ifStandAttack = FALSE;
				TryFree();
			}
			else{
				INT32 rst = m_pAttackSkill->HeartBeat(tUTCMilsec,tTickSpan);
				if(eEC_NullPointer == rst){
					m_ifStandAttack = FALSE;
					TryFree();
				}
			}
		}
		else{
			//是否是合法的攻击目标
			if(eNormal == m_pAttackSkill->IfSkillUsableWithNowTarget()){
			//if(pAttackTar != NULL && TRUE == CFunction::IfEnemy(m_pcHeroGU->GetCampID(), pAttackTar->GetCampID())){
				//普通攻击技能是否可用
				if(eNormal == m_pAttackSkill->IfSkillUsable()){ 
					//是否在攻击范围内
					if (TRUE == m_pcHeroGU->IfInReleaseSkillRange(pAttackTar, m_pAttackSkill->cpsCfg)){
						//距离内，则开始施法
						m_pAttackSkill->pcTarGU = pAttackTar;
						m_ifStandAttack = TRUE;
						if(eNormal != m_pAttackSkill->Start()){
							m_ifStandAttack = FALSE;
							TryFree();
						}
					}
					else{
						if( TRUE == m_ifStandAttack){
							m_ifStandAttack = FALSE;
							TryFree();
						}
					}
				}
			}
			else if( TRUE == m_ifStandAttack && tUTCMilsec + 100 > m_pAttackSkill->tCooldownMilsec){
				m_ifStandAttack = FALSE;
				TryFree();
			}
		}
	}
	else{
		if( TRUE == m_ifStandAttack && tUTCMilsec + 100 > m_pAttackSkill->tCooldownMilsec){
			m_ifStandAttack = FALSE;
			TryFree();
		}
	}
}

void	CSSAI_Hero::ClearAction(){
	//检查是否是施放技能中
	if(NULL != m_pNowSkill){
		m_pNowSkill->TryCancle();
	}
	//停止自动追击施放技能
	StopWantUseSkill();
	//停止自动攻击
	m_ifAutoAttack = FALSE;
	StopAttack();
	//停止吸附
	TryCancleAbsorb();
	//停止手动移动
	m_ifMoveDir = FALSE;
	//停止自动移动
	if(m_ifMoving){
		m_pcHeroGU->GetCurBattle()->AskStopMoveObjectTar(m_pcHeroGU);
	}
}

void	CSSAI_Hero::OnTeleport(){
	if(m_pNowSkill != NULL){
		m_pNowSkill->TryCancle();
	}
}

}