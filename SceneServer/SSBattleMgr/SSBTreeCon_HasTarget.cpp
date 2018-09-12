#include "SSBTreeCon_HasTarget.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HasTarget::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		vector<CSSGameUnit*> vecAreaObj;
		pHero->GetCurBattle()->FindAroundGU(pHero->GetCurPos(),(FLOAT)m_parameters[0]*100,vecAreaObj);
		map<float,CSSGameUnit*> distMap;
		for(auto  iter = vecAreaObj.begin(); iter!=vecAreaObj.end();iter++){
			CSSGameUnit* pGU = *iter;
			if(pGU->IsDead()) continue;
			if((m_parameters[1] == 1 && TRUE == pGU->IfNPC_Building() && pGU->IfEnemy(pHero))
				|| (m_parameters[1] == 2 && TRUE == pGU->IfHero() && pGU->IfEnemy(pHero))
				|| (m_parameters[1] == 3 && TRUE == pGU->IfSolider() && pGU->IfEnemy(pHero))
				|| (m_parameters[1] == 4 && TRUE == pGU->IfWildNPC())
				){
					float dist = pHero->GetCurPos().GetWatchDistSqr(pGU->GetCurPos());
					distMap[dist] = pGU;
			}
		}
		if(distMap.size() > 0){
			pHero->SetLockedTargetGUID(distMap.begin()->second->GetObjGUID());
			return TRUE;
		}
		return FALSE;
	}

};