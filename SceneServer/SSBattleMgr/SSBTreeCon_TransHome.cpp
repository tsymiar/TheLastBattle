#include "SSBTreeCon_TransHome.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_TransHome::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSSkill* pSkill = pAI->GetNowSkill();
		bool rst = pSkill != NULL && pSkill->ifRunning && pSkill->cpsCfg != NULL && pSkill->cpsCfg->un32SkillID == c_transHomeSkillID;
		if(rst == TRUE){
			int abc = 0;
		}
		return rst;
	}

};