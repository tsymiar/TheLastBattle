#include "SSBTreeAct_TransHome.h"
#include "SSAI_HeroRobot.h"

namespace SceneServer{
	BOOLEAN	CSSBTreeAct_TransHome::Action(CSSAI_HeroRobot* pAI){
		pAI->SetLastActionTime(GetUTCMiliSecond());
		//ELOG(LOG_DBBUG,"Action: Trans Home");
		return eNormal == pAI->AskUseSkill(c_transHomeSkillID);
	}
};