/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:SSCfgMgr.h
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
#include <map>

namespace SceneServer{

class CSSBattleMgr;

struct MapStaticData{
	INT32				m_n32XAreaNum;
	INT32				m_n32ZAreaNum;
	INT32				m_n32TotalAreaNum;
	const BYTE*			m_pbBlockList;
	INT32				m_n32XRegionNum;
	INT32				m_n32ZRegionNum;
	INT32				m_n32TotalRegionNum;
	SBattleRegion*		m_pcRegionList;
};

struct SSFuncSwitch 
{
	bool   logSwitch;
	bool   gmSwitch;
	bool   solderSwitch;
	bool   monsterSwitch;
	bool   altarSwitch;//祭坛出兵开关

	INT32  logRefreshTime;	//日志刷新时间间隔

	SSFuncSwitch()  { clear();}
	void clear()	{memset(this,0,sizeof(*this));}
	void set (const SSFuncSwitch & t)
	{
		logSwitch = t.logSwitch;
		gmSwitch = t.gmSwitch;
		solderSwitch = t.solderSwitch;
		monsterSwitch = t.monsterSwitch;
		altarSwitch = t.altarSwitch;
		logRefreshTime = t.logRefreshTime;
	}
};

struct SRobotPath{
	INT32 pathID;
	vector<CVector3D> path;
};

struct SRobotGrpPath{
	INT32 grpID;
	map<INT32, SRobotPath> pathMap;
};

struct SRobotMapPath{
	INT32 mapID;
	map<INT32,SRobotGrpPath> grpPathMap;
};

class CSSCfgMgr
{
	friend class CSSBattleMgr;
private:
	CSSCfgMgr(void);
	typedef	void	(CSSCfgMgr::*PFCSMsgHandler)(const char* pMsg, int n32MsgLength);
	typedef std::map<INT32, PFCSMsgHandler> CSMsgHandlerMap;
	CSMsgHandlerMap				m_asCSMsgHandler;
	std::default_random_engine  m_Generator;

public:
	
	~CSSCfgMgr(void);

	INT32		Initialize();
	INT32		Uninitialize();
	void		HandleMsg(const char* pMsg, int n32MsgLength, int n32MsgID);
	static CSSCfgMgr&				GetSSCfgInstance();
	MapStaticData*					GetMapStaticDataByMapID(UINT32 un32MapID);
	SBattleRegion*					GetRegionByXAndZ(INT32 n32XIdx, INT32 n32ZIdx);
	const SGoodsCfg*				GetGoodsCfg(EObjectType eOT);
	const SNPCCfg*					GetNPCCfg(EObjectType eOT);
	const SHeroCfg*					GetHeroCfg(EObjectType eOT);
	const SShopCfg*					GetShopCfg(EObjectType eOT);
	const SMapCfg*					GetMapCfg(UINT32 un32MapID);
	const map<UINT32, SMapObjCfg>&	GetMapObjCfg() { return m_cMapObjCfgMap; }
	const SSNewSkillCfg*			GetSkillCfg(UINT32 un32SkillID);
	const ESkillEffectType			GetSkillModelType(UINT32 un32SkillModelID);
	const SSkillModelAccountCfg*	GetSkillModelAccountCfg(UINT32 un32SkillModelID);
	const SSkillModelLeadingCfg*	GetSkillModelLeadingCfg(UINT32 un32SkillModelID);
	const SSkillModelEmitCfg*		GetSkillModelEmitCfg(UINT32 un32SkillModelID);
	const SSkillModelMoveCfg*		GetSkillModelMoveCfg(UINT32 un32SkillModelID);
	const SSkillModelSwitchCfg*		GetSkillModelSwitchCfg(UINT32 un32SkillModelID);
	const SSkillModelPurificationCfg*		GetSkillModelPurificationCfg(UINT32 un32SkillModelID);
	const SSkillModelLinkCfg*		GetSkillModelLinkCfg(UINT32 un32SkillModelID);
	const SSPassiveSkilllCfg*		GetPassitiveSkillCfg(UINT32 un32PassitiveSkillID);
	const EPassiveSkillType			GetPassitiveSkillType(UINT32 un32PassitiveEffectID);
	const SSPassiveEffectCfg_BloodSeek*	GetPassitiveEffectBloodSeekCfg(UINT32 un32PassitiveSkillID);
	const SSPassiveEffectCfg_Rebound*	GetPassitiveEffectReboundCfg(UINT32 un32PassitiveSkillID);
	const SSPassiveEffectCfg_Relive*	GetPassitiveEffectReliveCfg(UINT32 un32PassitiveSkillID);
	const vector<SRobotAICfg>&		GetRobotAICfgVec(){return m_cRobotAIVec;}
	const SRobotGrpPath*			GetRobotAIPath(INT32 mapID, INT32 grpID);


	bool							IfNPCCanControlByType(EObjectType et);
	bool							IfNPCSameGroupByType(EObjectType et, EWMGroup		egroup);

	const SSkillModelRangeCfg*	    GetSkillModelRangeCfg(UINT32 un32SkillModelID);
	const SSkillModelBufCfg*	    GetSkillModelBuffCfg(UINT32 un32BuffTypeID);

	const SSkillModuleSummonCfg*	GetSkillModelSummonCfg(UINT32 un32SkillModelID);

	const SSNewSkillCfg*			GetSkillFromOriSkillAndNewLevel(UINT32 un32OriSkillID, INT32 un32NewLevel);
	bool							IfInitAll(){return m_IfInitAll;}
	void							SetInitAll(bool ifInited){m_IfInitAll = ifInited;}

	vector<INT32>*					GetRobotUseHeroVec(){return &m_robotChooseHeroVec;}
private:
	map<EObjectType, SGoodsCfg>				m_cGoodsCfgMap;
	map<UINT32, SSNewSkillCfg>				m_cSkillCfgMap;
	map<UINT32, ESkillEffectType>			m_cSkillModelTypeMap;
	map<UINT32, SSkillModelAccountCfg>		m_cSkillModelAccountCfgMap;
	map<UINT32, SSkillModelLeadingCfg>		m_cSkillModelLeadingCfgMap;
	map<UINT32, SSkillModelEmitCfg>			m_cSkillModelEmitCfgMap;
	map<UINT32, SSkillModelRangeCfg>		m_cSkillModelRangeCfgMap;
	map<UINT32, SSkillModelBufCfg>			m_cSkillModelBufCfgMap;
	map<UINT32, SSkillModuleSummonCfg>		m_cSkillModelSummonCfgMap;
	map<UINT32, SSkillModelMoveCfg>			m_cSkillModelMoveCfgMap;
	map<UINT32, SSkillModelSwitchCfg>		m_cSkillModelSwitchCfgMap;
	map<UINT32, SSkillModelPurificationCfg>		m_cSkillModelPurificationCfgMap;
	map<UINT32, SSkillModelLinkCfg>			m_cSkillModelLinkCfgMap;

	map<UINT32, SSPassiveSkilllCfg>			m_cPassitiveSkillCfgMap;
	map<UINT32, EPassiveSkillType>			m_cPassitiveTypeMap;
	map<UINT32, SSPassiveEffectCfg_BloodSeek>	m_cPassitiveEffectBloodSeekCfgMap;
	map<UINT32, SSPassiveEffectCfg_Rebound>	m_cPassitiveEffectReboundCfgMap;
	map<UINT32, SSPassiveEffectCfg_Relive>	m_cPassitiveEffectReliveCfgMap;

	map<EObjectType, SNPCCfg>				m_cNPCCfgMap;
	map<EObjectType, SHeroCfg>				m_cHeroCfgMap;
	map<UINT32,		 SMapCfg*>				m_cMapCfgMap;
	map<UINT32, SMapObjCfg>					m_cMapObjCfgMap;
	map<EObjectType, SShopCfg>				m_cShopMap;
	map<UINT32, SSNewSkillCfg*>				m_cpSkillLevelMap;
	vector<SRobotAICfg>						m_cRobotAIVec;
	map<INT32, SRobotMapPath>				m_cRobotAIPathMap;
	volatile bool m_IfInitAll;
	typedef map<UINT32, MapStaticData>		MapStaticDataMap;
	MapStaticDataMap						m_MapStaticDataMap;
	string									m_pszPublicLuaStr;

	vector<INT32>							m_robotChooseHeroVec;

	INT32				OnMsgFromCS_NotifyNewSkillCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifySkillModelAccountCfgCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifySkillModelEmitCfgList(const char* pData, int n32DataLength); 
	INT32				OnMsgFromCS_NotifySkillModelRangeCfgList(const char* pData, int n32DataLength); 
	INT32				OnMsgFromCS_NotifySkillModelLeadingCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifySkillModelBufCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifySkillModelSummonCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifySkillModelMoveCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifySkillModelSwitchCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifySkillModelPurificationCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifySkillModelLinkCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyPassitiveSkillCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyPassitiveEffectBloodSeekCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyPassitiveEffectReboundCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyPassitiveEffectReliveCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyNPCCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyHeroCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyGoodsCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyShopCfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyRobotAICfgList(const char* pData, int n32DataLength);
	INT32				OnMsgFromCS_NotifyRobotAIPathCfgList(const char* pData, int n32DataLength);

	INT32				OnMsgFromCS_NotifyGameSwitchCfg(const char* pData, int n32DataLength);

	INT32							AddSkillCfg(const SSNewSkillCfg &crsSkillCfg);
	INT32							AddSkillModelAccountCfg(const SSkillModelAccountCfg &crsSkillModelCfg);
	INT32							AddSkillModelLeadingCfg(const SSkillModelLeadingCfg &crsSkillModelCfg);
	INT32							AddSkillModelEmitCfg(const SSkillModelEmitCfg &crsSkillModelCfg);
	INT32							AddNPCCfg(const SNPCCfg &crsNPCCfg);
	INT32							AddHeroCfg(const SHeroCfg &crsHeroCfg);
	INT32							AddGoodsCfg(const SGoodsCfg &crsGoodsCfg);
	INT32							AddShopCfg(const SShopCfg &crsGoodsCfg);
	INT32							AddRobotAICfg(const SRobotAICfg &crsRobotAICfg);
	INT32							AddRobotAIPathCfg(const SRobotAIPathCfg &crsRobotAIPathCfg);
	 
	INT32							AddSkillModelRangeCfg(const SSkillModelRangeCfg &crsSkillModelCfg);
	INT32							AddSkillModelBufgCfg(const SSkillModelBufCfg &crsSkillModelCfg);
	INT32							AddSkillModelSummongCfg(const SSkillModuleSummonCfg &crsSkillModelCfg);
	INT32							AddSkillModelMoveCfg(const SSkillModelMoveCfg &crsSkillModelCfg);
	INT32							AddSkillModelSwitchCfg(const SSkillModelSwitchCfg &crsSkillModelCfg);
	INT32							AddSkillModelPurificationCfg(const SSkillModelPurificationCfg &crsSkillModelCfg);
	INT32							AddSkillModelLinkCfg(const SSkillModelLinkCfg &crsSkillModelCfg);

	INT32							AddPassitiveSkillCfg(const SSPassiveSkilllCfg &crsSkillCfg);
	INT32							AddPassitiveEffectBloodSeekCfg(const SSPassiveEffectCfg_BloodSeek &crsSkillCfg);
	INT32							AddPassitiveEffectReboundCfg(const SSPassiveEffectCfg_Rebound &crsSkillCfg);
	INT32							AddPassitiveEffectReliveCfg(const SSPassiveEffectCfg_Relive &crsSkillCfg);

	UINT32							GetLevelSkillIDFromOriSkillAndLevel(UINT32 un32OriSkillID, INT32 n32Level);

	INT32							LoadMapObjCfg();
	INT32							LoadMapCfg();
	INT32							LoadMapData(UINT32 un32MapID, string &strMapDataFileName, string &strLuaFileName);
	INT32							LoadMapStaticData();
	string&							GetPublicLuaFileStr(){return m_pszPublicLuaStr;}
	SBattleRegion*					GetRegionByDir(const SBattleRegion *cpsRegion, EIntDir eDir, MapStaticData& sMapStaticData);
private:
	//typedef std::map<UINT64/*mapId*/, std::map<INT32/*seatPos*/,CVector3D> >	MAP_HERO_BORN_SEAT_POS; 
	typedef std::map<INT32/*mapid*/, std::map<INT32,SHeroBornPos > >			MAP_HERO_BORN_SEAT_POS;	
	MAP_HERO_BORN_SEAT_POS														mapHeroBornPosCfg;

	typedef	std::map<UINT64/*mapId*/, std::map<EWMLevel,INT64>	>				MAP_VM_LEVEL_DELAY_TIME_CFG;
	MAP_VM_LEVEL_DELAY_TIME_CFG													mapWMLvDelayTimeCfg;

	typedef std::map<UINT64/*mapId*/, std::map<INT32, SWMRefrshCfg> >			MAP_WMCDMilsec_Cfg;
	MAP_WMCDMilsec_Cfg															mapWMCDMilsecCfg;

	typedef std::map<UINT64/*mapId*/, std::map<UINT32, UINT32>	>				ALTAR_BORNNPC_DELAYTIME_CFG; 
	ALTAR_BORNNPC_DELAYTIME_CFG													mapAltarBornDelayNPCCfg;
	typedef std::map<UINT64/*mapId*/, std::map<UINT32, tagBornSolderCfg> >		ALTAR_BORNNPCTIMECONFIG; 
	ALTAR_BORNNPCTIMECONFIG													    mapAltarBornNPCCfg;												

	//bornNPC
	std::map<UINT64, INT32>														mapNPCBornDelayTime;
								
	// OB system
	typedef std::map<UINT64/*mapId*/, bool >									MAP_OB_SYSTEM_SWITCH;
	MAP_OB_SYSTEM_SWITCH														mapOBSystem;

	//new guide
	typedef std::map<UINT64/*mapId*/, bool >									MAP_GUIDE_SWITCH;
	MAP_GUIDE_SWITCH															mapGuideSwitch;

	typedef	vector<INT32>														GUIDE_HERO_ID_CFG;
	GUIDE_HERO_ID_CFG															m_asHeroIDCfg;  
	
public:  
//func switch
	SSFuncSwitch																m_FuncSwitch; 

	INT32						LoadSSHeroBornCfg();

	//void						addHeroBornPos(UINT64 mapId, INT32 seatPos, const CVector3D & bornPos); 
	CVector3D					getHeroBornPosByCamp(UINT64 mapId, INT32 campId);
	CVector3D					getHeroBornDirByCamp(UINT64 mapId, INT32 campId);

	bool						getHeroBornCfg(INT32 mapId, INT32 campId, SHeroBornPos & hb);

	// INT32						getHeroIdByMapCamp(UINT64 mapId, INT32 campId);

	void						addVMNPCBornDelayTimeCfg(UINT64 mapId, EWMLevel elv,INT32 timeCfg);
	void				 		getVMNPCBornDelayTimeCfg(UINT64 mapId , map<EWMLevel,INT64> & v);

	void						addWMCDMilsecCfg(UINT64 mapId, INT32 nPoint, const SWMRefrshCfg & vcfg);
	void						getWMCDMilsecCfg(UINT64 mapId ,map<INT32, SWMRefrshCfg> & v);


	void						addAltarBornNPCCfg(UINT64 mapId,INT32 altarIdx ,INT32 tSpace,INT32 delayTime, INT32 num);
	 void						 getAltarBornNPCCfg(UINT64 mapId, std::map<UINT32, tagBornSolderCfg>&  tbsCfg);
	void						  getAltarBornNPCDelayCfg(UINT64 mapId, std::map<UINT32, UINT32> &  adelayCfg);

	void						addNPCBornDelayCfg(UINT64 mapId, INT32 dealyTime);
	INT32						getNPCBornDelayCfg(UINT64 mapId );

	map<UINT32,		 SMapCfg*>& getMapCfg()	{return 				m_cMapCfgMap;}
	//////////////////////////////////////////////////////////////////////////
	//OB system
	//////////////////////////////////////////////////////////////////////////
	void						addOBSystemSwitchCfg(UINT64 mapId, bool b);
	bool						getOBSystemSwitchCfgByMapId(UINT64 mapId);

	//////////////////////////////////////////////////////////////////////////
	//new guide
	//////////////////////////////////////////////////////////////////////////
	void						addMapGuideSwitch(UINT64 mapId, bool b);
	bool						getMapGuideSwitch(UINT64 mapId );

	void						addGuideHeroIdCfg(UINT32  idx);
	INT32						getGuideCfgHeroId();  

	
	//game log switch
	void						SetSSFuncSwitch(const SSFuncSwitch & param)		{ m_FuncSwitch.set( param );}
	SSFuncSwitch&				GetSSFuncSwitch(void)							{return m_FuncSwitch;}
	
	///////////////////////////////////////出兵//////////////////////////////////////////////////////
private:
	std::map<INT32/*mapid*/,std::map<INT32/*pathid*/,SSolderGoWayPointCfg > >			m_MapPathPointCfg;	
	std::map<INT32/*mapid*/,std::map<INT32/*pathid*/,SMapSolderInfoCfg > >				m_MapSolderBaseInfoCfg;

	std::map<INT32/*mapid*/,std::map<INT32/*altarIdx*/,SAltarSolderCfg > >				m_MapAltarSolderInfoCfg;

	std::map<INT32/*mapid*/,SGameDelayTimeCfg>											m_GameDelayTimeCfg;		

	std::map<INT32/*mapId*/,std::map<INT32/*wm point*/,SGameWildMonsterCfg> >			m_GameWildMonsterCfg;
public:
	INT32						LoadSSMapPathPointCfg();
	INT32						LoadSSMapSolderInfoCfg();
	INT32						LoadSSMapAltarSolderInfoCfg();
	INT32						LoadSSMapFunDelayCfg();
	INT32						LoadSSMapWildMonsterCfg();

	INT64						GetSSMapFuncDelayTimeCfg(EGameFunDelay  etype, INT32 mapId );
	
	void						GetSSSolderSearPathPointCfg(INT32 mapId, std::map<INT32/*pathid*/,SSolderGoWayPointCfg > & pathCfg);

	void						GetSSSolderBornInfoCfg(INT32 mapId, std::map<INT32,SMapSolderInfoCfg >& pathCfg);


	INT32						GetSSAltarBelongPathCfg(INT32 mapId,INT32 altarId,std::vector<INT32> &altarCfg); 

	void						GetSSAltarSolderPosInfo(INT32 mapId, INT32 altarid,SAltarSolderCfg & ascfg);

	INT32						GetSSPathBelongCamp(INT32 mapId, INT32 pathId);

	void						GetWildMonsterCfg(INT32 mapid,std::map<INT32/*wm point*/,SGameWildMonsterCfg>& tmap );

	bool						CheckObjIdIsBaseBuild(INT32 mapid, INT64 objidx, INT32 &pathId);


	////////////////////////////////新手引导配置/////////////////////////////////////////////////////
private:
	//std::map<INT32/*mapid*/,SGameDelayTimeCfg>											m_GuideDelayCfg;
	//std::map<INT32/*mapId*/,std::map<INT32/*wm point*/,SGameWildMonsterCfg> >			m_GuideWildMonsterCfg;

	std::map<INT32/*mapId*/,std::map<INT32/*taskid*/,SGuideSolderCfg> >					m_GuideSolderCfg;  

	std::map<INT32/*mapId*/,std::map<INT32/*taskid*/,SGuideHeroCfg> >					m_GuideHeroCfg; 

	std::map<INT32/*mapId*/,std::vector<INT32> >										m_GuideUserHeroCfg;

	std::map<INT32/*mapId*/, std::map<INT32/*TASKID*/,SGuideHurtBuildCfg> >				m_GuideHurtBuildCfg;

    std::string                                                                         m_strExeDir;
public:  
	INT32						LoadSSGuideSolderCfg();	
	INT32						LoadSSGuideHeroCfg();
	INT32						LoadSSGuideUserHeroCfg();
	INT32						LoadSSGuideHurtObjCfg();

	void						GetGuideSolderCfg(INT32 mapId, INT32 taskId, SGuideSolderCfg & cfg);
#if 0
	INT32						LoadSSGuideMonsterCfg();
	INT32						LoadSSGuideDelayCfg();
	INT32						GetGuideDelayCfg(EGameFunDelay  etype, INT32 mapId );
#endif
	void						GetGuideHeroCfg(INT32 mapId, INT32 taskId,SGuideHeroCfg&  cfg);

	INT32						GetUserHeroIdByMapId(INT32 mapId);

	void						GetGuideHurtBuildCfg(INT32 mapId,INT32 taskId, SGuideHurtBuildCfg &cfg);

	void						GetGuideSolderPathPointCfg(INT32 mapId,INT32 altarId, std::vector<CVector3D> &vec);

	
};

}
	   