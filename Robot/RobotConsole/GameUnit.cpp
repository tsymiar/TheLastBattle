#include "stdafx.h"
#include "GameUnit.h"
#include "CfgMgr.h"
#include "RobotMgr.h"

namespace ReBot{
	void CClientGameObject::UpdateFP(const SFightProperty& fp){
		m_sCurFP = fp;
	}

	CClientGameObject::CClientGameObject():m_CurPersonCP(0)
		, m_n32TeamCP(0)
		, m_GOActionState(eOAS_Free){
	}

	CClientGameObject::~CClientGameObject(){

	}

	INT32 CClientGameObject::OnHeartBeat(TIME_MILSEC tUTCMilsec){
		return DoHeartBeat(tUTCMilsec);
	}

	void CClientGameObject::UpdateCP(INT32 n32PersonCP, INT32 n32TeamCP){
		m_CurPersonCP = n32PersonCP;
		m_n32TeamCP = n32TeamCP;
	}

	void CClientGameObject::UpdateSkillState(UINT8 un8SkillSlotID,  INT32 n32SkillState, UINT32 un32StateTime){
		RobotSkillData* skillRuntime = GetSkillData(un8SkillSlotID);
		if (NULL == skillRuntime){
			return;
		}
		skillRuntime->eSkillState = (ESkillState)n32SkillState;
	}

	void CClientGameObject::UpdateState(EGOActionState state, const CVector3D& curPos, const CVector3D& dir){
		m_GOActionState = state;
		if (m_GOActionState == eOAS_Running){
			m_tLastSyncMoveStateMilsec = gRobotMgr->GetTickCount();
		}

		m_CurPos = curPos;
		m_CurDir = dir;
	}

	INT32 CClientGameObject::RunState(TIME_MILSEC tUTCMilsec){
		if (m_sCurFP.n32MoveSpeed == 0){
			return eNormal;
		}
		float timeDiff = tUTCMilsec - m_tLastSyncMoveStateMilsec;
		float fMoveDist = timeDiff / 1000 * m_sCurFP.n32MoveSpeed;
		if (fMoveDist < 0.00001){
			return eNormal;
		}

		CVector3D moveDist = m_CurDir * fMoveDist;
		m_CurPos = m_CurPos + moveDist;
		m_tLastSyncMoveStateMilsec = tUTCMilsec;
		return eNormal;
	}

	void CClientGameObject::UpdateMoveSpeed(INT32 n32MoveSpeed){
		m_sCurFP.n32MoveSpeed = n32MoveSpeed;
	}

	INT32 CClientGameObject::DoHeartBeat(TIME_MILSEC tUTCMilsec){
		if (m_GOActionState == eOAS_Running){
			RunState(tUTCMilsec);
		}

		return eNormal;
	}

	INT32 CClientGameObject::Clear(){
		m_n32TeamCP = 0;
		m_GOActionState = eOAS_Free;
		m_CurPersonCP = 0;
		m_CurPos.Clear();
		m_eGOCamp = eGOCamp_1;
		m_sCurFP.Clear();
		m_tLastSyncMoveStateMilsec = 0;
		m_CurDir.Clear();
		return eNormal;
	}

	void CClientGameObject::UpdateDir(const CVector3D& dir){
		if (IfHero()){
		//	ELOG(LOG_INFO, "UpodateDir(%f, %f)", dir.m_fX, dir.m_fZ);
		}
	}

	BOOLEAN CClientGameObject::IfHero(){
		return FALSE;//CFunction::IfTypeHero(m_sObjGUID);
	}

	BOOLEAN CClientGameObject::IfNPC(){
		return FALSE;//CFunction::IfTypeNPC(m_sObjGUID);
	}

}
