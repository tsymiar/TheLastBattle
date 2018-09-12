#include "SSBTreeAct_UseSkill.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{
	BOOLEAN	CSSBTreeAct_UseSkill::Action(CSSAI_HeroRobot* pAI){
		pAI->SetLastActionTime(GetUTCMiliSecond());
		CSSSkill* pSkill = pAI->GetPrepareSkill();
		if(NULL == pSkill) return FALSE;
		//ELOG(LOG_DBBUG,"Action: Use Skill (%d)",pSkill->cpsCfg->un32SkillID);
		CSSHero* pHero = pAI->GetHero();
		BOOLEAN ifUseSkill = FALSE;
		SGUID_KEY oldLockedGUID = pHero->GetLockedTargetGUID();
		switch(m_parameters[0]){
		case 1:
			pHero->SetLockedTargetGUID(pHero->GetObjGUID());
		case 4:
			ifUseSkill = TRUE;
			break;
		case 2:
			{
				CSSGameUnit* pTar = pHero->GetCurBattle()->GetGameUnitByGUID(pHero->GetLockedTargetGUID());
				if(NULL == pTar) return FALSE;
				if(FALSE == pHero->IfEnemy(pTar)) return FALSE;
				ifUseSkill = TRUE;
				break;
			}
		case 3:
			{
				CSSGameUnit* pTar = pHero->GetCurBattle()->GetGameUnitByGUID(pHero->GetLockedTargetGUID());
				if(NULL == pTar) return FALSE;
				if(TRUE == pHero->IfEnemy(pTar)) return FALSE;
				ifUseSkill = TRUE;
				break;
			}
		default:
			return FALSE;
		}
		if(TRUE == ifUseSkill){
			//检查是否物品技能
			map<CSSSkill*,UINT8>* pGoodsSkillMap = pHero->GetGoodsSkillMap();
			auto iter = pGoodsSkillMap->find(pSkill);
			INT32 rst = eNormal;
			if(iter != pGoodsSkillMap->end()){
				rst = pHero->AskUseGoods(iter->second);
			}
			else{
				rst =  pAI->AskUseSkill(pSkill->cpsCfg->un32SkillID);
			}
			if(oldLockedGUID > 0 && oldLockedGUID != pHero->GetLockedTargetGUID() ){
				pHero->SetLockedTargetGUID(oldLockedGUID);
			}
			return eNormal == rst;
		}
		
		return FALSE;
	}
};