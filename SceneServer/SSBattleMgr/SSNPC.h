/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:SSNPC.h
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
#include "SSGameUnit.h"
#include "SSSkill.h"

namespace SceneServer{

class CSSNPC : public CSSGameUnit
{
private:
	const SNPCCfg*				m_cpsNPCCfg;
	CSSSkill*					m_aspSkillArr[c_n32MaxNPCSkillNum];

private:
	virtual void				CheckDeadState();
	INT32						m_n32BelongPoint; //just wild monster 
	EWMGroup					m_n32WMGroup;		   
	INT32						m_n32BelongAltar;
	bool					    m_bIfAtked;
	TIME_MILSEC					m_tLifeEndTime;
	INT32						m_totalLife;
	CVector3D					m_WMBornPos;
	INT32						m_goOrderNextNum; 

public:
	CSSNPC(UINT64 guid, EGameObjectCamp sCamp, UINT32 objectID, UINT64 sMasterGUID, CSSBattle* pBattle, CSSBattleMgr* pMgr);
	virtual ~CSSNPC(void);

	virtual	bool				IfNPC() {return true;}
	virtual	bool				IfNPC_Building() { Assert(m_cpsNPCCfg!=NULL); return m_cpsNPCCfg->eNPCCate == eNPCCate_Building; }
	virtual	bool				IfSolider();
	virtual	bool				IfWildNPC(){ Assert(m_cpsNPCCfg!=NULL); return m_cpsNPCCfg->eNPCCate == eNPCCate_WideAnimal;}
	virtual UINT8				GetDefalutImpactGruopID() {
		Assert(m_cpsNPCCfg!=NULL); 
		Assert(m_objType!=0);
		if (m_objType==21016) return SSImpactGruop_Zhuzi;
		switch (m_cpsNPCCfg->eNPCCate)
		{
		case eNPCCate_Building: return SSImpactGruop_All; break;
		case eNPCCate_Solider: return SSImpactGruop_Solider; break;
		case eNPCCate_WideAnimal: 
			if(GetBelongAltar() > 0){
				//祭坛兵碰撞和小兵一样
				return SSImpactGruop_Solider;
			}
			return SSImpactGruop_Wild; 
			break;
		
		default: Assert(false); return SSImpactGruop_None; break;
		}
	}

	virtual	INT32				OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	virtual CSSSkill*			GetSkillByID(UINT32 un32SkillID){return NULL;}
	virtual	FLOAT				GetSight();
	virtual CSSGameUnit*		LookForEnemy(CSSGameUnit* pOldEnemy = NULL, FLOAT maxDist = 0, bool ifChooseWildMonster = TRUE);
	virtual INT32				BeginAction_Dead(bool bIfNotifyGC, CSSGameUnit *pcReasonGO);
	const SNPCCfg*				GetNPCCfg(){return m_cpsNPCCfg;}
	INT32						LoadNPCCfg(const SNPCCfg* pcCfg);

	virtual void				OnAppearInSight(std::vector<CSSUser*>& cTempUserMap);
	virtual void				OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap);

public:
	INT32						GetPursueDist()const			{return m_cpsNPCCfg->n32PursueDist;}
	void						SetBelongPoint(INT32 l)			{m_n32BelongPoint = l;}
	//if m_n32BelongPoint > 0,it's Monster
	INT32						GetBelongPoint()const			{return m_n32BelongPoint;}
	void						SetEWMGroup(EWMGroup evm )		{m_n32WMGroup = evm;}
	EWMGroup					GetEWMGroup(void)				{return m_n32WMGroup;}

	void						SetBelongAltar(INT32 l)			{m_n32BelongAltar = l;}
	INT32						GetBelongAltar()const		{return m_n32BelongAltar;}

	void						SetIfBeAttack(bool bIfAtked);
	bool						GetIfBeAttack()						{ return m_bIfAtked;}

	void						SetObjEndLife(TIME_MILSEC  n)		{ m_tLifeEndTime = n;}
	TIME_MILSEC					GetObjEndLife(void)			{return m_tLifeEndTime;}
	
	void						SetObjTotalLife(INT32  n)		{ m_totalLife = n;}
	TIME_MILSEC					GetObjTotalLife(void)			{return m_totalLife;}

	INT32						SendSummonLifeMsg(ESummonFunc esf, CSSGameUnit*				pMasterGO= NULL);
	

	CVector3D					GetEmitPos(){return m_sCurOASI.cPos + m_cpsNPCCfg->cEmitPos;}
	CVector3D					GetOnHitPos(){return m_sCurOASI.cPos + m_cpsNPCCfg->cOnHitPos;}

	void						LoadPassitiveSkill();
	void						SetWMBornPos(const CVector3D &pcPos) {m_WMBornPos = pcPos;}
	CVector3D&					GetWMBornPos(){return m_WMBornPos;}

	virtual SSMoveObjectImpactType		GetImpactType() {return IfNPC_Building()?SSMoveObjectImpactType_ALL:SSMoveObjectImpactType_Group;}

	void						SetOrderNextNodeIdx(INT32 n)		{m_goOrderNextNum = n; }
	INT32						GetOrderNextNodeIdx(void)			{return m_goOrderNextNum;}
	//virtual EAttackWay			GetAttackWay(){return m_cpsNPCCfg->eAttackWay;}
	INT32						SetOccupyPath(std::vector<CVector3D> & tNode);//(const CVector3D *cpcPathNode, UINT32 un32NodeNum);
	void						AskStopAI();
	void						AskResumeAI();

	INT32						ChangeCurHP(CSSGameUnit *pcReasonGO, GSToGC::HPMPChangeReason eHPCR, INT32 n32ChangeValue,INT32 skillId =0, EEffectCate	eE = eEffectCate_None);
private:
	bool						IfNotNeedSynSight();
protected:
	virtual INT16				GetCollideRadius(){return m_n32CollideRadius;}

private:
	//用于祭坛放兵
	bool					m_bHeadIcoState;	
	INT32					m_SolderType;	
	INT64					m_OwnGuiId;
public:
	void					SetHoldNPCState(bool b, INT32 nType, INT64 OwnGuiId)		{m_bHeadIcoState = b; m_SolderType =  nType; m_OwnGuiId = OwnGuiId; }
	void					CheckGUnitNPCICO(EAltarBornNpcType eat,std::vector<CSSUser*>& cTempUserMap);
	bool					IfAltarHasMonster(){return m_OwnGuiId > 0;}
};
}
