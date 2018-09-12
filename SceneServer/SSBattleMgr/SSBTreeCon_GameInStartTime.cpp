#include "SSBTreeCon_GameInStartTime.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"
#include "SSBattle.h"

namespace SceneServer{

	BOOLEAN	CSSBTreeCon_GameInStartTime::Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
		CSSHero* pHero = pAI->GetHero();
		if(pAI->GetStartWaitTime() == 0) pAI->SetStartWaitTime(pAI->Random(1000,15000));
		return GetUTCMiliSecond() - pHero->GetCurBattle()->GetBattleStateStartTime() < pAI->GetStartWaitTime();
	}

};