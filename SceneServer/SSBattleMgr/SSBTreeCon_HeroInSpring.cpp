#include "SSBTreeCon_HeroInSpring.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSCfgMgr.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HeroInSpring::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		return pHero->GetCurPos().CanWatch(500, pHero->GetHeroBornPos());
	}

};