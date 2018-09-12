/*
* file name			:SSBTreeCon_HasAbsorbMonster.h
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
#include "SSBTreeCondition.h"

namespace SceneServer{
	class CSSHero;

	class CSSBTreeCon_HasAbsorbMonster : public CSSBTreeCondition{
	private:

	public:
		CSSBTreeCon_HasAbsorbMonster(INT32 id, INT32 parameters[]):CSSBTreeCondition(id,parameters){}
		~CSSBTreeCon_HasAbsorbMonster(){};

		BOOLEAN	Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec);
	};

};