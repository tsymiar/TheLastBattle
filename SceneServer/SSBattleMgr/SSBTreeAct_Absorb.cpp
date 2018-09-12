#include "SSBTreeAct_Absorb.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{
	BOOLEAN	CSSBTreeAct_Absorb::Action(CSSAI_HeroRobot* pAI){
		pAI->SetLastActionTime(GetUTCMiliSecond());
		//ELOG(LOG_DBBUG,"Action: Absorb");
		CSSHero* pHero = pAI->GetHero();
		CSSGameUnit* pTar = pHero->GetCurBattle()->GetGameUnitByGUID(pHero->GetLockedTargetGUID());
		if(NULL == pTar || FALSE == pTar->IfWildNPC()) return FALSE;
		//ªÒ»°ø’∏Ò
		UINT8	absorbSlot = 0;
		for (INT32 i = 0; i < c_n32MaxAbsorbSkillNum; ++i){
			if (eObjectType_None != pHero->GetAbsorbMonsterID(i)){
				absorbSlot = i;
			}
		}
		return eNormal == pAI->AskAbsorbMonster(absorbSlot);
	}
};