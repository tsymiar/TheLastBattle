/*
* file name			:SSBTreeAct_MoveByPath.h
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

	class CSSBTreeAct_MoveByPath : public CSSBTreeAction{
	private:
		void	FindNearestPathNode(CSSAI_HeroRobot* pAI,INT32 &nearestPathIndex,INT32 &nearestNodeIndex);

	public:
		CSSBTreeAct_MoveByPath(INT32 id, INT32 parameters[]):CSSBTreeAction(id,parameters){}
		~CSSBTreeAct_MoveByPath(){};

		BOOLEAN	Action(CSSAI_HeroRobot* pAI);
	};

};