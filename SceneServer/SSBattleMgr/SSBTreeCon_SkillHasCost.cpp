#include "SSBTreeCon_SkillHasCost.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_SkillHasCost::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		vector<CSSSkill*> *pSkillVec = pAI->GetSkillVec();
		for(auto iter = pSkillVec->begin(); iter != pSkillVec->end(); ){
			CSSSkill* pSkill = *iter;
			if(eNormal == pSkill->CheckConsume()){
				iter++;
			}
			else{
				iter = pSkillVec->erase(iter);
			}
		}
		return FALSE == pSkillVec->empty();
	}

};