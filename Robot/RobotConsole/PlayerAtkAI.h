#pragma once
#include "PlayerAI.h"

namespace ReBot
{
	class CPlayerMoveAI;

	class CPlayerAtkAI : public CPlayerAI
	{
	public:
		CPlayerAtkAI();
		virtual ~CPlayerAtkAI();
		virtual INT32 Begin(PlayerAIData& sPlayerAIData);
		virtual	INT32 Update(TIME_MILSEC tCurTime);
		virtual	INT32 End();
		virtual	INT32 Clear();

	private:
		enum ERobotAttackState{
			eRobotAttackState_Free,
			eRobotAttackState_Pursue,
			eRobotAttackState_UseSkill,
		};
		ERobotAttackState	m_RobotAttackState;
		CPlayerMoveAI*	m_pPlayerMoveAI;
		
	};
}


