/*
* file name			:SSBTreeAct_BuyItem.h
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

	class CSSBTreeAct_BuyItem : public CSSBTreeAction{
	private:

	public:
		CSSBTreeAct_BuyItem(INT32 id, INT32 parameters[]):CSSBTreeAction(id,parameters){}
		~CSSBTreeAct_BuyItem(){};

		BOOLEAN	Action(CSSAI_HeroRobot* pAI);
		BOOLEAN	IfNeedTime(CSSAI_HeroRobot* pAI){return FALSE;}
	};

};