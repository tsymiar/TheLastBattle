/*
* file name			:SSAI.h
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

namespace SceneServer{

	class CSSGameUnit;
	class CSSSkill;

	class CSSAI
	{
	private:
		ref_ptr<CSSGameUnit>		m_pcAttackGU;//自动攻击的目标
	protected:
		ref_ptr<CSSGameUnit>	m_pcMasterGU;//AI的拥有者
		bool					m_ifMoving;//是否正在移动中
		CSSSkill*				m_pAttackSkill;

		bool					IfPassitiveState();
		void					MoveToTar(CVector3D pos, bool ifMoveToBlackPoint, TIME_TICK nowTime);

		//自动攻击相关变量和函数
		enum EAttackState{
			eAttackState_Pursue,
			eAttackState_UseSkill,
		};
		EAttackState m_eAttackState;

		TIME_TICK					m_tLastTryMoveTime;//上次尝试移动失败的时间
		CVector3D					m_pcMoveTarPos;//当前前进的目标地址
		TIME_TICK					m_tLastCheckMoveTarTime;//上一次检查目标坐标的时间
		bool						m_bIfFliter;
		

		
		void	SetAttackGU(CSSGameUnit* pcAttackGU);
		INT32	AttackHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
		void	StopAttack();
		void	CancleAttack();
		void	TryFree();
	public:
		virtual	~CSSAI();
		CSSAI(CSSGameUnit* pMaster);
		void						SetNormalAttackSkill(CSSSkill* pAttackSkill){m_pAttackSkill = pAttackSkill;}
		CSSSkill*					GetNormalAttackSkill(){return m_pAttackSkill;}
		virtual void				OnMoveBlock();
		bool						IfMoving(){return m_ifMoving;}
		bool						IfAttacking();
		CSSGameUnit*				GetAttackGU(){return m_pcAttackGU;}
		virtual INT32				HeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan) = 0;
		virtual void				OnTeleport(){}
	};

}