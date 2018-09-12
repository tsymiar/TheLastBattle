/*
* file name			:SSSkill.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:Octorber 9 2014
* summary			:
*
*/

#pragma once

#include "stdafx.h"
#include "ISSParameterChangeCallBack.h"

namespace SceneServer{

	class CSSSkill : public referable<CSSSkill> ,public ISSParameterChangeCallBack
	{
	private:
		INT32	m_normalAttackReleaseTime;

		void	CostConsume();
		INT32	CheckStatus();
		INT32	CheckAndSetTarget();
		INT32	CheckTargetTypeCamp();
		void	SetSkillDir();
		void	CheckAndDoInstantSkill();

		void	MakeSkillEffect(TIME_MILSEC	tUTCMilsec);

	public:
		const SSNewSkillCfg	*cpsCfg;

		bool			ifRunning;
		ref_ptr<CSSGameUnit>	pcMasterGU;
		ref_ptr<CSSGameUnit>	pcTarGU;
		CVector3D				cTargetPos;
		ESkillState		eSkillState;
		TIME_MILSEC		tStateMilsec;//状态切换时间
		TIME_MILSEC		tCooldownMilsec;//技能冷却时间
		TIME_MILSEC		tBeginMilsec;//技能开始时间
		UINT32			un32UsingEffectsArr[c_n32UsingEffectsNum];
		bool			bIfCanCooldown;
		bool			bIfOpen;
		UINT32			un32PassitiveSkillArr[c_n32MaxNextSkillEffectCfgNum];
		CVector3D		cDir;

		void Clear(){
			cpsCfg = NULL;
			ifRunning = FALSE;
			eSkillState = eSkillState_Free;
			tStateMilsec = 0;
			tCooldownMilsec = 0;
			pcTarGU = NULL;
			bIfCanCooldown = TRUE;
			tBeginMilsec = 0;
			bIfOpen = FALSE;
			m_normalAttackReleaseTime = 0;
			cTargetPos.Clear();

			ClearUsingEffects();
			ClearPassSkill();
		}

		void ClearUsingEffects(){
			memset(un32UsingEffectsArr,0,c_n32UsingEffectsNum*sizeof(UINT32));
		}
		void ClearPassSkill(){
			memset(un32PassitiveSkillArr,0,c_n32MaxNextSkillEffectCfgNum*sizeof(UINT32));
		}

		CSSSkill(){Clear();}

		INT32	IfSkillUsableWithNowTarget();
		INT32	IfSkillUsable();
		INT32	DoCoolDown();//设置冷却时间
		INT32	IfInCD();
		INT32	CheckConsume();
		bool	IfHasPreTime(){return cpsCfg->n32PrepareMilsec > 0 || cpsCfg->n32ReleaseMilsec > 0;}
		bool	IfImpactSkill(){return FALSE == IfHasPreTime() && cpsCfg->bIfImpact == TRUE;}
		bool	IfSkillBeforeHit(){return eSkillState <= eSkillState_Releasing;}
		bool	IfSkillRunning(){return eSkillState != eSkillState_Free && eSkillState != eSkillState_End;}
		ESkillState		GetSkillState(){return eSkillState;}
		void	ReFreshCD();
		INT32	Start();
		bool	TryCancle();
		bool	IfMasterMoveAble();
		INT32	End();

		INT32	HeartBeat(TIME_MILSEC	tUTCMilsec, TIME_TICK	tTickSpan);

		VOID	OnValueChanged(INT32 oldValue,INT32 newValue);

		//static INT32  GetCurtSkillId(void)						{return cpsCfg->un32SkillID;}
	};

}