#include "SSBTreeCon_FullFury.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_FullFury::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		return pHero->GetFuryState() != eFuryRunState && pHero->GetFuryValue() >= c_un32PerFuryValue;
	}

};