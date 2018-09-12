/*
* file name			:SSBTreeAct_Wait.h
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

	class CSSBTreeAct_Wait : public CSSBTreeAction{
	private:

	public:
		CSSBTreeAct_Wait(INT32 id, INT32 parameters[]):CSSBTreeAction(id,parameters){}
		~CSSBTreeAct_Wait(){};

		BOOLEAN	Action(CSSAI_HeroRobot* pAI){
			return TRUE;
		}
	};

};