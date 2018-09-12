/*
* file name			:SSBTreeSelector.h
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
	class CSSBTreeSelector : public CSSBTreeNode{
	private:

	public:
		CSSBTreeSelector(INT32 id):CSSBTreeNode(id, eBTreeNodeType_Selector){}
		virtual ~CSSBTreeSelector(){};

		BOOLEAN			Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
			if(6038 == GetID()){
				int abc = 0;
			}
			CSSBTreeNode* pTempActionNode = NULL;
			for(auto iter = m_children.begin(); iter != m_children.end(); iter++){
				CSSBTreeNode* pNode = (*iter);
				SBTreeResult rst;
				rst.id = pNode->GetID();
				if(pNode->GetType() == eBTreeNodeType_Condition){
					if(FALSE == pNode->Travel(pAI,pActionNode,rstVec)){
						rst.rst = false;
						rstVec->push_back(rst);
						return FALSE;
					}
				}
				else if(pNode->GetType() == eBTreeNodeType_Action){
					pTempActionNode = pNode;
				}
				else if(TRUE == pNode->Travel(pAI,pActionNode,rstVec)){
					if(pTempActionNode != NULL){
						pActionNode = pTempActionNode;
					}
					rst.rst = true;
					rstVec->push_back(rst);
					return TRUE;
				}
			}
			if(pTempActionNode != NULL){
				pActionNode = pTempActionNode;
				SBTreeResult rst;
				rst.id = GetID();
				rst.rst = true;
				rstVec->push_back(rst);
				return TRUE;
			}
			SBTreeResult rst;
			rst.id = GetID();
			rst.rst = false;
			rstVec->push_back(rst);
			return FALSE;
		}
	};

};