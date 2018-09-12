/*
* file name			:SSBTreeAct_TransHome.h
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

	class CSSBTreeAct_TransHome : public CSSBTreeAction{
	private:

	public:
		CSSBTreeAct_TransHome(INT32 id, INT32 parameters[]):CSSBTreeAction(id,parameters){}
		~CSSBTreeAct_TransHome(){};

		BOOLEAN	Action(CSSAI_HeroRobot* pAI);
	};

};