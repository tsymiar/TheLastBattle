/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:SSHero.h
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
#include "SSAI_Hero.h"
#include "SSSkill.h"
 
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <set>
 

struct SGoodsCfg;

namespace SceneServer{

enum ECPCostType
{
	eCPCP_Unlegal = 0,
	eCPCP_Personnal,
	eCPCP_Team,
};

enum EGoodsOp
{
	eGoodsOP_Create,
	eGoodsOP_Remove,
	eGoodsOP_OverlapChange,
	eGoodsOP_Move
};

enum EFuryCostType
{ 
	eFuryAdd,
	eFuryUseAll,
};

enum EFuryState
{
	eFuryNullState = 0,
	eFuryFullState,
	eFuryRunState,
};

enum EHeroKillState
{
	eNoneKill,
	eNormalKill,
	eDoubleKill,
	eTripleKill,
	eFirstBoold,
};

enum EHeroKillTitle
{
	eNoneTitel = 10018,
	eKillingSpree = 10019,
	eDominating = 10020,
	eMegaKill = 10021,
	eUnstoppable = 10022,
	eWhickedSick = 10023,
	eMonsterKill = 10024,
	eGodLike = 10025,
	eHolyShit = 10026,
};

enum EDecreaseReason{
	EDecreaseReason_Use,
	EDecreaseReason_Sell,
	EDecreaseReason_Compose
};

struct SGoodsRunTimeData{
	SGoodsRunTimeData() : sGoodsCfg(NULL)
		, tStateBeginMilsec(0)
		, sGoodsState(eGoodsState_Free)
		, n32Num(0)
		, eGoodsSpecificaFlag(eGoodsSpecificaFlag_NoSpecifical)
		, bIsComposing(FALSE)
		, pSkill(NULL) {
			Clear();
	}

	CSSSkill*			pSkill;
	SGoodsCfg*			sGoodsCfg;
	eGoodsState			sGoodsState;
	TIME_MILSEC			tStateBeginMilsec;
	INT32				n32Num;
	EGoodsSpecificaFlag eGoodsSpecificaFlag;
	bool				bIsComposing;
	INT32				ln32PassSkillUniqueID[c_n32MaxNextSkillEffectCfgNum];

	void Clear(){
		sGoodsCfg = NULL;
		sGoodsState = eGoodsState_Free;
		n32Num = 0;
		tStateBeginMilsec = 0;
		eGoodsSpecificaFlag = eGoodsSpecificaFlag_NoSpecifical;
		bIsComposing = FALSE;
		for(int i=0 ;i< c_n32MaxNextSkillEffectCfgNum; i++){
			ln32PassSkillUniqueID[i] = 0;
		}
	}
};

struct AbsorbRuntimeInfo{
	AbsorbRuntimeInfo(): sNPCCfg(NULL){
		
	}
	~AbsorbRuntimeInfo(){

	}

	CSSSkill* pSkill;
	SNPCCfg* sNPCCfg;

	void Clear(){
		if(NULL != pSkill) {
			pSkill->Clear();
		}
		sNPCCfg = NULL;
	}
};

struct sRecentlyHurt{
	CSSHero*		pHero;
	CSSNPC*			pNpc;
	TIME_MILSEC		tHurtTime;

	sRecentlyHurt(){
		Clear();
	}

	void Clear(){
		pHero = NULL;
		pNpc = NULL;
		tHurtTime = 0;
	}
};

struct SHurtRecord{
	SGUID_KEY		pHurterGUID;
	TIME_MILSEC		tHurtTime;
	INT32			n32HurtValue;
};

struct sHeroServiceData{
	UINT32			PersonCP;
	UINT32			TotalPersonCP;
	UINT32			TeamCP;
	UINT32			TotalTeamCP;
	UINT32			HeroLevel;
	UINT32			CurLvExp;
	UINT32			CurFuryValue;
	UINT32			ContTitelKills;
	UINT32			HeroRebornTimes;		//英雄剩余复活次数

	sHeroServiceData(){
		Clear();
	}

	void Clear(){
		PersonCP = 0;
		TotalPersonCP = 0;
		TeamCP = 0;
		TotalTeamCP = 0;
		HeroLevel = 1;
		CurLvExp = 0;
		CurFuryValue = 0;
		ContTitelKills = 0;
		HeroRebornTimes = 0;
	}

};

struct GoodsSendData{
	UINT8	un8GridID;
	UINT8	un8IsComposed;
	GoodsSendData(UINT8	un8GridID, UINT8 un8IsComposed): un8GridID(un8GridID)
		, un8IsComposed(un8IsComposed){}
	GoodsSendData():un8GridID(0),
		un8IsComposed(0){}
};

class CSSBattleMgr;
//class CSSGuide;
class CSSGameLogMgr;

const INT32 c_n32MaxGoodsSkillNum = 100;

class CSSHero : public CSSGameUnit
{
private:
	SGoodsRunTimeData			m_asGoodsRunTimeData[c_un32MaxGridNum];
	map<CSSSkill*,UINT8>		m_goodsSkillMap;//物品序列与技能的对应表
	CSSSkill*					m_aspSkillArr[c_n32MaxHeroSkillNum];
	AbsorbRuntimeInfo			m_asAbsorbRuntimeInfo[c_n32MaxAbsorbSkillNum];

	typedef boost::unordered_map<UINT32, CSSSkill*>	AllGoodsSkillDataMap;
	AllGoodsSkillDataMap		m_asAllGoodsSkillDataMap;
	map<UINT64, sRecentlyHurt>	m_sRecentlyHurtbyHeroMap;
	vector<SHurtRecord>			m_hurtRecordVec;
	TIME_MILSEC					m_tClearHurtRecordTime;

	const SHeroCfg*				m_cpsHeroCfg;
	CVector3D					m_heroBornPos;

	SFightProperty				m_sNakeFP;
	SFightProperty				m_sBaseFP;	
	SSendData					m_sSendData;
	sHeroServiceData			m_sServiceData;
	
	EFuryState					m_eFuryState;
	EHeroKillState				m_eHeroKillState;
	EHeroKillTitle				m_eKillTitel;
	bool						m_bCheckFuryTimeTag;
	bool						m_bStartRecoverCpTag;
	bool						m_bAutoAttack;
	bool						m_bAlwaysAutoAtk;

	INT16						m_n16ContinuousKill;
	INT16						m_n16ContinuousDead;
	UINT8						m_un8BuyRebornTimes;

	TIME_MILSEC					m_tFuryMilsec;
	TIME_MILSEC					m_tRecoverCpMilsec;
	TIME_MILSEC					m_tConKillMilTime;
	TIME_MILSEC					m_tConDeadMilTime;
	TIME_MILSEC					m_tHeroLastDeadMilsec;

	//CSSGuide				*	m_guide;
	INT64						m_n64KillNPC;
	ref_ptr<CSSUser>			m_piUser;
	SGUID_KEY					m_sLockedTargetGUID;
private:
	ECPCostType					ConvertCPType(const SGoodsCfg* pCfg);
	bool						TryDownCompose(SGoodsCfg* pGoodsCfg, INT32& n32CPCost, set<UINT8>& toDecGirdVec);
	SGoodsCfg*					TryUpCompose(SGoodsCfg* pGoodsCfg, set<UINT8>& toDecGirdVec);
	const SGoodsCfg*			GetUpComposeRunTimsGoods(const SGoodsCfg* pGoodsCfg, set<UINT8>& un8GridIdList, SGoodsCfg*& pNewGoodsCfg);
	void						GetDownComposeRunTimsGoods(const SGoodsCfg* pGoodsCfg, set<UINT8>& un8GridIdList);
	SGoodsRunTimeData*			InsertOneGoods(SGoodsCfg* psCfg, INT32 n32GoodsNum, UINT8& un8GridID, INT32& n32Reflag, UINT8 bIsComposed = 0);
	SGoodsRunTimeData*			HasSameEOTIDGoods(UINT32 eOT);
	SGoodsRunTimeData*			HasSameEOTIDGoods(UINT32 eOT, UINT8& un8GridID);
	SGoodsRunTimeData*			HasSameTypeIDGoods(UINT32 un32Type);
	SGoodsRunTimeData*			HasSameFunTypeGoods(UINT32 un32Type);
	void						HasSameEOTIDGoods(UINT32 eOT, set<UINT8>& un8GridIDSet);
	bool						IsValidGrid(UINT8 un8GirdID);
	bool						IsStillHasGoods(UINT8 un8GridID);

	INT32						SynGoodsUseStateToGC(UINT8 un8GridID, UINT8 un8IfComposed = 0);
	INT32						AskUseSkillInGoods(SGoodsCfg& sCfg, const SSNewSkillCfg* pSkillCfg);
	INT32 						DecreaseOneGoods(UINT8 un8GridID, EDecreaseReason eReseon);

	void                        CheckFuryRunEnd(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	void						RecoverPlayerCp(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	virtual void				CheckDeadState();
	void						RefreshGoodsSkillMap();
	
public:

	CSSHero(UINT64 guid, EGameObjectCamp sCamp, UINT32 objectID, UINT64 sMasterGUID, CSSBattle* pBattle, CSSBattleMgr* pMgr, bool IfAI);
	virtual ~CSSHero(void);

	virtual	bool				IfHero() {return true;}
	virtual UINT8				GetDefalutImpactGruopID() { return SSImpactGruop_Hero; }
	virtual void				OnAppearInSight(std::vector<CSSUser*>& cTempUserMap);
	virtual void				OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap);

	virtual	INT32				OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	virtual INT32				BeginAction_Dead(bool bIfNotifyGC, CSSGameUnit *pcReasonGO);

	virtual CSSSkill*			GetSkillByID(UINT32 un32SkillID);
	virtual	FLOAT				GetSight();
	virtual SSendData&			GetSSendData(){ return m_sSendData; };
	virtual CSSGameUnit*		LookForEnemy();
	virtual CSSGameLogMgr*		GetSSUserBattleLogMgr()	{return NULL;}
	virtual ESSSightObjectType	GetSightObjectType(){return ESSSightObjectType_Sensitive;}

	bool						IsEmptyGrid(UINT8 un8GirdID);
	bool						IfDamage(GSToGC::HPMPChangeReason eHPCR, INT32 n32HP);
	void						InitLevelByUserName(const char* name);

	INT32						ChangeExp(INT32 n32ExpNum);
	INT32						ChangeCP(ECPCostType eType, INT32 n32CP, bool bIfIncomByKill = FALSE);
	void                        ChangeFuryValue(EFuryCostType eType, INT32 n32FURY);
	void                        ChangeFuryState(EFuryState eState); 

	INT32						ChangeCurHP(CSSGameUnit *pcReasonGO, GSToGC::HPMPChangeReason eHPCR, INT32 n32ChangeValue,INT32 skillId =0, EEffectCate	eE = eEffectCate_None);

	
	INT32						LoadHeroCfg(const SHeroCfg* pcCfg);
	void						LoadPassitiveSkill();
	INT32						AskBuyGoods(UINT32 un32GoodsTypeID, INT32& goodsId );
	INT32						AskUseGoods(UINT8 un8GridID);
	INT32						AskSelGoods(UINT8 un8GridID);
	INT32						AskMoveGoods(UINT8 un8FromGridID, UINT8 un8GridID);
	INT32						GetBuyGoodsCp(UINT32 un32GoodsTypeID);
	INT32						SyncSkillStateToGC(UINT32 un32SkillID);
	INT32						SyncSkillStateToGC(CSSSkill *psSkill, UINT32 un32SkillSlotIdx);
	INT32						CheckUpgradeSkill();

	INT32						SyncExpInfoToGC();
	void						SyncCpToCl();
	void						NotifyFuryStateToCl();
	INT32						BroadcastLvInfoToGC();	
	
	void						AddSSendData_DeadTimes(){ ++m_sSendData.un32DeadTimes;}
	void						AddSSendData_DestoryBuilding(){ ++m_sSendData.un32DestoryBuildings;}
	void						AddSSendData_HeroKills(){ ++m_sSendData.un32HeroKills;}
	void						AddSSendData_LastHit(){ ++ m_sSendData.un32LastHit;}
	void						AddTitelkills(){ ++ m_sServiceData.ContTitelKills;}
	void						AddContinuousKill() { ++m_n16ContinuousKill;}
	void						AddContinuousDead() { ++m_n16ContinuousDead;}
	void						AddAssistKill(){ ++m_sSendData.un32Assist;}
	void						AddBuyRebornTimes(){ ++ m_un8BuyRebornTimes;}

	UINT32						GetExp(){return m_sServiceData.CurLvExp;}
	UINT32						GetLevel(){return m_sServiceData.HeroLevel;}
	UINT32						GetAbsorbMonsterID(INT32 n32Index);
	INT32						GetTotalPersonCP() {return m_sServiceData.TotalPersonCP;}
	const SGoodsRunTimeData	GetGridGoodsInfo(INT16 gridid) {return m_asGoodsRunTimeData[gridid];}
	INT32						GetCP(ECPCostType eType);
	INT32						GetContinuousKill() const{ return m_n16ContinuousKill;}
	INT32						GetContinuousDead() const{ return m_n16ContinuousDead;}
	INT32						GetHeroKillState()	const{ return m_eHeroKillState;}
	INT32                       GetFuryValue(){ return m_sServiceData.CurFuryValue;}
	INT32						GetAssistNum(){ return m_sSendData.un32Assist;}
	INT32						GetBuyRebornCost();
	CVector3D					GetEmitPos(){return m_sCurOASI.cPos + m_cpsHeroCfg->cEmitPos;}
	CVector3D					GetOnHitPos(){return m_sCurOASI.cPos + m_cpsHeroCfg->cOnHitPos;}
	CSSUser*					GetUser(){return m_piUser;}
	CSSHero*					GetLastValidHeroHurt();
	AbsorbRuntimeInfo*			GetAbsorbMonsterSkillInfo(INT32 n32Index){return &m_asAbsorbRuntimeInfo[n32Index];}
	SGUID_KEY&					GetLockedTargetGUID();
	EHeroKillTitle				GetCurTitel();
	EFuryState                  GetFuryState();
	TIME_MILSEC					GetConDeadMilTime() const{ return m_tConDeadMilTime;}
	TIME_MILSEC					GetConKillMilTime() const{ return m_tConKillMilTime;}
	TIME_MILSEC					GetHeroLastDeadMilsec() const{ return m_tHeroLastDeadMilsec;}
	bool						GetAutoAttack()const{return m_bAutoAttack;}
	INT64						GetAddKillNPC()			{return m_n64KillNPC;}

	void                        ResetSelfSkillCoolDown();
	void						ResetContinuousKill() { m_n16ContinuousKill = 1; }
	//void						ResetContinuousDead() { m_n16ContinuousDead = 1;}
	void						ResetTitelkills() {m_sServiceData.ContTitelKills = 0;}

	INT32						SetUser(CSSUser *piUser);
	void						SetLevel(UINT32 lv);
	INT32						DoUpgrade();
	void						SetHeroKillState();
	void						SetConDeadMilTime(TIME_MILSEC tCurMilTime) {m_tConDeadMilTime = tCurMilTime;}
	void						SetConKillMilTime(TIME_MILSEC tCurMilTime) {m_tConKillMilTime = tCurMilTime; }
	void						SetHeroLastDeadMilsec(TIME_MILSEC tCurMilTime) {m_tHeroLastDeadMilsec = tCurMilTime;}
	void						SetAutoAttack(bool bAttack){m_bAutoAttack = bAttack;}
	void						SetAlwaysAutoAtk(INT8 n8Flag);

	INT32						GasExpersion();
	bool						IsGasSkill(UINT32 skillId);
	INT32						CheckUseGasSkill(UINT32 skillId);
	INT32                       SyncFuryValueToGC();
	
	void                        StartRecoverCp();
	INT32						CalculateKillResult(CSSHero* pcReason);
	INT32						GetEndTitleCP(EHeroKillTitle title);
	void						CalculateAssit(CSSHero* pHero); 

	void						AddKillNPC();
	INT32						UserAskReconnect();
	INT32						GetHeroGoodsInfo();
	 
	virtual EAttackWay			GetAttackWay(){return m_cpsHeroCfg->eAttackWay;}

	 
	bool						IsSkillFromNPC(INT32 nSkillId); 
	//bool						CheckGuideStep(INT32 nStepId);
	//bool						GetGuideCompState();
	//void						ClearGuideSteps();

	INT32						SynAbsobInfoToGC(UINT8 un8Res, UINT32 un32MonsterID1, UINT32 un32MonsterID2);
	INT32						ChangeFPInAbsorb(const SNPCCfg& pNPCCfg, bool bAdd);

	void						SetLockedTargetGUID(const SGUID_KEY & guid){ m_sLockedTargetGUID = guid;}
	void						ClearLockedTargetGUID(){m_sLockedTargetGUID = 0;}
	INT32						GetHeroRebornTimes()	{return m_sServiceData.HeroRebornTimes;}
	void						ChangeUserHeroRebornTimes(INT16 RebornTimes){ m_sServiceData.HeroRebornTimes += RebornTimes;}

	bool						GetAlwaysAutoAtk()const{return m_bAlwaysAutoAtk;}

	SGoodsRunTimeData*			GetGoodRunTimeDateArr(){return m_asGoodsRunTimeData;}
	CSSSkill**					GetSkillArr(){return m_aspSkillArr;}
	AbsorbRuntimeInfo*			GetAbsorRunTimeArr(){return m_asAbsorbRuntimeInfo;}
	void						RefreshCD();
	map<CSSSkill*,UINT8>*		GetGoodsSkillMap(){return &m_goodsSkillMap;}

	void						SetHeroBornPos(CVector3D pos){m_heroBornPos = pos;}
	CVector3D					GetHeroBornPos(){return m_heroBornPos;}
protected:
	virtual INT16				GetCollideRadius(){return m_n32CollideRadius;}
	/************************************************************************/
	/* ask handler.		                                                    */
	/************************************************************************/
public:
	INT32						AskMoveDir(const CVector3D &crcDir, EGOOrderPriority ePriority){if(IfAI()) return eNormal;return ((CSSAI_Hero*)m_pAI)->AskMoveDir(crcDir);}
	INT32						AskStopMove(EGOOrderPriority ePriority){if(IfAI()) return eNormal;return ((CSSAI_Hero*)m_pAI)->AskStopMove();}
	INT32						AskUseSkill(UINT32 un32SkillID, EGOOrderPriority ePriority){if(IfAI()) return eNormal;return ((CSSAI_Hero*)m_pAI)->AskUseSkill(un32SkillID);}
	INT32						AskMoveTar(const CVector3D &crcPos){if(IfAI()) return eNormal;return ((CSSAI_Hero*)m_pAI)->AskMoveTar(crcPos);}
	INT32						AskStartAutoAttack(){if(IfAI()) return eNormal;return ((CSSAI_Hero*)m_pAI)->AskStartAutoAttack();}
	INT32						AskAbsorbMonster(INT32 n32ToRemoveIndex){if(IfAI()) return eNormal;return ((CSSAI_Hero*)m_pAI)->AskAbsorbMonster(n32ToRemoveIndex);}
	INT32						AskRemoveAbsorbSkill(UINT32 un32MonsterID);
	INT32						AskHeroAttributes();
	INT32						AskLockTar(CSSGameUnit *pcTarGU);
	INT32						AskGasExpersion(){if(IfAI()) return eNormal;return ((CSSAI_Hero*)m_pAI)->AskGasExpersion();}
	/************************************************************************/
	/* AI   相关		                                                    */
	/************************************************************************/
private:
	bool						m_bIfAI;
public:
	bool						IfAI(){return m_bIfAI;}
	void						SetIfAI(bool bIfAI){m_bIfAI = bIfAI;}
	void						ResetAI();
	void						GetHurtMeList(UINT32 time,map<CSSGameUnit*,INT32> *pMap);
	INT32						GetHeroID(){return m_cpsHeroCfg->eOT;}
	const SHeroCfg*				GetHeroCfg(){return m_cpsHeroCfg;}

};


}
