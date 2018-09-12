/*
* file name			:SSBTreeCondition.h
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
#include "SSBTreeNode.h"

namespace SceneServer{
	class CSSHero;

	class CSSBTreeCondition : public CSSBTreeNode{
	private:

	protected:
		INT32 m_parameters[16];
	public:
		CSSBTreeCondition(INT32 id, INT32 parameters[]):CSSBTreeNode(id, eBTreeNodeType_Condition){memcpy(m_parameters,parameters,16*sizeof(INT32));}
		virtual ~CSSBTreeCondition(){};

		virtual BOOLEAN	Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec) = 0;
	};

};