#include "SSBTreeCon_SkillInDist.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"
namespace SceneServer{

	BOOLEAN	CSSBTreeCon_SkillInDist::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		CSSGameUnit* pTar = pHero->GetCurBattle()->GetGameUnitByGUID(pHero->GetLockedTargetGUID());
		if(NULL == pTar) return FALSE;
		if(m_parameters[0] == 1){
			//ÅÐ¶ÏÆÕÍ¨¹¥»÷¾àÀë
			float dist = pHero->GetFPData(eEffectCAte_AttackDist);
			return pHero->GetCurPos().CanWatch(dist, pTar->GetCurPos());
		}
		else if(m_parameters[0] == 2){
			vector<CSSSkill*> *pSkillVec = pAI->GetSkillVec();
			for(auto iter = pSkillVec->begin(); iter != pSkillVec->end(); ){
				CSSSkill* pSkill = *iter;
				float dist = pSkill->cpsCfg->fReleaseDist;
				if(TRUE == pHero->GetCurPos().CanWatch(dist, pTar->GetCurPos())){
					iter++;
				}
				else{
					iter = pSkillVec->erase(iter);
				}
			}
			return FALSE == pSkillVec->empty();
		}
		return FALSE;
	}

};