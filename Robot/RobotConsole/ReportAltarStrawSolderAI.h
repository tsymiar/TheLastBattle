#pragma once
#include "PlayerAI.h"

namespace ReBot
{
	class CReportAltarStrawSolderAI : public CPlayerAI
	{
	public:
		CReportAltarStrawSolderAI(void);
		~CReportAltarStrawSolderAI(void);

		virtual INT32 Begin(PlayerAIData& sPlayerAIData);
		virtual	INT32 Update(TIME_MILSEC tCurTime);
		virtual	INT32 End();
		virtual	INT32 Clear();
	};
}


