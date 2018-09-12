 

#ifndef SSSKILLEFFECT_SUMMON_H_
#define SSSKILLEFFECT_SUMMON_H_


#include "stdafx.h"
#include "SSSkillEffect.h"
 

namespace SceneServer{

class CSSGameUnit;
class CSSNPC;

class CSSSkillEffect_Summon : public CSSSkillEffect
{
private:   
	SSkillModuleSummonCfg* m_pSSkillModuleSummonCfg;

	INT32	   SendSummonMsg(CSSNPC* pBornNPC);

public:
	CSSSkillEffect_Summon();
	~CSSSkillEffect_Summon();

	virtual	INT32				Begin();
	virtual INT32				End();
	virtual INT32				Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan); 
};

}



#endif