#pragma once
#include "stdafx.h"

namespace ReBot{
	class CClientGameObject
	{
	public:
		EGOActionState				m_GOActionState;
		SGUID_KEY					m_sObjGUID;
		SGUID_KEY					m_sMasterGUID;
		SFightProperty				m_sCurFP;
		INT32						m_CurPersonCP;
		INT32						m_n32TeamCP;
		INT32						m_n32CurHP;
		INT32						m_n32MaxHP;
		INT32						m_n32CurMP;
		INT32						m_n32MaxMP;
		SGUID_KEY					m_CurEnemyGUID;
		SGUID_KEY					m_CurAtkGUID;
		TIME_MILSEC					m_tCurAtkMilsec;
		INT32						m_n32CollideRadius;
		EGameObjectCamp				m_eGOCamp;
		TIME_MILSEC					m_tLastSyncMoveStateMilsec;
		CVector3D					m_CurPos;
		CVector3D					m_CurDir;

	public:
		CClientGameObject();
		virtual ~CClientGameObject();

		virtual INT32				OnHeartBeat(TIME_MILSEC tUTCMilsec);
		virtual INT32				Clear();
		void						UpdateFP(const SFightProperty& fp);
		BOOLEAN						IfHero();
		BOOLEAN						IfNPC();
		
		void						SetObjectGUID(const SGUID_KEY& sGuid){m_sObjGUID = sGuid;}
		void						SetMasterGUID(const SGUID_KEY& sGuid){m_sMasterGUID = sGuid;}
		void						UpdateMP(INT32 n32Val){m_n32CurMP = n32Val; }
		void						UpdateMaxMP(INT32 n32Val){m_n32MaxMP = n32Val;}
		virtual	void				UpdateHP(INT32 n32Val){m_n32CurHP = n32Val;}
		void						UpdateMaxHP(INT32 n32Val){m_n32MaxHP = n32Val;}
		void						UpdateMoveSpeed(INT32 n32MoveSpeed);
		void						SetCamp(EGameObjectCamp camp){m_eGOCamp = camp;}
		virtual void				UpdateCP(INT32 n32PersonCP, INT32 n32TeamCP);
		virtual void				UpdateSkillState(UINT8 un8SkillSlotID, INT32 n32SkillState, UINT32 un32StateTime);
		virtual void				UpdateSkill(UINT32 n32SkillID, UINT32 un32StateTime){}
		virtual	RobotSkillData*		GetSkillData(UINT8 un8SkillSlotID) = 0;
		virtual	BOOLEAN				IsHero(){return FALSE;}
		void						UpdateDir(const CVector3D& dir);
		void						UpdatePos(const CVector3D& pos){m_CurPos = pos;}
		CVector3D&					GetCurPos(){return m_CurPos;}
		virtual void				UpdateState(EGOActionState state, const CVector3D& curPos, const CVector3D& dir);
		EGOActionState				GetState(){return m_GOActionState;}
		const SGUID_KEY&			GetObjectGUID(){return m_sObjGUID;}
		EGameObjectCamp				GetCampID(){return m_eGOCamp;}
		INT32						GetCP(){return m_CurPersonCP;}
		INT32						GetMP(){return m_n32CurMP;}
		INT32						GetHP(){return m_n32CurHP;}
		virtual	INT32				GetCollideRadius() = 0;

	protected:
		INT32						RunState(TIME_MILSEC tUTCMilsec);
		INT32						DoHeartBeat(TIME_MILSEC tUTCMilsec);
	};
}

