/*
* file name			:SSBTreeCon_SkillTypeCheck.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:December 2 2014
* summary			:
*
*/
#pragma once
#include "stdafx.h"
#include "SSBTreeCondition.h"

namespace SceneServer{
	class CSSHero;

	class CSSBTreeCon_SkillTypeCheck : public CSSBTreeCondition{
	private:

	public:
		CSSBTreeCon_SkillTypeCheck(INT32 id, INT32 parameters[]):CSSBTreeCondition(id,parameters){}
		~CSSBTreeCon_SkillTypeCheck(){};

		BOOLEAN	Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec);
	};

};