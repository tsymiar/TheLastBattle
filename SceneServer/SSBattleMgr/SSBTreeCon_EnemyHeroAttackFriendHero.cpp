#include "SSBTreeCon_EnemyHeroAttackFriendHero.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_EnemyHeroAttackFriendHero::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		vector<CSSGameUnit*> vecAreaObj;
		pHero->GetCurBattle()->FindAroundGU(pHero->GetCurPos(),(FLOAT)m_parameters[0]*100+1000,vecAreaObj);
		vector<CSSHero*> enemyHeroVec;
		for(auto  iter = vecAreaObj.begin(); iter!=vecAreaObj.end();iter++){
			CSSGameUnit* pGU = *iter;
			if(pGU->IsDead()) continue;
			if(pGU == pHero) continue;
			if(FALSE == pGU->IfHero()) continue;
			if(pGU->IfEnemy(pHero)) continue;
			CSSHero* pFriendHero = pGU->TryCastToHero();
			if(pFriendHero->IsDead()) continue;
			map<CSSGameUnit*,INT32> hurtMap;
			pFriendHero->GetHurtMeList(3000,&hurtMap);
			for(auto iter = hurtMap.begin(); iter!= hurtMap.end();iter++){
				if(iter->first->IfHero()){
					enemyHeroVec.push_back(iter->first->TryCastToHero());
				}
			}
		}

		for(auto  iter = vecAreaObj.begin(); iter!=vecAreaObj.end();iter++){
			CSSGameUnit* pGU = *iter;
			if(pGU == pHero) continue;
			if(pGU->IsDead()) continue;
			for( auto heroIter = enemyHeroVec.begin();heroIter != enemyHeroVec.end(); heroIter++){
				CSSHero* pEnemyHero = *heroIter;
				if(pEnemyHero->IsDead()) continue;
				if(pEnemyHero == pGU && pGU->GetCurPos().CanWatch(m_parameters[0]*100,pEnemyHero->GetCurPos())){
					return TRUE;
				}
			}
		}
		return FALSE;
	}

};