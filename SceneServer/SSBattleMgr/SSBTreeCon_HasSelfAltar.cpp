#include "SSBTreeCon_HasSelfAltar.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSNPC.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HasSelfAltar::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		vector<CSSGameUnit*> vecAreaObj;
		pHero->GetCurBattle()->FindAroundGU(pHero->GetCurPos(),(FLOAT)m_parameters[0]*100,vecAreaObj);
		for(auto  iter = vecAreaObj.begin(); iter!=vecAreaObj.end();iter++){
			CSSGameUnit* pGU = *iter;
			if(FALSE == pGU->IfNPC() || FALSE == pGU->IfNPC_Building() || pGU->IfEnemy(pHero)) continue;
			CSSNPC* pNPC = pGU->TryCastToNPC();
			if(NULL == pNPC) continue;
			if(pNPC->GetNPCCfg()->eNPCCateChild != eNPCChild_BUILD_Altar) continue;
			if(m_parameters[1] == 1 && TRUE  == pNPC->IfAltarHasMonster()) continue;
			if(m_parameters[1] == 2 && FALSE == pNPC->IfAltarHasMonster()) continue;
			pHero->SetLockedTargetGUID(pNPC->GetObjGUID());
			return TRUE;
		}
		return FALSE;
	}

};