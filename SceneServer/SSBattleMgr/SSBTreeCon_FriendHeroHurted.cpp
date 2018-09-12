#include "SSBTreeCon_FriendHeroHurted.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_FriendHeroHurted::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		vector<CSSGameUnit*> vecAreaObj;
		pHero->GetCurBattle()->FindAroundGU(pHero->GetCurPos(),(FLOAT)m_parameters[0]*100,vecAreaObj);
		for(auto  iter = vecAreaObj.begin(); iter!=vecAreaObj.end();iter++){
			CSSGameUnit* pGU = *iter;
			if(pGU == pHero) continue;
			if(pGU->IsDead()) continue;
			if(FALSE == pGU->IfHero()) continue;
			if(pGU->IfEnemy(pHero)) continue;
			CSSHero* pFriendHero = pGU->TryCastToHero();
			map<CSSGameUnit*,INT32> hurtMap;
			pFriendHero->GetHurtMeList(3000,&hurtMap);
			for(auto hurtIter = hurtMap.begin(); hurtIter!= hurtMap.end();hurtIter++){
				if(hurtIter->first->IfHero()){
					pHero->SetLockedTargetGUID(hurtIter->first->GetObjGUID());
					return TRUE;
				}
			}
		}
		return FALSE;
	}

};