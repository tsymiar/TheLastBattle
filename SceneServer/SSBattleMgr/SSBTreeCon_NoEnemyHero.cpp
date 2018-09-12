#include "SSBTreeCon_NoEnemyHero.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_NoEnemyHero::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		vector<CSSGameUnit*> vecAreaObj; 
		pHero->GetCurBattle()->FindAroundGU(pHero->GetCurPos(),(FLOAT)m_parameters[0]*100,vecAreaObj);
		for(auto  iter = vecAreaObj.begin(); iter!=vecAreaObj.end();iter++){
			CSSGameUnit* pGU = *iter;
			if(pGU->IsDead()) continue;
			if(FALSE == pGU->IfHero()) continue;
			if(pGU->IfEnemy(pHero)){
				return FALSE;
			}
		}
		return TRUE;
	}

};