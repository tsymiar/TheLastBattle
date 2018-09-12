/*
* file name			:SSBTreeNode.h
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

namespace SceneServer{

	struct SBTreeResult{
		INT32 id;
		bool	rst;
		SBTreeResult():id(0), rst(false){}
		SBTreeResult(INT32 id, bool rst):id(id), rst(rst){}
	};

	class CSSAI_HeroRobot;

	enum EBTreeNodeType{
		eBTreeNodeType_Selector = 0,
		eBTreeNodeType_Sequence,
		eBTreeNodeType_Condition,
		eBTreeNodeType_Action,
	};
	class CSSBTreeNode{
	protected:
		INT32					m_id;
		EBTreeNodeType			m_type;
		list<CSSBTreeNode*> m_children;

		BOOLEAN					IfLeafNode(){return m_type == eBTreeNodeType_Condition || m_type == eBTreeNodeType_Action;}
		
	public:
		CSSBTreeNode(INT32 id, EBTreeNodeType eType):m_id(id), m_type(eType){}
		virtual ~CSSBTreeNode(){
			for(auto iter = m_children.begin(); iter != m_children.end();){
				CSSBTreeNode* pNode = *iter;
				delete pNode;
				iter = m_children.erase(iter);
			}
		};

		INT32					GetID(){return m_id;}
		void					AddChild(CSSBTreeNode* pChild){
			if(pChild->GetType() == eBTreeNodeType_Action){
				m_children.push_back(pChild);
				return;
			}
			else{
				for(auto iter = m_children.begin(); iter != m_children.end(); iter++){
					CSSBTreeNode* pNode = *iter;
					if(pNode->GetType() != eBTreeNodeType_Condition && pChild->GetType() == eBTreeNodeType_Condition){
						m_children.insert(iter,pChild);
						return;
					}
					if(pNode->GetType() == eBTreeNodeType_Action && pChild->GetType() <= eBTreeNodeType_Sequence){
						m_children.insert(iter,pChild);
						return;
					}
				}
				m_children.push_back(pChild);
				return;
			}
			
		}
		EBTreeNodeType			GetType(){return m_type;}

		virtual	BOOLEAN			Travel(CSSAI_HeroRobot* pAI,CSSBTreeNode *&pActionNode,vector<SBTreeResult> *rstVec) = 0;
	};

};