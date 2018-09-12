#include "SSBTreeCon_MonsterType.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSNPC.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_MonsterType::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		vector<CSSGameUnit*> vecAreaObj;
		CSSGameUnit* pGU = pHero->GetCurBattle()->GetGameUnitByGUID(pHero->GetLockedTargetGUID());
		if(pGU == NULL) return FALSE;
		CSSNPC* pNPC = pGU->TryCastToNPC();
		if(pNPC == NULL) return FALSE;
		if(pNPC->IfWildNPC() == FALSE || pNPC->GetBelongAltar() > 0) return FALSE;
		if( (pNPC->GetNPCCfg()->eNPCCateChild == eNPCChild_NPC_Per_Small && m_parameters[0] == 1) || (pNPC->GetNPCCfg()->eNPCCateChild == eNPCChild_NPC_Per_Big && m_parameters[0] == 2)){
			return TRUE;
		}
		return FALSE;
	}

};