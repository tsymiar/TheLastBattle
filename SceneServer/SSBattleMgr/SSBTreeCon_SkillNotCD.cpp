#include "SSBTreeCon_SkillNotCD.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_SkillNotCD::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		vector<CSSSkill*>* pSkillVec = pAI->GetSkillVec();
		for(auto iter = pSkillVec->begin(); iter != pSkillVec->end(); ){
			CSSSkill* pSkill = *iter;
			if(eNormal == pSkill->IfInCD()){
				iter++;
			}
			else{
				iter = pSkillVec->erase(iter);
			}
		}
		return FALSE == pSkillVec->empty();
	}

};