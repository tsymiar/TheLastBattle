#include "SSBTreeCon_Control.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

BOOLEAN	CSSBTreeCon_Control::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
	CSSHero* pHero = pAI->GetHero();
	return pHero->GetGOActionStateInfo().eOAS >= eOAS_PassiveState;
}

};