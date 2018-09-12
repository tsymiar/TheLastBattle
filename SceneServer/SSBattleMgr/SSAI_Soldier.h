/*
* file name			:SSAI_Soldier.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:Octorber 13 2014
* summary			:
*
*/

#pragma once

#include "stdafx.h"
#include "SSAI.h"
#include "SSGameUnit.h"

namespace SceneServer{
	class CSSSkill;
	class CSSSoldier;
	class CSSNPC;

	class CSSAI_Soldier : public CSSAI
	{
	private:
		enum	EOccupyState{
			eOccupyState_Walk,
			eOccupyState_Attack,
			eOccupyState_Back
		};
		EOccupyState			m_eOccupyState;//巡逻状态

		CSSNPC*					m_pcSoldierGU;	//小兵

		CVector3D				m_acOrderPath[c_n32MaxOrderPathNode];//寻路节点
		INT32					m_n32NodeNum;//节点数量
		INT32					m_n32CurNodeIdx;//当前运行到的路点序号

		bool					m_ifNeedRestartMove;//是否需要重新开始移动
		CVector3D				m_cStartAttackPos;//开始进行攻击时候的起点位置
		CVector3D				m_cBackToLinePos;//回到线上的目标点
		TIME_TICK				m_tLastWatchEnemyMilsec;//上次检查目标的时间
		bool					m_ifAttackTarDead;//是否攻击目标死亡了

		void					SetNowPathIndex(INT32 index){m_n32CurNodeIdx = index;if (m_n32CurNodeIdx >= m_n32NodeNum)m_n32CurNodeIdx = m_n32NodeNum - 1;}
		void					SetAttackTarget(CSSGameUnit* pTarget);
		bool					IfPointNearPath(CVector3D cPoint, CVector3D cPath1, CVector3D cPath2, float &distSqrt, CVector3D &nearesatPoint);
	protected:

	public:
		~CSSAI_Soldier(){}
		CSSAI_Soldier(CSSGameUnit* pGU);

		void				SetOccupyPath(std::vector<CVector3D> & pathNode);//(const CVector3D *cpcPathNode, UINT32 un32NodeNum);
		INT32				HeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	};

}