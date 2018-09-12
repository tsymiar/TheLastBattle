
#ifndef SSSKILLEFFECT_MOVE_H_
#define SSSKILLEFFECT_MOVE_H_


#include "stdafx.h"
#include "SSSkillEffect.h"
#include "SSMoveObjectHolder.h"
#include "SafeRefPtr.h"

namespace SceneServer{

class CSSSkillEffect_Move : public CSSSkillEffect, public ISSMoveObjectHolder
{
private: 
	SSkillModelMoveCfg* m_pMoveCfg;
	ref_ptr<CSSGameUnit>		m_pMoveTarget;//移动者
	bool				m_bIfEnd;//是否移动结束了
	bool				m_bIfStartForceMove;//是否强制移动已开始
	CVector3D			m_moveDir;//移动方向
	CVector3D			m_lastMovePos;//上一帧位置
	CVector3D			m_moveTargetPos;//移动终点位置
	TIME_MILSEC			m_tBeginMilsec;//开始移动的时间
	TIME_MILSEC			m_tLastMoveMilsec;//上一帧的时间
	TIME_MILSEC			m_tEndMilsec;//结束时间

public:
	CSSSkillEffect_Move();
	~CSSSkillEffect_Move();

private:
	bool				ResetPos(CSSGameUnit* pTarget, CVector3D& pos, CVector3D& dir, bool bIfEnd = FALSE, bool bIfCheckCollide = TRUE, bool bIFFindForward = TRUE);
	INT32				NotifyTeleportToGC(CSSGameUnit* pTarget, CVector3D& pStartPos, CVector3D& cDir);
	INT32				NotifyStartMoveToGC(CSSGameUnit* pStartGU, CVector3D& cPos, CVector3D& cDir, INT32 n32Speed);
	INT32				NotifyStopMoveToGC(CSSGameUnit* pStartGU, CVector3D& cPos);
	void				StartMove(CVector3D& cStartPos);
	void				StopMove();

public:
	virtual INT32		Begin();
	INT32				Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan); 
	virtual INT32		End();
	virtual bool		IfParaInvalid();

	virtual void		OnStopMove();
};

}



#endif