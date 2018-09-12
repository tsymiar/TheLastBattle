#pragma once
#include "PlayerAI.h"

namespace ReBot
{
	class AbsorbAI : public CPlayerAI
	{
	public:
		AbsorbAI();
		~AbsorbAI();

		virtual INT32 Begin(PlayerAIData& sPlayerAIData);
		virtual	INT32 Update(TIME_MILSEC tCurTime);
		virtual	INT32 End();
		virtual	INT32 Clear();
	private:
		INT32	CheckValid();
		BOOLEAN	m_IfSend;
	};
}


