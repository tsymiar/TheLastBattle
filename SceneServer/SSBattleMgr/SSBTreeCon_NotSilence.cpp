#include "SSBTreeCon_NotSilence.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_NotSilence::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		return pHero->GetFPData(eEffectCate_Silence) <= 0;
	}

};