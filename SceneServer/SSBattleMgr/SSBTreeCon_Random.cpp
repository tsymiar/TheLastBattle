#include "SSBTreeCon_Random.h"
#include "SSAI_HeroRobot.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_Random::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		return m_parameters[0] <= pAI->Random(0,100);
	}
};