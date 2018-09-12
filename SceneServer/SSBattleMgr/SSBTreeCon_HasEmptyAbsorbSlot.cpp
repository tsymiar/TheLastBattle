#include "SSBTreeCon_HasEmptyAbsorbSlot.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HasEmptyAbsorbSlot::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		if(pHero->GetLevel() < 3) return FALSE;
		UINT8	absorbNum = 0;
		for (INT32 i = 0; i < c_n32MaxAbsorbSkillNum; ++i){
			if (eObjectType_None != pHero->GetAbsorbMonsterID(i)){
				absorbNum++;
			}
		}
		if( absorbNum >= 2) return FALSE;
		if(pHero->GetLevel() < 7 && absorbNum >= 1) return FALSE;
		return TRUE;
	}

};