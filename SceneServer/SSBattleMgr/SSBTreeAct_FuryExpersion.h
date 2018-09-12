/*
* file name			:SSBTreeAct_FuryExpersion.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:November 11 2014
* summary			:
*
*/
#pragma once
#include "stdafx.h"
#include "SSBTreeAction.h"

namespace SceneServer{
	class CSSHero;

	class CSSBTreeAct_FuryExpersion : public CSSBTreeAction{
	private:

	public:
		CSSBTreeAct_FuryExpersion(INT32 id, INT32 parameters[]):CSSBTreeAction(id,parameters){}
		~CSSBTreeAct_FuryExpersion(){};

		BOOLEAN	Action(CSSAI_HeroRobot* pAI);
		virtual BOOLEAN	IfNeedTime(CSSAI_HeroRobot* pAI){return FALSE;}
	};

};