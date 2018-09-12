#include "SSBTreeCon_SelfHPLower.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_SelfHPLower::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		return (float)pHero->GetCurHP()/pHero->GetFPData(eEffectCate_MaxHP)*100 < m_parameters[0];
	}

};