/*
* file name			:SSPassiveEffect_Rebound.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:June 24 2014
* summary			:
*
*/
#pragma once

#include "stdafx.h"
#include <map>
#include "SSPassitiveEffect.h"

namespace SceneServer{

class CSSGameUnit;

class CSSPassiveEffect_Rebound : public CSSPassiveEffect
{

public:

	const SSPassiveEffectCfg_Rebound* m_pCfg;

	CSSPassiveEffect_Rebound();
	~CSSPassiveEffect_Rebound();

	void						Trigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1 = 0, INT32 para2 = 0, INT32 para3 = 0);
};

}