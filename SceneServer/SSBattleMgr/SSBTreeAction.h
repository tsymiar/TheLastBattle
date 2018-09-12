/*
* file name			:SSBTreeAction.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:October 31 2014
* summary			:
*
*/
#pragma once
#include "stdafx.h"
#include "SSBTreeNode.h"

namespace SceneServer{
	class CSSHero;

	class CSSBTreeAction : public CSSBTreeNode{
	private:

	protected:
		INT32 m_parameters[16];

	public:
		CSSBTreeAction(INT32 id, INT32 parameters[]):CSSBTreeNode(id, eBTreeNodeType_Action){memcpy(m_parameters,parameters,16*sizeof(INT32));}
		virtual ~CSSBTreeAction(){};

		BOOLEAN	Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){return TRUE;}
		virtual BOOLEAN	Action(CSSAI_HeroRobot* pAI) = 0;
		virtual BOOLEAN	IfNeedTime(CSSAI_HeroRobot* pAI){return TRUE;}
	};

};