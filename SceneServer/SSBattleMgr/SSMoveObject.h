/*
* file name			:SSMoveObject.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:Auguest 6 2014
* summary			:
*
*/

#ifndef __ISSMoveObject_h__
#define __ISSMoveObject_h__

#include "stdafx.h"
#include "SSIntersectObject.h"

namespace SceneServer{

enum SSMoveObjectMoveType{
	SSMoveObjectMoveType_None,
	SSMoveObjectMoveType_Dir,
	SSMoveObjectMoveType_Tar,
	SSMoveObjectMoveType_ForceMove,
};

enum SSMoveObjectStatus{
	SSMoveObjectStatus_None,
	SSMoveObjectStatus_Stand,
	SSMoveObjectStatus_Move,
	SSMoveObjectStatus_ForceMove,
};

enum SSMoveObjectImpactType
{
	SSMoveObjectImpactType_Undefined = -1,
	SSMoveObjectImpactType_NoImpact = 0,
	SSMoveObjectImpactType_Group,
	SSMoveObjectImpactType_ALL,//野怪、建筑全碰撞//
};

enum SSImpactGruop
{
	SSImpactGruop_None,//默认//
	SSImpactGruop_Hero,
	SSImpactGruop_Solider,
	SSImpactGruop_Zhuzi,
	SSImpactGruop_Wild,//野怪，只和同类型碰撞
	SSImpactGruop_All,//建筑全碰撞//
};

class ISSMoveObject
{
private:
	SSMoveObjectImpactType	m_savedImpactType; //缓存的碰撞类型
	SSMoveObjectStatus	m_moveStatus;//移动状态(停止，移动中)
	SSMoveObjectMoveType m_moveType;//移动类型（方向，寻路，强制）
	UINT8				m_grpID;//组ID
	TIME_MILSEC			m_startMoveTime;//开始移动时间
	TIME_MILSEC			m_lastFailStartTime;//上次尝试移动失败时间
	FLOAT				m_oldSpeed;//缓存的速度
	bool				m_bIfSpeedChanged;//是否发生过速度改变
	FLOAT				m_forceMoveSpeed;//强制位移速度
	ColVector			m_askMoveDir;//请求移动的方向
	TIME_MILSEC			m_redirectTime;//上次尝试自动转向时间
	ColVector			m_dir;//实际移动方向
	ColSphere			m_stepMoveTarget;//自动寻路当前移动目标
	ColVector			m_beforeMovePos;//上一次位移的坐标
	SIntPoint			m_asPathCell[c_n32MaxOrderPathNode];//自动寻路的路径点
	UINT16				m_pathLength;//自动寻路路径长度
	UINT16				m_moveStep;//自动寻路当前坐标序列号
	bool				m_ifStepMoved;//未使用
	bool				m_ifForceMoveImpact;//强制位移是否需要检测碰撞


public:
	ISSMoveObject():m_moveType(SSMoveObjectMoveType_None)
		, m_moveStatus(SSMoveObjectStatus_None)
		, m_savedImpactType(SSMoveObjectImpactType_Undefined)
		, m_grpID(SSImpactGruop_None)
		, m_startMoveTime(0)
		, m_lastFailStartTime(0)
		, m_oldSpeed(0.0f)
		, m_bIfSpeedChanged(FALSE)
		, m_forceMoveSpeed(0.0f)
		, m_pathLength(0)
		, m_moveStep(0)
		, m_ifStepMoved(FALSE)
		, m_ifForceMoveImpact(FALSE){
		for (int i = 0; i < c_n32MaxOrderPathNode; ++i){
			m_asPathCell[i].Clear();
		}
	}
	virtual ~ISSMoveObject(){}

	//获取数据的抽象接口
	virtual bool				IfCanImpact() = 0;
	virtual ColSphere			GetColSphere() = 0;
	virtual ColVector			GetColVector() = 0;
	virtual INT16				GetColRadius() = 0;
	virtual FLOAT				GetSpeed() = 0;
	virtual SSMoveObjectImpactType		GetImpactType() = 0;

	//进行事件通知的抽象接口
	virtual void				OnStartMove(ColVector dir) = 0;
	virtual void				OnMoved(ColVector pos) = 0;
	virtual void				OnMoveBlock() = 0;
	virtual void				OnChangeDir(ColVector dir) = 0;

	bool						IfStart;


	//逻辑函数
	void				AddMoveStep(){++m_moveStep;}
	//获取当前自动寻路位移的目标点
	bool				GetNowMoveIntTar(SIntPoint &point){
		if(m_moveStep >= m_pathLength)
			return FALSE;

		point = m_asPathCell[m_moveStep];
		return TRUE;
	}
	//根据时间计算将要到达的点的坐标
	void				CalculateStepMoveTarget(TIME_MILSEC now){
		INT64 timeDiff = now - GetStartMoveTime();
		if(timeDiff > 500) 
			timeDiff = 500;
		if (timeDiff <= 0){
			//ELOG(LOG_WARNNING, "timeDiff:%lld", timeDiff);
			timeDiff = 0;
		}
		FLOAT speed = (m_moveStatus == SSMoveObjectStatus_ForceMove) ? m_forceMoveSpeed : GetSpeed();
		if(speed != m_oldSpeed){
			m_oldSpeed = speed;
			m_bIfSpeedChanged = TRUE;
		}
		ColVector moveVec = GetDir() * speed * timeDiff;
		float moveDist = moveVec.Length();
		if(moveDist > 200){
			int abc = 0;
		}
		ColSphere sph = GetColSphere();
		sph.c = sph.c + moveVec;
		Assert(sph.c.x < 100000 && sph.c.y < 100000 && sph.c.x > 0 && sph.c.y > 0);
		SetStepMoveTarget(sph);
	}
	//位移。该位移已经经过了验证，所以直接移动坐标
	FLOAT				Move(TIME_MILSEC now){
		SetBeforeMovePos(GetColVector());
		ColSphere sph = GetStepMoveTarget();
		FLOAT dist = (sph.c - GetBeforeMovePos()).Length();
		OnMoved(sph.c);
		SetStartMoveTime(now);
		SetStepMoved(TRUE);
		if(m_bIfSpeedChanged){
			m_bIfSpeedChanged = FALSE;
			OnStartMove(GetDir());
		}
		return dist;
	}
	//传送，和位移基本相同，但是不算是移动，不触发位移的回调
	void				Teleport(TIME_MILSEC now){
		ColSphere sph = GetStepMoveTarget();
		SetBeforeMovePos(sph.c);
		OnMoved(sph.c);
		SetStartMoveTime(now);
	}
	//停止移动，并回调停止的接口
	void				Stop(TIME_MILSEC now, bool ifNeedCallBack = TRUE){
		SetMoveStatus(SSMoveObjectStatus_Stand);
		SetLastFailStartTime(now);
		ColSphere nowPos = GetColSphere();
		SetStepMoveTarget(nowPos);
		SetBeforeMovePos(nowPos.c);
		if(ifNeedCallBack)
			OnMoveBlock();
	}
	//获取碰撞类型
	SSMoveObjectImpactType GetImpactType_Now(){if(m_savedImpactType == SSMoveObjectImpactType_Undefined){m_savedImpactType = GetImpactType();} return m_savedImpactType;}

	//所有的Get和Set方法
	void					SetMoveType(SSMoveObjectMoveType type){m_moveType = type;}
	SSMoveObjectMoveType	GetMoveType(){return m_moveType;}

	SSMoveObjectStatus  GetMoveStatus(){return m_moveStatus;}
	void  SetMoveStatus(SSMoveObjectStatus status){m_moveStatus = status;}

	UINT8				GetGroupID(){return m_grpID;}
	void				SetGroupID(UINT8 grpID){m_grpID = grpID;}

	TIME_MILSEC			GetStartMoveTime(){return m_startMoveTime;}
	void				SetStartMoveTime(TIME_MILSEC startMoveTime){m_startMoveTime = startMoveTime;}

	TIME_MILSEC			GetLastFailStartTime(){return m_lastFailStartTime;}
	void				SetLastFailStartTime(TIME_MILSEC lastFailStartTime){m_lastFailStartTime = lastFailStartTime;}

	ColVector&			GetDir(){return m_dir;}
	void				SetDir(ColVector& dir){m_dir = dir.Unit(); OnChangeDir(m_dir);}

	ColVector&			GetAskMoveDir(){return m_askMoveDir;}
	void				SetAskMoveDir(ColVector& dir){m_askMoveDir = dir.Unit();}

	TIME_MILSEC			GetRedirectTime(){return m_redirectTime;}
	void				SetRedirectTime(TIME_MILSEC redirectTime){m_redirectTime = redirectTime;}

	ColVector&			GetBeforeMovePos(){return m_beforeMovePos;}
	void				SetBeforeMovePos(ColVector& beforeMovePos){m_beforeMovePos = beforeMovePos;}

	ColSphere&			GetStepMoveTarget(){return m_stepMoveTarget;}
	void				SetStepMoveTarget(ColSphere& stepMoveTarget){m_stepMoveTarget = stepMoveTarget;}

	void				SetForceMoveSpeed(FLOAT speed){m_forceMoveSpeed = speed;}

	bool				IfStepMoved(){return m_ifStepMoved;}
	void				SetStepMoved(bool moved){m_ifStepMoved = moved;}

	bool				IfForceMoveImpact(){return m_ifForceMoveImpact;}
	void				SetIfForceMoveImpact(bool ifForceMoveImpact){m_ifForceMoveImpact = ifForceMoveImpact;}

	SIntPoint*			GetPathCell(){return m_asPathCell;}
	void				SetPathLength(UINT16 length){m_pathLength = length;}
	void				SetMoveStep(UINT16 step){m_moveStep = step;}
};

}

#endif