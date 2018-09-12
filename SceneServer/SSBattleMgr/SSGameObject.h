/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:SSGameObject.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:March 25 2014
* summary			:
*
*/

#pragma once
#include "stdafx.h"
#include "SSAI.h"
#include "SafeRefPtr.h"

namespace SceneServer{

class CSSArea;
class CSSNPC;
class CSSHero;
struct SBattleRegion;
class CSSBattleMgr;
class CSSUser;
class CSSSkill;

class CSSGameObject
{
protected:
	//Íæ¼Òguid£¡
	UINT64						m_sMasterGUID;

	SGUID_KEY					m_sObjGUID;
	SGUID_KEY					m_CurEnemyGUID;
	SGUID_KEY					m_CurAtkGUID;
	TIME_MILSEC					m_tCurAtkMilsec;
	SGOActionStateInfo			m_sCurOASI;
	INT32						m_n32CollideRadius;
	EGameObjectCamp				m_eGOCamp;
	INT32						m_n32Seat;//Î»ÖÃºÅ//
	CSSAI*						m_pAI;
	CSSBattle*					m_pcBattle;
	bool						m_bExpire;
	TIME_MILSEC					m_tEnterBattleUTCMilsec;
	TIME_MILSEC					m_tLastSyncMoveStateMilsec;
	CSSSkill*					m_pNormalAttackSkill;
	SGUID_KEY					m_FatherHeroGUID;
	UINT32						m_objType;
	CSSBattleMgr*				m_pCSSBattleMgr;

protected:
	INT32						SetGOActionState(EGOActionState eState);

public:
	CSSGameObject(UINT64 guid, EGameObjectCamp sCamp, UINT32 objectID, UINT64 sMasterGUID, CSSBattle* pBattle, CSSBattleMgr* pMgr);
	virtual ~CSSGameObject(void);

	CSSSkill*					GetNormalAttackSkill_NEW() {return m_pNormalAttackSkill;}
	bool						GetExpire(){return m_bExpire;}
	bool						SetExpire(bool bExpire);
	const SGOActionStateInfo&	GetGOActionStateInfo(){return m_sCurOASI;}
	INT32						SetGOActionState_ASUTCMilsec(TIME_MILSEC tMilsec);
	INT32						SetGOActionState_Pos(const CVector3D &crcPos);
	INT32						SetGOActionState_Dir(const CVector3D &crcDir);
	INT32						SetGOActionState_SkillID(UINT32 un32SkillID);
	INT32						SetGOActionState_SkillTarGUID(const SGUID_KEY &crsTarGUID);

	bool						IfCanBeTarget();

	bool						IfSkillAction(){return eOAS_PreparingSkill == m_sCurOASI.eOAS || eOAS_ReleasingSkill == m_sCurOASI.eOAS;}
	INT32						SetGOSceneData_IntDir(EIntDir eIntDir);
	void						SetGOCurOASI(EGOActionState eOas) { m_sCurOASI.eOAS = eOas;};
	TIME_TICK					GetFreeActionTick();

	UINT64						GetCurBattleID();
	SGUID_KEY&					GetObjGUID(){return m_sObjGUID;}
	UINT64						GetMasterGUID(){return m_sMasterGUID;}
	CVector3D&					GetCurPos(){return m_sCurOASI.cPos;}
	virtual CVector3D			GetEmitPos(){return m_sCurOASI.cPos;}
	virtual CVector3D			GetOnHitPos(){return m_sCurOASI.cPos;}
	CVector3D&					GetCurDir(){return m_sCurOASI.cDir;}
	CSSBattle*					GetCurBattle(){return m_pcBattle;}
	INT32						GetObjCollideRadius(){return m_n32CollideRadius;}
	EGameObjectCamp				GetCampID(){return m_eGOCamp;}
	INT32						GetSeatID() { return m_n32Seat; }
	void						SetSeatID(INT32 seat) { m_n32Seat = seat; }
	TIME_MILSEC					GetEnterBattleUTCMilsec(){return m_tEnterBattleUTCMilsec;}
	INT32						SetEnterBattleUTCMilsec(TIME_MILSEC tUTCMilsec){m_tEnterBattleUTCMilsec = tUTCMilsec;return eNormal;}

	void						SetCurAtkGUID(const SGUID_KEY &crsGUID);
	const SGUID_KEY &			GetCurAtkGUID();

	void						SetCurEnemyGUID(const SGUID_KEY &crsGUID)			{m_CurEnemyGUID = crsGUID;}
	const SGUID_KEY &			GetCurEnemyGUID()								{return m_CurEnemyGUID; }

	bool						IsSameCamp(CSSGameObject *pcGo);
	bool						IfEnemy(CSSGameObject *pcGo);

	void						SetFatherHeroGUID(const SGUID_KEY & guid)	{m_FatherHeroGUID = guid;}
	SGUID_KEY					GetFatherHeroGUID(void)						{return m_FatherHeroGUID; }

	UINT32						GetObjType(void)							{return m_objType;}	

	void						SetAI(CSSAI* pAI){m_pAI = pAI;}};

}
