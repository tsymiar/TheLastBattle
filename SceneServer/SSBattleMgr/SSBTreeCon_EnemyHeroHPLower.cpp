#include "SSBTreeCon_EnemyHeroHPLower.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_EnemyHeroHPLower::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		CSSGameUnit* pTar = pHero->GetCurBattle()->GetGameUnitByGUID(pHero->GetLockedTargetGUID());
		if(pTar == NULL) return FALSE;
		if(pTar->IsDead()) return FALSE;
		if(FALSE == pTar->IfHero()) return FALSE;
		if(FALSE == pTar->IfEnemy(pHero)) return FALSE;
		return (float)pTar->GetCurHP()/pTar->GetFPData(eEffectCate_MaxHP)*100 < m_parameters[0];
	}

};