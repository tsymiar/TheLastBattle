#pragma once
#include "PlayerAI.h"

namespace ReBot
{
	class CPlayerMoveAI;
	class CPlayerAtkAI;
	class CClientGameObject;
	class AbsorbAI;

	class CPlayerOccupyAI : public CPlayerAI
	{
	public:
		CPlayerOccupyAI();
		virtual ~CPlayerOccupyAI();
		virtual INT32	Begin(PlayerAIData& sPlayerAIData);
		virtual INT32	Update(TIME_MILSEC tCurTime);

		virtual	INT32	Clear();
		virtual INT32	End();

		void OnDead();

	private:
		INT32	TryToUpdatePathNode();
		INT32	BeginAtkAI(TIME_MILSEC tCurTime);
		INT32	BeginMoveAI(TIME_MILSEC tCurTime);

	public:
		enum	EOccupyState{
			eOccupyState_Hesitation = 0,
			eOccupyState_Walk,
			eOccupyState_Attack,
		};

		EOccupyState	m_eOccupyState;
		TIME_MILSEC		m_tStateBeginMilsec;
		INT32			m_n32CurNodeIdx;
		CPlayerMoveAI*	m_pPlayerMoveAI;
		CPlayerAtkAI*	m_pPlayerAtkAI;
		TIME_MILSEC		m_tLastWatchEnemyMilsec;
		struct StayPosTime{
			CVector3D sPos;
			TIME_MILSEC sTime;
		};
		StayPosTime	m_StayPosTime;
		CClientGameObject* m_pCurEnemy;
		TIME_MILSEC		m_tLastMoveMilsec;
		INT32			m_MoveType;
		TIME_MILSEC		m_LastBuyGoodsTime;
	};
}


