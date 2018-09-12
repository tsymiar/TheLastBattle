#include "SSBTreeCon_CheckAttackTarget.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_CheckAttackTarget::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		return pAI->GetAttackGU() != NULL && pHero->GetLockedTargetGUID() == pAI->GetAttackGU()->GetObjGUID();
	}

};