#include "SSBTreeAct_NormalAttack.h"
#include "SSAI_HeroRobot.h"

namespace SceneServer{
	BOOLEAN	CSSBTreeAct_NormalAttack::Action(CSSAI_HeroRobot* pAI){
		pAI->SetLastActionTime(GetUTCMiliSecond());
		//ELOG(LOG_DBBUG,"Auto Attack");
		return eNormal == pAI->AskStartAutoAttack();
	}
};