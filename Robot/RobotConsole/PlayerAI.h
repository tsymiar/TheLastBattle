#pragma once
#include "../ELogging/ELogging/ThreadSafeQueue.h"

namespace ReBot
{
	enum ERebotAIState{
		eRebotAIState_Cleared = 0,
		eRebotAIState_Running,
		eRebotAIState_Complete,
	};

	class CRobotPlayer;
	class CPlayerMoveAI;
	class CPlayerAtkAI;
	class AbsorbAI;
	class CPlayerOccupyAI;

	class CPlayerAI
	{
	public:
		CPlayerAI();
		virtual ~CPlayerAI();
		virtual INT32	Begin(PlayerAIData& sPlayerAIData);
		virtual INT32	Update(TIME_MILSEC tCurTime) = 0;
		virtual INT32	End() = 0;
		virtual	INT32	Clear() {m_RebotAIState = eRebotAIState_Cleared; return eNormal;};
		ERebotAIState	GetAIState(){return m_RebotAIState;}

		static	CPlayerMoveAI* GetMoveAI();
		static	void FreeMoveAI(CPlayerMoveAI* aAI);
		static	CPlayerAtkAI* GetAtkAI();
		static	void FreeAtkAI(CPlayerAtkAI* aAI);
		static AbsorbAI* GetAbosorbAI();
		static void FreeAbosorbAI(AbsorbAI* pAI);
		static void FreeOccupyAI(CPlayerOccupyAI* pAI);
		static CPlayerOccupyAI* GetOccupyAI();

	protected:
		CRobotPlayer*	m_pPlayer;
		ERebotAIState	m_RebotAIState;
		TIME_MILSEC		m_BeginTime;
	};

}

