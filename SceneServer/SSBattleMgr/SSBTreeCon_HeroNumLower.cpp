#include "SSBTreeCon_HeroNumLower.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HeroNumLower::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		vector<CSSGameUnit*> vecAreaObj; 
		pHero->GetCurBattle()->FindAroundGU(pHero->GetCurPos(),(FLOAT)m_parameters[0]*100,vecAreaObj);
		INT32 friendHero = 0;
		INT32 enemyHero = 0;
		for(auto  iter = vecAreaObj.begin(); iter!=vecAreaObj.end();iter++){
			CSSGameUnit* pGU = *iter;
			if(pGU->IsDead()) continue;
			if(FALSE == pGU->IfHero()) continue;
			if(pGU->IfEnemy(pHero)){
				enemyHero++;
			}
			else{
				friendHero++;
			}
		}
		return friendHero < enemyHero;
	}

};