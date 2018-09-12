#include "SSBTreeCon_HaveSkillByType.h"
#include "SSAI_HeroRobot.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HaveSkillByType::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		vector<CSSSkill*> *pSkillVec = pAI->GetSkillVec();
		for(auto iter = pSkillVec->begin(); iter != pSkillVec->end(); ){
			CSSSkill* pSkill = *iter;
			bool ifHasType = false;
			for(int i=0;i<16;i++){
				INT32 type = m_parameters[i];
				if(type == 0) break;
				if(type == 1 && ( pSkill->cpsCfg->eSkillTargetCate == eSTC_Self || pSkill->cpsCfg->eSkillTargetCate == eSTC_TeamMember || pSkill->cpsCfg->eSkillTargetCate == eSTC_AllObject) ){
					ifHasType = TRUE;
					break;
				}
				if(type == 2 ){
					ifHasType = TRUE;
					break;
				}
				if(type == 3 && (pSkill->cpsCfg->eSkillTargetCate == eSTC_Enemy || pSkill->cpsCfg->eSkillTargetCate == eSTC_AllObject)){
					ifHasType = TRUE;
					break;
				}
				if(type == 4 && (pSkill->cpsCfg->eSkillTargetCate == eSTC_TeamMember || pSkill->cpsCfg->eSkillTargetCate == eSTC_TeamMember_Without_Self || pSkill->cpsCfg->eSkillTargetCate == eSTC_AllObject || pSkill->cpsCfg->eSkillTargetCate == eSTC_AllFriendUnit)){
					ifHasType = TRUE;
					break;
				}
			}
			if(FALSE == ifHasType){
				iter = pSkillVec->erase(iter);
			}
			else{
				iter++;
			}
		}

		return FALSE == pSkillVec->empty();
	}

};