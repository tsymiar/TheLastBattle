/*
* file name			:SSBTreeSequence.h
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
	class CSSBTreeSequence : public CSSBTreeNode{
	private:

	public:
		CSSBTreeSequence(INT32 id):CSSBTreeNode(id, eBTreeNodeType_Sequence){}
		virtual ~CSSBTreeSequence(){};

		BOOLEAN			Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec){
			UINT8	nodeNum = 0;
			CSSBTreeNode* pTempActionNode = NULL;
			for(auto iter = m_children.begin(); iter != m_children.end(); ++iter){
				SBTreeResult rst;
				rst.id = (*iter)->GetID();
				if((*iter)->GetType() == eBTreeNodeType_Action){
					pTempActionNode = *iter;
				}
				else{
					nodeNum++;
				}
				if(FALSE == (*iter)->Travel(pAI,pActionNode,rstVec)){
					rst.rst = false;
					rstVec->push_back(rst);
					return FALSE;
				}
			}
			if(pTempActionNode != NULL){
				pActionNode = pTempActionNode;
			}
			if(NULL == pActionNode){ 
				SBTreeResult rst(GetID(), false);
				rstVec->push_back(rst);
				return FALSE;
			}
			if(nodeNum == 0){ 
				SBTreeResult rst(GetID(), false);				
				rstVec->push_back(rst);
				return FALSE;
			}
			SBTreeResult rst(GetID(), true);
			rstVec->push_back(rst);
			return TRUE;
		}
	};

};