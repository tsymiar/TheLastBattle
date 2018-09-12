/*
* file name			:SSSkillModel_Purification.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:Novenber 05 2014
* summary			:
*
*/
#pragma once

#include "stdafx.h"
#include "SSSkillEffect.h"

namespace SceneServer{

	class CSSSkillEffect_Purification : public CSSSkillEffect
	{
	private:
		INT32		NotifySkillModelPurificationToGC();

	public:
		CSSSkillEffect_Purification();

	public:
		INT32		Begin();
		INT32		Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
		INT32		End();
	};

}