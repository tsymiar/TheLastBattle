#pragma once
#include "PlayerAI.h"

namespace ReBot
{
	class CPlayerMoveAI : public CPlayerAI
	{
	public:
		CPlayerMoveAI();
		virtual ~CPlayerMoveAI();
		virtual INT32 Begin(PlayerAIData& sPlayerAIData);
		virtual	INT32 Update(TIME_MILSEC tCurTime);
		virtual	INT32 End();
		virtual	INT32 Clear();

	private:
		CVector3D	m_TarPos;
		CVector3D	m_BeginPos;
		BOOLEAN		m_BIfBegin;
	};

}

