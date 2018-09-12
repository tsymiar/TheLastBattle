#include "SSBTreeAct_BuyItem.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{
	BOOLEAN	CSSBTreeAct_BuyItem::Action(CSSAI_HeroRobot* pAI){
		//ELOG(LOG_DBBUG,"Action: Buy Item");
		CSSHero* pHero = pAI->GetHero();
		const SHeroCfg* pHeroCfg = pHero->GetHeroCfg();
		UINT32 itemID =  pHeroCfg->aun32BuyItemList[pAI->GetNextBuyItemIndex()];
		if(itemID == 0) return FALSE;
		INT32  goodsId = 0;
		INT32 rst = pHero->AskBuyGoods(itemID, goodsId);
		pAI->OnBoughtOneItem();
		return rst  == eNormal;
	}
};