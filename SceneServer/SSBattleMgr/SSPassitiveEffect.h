/*
* file name			:SSPassiveEffect.h
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

namespace SceneServer{

class CSSGameUnit;

class CSSPassiveEffect
{

public:
	INT32						m_pEffectUniqueID;
	ref_ptr<CSSGameUnit>		m_pcMasterGO;
	bool						m_bIfKeeped;
	EPassiveSkillType			m_effectType;

	virtual INT32				OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){return eEC_EffectEnd;}
	virtual void				Trigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1 = 0, INT32 para2 = 0, INT32 para3 = 0) = 0;
	virtual bool				IfLongTimeEffect(){return FALSE;}

	CSSPassiveEffect();
	virtual ~CSSPassiveEffect();
};

}