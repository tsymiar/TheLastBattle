/*
* file name			:SSSkillModel_Account.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:May 29 2014
* summary			:
*
*/
#pragma once

#include "stdafx.h"
#include "SSSkillEffect.h"

namespace SceneServer{

class CSSSkillEffect_Caculate : public CSSSkillEffect
{
private:
	INT32		NotifySkillModelHitObjectToGC();

public:
	CSSSkillEffect_Caculate();

public:
	INT32		Begin();
	INT32		Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	INT32		End();

	static INT32	CaculateSkillEffectOnce(CSSGameUnit *pcMasterGO, CSSGameUnit *pcTarGU, const SEffectInfo &rsEffectInfo, UINT32 skillID, bool bIfNormalAtk = FALSE, bool ifAdd = TRUE);
};

}