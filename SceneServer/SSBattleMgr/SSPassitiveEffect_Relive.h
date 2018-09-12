/*
* file name			:SSPassiveEffect_Relive.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:June 27 2014
* summary			:
*
*/
#pragma once

#include "stdafx.h"
#include <map>
#include "SSPassitiveEffect.h"

namespace SceneServer{

class CSSGameUnit;

class CSSPassiveEffect_Relive : public CSSPassiveEffect
{
private:
	TIME_MILSEC					m_tStartReliveTime;

public:

	const SSPassiveEffectCfg_Relive* m_pCfg;

	CSSPassiveEffect_Relive();
	~CSSPassiveEffect_Relive();

	INT32						OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	void						Trigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1 = 0, INT32 para2 = 0, INT32 para3 = 0);
	bool						IfLongTimeEffect(){return TRUE;}
};

}