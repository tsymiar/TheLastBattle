#include "SSBTreeCon_SkillTypeCheck.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_SkillTypeCheck::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		vector<CSSSkill*>* pSkillVec = pAI->GetSkillVec();
		for(auto iter = pSkillVec->begin(); iter != pSkillVec->end(); ){
			CSSSkill* pSkill = *iter;
			if(pSkill->cpsCfg->eSkillType == m_parameters[0]){
				iter++;
			}
			else{
				iter = pSkillVec->erase(iter);
			}
		}
		return FALSE == pSkillVec->empty();
	}

};