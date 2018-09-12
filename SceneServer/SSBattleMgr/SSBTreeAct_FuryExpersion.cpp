#include "SSBTreeAct_FuryExpersion.h"
#include "SSAI_HeroRobot.h"
#include "SSHero.h"

namespace SceneServer{
	BOOLEAN	CSSBTreeAct_FuryExpersion::Action(CSSAI_HeroRobot* pAI){
		//ELOG(LOG_DBBUG,"Action: Fury Expersion");
		CSSHero* pHero = pAI->GetHero();
		return eNormal == pAI->AskGasExpersion();
	}
};