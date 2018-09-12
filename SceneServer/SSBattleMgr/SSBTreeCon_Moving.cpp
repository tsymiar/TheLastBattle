#include "SSBTreeCon_Moving.h"
#include "SSAI_HeroRobot.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_Moving::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		bool ifMoving = pAI->IfMoving();
		return ifMoving;
	}

};