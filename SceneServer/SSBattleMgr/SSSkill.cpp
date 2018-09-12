#include "SSSkill.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSNPC.h"
#include "SSBattle.h"
#include "SSEffectMgr.h"

namespace SceneServer{

INT32	CSSSkill::CheckStatus(){
	//检查技能所有者状态
	if (eOAS_PassiveState < pcMasterGU->GetGOActionStateInfo().eOAS){
		return eEC_AbsentOrderPriority;
	}
	if (pcMasterGU->GetFPData(eEffectCate_Dizziness) > 0){
		return eEC_UseSkillFailForDizziness;
	}
	if (pcMasterGU->GetFPData(eEffectCate_Silence) > 0 && FALSE == cpsCfg->bIfNomalAttack){
		return eEC_UseSkillFailForSilenced;
	}
	if (pcMasterGU->GetFPData(eEffectCate_Disarm) > 0 && TRUE == cpsCfg->bIfNomalAttack){
		return eEC_UseSkillFailForDisarmed;
	}

	return eNormal;
}

INT32	CSSSkill::CheckConsume(){
	if (pcMasterGU->IfHero()){
		CSSHero *pcHero = pcMasterGU->TryCastToHero();
		if (pcHero->GetCP(eCPCP_Personnal) < cpsCfg->n32UseCP){
			return eEC_UseSkillFailForLackCP;
		}
		if (pcHero->GetCurMP() < cpsCfg->n32UseMP){
			return eEC_UseSkillFailForLackMP;
		}
		if (pcHero->GetCurHP() < cpsCfg->n32UseHP){
			return eEC_UseSkillFailForLackHP;
		}
	}
	return eNormal;
}

void	CSSSkill::CostConsume(){
	if (pcMasterGU->IfHero()){
		CSSHero *pcHero = pcMasterGU->TryCastToHero();

		if(cpsCfg->n32UseCP > 0) pcHero->ChangeCP(eCPCP_Personnal, -cpsCfg->n32UseCP, false);
		if(cpsCfg->n32UseHP > 0) pcHero->ChangeCurHP(pcMasterGU, GSToGC::SkillConsume, -cpsCfg->n32UseHP);
		if(cpsCfg->n32UseMP > 0) pcHero->ChangeCurMP(pcMasterGU, GSToGC::SkillConsume, -cpsCfg->n32UseMP);
	}
}

INT32	CSSSkill::CheckAndSetTarget(){
	if (cpsCfg->eReleaseWay == ESReleaseWay_NO_TARGET){
		pcTarGU = NULL;
	}
	else{
		if(pcMasterGU->IfHero()){
			CSSHero *pcHero = pcMasterGU->TryCastToHero();
			pcTarGU = pcHero->GetCurBattle()->GetGameUnitByGUID(pcHero->GetLockedTargetGUID());
			if (NULL == pcTarGU && cpsCfg->eReleaseWay != ESReleaseWay_AUTO){
				return eEC_UseSkillFailForNULLTarget;
			}
		}
	}

	if(pcMasterGU->IfHero()){
		CSSHero *pcHero = pcMasterGU->TryCastToHero();
		UINT32 un32SkillID = cpsCfg->un32SkillID;
		if(NULL != pcHero && pcHero->IsGasSkill(un32SkillID)){
			INT32 n32CheckRet = pcHero->CheckUseGasSkill(un32SkillID);
			if(eNormal != n32CheckRet){
				return n32CheckRet;
			}
		}
	}
	return eNormal;
}

INT32	CSSSkill::CheckTargetTypeCamp(){
	if(pcTarGU == NULL){
		return eNormal;
	}

	if(pcTarGU->IsDead()){
		return eEC_UseSkillTargetIsDead;
	}

	switch(cpsCfg->eSkillTargetCate)
	{
	case eSTC_Self://自己
		break;
	case eSTC_AllObject://所有目标
		break;
	case eSTC_TeamMember://队友
		{
			if(pcMasterGU->GetObjGUID() == pcTarGU->GetObjGUID()){
				return eEC_UseSkillTargetWrongCamp;
			}
			if(pcTarGU->GetCampID() > 1 && pcMasterGU->GetCampID() % 2 != pcTarGU->GetCampID() % 2){
				return eEC_UseSkillTargetWrongCamp;
			}
		}
		break;
	case eSTC_Enemy://敌人
		if(CFunction::IfEnemy(pcMasterGU->GetCampID(), pcTarGU->GetCampID()) == false){
			return eEC_UseSkillTargetWrongCamp;
		}
		break;
	case eSTC_AllFriendUnit://所有友军目标
		if((pcTarGU->GetCampID() > 1 && pcMasterGU->GetCampID() % 2 != pcTarGU->GetCampID() % 2) || pcMasterGU->GetCampID() == eGOCamp_AllEnemy){
				return eEC_UseSkillTargetWrongCamp;
		}
		break;
	}

	if(pcTarGU->IfHero()){
		if(cpsCfg->bIfAffectHero == false){
			return eEC_UseSkillTargetWrongType;
		}
	}
	else if(pcTarGU->IfNPC()){
		CSSNPC* pNpc = pcTarGU->TryCastToNPC();

		if(pNpc->GetNPCCfg()->eNPCCate == eNPCCate_WideAnimal || pNpc->GetNPCCfg()->eNPCCate == eNPCCate_Solider){
			if(cpsCfg->bIfAffectMonster == false){
				return eEC_UseSkillTargetWrongType;
			}
		}
		if(pNpc->GetNPCCfg()->eNPCCate == eNPCCate_Building){
			if(cpsCfg->bIfAffectBuilding == false){
				return eEC_UseSkillFailForBuildingNullity;
			}
		}
	}
	return eNormal;
}

void	CSSSkill::SetSkillDir(){
	cDir = pcMasterGU->GetCurDir();
	switch (cpsCfg->eReleaseWay){
	case ESReleaseWay_NEED_TARGET:
	case ESReleaseWay_AUTO:
		if(NULL != pcTarGU && FALSE == pcMasterGU->IfNPC_Building()){
			cDir = pcTarGU->GetCurPos() - pcMasterGU->GetCurPos();
			cDir.m_fY = 0;
			cDir.unit();
			pcMasterGU->SetGOActionState_Dir(cDir);
		}
		break;
	}
}

INT32	CSSSkill::IfSkillUsableWithNowTarget(){
	INT32 rst = CheckStatus();
	if(eNormal != rst) return rst;

	//检查消耗
	rst = CheckConsume();
	if(eNormal != rst){
		return rst;
	}

	rst = CheckAndSetTarget();
	if(eNormal != rst) return rst;

	rst = CheckTargetTypeCamp();
	if(eNormal != rst) return rst;

	if(pcTarGU != NULL && (pcTarGU->IsDead() || pcTarGU->GetGOActionStateInfo().eOAS == eOAS_Reliving)){
		return eEC_UseSkillFailForNULLTarget;
	}

	if (FALSE == pcMasterGU->IfInReleaseSkillRange(pcTarGU, cpsCfg)){
		return eEC_AbsentSkillDistance;
	}

	return rst;
}

INT32 CSSSkill::DoCoolDown(){
	TIME_MILSEC curTime = GetUTCMiliSecond();
	tStateMilsec = curTime;

	//CD需要将普通攻击和技能攻击分开计算
	if(cpsCfg->bIfNomalAttack){
		INT32 attackSpeed = pcMasterGU->GetFPData(eEffectCate_AttackSpeed);
		INT32 n32ReleaseMilsec = cpsCfg->n32ReleaseMilsec * attackSpeed / 1000;
		INT32 n32StandCoolDownMilsec = (cpsCfg->n32CoolDown + cpsCfg->n32ReleaseMilsec ) * attackSpeed / 1000;
		//如果距离上一次释放普通攻击技能间隔时间比较久，则冷却时间直接从前摇结束的时间开始算。否则，直接在上一次冷却时间的基础上加上整个技能周期
		if(tCooldownMilsec + n32ReleaseMilsec + 150 > tStateMilsec){
			tCooldownMilsec += n32StandCoolDownMilsec;
		}
		else{
			tCooldownMilsec = tBeginMilsec + n32StandCoolDownMilsec;
		}
	}
	//如果是普通技能，直接从技能放出来的时间加上CD即可
	else{
		tCooldownMilsec = tStateMilsec + ( cpsCfg->n32CoolDown * (1.0f - pcMasterGU->GetFPData(eEffectCate_CooldownReduce) / 1000.0f) );
	}

	CSSHero* pHero = pcMasterGU->TryCastToHero();
	if (pHero){
		pHero->SyncSkillStateToGC(cpsCfg->un32SkillID);
	}

	return eNormal;
}

INT32	CSSSkill::IfInCD(){
	//检查技能状态
	if(GetUTCMiliSecond() < tCooldownMilsec){
		return eEC_UseSkillFailForSkillCoolDown;
	}
	/*
	if (eSkillState_Free != eSkillState){
		TryFree();
	}
	if (eSkillState_Free != eSkillState){
		if (eSkillState_End == eSkillState){
			return eEC_UseSkillFailForSkillCoolDown;
		}
		return eEC_InvalidSkillState;
	}
	*/
	return eNormal;
}

INT32	CSSSkill::IfSkillUsable(){
	INT32 rst = CheckStatus();
	if(eNormal != rst) return rst;

	//检查消耗
	rst = CheckConsume();
	if(eNormal != rst){
		return rst;
	}

	//检查CD
	rst = IfInCD();
	return rst;
}

void CSSSkill::MakeSkillEffect(TIME_MILSEC	tUTCMilsec){
	CostConsume();
	CSSBattle *pcCurBattle = pcMasterGU->GetCurBattle();

	CVector3D cPos = pcMasterGU->GetCurPos();
	if(NULL != pcTarGU) {
		cPos = pcTarGU->GetCurPos();
	}

	//根据配置设置目标
	if(cpsCfg->eSkillTargetCate == eSTC_Self){
		pcTarGU = pcMasterGU;
	}

	//调用普攻前的被动技能
	if(TRUE == cpsCfg->bIfNomalAttack)
	{
		pcMasterGU->OnPassitiveSkillCalled(EPassiveSkillTriggerType_Attack, pcTarGU);
	}
	//调用使用技能前的被动技能
	if(FALSE == cpsCfg->bIfNomalAttack)
	{
		pcMasterGU->OnPassitiveSkillCalled(EPassiveSkillTriggerType_UseSkill, pcTarGU);
	}

	pcCurBattle->GetEffectMgr()->AddEffectsFromCfg((SSNextSkillEffectCfg*)cpsCfg->asSkillModelList, pcMasterGU.get(), pcTarGU.get(), cPos, pcMasterGU->GetCurDir(), this, tUTCMilsec);
}

void CSSSkill::CheckAndDoInstantSkill(){
	if(IfImpactSkill()){
		cDir = pcMasterGU->GetCurDir();
		MakeSkillEffect(tBeginMilsec);
		if(cpsCfg->n32SkillLastTime <= 0){
			End();
		}
		else{
			eSkillState = eSkillState_Using;
			tStateMilsec = GetUTCMiliSecond();
		}
	}
}

INT32	CSSSkill::Start(){
	INT32 rst = IfSkillUsable();
	if(eNormal != rst) return rst;

	eSkillState = eSkillState_Free;
	tBeginMilsec = GetUTCMiliSecond();
	ifRunning = TRUE;
	if(pcTarGU != NULL)
		cTargetPos = pcTarGU->GetCurPos();

	CheckAndDoInstantSkill();

	return eNormal;
}

bool	CSSSkill::TryCancle(){
	bool ifCanCancle = FALSE;
	//当还在前摇或者以前的时候，可以取消
	if(eSkillState_Releasing >= eSkillState){
		ifCanCancle = TRUE;
	}
	//当引导状态时，需要依次询问模块时候可以取消
	else if(eSkillState_Using == eSkillState){
		bool ifCanStopUsing = TRUE;
		for (INT32 i = 0; i < c_n32UsingEffectsNum; ++i){
			UINT32 un32EfffectUniqueID = un32UsingEffectsArr[i];
			if (0 != un32EfffectUniqueID){
				CSSSkillEffect* pEffect = pcMasterGU->GetCurBattle()->GetEffectMgr()->GetEffect(un32EfffectUniqueID);
				if (NULL != pEffect){
					if(FALSE == pEffect->IsCanStopUsing()){
						ifCanStopUsing = FALSE;
						break;
					}
				}
			}
		}
		if(ifCanStopUsing){
			ifCanCancle = TRUE;
		}
	}
	//当后摇阶段的时候，可以取消
	else if(eSkillState_Lasting == eSkillState){
		ifCanCancle = TRUE;
	}
	//如果可以取消，取消技能施放
	if(ifCanCancle){
		//取消所有使用中的模块
		if(eSkillState_Using == eSkillState){
			for (INT32 i = 0; i < c_n32UsingEffectsNum; ++i){
				UINT32 un32EfffectUniqueID = un32UsingEffectsArr[i];
				if (0 != un32EfffectUniqueID){
					CSSSkillEffect* pEffect = pcMasterGU->GetCurBattle()->GetEffectMgr()->GetEffect(un32EfffectUniqueID);
					if (NULL != pEffect){
						pEffect->ForceStop();
					}
				}
			}
			ClearUsingEffects();
		}
		End();
		return TRUE;
	}else{
		return FALSE;
	}
}

bool	CSSSkill::IfMasterMoveAble(){
	//如果不是引导阶段，那么可以移动
	if(eSkillState_Using != eSkillState){
		return TRUE;
	}
	else if(eSkillState_Using == eSkillState){
		for (INT32 i = 0; i < c_n32UsingEffectsNum; ++i){
			UINT32 un32EfffectUniqueID = un32UsingEffectsArr[i];
			if (0 == un32EfffectUniqueID) continue;
			CSSSkillEffect* pEffect = pcMasterGU->GetCurBattle()->GetEffectMgr()->GetEffect(un32EfffectUniqueID);
			if (NULL != pEffect){
				if(FALSE == pEffect->IfCanMove()){
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

INT32	CSSSkill::End(){
	ifRunning = FALSE;
	eSkillState = eSkillState_End;
	return eNormal;
}

INT32	CSSSkill::HeartBeat(TIME_MILSEC	tUTCMilsec, TIME_TICK	tTickSpan){
	ESkillState heartBeatStartState = eSkillState;

	INT32 rst = eNormal;

	do{
		rst = CheckStatus();
		if(eNormal != rst) break;

		//如果距离太远，也需要取消技能施放
		if(eSkillState_Releasing >= eSkillState && FALSE == pcMasterGU->IfInReleaseSkillRange(pcTarGU, cpsCfg, 1000)){
			rst = eEC_NullPointer;
			break;
		}

		/**************************************
			等待 状态
		**************************************/
		if (eSkillState_Free ==  eSkillState){
			eSkillState = eSkillState_Preparing;
			tStateMilsec = tUTCMilsec;
			SetSkillDir();
		}

		/**************************************
			吟唱 状态
		**************************************/
		if (eSkillState_Preparing ==  eSkillState){
			//如果尚需等待，则直接返回
			TIME_MILSEC tMilsecSpan = tUTCMilsec - tStateMilsec;
			if (tMilsecSpan < cpsCfg->n32PrepareMilsec){
				rst = eNormal;
				break;
			}
			//不需要等待，则进入施法前摇状态
			eSkillState = eSkillState_Releasing;
			tStateMilsec = tUTCMilsec;
		}

		/**************************************
			前摇 状态
		**************************************/
		if (eSkillState_Releasing ==  eSkillState){
			//如果前摇时间尚未结束，则直接返回
			INT32 n32ReleaseMilsec = cpsCfg->n32ReleaseMilsec;
			//普通攻击的前摇时间要计算攻速加成
			if (cpsCfg->bIfNomalAttack){
				if(m_normalAttackReleaseTime == 0){
					m_normalAttackReleaseTime = n32ReleaseMilsec * pcMasterGU->GetFPData(eEffectCate_AttackSpeed) / 1000.0f;
				}
				n32ReleaseMilsec = m_normalAttackReleaseTime;
			}

			INT32 tMilsecSpan = tUTCMilsec - tStateMilsec;
			if (tMilsecSpan < n32ReleaseMilsec){
				rst = eNormal;
				break;
			}
			//时间到了，开始调用技能模块
		
			//检查并扣费
			rst = CheckConsume();
			if(eNormal != rst){
				break;
			}
			MakeSkillEffect(tUTCMilsec);
			//进入下一阶段
			eSkillState = eSkillState_Using;
			tStateMilsec = tUTCMilsec;
			m_normalAttackReleaseTime = 0;
		}

		/**************************************
			引导 状态
		**************************************/
		if (eSkillState_Using ==  eSkillState){
			bool bIfUsing = FALSE;
			//遍历所有的引导中的技能效果,看其是否依然在占用中
			for (INT32 i = 0; i < c_n32UsingEffectsNum; ++i){
				UINT32 un32EfffectUniqueID = un32UsingEffectsArr[i];
				if (0 != un32EfffectUniqueID){
					CSSSkillEffect* pEffect = pcMasterGU->GetCurBattle()->GetEffectMgr()->GetEffect(un32EfffectUniqueID);
					if (NULL != pEffect){
						if(TRUE == pEffect->IsUsingSkill()){
							bIfUsing = TRUE;
						}
						else{
							un32UsingEffectsArr[i] = 0;
						}
					}
				}
			}
			//如果不在使用状态，则进入技能后摇阶段
			if(FALSE == bIfUsing){
				ClearUsingEffects();
				eSkillState = eSkillState_Lasting;
				tStateMilsec = tUTCMilsec;
			}
		}

		/**************************************
			后摇 状态
		**************************************/
		if (eSkillState_Lasting ==  eSkillState){
			//如果后摇结束，则技能进入结束状态
			if(tStateMilsec + cpsCfg->n32SkillLastTime > tUTCMilsec){
				rst = eNormal;
				break;
			}
			eSkillState = eSkillState_End;
			tStateMilsec = tUTCMilsec;
		}

		/**************************************
			结束 状态
		**************************************/
		if (eSkillState_End ==  eSkillState){
			rst = eSkillState_End;
			End();
			break;
		}
	}
	while(FALSE);

	//如果状态改变了，则需要同步到对象身上
	if(heartBeatStartState != eSkillState){
		if(eSkillState_Preparing == eSkillState){
			pcMasterGU->BeginAction_PrepareSkill(this, cDir, TRUE);
		}
		else if(eSkillState_Releasing == eSkillState){
			pcMasterGU->BeginAction_ReleaseSkill(this, cDir, TRUE);
		}
		else if(eSkillState_Using == eSkillState){
			pcMasterGU->BeginAction_UsingSkill(this, cDir, TRUE);
		}
		else if(eSkillState_Lasting == eSkillState){
			pcMasterGU->BeginAction_LastingSkill(this, cDir, TRUE);
		}
	}

	return rst;
}

void CSSSkill::ReFreshCD(){
	tCooldownMilsec = GetUTCMiliSecond();
}

void CSSSkill::OnValueChanged(INT32 oldValue,INT32 newValue){
	//tStateMilsec = curTime;
	//如果是普通攻击，则攻速改变的时候需要改变冷却时间
	if(cpsCfg->bIfNomalAttack){
		TIME_MILSEC curTime = GetUTCMiliSecond();
		INT32 attackSpeed = pcMasterGU->GetFPData(eEffectCate_AttackSpeed);
		INT32 n32ReleaseMilsec = cpsCfg->n32ReleaseMilsec * attackSpeed / 1000;
		INT32 n32StandCoolDownMilsec = (cpsCfg->n32CoolDown + cpsCfg->n32ReleaseMilsec ) * attackSpeed / 1000;

		//如果还在前摇阶段，则需要改变前摇的时间
		if( m_normalAttackReleaseTime > 0){
			//将前摇时间分为已运行时间和等待运行时间
			INT32 releaseTimeBefore = curTime - tStateMilsec;
			INT32 releaseTimeAfterOld = m_normalAttackReleaseTime - releaseTimeBefore;
			if(releaseTimeAfterOld > 0){
				INT32 normalAttackReleaseTimeNew = m_normalAttackReleaseTime * newValue / oldValue;
				INT32 releaseTimeAfterNew = releaseTimeAfterOld * newValue / oldValue;
				m_normalAttackReleaseTime += (releaseTimeAfterNew - releaseTimeAfterOld);

				tBeginMilsec += (normalAttackReleaseTimeNew - m_normalAttackReleaseTime);
			}
		}
		else if( tCooldownMilsec > curTime){
			//冷却中，需要改变冷却时间
			//将冷却时间分为已冷却时间和等待冷却时间
			INT32 coldTimeAfterOld = tCooldownMilsec - curTime;
			if(coldTimeAfterOld > 0){
				INT32 coldTimeAfterNew = coldTimeAfterOld * newValue / oldValue;
				tCooldownMilsec += (coldTimeAfterNew - coldTimeAfterOld);
			}
		}
	}
}

}