/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:SSGameUnit.h
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
#include "SSGameObject.h"
#include "SSSkillEffect_Buf.h"
#include "SSSkillEffect_Move.h"
#include "ISSightLight.h"
#include "SSightObject.h"
#include "SSMoveObject.h"
#include "SSMoveObjectHolder.h"
#include "SSGUParameterMgr.h"

namespace SceneServer{

class CSSGameLogMgr;

typedef ElementArr<INT32, 16> PassiveSkillArr;
typedef ElementArr<UINT32, 32> BufArr;
class CSSGameUnit : public CSSGameObject, public ISSightLight, public ISSightObject, public ISSMoveObject, public referable<CSSGameUnit>
{
protected:
	CSSGUParameterMgr		m_sFpMgr;
	BufArr					m_BufArr;
	UINT32					m_MoveEffectID;
	INT32					m_n32LastRecHP;
	INT32					m_n32LastShouldRecHP;
	INT32					m_n32LastRecMP;
	INT32					m_n32LastShouldRecMP;
	TIME_MILSEC				m_tRecoverTickCounter;
	TIME_MILSEC				m_tLastRecoverTime;
	PassiveSkillArr			m_mPassiveSkillMap[EPassiveSkillTriggerType_Max];
	bool					m_bIfActiveMove;
	INT32					OnGameUnitHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	virtual void			CheckDeadState() = 0;

private:

public:
	UINT32					m_n32FastArrayID;
	CSSGameUnit(UINT64 guid, EGameObjectCamp sCamp, UINT32 objectID, UINT64 sMasterGUID, CSSBattle* pBattle, CSSBattleMgr* pMgr);
	virtual ~CSSGameUnit(void);
	virtual	INT32			OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan) = 0;
	void					SetBattleMgr(CSSBattleMgr* pMgr);
	virtual	FLOAT			GetSight() = 0;
	virtual CSSGameUnit*	LookForEnemy(CSSGameUnit* pOldEnemy = NULL){return NULL;}
	virtual	bool				IfNPC(){return FALSE;};
	virtual	bool				IfNPC_Building(){return FALSE;};
	virtual	bool				IfSolider() {return FALSE;};
	virtual	bool				IfWildNPC(){return FALSE;}
	virtual	bool				IfHero(){return FALSE;};
	virtual UINT8				GetDefalutImpactGruopID() { return 0; }
	CSSNPC*						TryCastToNPC();
	CSSHero*					TryCastToHero();
	virtual CSSGameLogMgr*		GetSSUserBattleLogMgr(){return NULL;}
	string					GetDebugString();
	CSSGUParameterMgr*		GetParaMgr(){return &m_sFpMgr;}

	virtual INT32			BeginAction_Dead(bool bIfNotifyGC, CSSGameUnit *pcReasonGO) = 0;
	INT32					BeginAction_Controled(bool bIfNotifyGC, CSSGameUnit *pcReasonGO);
	INT32					EndAction_Controled(bool bIfNotifyGC, CSSGameUnit *pcReasonGO);
	INT32					BeginAction_Reliving(bool bIfNotifyGC, CSSGameUnit *pcReasonGO);
	INT32					EndAction_Reliving(bool bIfNotifyGC, CSSGameUnit *pcReasonGO);

	INT32					CheckBeginAction_Free(bool bSyncToGC);

	INT32					BeginAction_Move(const CVector3D &crsDir, bool bIfNotifyGC);
	INT32					BeginAction_Free(bool bIfNotifyGC);
	INT32					BeginAction_PrepareSkill(CSSSkill* pSkill, const CVector3D &crsDir, bool bIfNotifyGC);
	INT32					BeginAction_ReleaseSkill(CSSSkill* pSkill, const CVector3D &crsDir,bool bIfNotifyGC);
	INT32					BeginAction_UsingSkill(CSSSkill* pSkill, const CVector3D &crsDir,bool bIfNotifyGC);
	INT32					BeginAction_LastingSkill(CSSSkill* pSkill, const CVector3D &crsDir,bool bIfNotifyGC);
	INT32					CheckSyncMoveState();

	google::protobuf::Message*	ConstructObjStateMsg(INT32& n32MsgID,bool spot = false);

	EHateLevel				GetHateLevel(CSSGameUnit& sGO);
	INT32					CheckRecover(TIME_TICK tTickSpan);

	INT32					FullHP();
	INT32					FullMP();

	INT32					GetFPData(EEffectCate eEffectCate);
	INT32					ChangeFPData(EEffectCate eEffectCate, INT32 n32ChangeNum, INT32 n32Persent = 0, bool ifAdd = TRUE, INT32 n32UniqueID = 0);

	INT32					ApplyHurt(CSSGameUnit *pcEnemyGO, INT32 n32HurtValue, EEffectCate hurtCate, INT32 &hurtValue, bool bIfNormalAtk , bool ifBlast );
	virtual INT32			ChangeCurHP(CSSGameUnit *pcReasonGO, GSToGC::HPMPChangeReason eHPCR, INT32 n32ChangeValue,INT32 skillId =0, EEffectCate	eE = eEffectCate_None) = 0;
	INT32					ChangeCurMP(CSSGameUnit *pcReasonGO, GSToGC::HPMPChangeReason eMPCR, INT32 n32ChangeValue);
	INT32					GetCurHP(){return GetFPData(eEffectCate_CurHP);}
	INT32					GetCurMP(){return GetFPData(eEffectCate_CurMP);}
	bool					CanAddBuf();
	INT32					AddBuf(UINT32 un32BufTypeId);
	INT32					RemoveBuf(INT32	n32BufProjectId);
	BufArr&					GetBufArr(){return m_BufArr;}
	void					SetMoveEffect(UINT32 un32MoveID, bool bIfActiveMove);
	bool					IfForceMoved(){return FALSE == m_bIfActiveMove && m_MoveEffectID != 0;}
	void					RemoveMoveEffect(UINT32 un32MoveID);

	INT8					GetHeroKillState(CSSGameUnit * pcReasonGO);
	INT32					NotifyKillHeroMsgToGC(SGUID_KEY& killerGUID, CSSHero * pOtherHero);
	bool					IfInReleaseSkillRange(CSSGameUnit* sTarget, const SSNewSkillCfg* pCfg,float addedDist = 0);

	
	virtual EAttackWay		GetAttackWay(){return EAttackWay_Building;}

	void					OnTeleport(){m_pAI->OnTeleport();}
	/************************************************************************/
	/* about obj func                                                        */
	/************************************************************************/

	INT32					AddPassiveSkill(EPassiveSkillTriggerType eType, INT32 n32UniqueId);
	INT32					RemovePassiveSkill(EPassiveSkillTriggerType eType, INT32 n32UniqueId);
	PassiveSkillArr&		GetPassiveSkillVec(EPassiveSkillTriggerType eType);
	void					OnPassitiveSkillHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	void					OnPassitiveSkillCalled(EPassiveSkillTriggerType type, CSSGameUnit* pTarget, INT32 para1= 0, INT32 para2= 0, INT32 para3= 0);
	 
	INT32					GetNowMoveSped(); 

	INT32					CheckSummonGUNEndLife(ESummonFunc esf,CSSGameUnit * pcSelfG); 

	bool					IsDead(){return GetGOActionStateInfo().eOAS == eOAS_Dead || GetExpire() == TRUE;} 
 
protected:
	INT32					DoChangeHP(INT32 n32ChangeValue, GSToGC::HPMPChangeReason eHPCR, CSSGameUnit *pcReasonGO,CSSGameUnit *deadGO);   
	
	/************************************************************************/
	/* about sight func                                                     */
	/************************************************************************/
	INT32					SendMsgToWatchersByType(ESysSightType est, google::protobuf::Message& sMsg, int n32MsgID, vector<CSSUser*>& pUserVec, TIME_MILSEC dealyTime = 0);

public:
	virtual float			GetSightX();
	virtual float			GetSightY();
	virtual float			GetLightDistance(){return 3000;}
	virtual float			GetSightPosX(){return GetCurPos().m_fX;}
	virtual float			GetSightPosY(){return GetCurPos().m_fZ;}
	virtual ESSSightObjectType	GetSightObjectType(){return ESSSightObjectType_Insensitive;}
	virtual bool			IfInvisible(){return GetFPData(eEffectCate_Invisible) > 0;}

protected:
	void					SendAppearMsg(std::vector<CSSUser*>& cTempUserMap, UINT32 objTypeID);
	void					SendDisppearMsg(std::vector<CSSUser*>& cTempUserMap);
	void					SendHPMPMsg(std::vector<CSSUser*>& cTempUserMap);

	/************************************************************************/
	/* about move func                                                      */
	/************************************************************************/

private:
	ISSMoveObjectHolder*			m_moveHolder; 
public:
	virtual INT16				GetCollideRadius(){return 100;}
	void						SetMoveHolder(ISSMoveObjectHolder* holder){m_moveHolder = holder;}
	void						ClearMoveHolder(){m_moveHolder = NULL;}
	ISSMoveObjectHolder*		GetMoveHolder(){return m_moveHolder;}

	virtual bool				IfCanImpact(){return IsDead() == FALSE;}
	virtual ColSphere			GetColSphere(){return ColSphere(GetColVector(), GetColRadius()); }
	virtual ColVector			GetColVector(){return ColVector(GetCurPos().m_fX, GetCurPos().m_fZ); }
	virtual INT16				GetColRadius(){return GetCollideRadius();}
	virtual FLOAT				GetSpeed() {return (FLOAT)GetNowMoveSped() / 1000.0f; }
	virtual SSMoveObjectImpactType		GetImpactType() {return SSMoveObjectImpactType_Group;}

	virtual void				OnStartMove(ColVector dir);
	virtual void				OnMoved(ColVector pos){ CVector3D cPos; cPos.m_fX = pos.x; cPos.m_fZ = pos.y; cPos.m_fY = GetCurPos().m_fY; SetGOActionState_Pos(cPos);}
	virtual void				OnMoveBlock();
	virtual void				OnChangeDir(ColVector dir){CVector3D cDir;cDir.m_fX = dir.x;cDir.m_fZ=dir.y;SetGOActionState_Dir(cDir);}

	virtual void				OnDizziness(){if(m_sCurOASI.eOAS < eOAS_PassiveState ) BeginAction_Free(TRUE);}
	virtual	void				OnDisDizziness(){/*EndAction_Controled(TRUE,this);*/}

	//log
public:
	virtual void				CelObjHurtLog(CSSGameUnit *pcEnemyGO,INT32 skillId, INT32 n32HurtValue, EEffectCate ec);
	virtual void				CelObjDeadLog(CSSGameUnit *pcEnemyGO);
};

}
