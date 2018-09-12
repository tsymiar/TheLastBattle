#include "SSBTreeAct_PutAltarMonster.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSNPC.h"

namespace SceneServer{
	BOOLEAN	CSSBTreeAct_PutAltarMonster::Action(CSSAI_HeroRobot* pAI){
		//ELOG(LOG_DBBUG,"Action: Put Altar Monster");
		CSSHero* pHero = pAI->GetHero();
		EObjectType eOT = eObjectType_None;
		for (INT32 i = 0; i < c_n32MaxAbsorbSkillNum; ++i){
			eOT = (EObjectType)pHero->GetAbsorbMonsterID(i);
			if(eObjectType_None != eOT){
				break;
			}
		}
		if(eObjectType_None == eOT) return FALSE;

		CSSGameUnit* pTar = pHero->GetCurBattle()->GetGameUnitByGUID(pHero->GetLockedTargetGUID());
		return eNormal == pHero->GetCurBattle()->StrawSolder(pHero,pTar->GetObjGUID(),eOT);
	}
};