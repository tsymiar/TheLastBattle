#include "SSBTreeCon_HasCpForAbsorb.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSNPC.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HasCpForAbsorb::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		CSSGameUnit* pGU = pHero->GetCurBattle()->GetGameUnitByGUID(pHero->GetLockedTargetGUID());
		if(pGU == NULL) return FALSE;
		CSSNPC* pNPC = pGU->TryCastToNPC();
		if(pNPC == NULL) return FALSE;
		if (pHero->GetCP(eCPCP_Personnal) >= pAI->GetAbsorbCost(pNPC)){
			return TRUE;
		}
		return FALSE;
	}

};