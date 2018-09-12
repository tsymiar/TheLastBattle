#include "SSBTreeCon_AutoAttacking.h"
#include "SSAI_HeroRobot.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_AutoAttacking::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		bool rst =  pAI->IfAutoAttacking();
		return rst;
	}

};