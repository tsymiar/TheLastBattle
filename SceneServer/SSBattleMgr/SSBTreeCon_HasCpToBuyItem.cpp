#include "SSBTreeCon_HasCpToBuyItem.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_HasCpToBuyItem::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		const SHeroCfg* pHeroCfg = pHero->GetHeroCfg();
		UINT32 itemID =  pHeroCfg->aun32BuyItemList[pAI->GetNextBuyItemIndex()];
		if(itemID == 0) return FALSE;
		INT32 needCP = pHero->GetBuyGoodsCp(itemID);
		return needCP >= 0 && needCP <= pHero->GetCP(eCPCP_Personnal);
	}

};