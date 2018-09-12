#include "SSBTreeCon_HasAbsorbMonster.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HasAbsorbMonster::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		for (INT32 i = 0; i < c_n32MaxAbsorbSkillNum; ++i){
			if (eObjectType_None != pHero->GetAbsorbMonsterID(i)){
				return TRUE;
			}
		}
		return FALSE;
	}

};