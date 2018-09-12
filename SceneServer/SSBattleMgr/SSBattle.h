/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:SSBattle.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:March 21 2014
* summary			:
*
*/
#pragma once
#include "stdafx.h"

#include <boost/shared_ptr.hpp>
#include <hash_map>
#include <set>
#include <random>


namespace SceneServer{

class CSSGameObject;
class CSSGameUnit;
class CSSNPC;
class CSSHero;
class CSSArea;
class CSSAltarSev;
class CSSEffectMgr;
class CSSEffectSightCB;
class CSSOBSev; 
class CSSPassiveSkillMgr; 
class CSSight;
class ISSightObject;
class ISSightLight;
struct SFindPathInfoNew;
class CSSMoveMgr;
class CSSightMgr;
class ISSMoveObject;
class CSSVMBornMgr; 
class CSSBattleMgr;
 
class SSSolderSev;

typedef std::hash_map<UINT64, CSSGameUnit*> GameObjectMap;

struct SBattleResult{
	UINT32		un32SaveTime;
	UINT32		un32DeadTimes;
	UINT32		un32KillHero;
	UINT32		un32KillNpcs;
	UINT32		un32KillBuilding;
	UINT32		un32ContinueKills;
	UINT32		un32MaxKills;
	UINT32		un32CpGet;
	UINT32		un32CpUsed;
	UINT32		un32Lv;

	SBattleResult():un32SaveTime(0)
		, un32DeadTimes(0)
		, un32KillHero(0)
		, un32KillNpcs(0)
		, un32KillBuilding(0)
		, un32ContinueKills(0)
		, un32MaxKills(0)
		, un32CpGet(0)
		, un32CpUsed(0)
		, un32Lv(0)
	{}
};

struct SBattleUserInfo{
	CSSUser*	piUser;
	CSSHero*	pcHero;
	UINT32		un32HeroTypeID;
	BOOLEAN		bIfHeroChoosed;
	UINT16		un16GrpID;
	UINT16		un16RunePage;
	BOOLEAN		bIfOB;
	BOOLEAN		bIfGM;
	SBattleResult asBattleResult;
	BOOLEAN		bIfLoadComplete;
	BOOLEAN		bIfReconnect;
	UINT16		un16Pos;
	UINT64		un64Guid;//AiRobot需要
	string		stNickName;
	TIME_MILSEC	offLineTime;

	//AI相关
	BOOLEAN		ifAI;
	UINT32		un32AIChooseRandomNum;
	TIME_MILSEC tAiNextSelectHeroTime;
	UINT8		un8AIChooseTimes;
	SBattleUserInfo():un32HeroTypeID(0)
		, bIfHeroChoosed(FALSE)
		, piUser(NULL)
		, pcHero(NULL)
		, bIfOB(FALSE)
		, bIfGM(FALSE)
		, asBattleResult()
		, bIfLoadComplete(FALSE)
		, bIfReconnect(FALSE)
		, un16Pos(0)
		, un64Guid(0)
		, un16RunePage(0)
		, stNickName("")
		, ifAI(FALSE)
		, un32AIChooseRandomNum(0)
		, tAiNextSelectHeroTime(0)
		, un8AIChooseTimes(0)
		, offLineTime(0)
	{}
	BOOLEAN IfNoGameUnit() { return piUser == NULL && FALSE == ifAI;}
};

enum EBattleType
{
	eBattleType_Free,		//自由中//
	eBattleType_Room,		//房间中//
	eBattleType_Match,		//匹配中//
	eBattleType_Guide1,		//引导1中//
	eBattleType_Guide2,		//引导2中//
};

enum ESSBattleState
{
	eSSBS_SelectHero = 0,
	eSSBS_SelectRune,
	eSSBS_Loading,
	eSSBS_Playing,
	eSSBS_Finished,
};

class CSSBattle
{
	const static UINT32	c_tSelectHeroTime = 60000;
	const static UINT32	c_tSelectRuneTime = 10000;
	const static UINT32	c_tLoadTime	= 10000;

	struct DealySendMsg{
		TIME_MILSEC beginSendTime;
		INT32	delayTime;
		string msgStr;
		INT32	n32MsgID;
		vector<SUserNetInfo> pUsers;
		DealySendMsg():beginSendTime(0)
			, delayTime(0)
			, n32MsgID(0){

		}
		~DealySendMsg(){
		}
	};

public:
	CSSBattle();
	CSSBattle(CSSBattleMgr* pOwner,EBattleType type,UINT64 battleID);
	virtual ~CSSBattle(void);
	
public:
	CSSEffectMgr*		GetEffectMgr() { return m_pEffectMgr; }
	CSSPassiveSkillMgr* GetPassiveSkillMgr() { return m_pPassiveSkillMgr; }
	CSSightMgr*			GetSightMgr() { return m_pSightMgr; }
	CSSVMBornMgr*		GetVMBornMgr(){ return m_SSVMMgr; }
	CSSMoveMgr*			GetMoveMgr(){ return m_pMoveMgr; }
	const SMapCfg*		GetMapCfg()	{return m_cpsMapCfg;}
	std::default_random_engine& GetRandomEngine() { return m_Generator; }

private:
	CSSBattleMgr*		m_pCSSBattleMgr;
	CSSEffectMgr*		m_pEffectMgr;
	CSSPassiveSkillMgr* m_pPassiveSkillMgr;
	CSSightMgr*			m_pSightMgr; 
	CSSMoveMgr*			m_pMoveMgr;
	
	std::default_random_engine  m_Generator;
	const SMapCfg*		m_cpsMapCfg;
	
protected:
	ESSBattleState		m_eBattleState;
	bool				m_bIsCanRushNpc;
	UINT64				m_un64BattleID;
	EBattleType			m_eBattleType;
	//各种游戏用到的时间
	TIME_MILSEC			m_tBattleStateTime;//战场当前状态开始时间
	TIME_MILSEC			m_tBattlePlayTime;//战场整个的游戏时间
	TIME_MILSEC			m_tLastCheckPlayTimeout;
	TIME_MILSEC			m_BattleFinishProtectTime;
	TIME_MILSEC			m_BattleRushNpcTime;
	SBattleUserInfo		m_asBattleUserInfo[c_un32MaxUserInBattle];
	//本次战斗拥有的英雄数量(非ai)
	INT32				m_BattleHero;
	GameObjectMap		m_cGameObjectMap;
	// GameObject的查询快表//
	CSSGameUnit**		m_cGameObjectArray;
	UINT32				m_cGameObjectCount;//最大数//
	UINT32				m_cGameObjectCountMax;//最大数上限//
	UINT32				m_cGameReleased[100];//保存100个)//
	UINT32				m_cGameReleasePos;
	void				AddGameArray(CSSGameUnit* pUnit);
	void				DelGameArray(CSSGameUnit* pUnit);
	//////////////////////////////////////////////////////////////////////////
	UINT32				m_un32MapID;
	//TIME_MILSEC			m_tNPCNextBornCDMilsec;
	
	CSSVMBornMgr*		m_SSVMMgr;
	CSSAltarSev*		m_cpAltarSev;
	CSSOBSev*			m_cpOBSev;
	SSSolderSev*		m_SolderSev; 

	INT64				m_n64TotalCashPledge;
	bool				m_bIfFirstBlood;
	bool				m_bIfAced;
	map<INT32,CSSight*>	m_pSightMap;

	UINT64				m_n64ObjectID; 
	INT32				m_n32EffectID;
	
	typedef std::map<INT32, NPCHeroCfg> GuideNPCHeroReBornDelayTime;
	GuideNPCHeroReBornDelayTime m_NPCHeroReBornDelayTime;

	typedef std::map<INT32, NPCHeroCfg> GuideNPCHeroRebornTime;
	GuideNPCHeroRebornTime			m_GuideNPCHeroRebornTime;
	set<CSSGameUnit*>				m_BuildingBaseSet;

	std::map<ETipsType, INT32>		m_mapTips;
	set<DealySendMsg*>				m_DealySendMsgSet;
	time_t							m_lastSaveLogTime;

public:
	UINT32				GetBattleMapId() {return m_un32MapID;}
	UINT64				GetBattleID()const{return m_un64BattleID;}
	EBattleType			GetBattleType(){return m_eBattleType;}
	ESSBattleState		GetBattleState(){return m_eBattleState;}
	TIME_MILSEC			GetBattleStateStartTime(){return m_tBattleStateTime;}
	void				SetBattleState(ESSBattleState state, BOOLEAN ifSendToClient = TRUE);
	INT32				Finish(EBattleEndType ebe,INT32 n32WinCampID = 0);

	INT32				CheckBattleFinish(CSSNPC* pNPC, CSSGameUnit *pcReasonGO);
public:
 	INT32				OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	virtual void				CheckSelectHeroTimeout(TIME_MILSEC tCurUTCMilsec);
	virtual void				CheckSelectRuneTimeout(TIME_MILSEC tCurUTCMilsec); 
	void				CheckLoadingTimeOut(TIME_MILSEC tUTCMilsec); 
	virtual bool		CheckPlayTimeout(TIME_MILSEC tCurUTCMilsec);
	void				DoPlayHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan);
	INT32				RandomPickHero(vector<INT32>* pPickHeroVec,INT32 grpID,INT32 exceptID);

public:
	INT32				AskTrySelectHero(CSSUser *piUser, INT32 heroType);
	INT32				AskSelectHero(CSSUser *piUser, INT32 heroType);
	INT32				AskSelectRunePage(CSSUser *piUser, INT32 pageIndex);
	INT32				AskGetHerosBattleInfo(CSSUser *pUser);
	INT32				IfCanSelectHero(CSSUser *piUser, INT32 heroType, bool ifCheckChange);
	bool				AskStartMoveDir(ISSMoveObject* pMObject, const CVector3D&  dir);
	bool				AskStartMoveToTar(ISSMoveObject* pMObject, const CVector3D&  pos, bool ifMoveToBlackPos, bool bIfFliter);
	bool				AskStartMoveForced(ISSMoveObject* pMObject, const CVector3D&  dir, FLOAT speed, bool bIfImpact);
	bool				AskStopMoveObjectAll(ISSMoveObject* pMObject);
	bool				AskStopMoveObjectDir(ISSMoveObject* pMObject);
	bool				AskStopMoveObjectTar(ISSMoveObject* pMObject);
	bool				AskStopMoveObjectForceMove(ISSMoveObject* pMObject);
	INT32				AskBuyReborn(CSSHero *piHero);

public:
	INT32				LoadMapData(UINT32 un32MapID);
	INT32				LoadMapConfigNpc();
	 			
public:
	SBattleUserInfo*	InsertUser();
	void				OnUserOnline(UINT32 pos,SUserNetInfo& pNetInfo);
	void				OnUserOffline(UINT32 pos); 

public:
	INT32				GetIfAced()	{ return m_bIfAced;}
	bool				CheckIfAced(EGameObjectCamp CampID);
	INT32				GetIfFirsetBlood(){ return m_bIfFirstBlood;}
	INT32				SetIfFirstBloodState(bool bState){ m_bIfFirstBlood = bState; return eNormal;}


	INT32				GetUserSeatPos(CSSUser *piUser);
	SBattleUserInfo*	GetUserBattleInfo(CSSUser *piUser);
	SBattleUserInfo*	GetUserBattleInfo(INT32 index){if (0 > index || c_un32MaxUserInBattle <= index) return NULL; return &m_asBattleUserInfo[index];}
	CSSGameUnit*		GetGameObjectByGUIDIdx(INT64 guidIdx);
	CSSGameUnit*		GetGameUnitByGUID(const SGUID_KEY &crsGUID);
	
	CSSNPC*				AddNPC(EObjectType eOT,UINT64 un64ObjIdx, const CVector3D &crsPos, const CVector3D &crsDir, EGameObjectCamp eCampID, INT32 altar =0,INT32 summonEndTime = 0/*召唤物专用*/);
	CSSHero*			AddHero(EObjectType eOT, const CVector3D &crsPos, const CVector3D &crsDir, CSSUser *piWatchUser, SGUID_KEY rsMasterGUID, EGameObjectCamp eCampID, INT32 seat, bool ifAI = FALSE);
	EGameObjectCamp		GetHeroCampByGrp(INT32 grpID);
	INT32				RemoveGameUnit(CSSGameUnit *pcGO);

	CSSAltarSev  *		GetSSAltarSev()											{return				m_cpAltarSev;}
	CSSOBSev 	 *		GetSSOBSev()											{return				m_cpOBSev;	}
	GameObjectMap&		GetBattleGameObjectList()								{return				m_cGameObjectMap;}  
	 
	//move relate functions.
	bool				IsInRect(const CVector3D &position,const CVector3D &center,const float length,const float width,CVector3D& dir);
	bool				IsInCircle(const CVector3D &position,const CVector3D &origin,const float radius);
	bool				IsInFan(const CVector3D &position,const CVector3D &origin,const float radius,const CVector3D& startRadius,const CVector3D& endRadius);	
	static	bool		IsPointInCircularSector(const CVector3D &position,const CVector3D &origin, FLOAT theta, FLOAT radius, const CVector3D &dir);
	bool				SetWildNPCGroupCurEnemy(CSSGameUnit *pcMasterObj,CSSGameUnit *pcReasonGO);
	CSSGameUnit*		CreateNPCAndAdd(CSSGameUnit *pcMasterObj, UINT32 eOT,  const CVector3D & tarPos ,INT64 summonEndTime = 0);
	 
	

	  void				InitBattleParamCfg();
	/************************************************************************/
	/* new move system                                                      */
	/************************************************************************/

	void				AddMoveObject(ISSMoveObject* pMObject, UINT8 ui8GrpID);
	void				RemoveMoveObject(ISSMoveObject* pMObject);

	bool				GetAroundFreePos(const CVector3D&  pos, CVector3D &outPos, ISSMoveObject *pGU, UINT8 ui8MoveGrpID);
	bool				ResetPos(CSSGameUnit *pGU,const CVector3D&  pos, bool bIfImpact);

	void				FindAroundGU(const CVector3D& startPos, FLOAT radios, vector<CSSGameUnit*> &objVec);
	void				FindAroundEmemy(CSSGameUnit *pMaster, const CVector3D& startPos, FLOAT radios, CSSGameUnit** targetArr, FLOAT* distArr, UINT16 &length, UINT16 maxLength);

	INT32				GetFirstFreePosByPath(CSSGameUnit* pGU, const CVector3D&  cStartPos, const CVector3D&  cEndPos, CVector3D &rcOutPos);
	INT32				GetLastFreePosByPath(CSSGameUnit* pGU, const CVector3D& cStartPos, const CVector3D&  cEndPos, CVector3D &rcOutPos);
	/************************************************************************/
	/* ask move.                                                            */
	/************************************************************************/
	INT32				AskMoveDir(CSSUser *piUser, const CVector3D &crsDir);
	INT32				AskStopMove(CSSUser *piUser);
	INT32				AskAttack(CSSUser *piUser, const SGUID_KEY &crsTarGUID);
	INT32				AskRebornHero(CSSHero *pcHero);
	INT32				AskLockTar(CSSUser *piUser, const SGUID_KEY &crsTarGUID);
	INT32				AskUseSkill(CSSUser *piUser, UINT32 un32SkillID);
	INT32				AskAbsorbMonster(CSSUser *piUser, INT32 n32ToRemoveIndex);
	INT32				AskRemoveAbsorb(CSSUser *piUser, UINT32 un32MonsterID);
	INT32				AskGasExplosion(CSSUser *piUser );
	INT32				ReportStrawSolder(CSSUser *piUser,   INT32  crsAltarIdex,   EObjectType eOT );
	INT32				StrawSolder(CSSHero* pcHero,   INT32  crsAltarIdex,   EObjectType eOT );
	INT32				SendAltarIco(INT32 altarIdx,EObjectType eOT, EAltarBornNpcType etype);
	INT32				SendMsgWhenAltarBornSolder(INT64 objIdx, INT32 eOt,INT32 camp,const CVector3D & bornPos);

	 
	/************************************************************************/
	/* scene about function                                                 */
	/************************************************************************/
	INT32				EnterBattle(CSSGameUnit *pcGO, const CVector3D &crsPos, const CVector3D &crsDir);
	INT32				LeaveBattle(CSSGameUnit *pcGO);
	INT32				GetAroundHero(const CVector3D &crsCenterPos, INT32 n32Sight, CSSHero **ppcHeroBuff, INT32 n32BuffSize);
	INT32				GetAroundEnemyHero(const CVector3D &crsCenterPos, INT32 n32Sight, INT32 n32CampID, CSSHero **ppcHeroBuff, INT32 n32BuffSize);
	INT32				GetAroundFriendHero(const CVector3D &crsCenterPos, INT32 n32Sight, INT32 n32CampID, CSSHero **ppcHeroBuff, INT32 n32BuffSize);
	INT32				GetAllEnemyHero(INT32 n32CampID, CSSHero **ppcHeroBuff, INT32 n32BuffSize);

	UINT32				GetGONum(){return (UINT32)m_cGameObjectMap.size();}
	
	 
	virtual  INT32				DoAltarBornMonster(TIME_MILSEC tUTCMilsec); 
			 INT32				CheckWildMonsterBorn(TIME_MILSEC tUTCMilsec);
	INT32				DoWildMonsterBorn(EWMLevel el,TIME_MILSEC tUTCMilsec);
	INT32				DoWildMonsterDead( CSSNPC* pNPC); 
	INT32				StartBornWildMonster(INT32 pointId, const SGameWildMonsterCfg &wmcfg);
	INT32				DoNPCBuildDead(CSSNPC *pNPC, CSSGameUnit*pUnit  );

	INT32				OnAllNPCFunc(INT64 t);
	virtual INT32				OnBornNPCFunc(TIME_TICK tTickSpan);
	INT32				UserReconnectToBattleSS(CSSUser *piUser);
	INT32				AskMoveTar(CSSUser *piUser, const CVector3D& sPos);
	/************************************************************************/
	/*Sight                                                            */
	/************************************************************************/
	CSSight*			GetSightByCamp(EGameObjectCamp eCamp, bool bIfAutoBuild = FALSE, bool bIfOBSight = FALSE);
	void				RemoveSightByCamp(EGameObjectCamp eCamp);
	void				AddISSUserToSight(CSSUser* pIUser, EGameObjectCamp eCamp, bool bIfOBSight = FALSE);
	void				RemoveISSUserToSight(EGameObjectCamp eCamp, CSSUser* pWatcher);
	void				AddSightObject(ISSightObject* pSO);
	void				AddChildSightObject(ISSightObject* pFather, ISSightObject* pChild);
	void				RemoveSightObject(ISSightObject* pSO);
	void				RemoveChildSightObject(ISSightObject* pFather, ISSightObject* pChild);
	void				AddSightLight(ISSightLight* pLight, ISSightObject* pAsSightObj, EGameObjectCamp eCamp);
	void				RemoveSightLight(ISSightLight* pLight, ISSightObject* pAsSightObj, EGameObjectCamp eCamp);
	void				SendMsgToAllWatcher(google::protobuf::Message& sMsg, int n32MsgID,CSSGameUnit* pGU);
	void				SendMsgToWatchers(google::protobuf::Message& sMsg, int n32MsgID,std::vector<CSSUser*>& cTempUserMap, TIME_MILSEC dealyTime = 0);
	void				SendMsgToSelfAndOB(google::protobuf::Message& sMsg, int n32MsgID,CSSUser* pISelfUser);
	void				SendMsgToOB(google::protobuf::Message& sMsg, int n32MsgID);
	void				SendMsgToAllWatcher(google::protobuf::Message& sMsg, int n32MsgID);
	static	void		SetPos(const CVector3D& pos, GSToGC::Pos& toPos);
	static	void		SetDir(const CVector3D& dir, GSToGC::Dir& toDir);
	/************************************************************************/
	/*Msg Poster                                                            */
	/************************************************************************/
	INT32				SyncBattleDelayTime(CSSUser *pUser);
	INT32				SyncBattleSpanTime(CSSUser *pUser);
	INT32				SyncState(CSSGameUnit *pcGO,bool spot = false);
	INT32				SyncState(CSSGameUnit *pcGO, std::vector<CSSUser*>& cTempUserMap);
	INT32				SyncMsgToGC(google::protobuf::Message& sMsg, int n32MsgID, const SGUID_KEY &guid, CSSGameUnit *pcGO, CSSUser *piSightUser);
	INT32				SyncMsgToGC(google::protobuf::Message& sMsg, int n32MsgID,const SGUID_KEY &guid, map<CSSUser*, INT32> &rcUserSightMap);
	INT32				BroadMsgToGC(google::protobuf::Message& sMsg, int n32MsgID);
	INT32				BroadMsgToGC_TrySelectHero(INT32 pos, INT32 heroType);
	INT32				BroadMsgToGC_SelectHero(INT32 pos, INT32 heroType);
	INT32				PostMsgToGC_EnterBattleState(CSSUser* pcUser,int64_t clientTime);
	INT32				PostMsgToGC_NotifyReconnectInfo(CSSUser *piUser);
	INT32				PostMsgToGC_BattleHeroInfo(CSSUser* pcUser);
	INT32				PostMsgToGC_NotifyHeroReborn(CSSUser *piUser);
	INT32				PostMsgToGC_BattleResult(INT32 n32WinCampID);
	INT32				PostMsgToCS_BattleEnd(INT32 n32WinCampID);
	INT32				SyncSpeedMsgToGC( CSSGameUnit *pcGO , std::vector<CSSGameUnit*> & vecObject);
	INT32				GetUniqueEffectID(); 
	INT32				SyncOBSystems(EOBSystemFunc eOb,const SGUID_KEY& guid, google::protobuf::Message& sMsg, int n32MsgID, CSSUser *piUser = NULL, void* p = NULL);	 
	 

public: 
	virtual  void		  BroadTipsByType(ETipsType et, INT32 campId = 0);  
 
	void					FinishCallScript(); 
 
	void					UpdateSaveLogTime()							{ m_lastSaveLogTime = time(NULL);}
	INT64					GetLastSaveLogTime()						{return m_lastSaveLogTime;}

private:
	INT32					KickoutAllUser();
 
	virtual UINT64			GenerateObjGUID(){return ++m_n64ObjectID;} 
 
public:
	SBattleUserInfo	*		GetBattleUserInfo()												 {return 	m_asBattleUserInfo;}
	virtual bool			CheckObjCanBeHurt(CSSGameUnit* beHurtObj, CSSGameUnit *atkObj )	 { return true; }
	 
	virtual	void			OnBattleStateChange()	{}
	virtual	void			OnBattleFinish(){}
	virtual	void			ResetAIAtOnUserOffline(CSSHero* pHero);
	virtual void			SetCaseDeadReasonInfo(CSSGameUnit* deadObj, CSSGameUnit *pcReasonGO){} 
	virtual void			PostMsgToGC_BattleResultHeroInfo(INT32 n32WinCampID);
	virtual void			DelHeroNoHurtStateAtDead(CSSHero* deadObj){}
	virtual bool			IsGuideBattle()							{return m_eBattleType==eBattleType_Guide1;} 		
public:
	void					GetBattleHeroLog();
	void					GetBattleEndLog(INT32  campId); 
 
};

}
