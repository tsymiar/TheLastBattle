#include "SSBTreeCon_Absorbing.h"
#include "SSAI_HeroRobot.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_Absorbing::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		return pAI->IfAbsorbing();
	}

};