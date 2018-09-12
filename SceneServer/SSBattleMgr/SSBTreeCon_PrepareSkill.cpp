#include "SSBTreeCon_PrepareSkill.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_PrepareSkill::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		vector<CSSSkill*> *pSkillVec = pAI->GetSkillVec();
		pSkillVec->clear();
		SGoodsRunTimeData* pGoodArr = pHero->GetGoodRunTimeDateArr();
		for(int i=0;i<c_un32MaxGridNum;i++){
			SGoodsRunTimeData* pGood = &pGoodArr[i];
			if(pGood->sGoodsCfg != NULL && pGood->pSkill != NULL && pGood->pSkill->cpsCfg != NULL &&  pGood->n32Num > 0){
				pSkillVec->push_back(pGood->pSkill);
			}
		}
		CSSSkill** pSkillArr = pHero->GetSkillArr();
		for(int i=0;i<c_n32MaxHeroSkillNum;i++){
			CSSSkill* pSkill = pSkillArr[i];
			if(pSkill != NULL && pSkill->cpsCfg != NULL && pSkill->cpsCfg->un32SkillID != c_transHomeSkillID && FALSE == pSkill->cpsCfg->bIfNomalAttack){
				if((pHero->GetFuryState() != eFuryRunState && i<= 1) || (pHero->GetFuryState() == eFuryRunState && i>=2 && i <=3))
					pSkillVec->push_back(pSkill);
			}
		}
		AbsorbRuntimeInfo* pAbsorbArr = pHero->GetAbsorRunTimeArr();
		for(int i=0;i<c_n32MaxAbsorbSkillNum;i++){
			AbsorbRuntimeInfo* pAbsorb = &pAbsorbArr[i];
			if(pAbsorb->pSkill != NULL && pAbsorb->pSkill->cpsCfg != NULL && pAbsorb->sNPCCfg != NULL){
				pSkillVec->push_back(pAbsorb->pSkill);
			}

		}
		return FALSE == pSkillVec->empty();
	}

};