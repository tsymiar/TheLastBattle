#include "SSBTreeCon_HasItemToBuy.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HasItemToBuy::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		const SHeroCfg* pHeroCfg = pHero->GetHeroCfg();
		return pHeroCfg->aun32BuyItemList[pAI->GetNextBuyItemIndex()] > 0;
	}

};