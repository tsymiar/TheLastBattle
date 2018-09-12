/*
* file name			:SSAI_Hero.h
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
#include "SSAI.h"
#include "SSGameUnit.h"

namespace SceneServer{
	class CSSSkill;
	class CSSHero;
	class CSSNPC;

	class CSSAI_Hero : public CSSAI
	{
	private:
		CSSHero*			m_pcHeroGU;	//英雄

		bool				m_ifMoveDir;//是否在手动移动中
		//自动攻击相关的内容
		bool				m_ifAutoAttack;
		CSSSkill*			m_pNowSkill;
		//自动施放技能相关
		CSSSkill*			m_pWantUseSkill;//希望使用的技能
		CSSSkill*			m_nextSkill;//预备使用的下一个技能

		//吸附相关的内容
		bool				m_ifAbsorb;
		ref_ptr<CSSGameUnit>	m_pcAbsorbNPC;//吸附的目标
		INT32				m_n32ToRemoveAbsorbIndex;//将要被移除的吸附者序号
		TIME_MILSEC			m_tStartAbsorbTime;//开始吸附的时间
		bool				m_ifStandAttack;//是否在站立自动攻击中
		INT32				AbsorbHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
		INT32				WantUseSkillHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
		INT32				UseSkillHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
		void				TryCancleAbsorb();		INT32				CheckAttackTarget();
		INT32				TryUseSkillWithAnyType(CSSSkill* pSkill);
		void				StopWantUseSkill(BOOLEAN ifStopMove = TRUE);
		bool				StopAllSkill(bool bIfForceStop = FALSE);

	protected:
		void				ClearAction();
		virtual	void		DoStandNormalAttack(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	public:
		virtual ~CSSAI_Hero();
		CSSAI_Hero(CSSGameUnit* pGU);

		INT32				HeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);

		INT32				AskMoveDir(const CVector3D &crcDir);
		INT32				AskMoveTar(const CVector3D &crcPos);
		INT32				AskStopMove();
		INT32				AskStartAutoAttack();

		INT32				AskUseSkill(UINT32 un32SkillID);
		INT32				AskAbsorbMonster(INT32 n32ToRemoveIndex);
		INT32				AskGasExpersion();

		bool				IfAbsorbing(){return m_ifAbsorb;}
		bool				IfAutoAttacking(){return m_ifAutoAttack;}
		bool				IfUsingSkill(){return NULL != m_pNowSkill && TRUE == m_pNowSkill->ifRunning;}

		CSSHero*			GetHero(){return m_pcHeroGU;}
		CSSSkill*			GetNowSkill(){return m_pNowSkill;}
		INT32				GetAbsorbCost(CSSNPC *sspNPC);

		virtual void		OnTeleport();
	};

}