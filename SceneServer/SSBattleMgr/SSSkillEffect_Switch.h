/*
* file name			:SSSkillModel_Switch.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:June 25 2014
* summary			:
*
*/
#pragma once

#include "stdafx.h"
#include "SSSkillEffect.h"

namespace SceneServer{

class CSSkillModel_Switch : public CSSSkillEffect
{
public:
	CSSkillModel_Switch();

public:
	INT32		Begin();
	INT32		Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	INT32		End();
};

}