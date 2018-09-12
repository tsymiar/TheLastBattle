/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:CSCfgMgr.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:April 2 2014
* summary			:
*
*/

#pragma once
#include "stdafx.h"
#include <unordered_map>

namespace CentralServer{

class CCSBattleMgr;

typedef map<UINT32, SGoodsBuyCfg> GoodsCfgMap;

class CCSCfgMgr
{
	friend class CCSBattleMgr; 

private:
	std::map<INT32, SNPCCfg>				m_cNPCCfgMap;
	std::map<INT32, SHeroCfg>				m_cHeroCfgMap;
	std::vector<UINT32>						m_cHeroHeadMap;
	std::map<INT32, SGoodsCfg>				m_cGoodsCfgMap;
	std::map<INT32, SShopCfg>				m_cShopCfgMap;
	std::set<UINT32>						m_cMapCfgSet;
	std::map<UINT32, SMapLogicCfg*>			m_cMapLogicCfg;
	map<UINT32, SHeroChooseCfg>				m_cHeroChooseCfg;
	vector<SRobotAICfg>						m_cRobotAICfg;
	map<INT32, SRobotAIPathCfg>				m_cRobotAIPathCfg;
	CHAR*									m_voipServerStr;
	INT32									m_voipPort;
	map<UINT32, SSNewSkillCfg>				m_cNewSkillCfgMap;
	map<UINT32, ESkillEffectType>			m_cSkillModelTypeMap;
	map<UINT32, SSkillModelAccountCfg>		m_cSkillModelAccountCfgMap;
	map<UINT32, SSkillModelLeadingCfg>		m_cSkillModelLeadingCfgMap;
	map<UINT32, SSkillModelEmitCfg>			m_cSkillModelEmitCfgMap;
	map<UINT32, SSkillModelRangeCfg>		m_cSkillModelRangeCfgMap;
	map<UINT32, SSkillModelBufCfg>			m_cSkillModelBufCfgMap;
	map<UINT32, SSkillModuleSummonCfg>		m_cSSkillModuleSummonCfgMap;
	map<UINT32, SSkillModelMoveCfg>			m_cSkillModuleMoveCfgMap;
	map<UINT32, SSkillModelSwitchCfg>		m_cSkillModuleSwitchCfgMap;
	map<UINT32, SSkillModelPurificationCfg>	m_cSkillModulePurificationCfgMap;
	map<UINT32, SSkillModelLinkCfg>			m_cSkillModuleLinkCfgMap;
	map<UINT32, SSPassiveSkilllCfg>			m_cPassitiveSkillCfgMap;
	map<UINT32, EPassiveSkillType>			m_cPassitiveSkillTypeMap;
	map<UINT32, SSPassiveEffectCfg_BloodSeek>	m_cPassitiveEffectBloodSeekCfgMap;
	map<UINT32, SSPassiveEffectCfg_Rebound>	m_cPassitiveEffectReboundCfgMap;
	map<UINT32, SSPassiveEffectCfg_Relive>	m_cPassitiveEffectReliveCfgMap;
	vector<string>							m_cAiRobotNameMap;
	map<string,bool>						m_cAiRobotNameMapForCheck;
	map<UINT32, STaskConfig>				m_cTaskConfigs;
	
private:
	CCSCfgMgr();
public:
	~CCSCfgMgr();
	static CCSCfgMgr &getInstance(){
		static CCSCfgMgr cfg;
		return cfg;
	}

public:
	std::set<UINT32>&	GetMapCfgMap(){return m_cMapCfgSet;}
	const CHAR*		GetVoipServer();
	INT32			GetVoipPort();
	SMapLogicCfg*	GetMapLogicCfg(UINT32 id) { return m_cMapLogicCfg[id]; }
	vector<UINT32>& GetHeroHeadMap() { return m_cHeroHeadMap; }
	vector<string>& GetAiRobotNameMap() { return m_cAiRobotNameMap; }
	map<string,bool>& GetAiRobotNameMapForCheck() { return m_cAiRobotNameMapForCheck; }

public:
	INT32			Initalize();
	INT32			Uninitialize();

public:
	INT32			LoadNPCCfg();
	INT32			LoadHeroChooseCfg();
	VOID			DoLoadRobotAICfg(ERobotAIType eAiType, const char* fileName);
	INT32			LoadRobotAICfg();
	INT32			LoadRobotAIPathCfg();
	INT32			LoadHeroCfg();
	INT32			LoadHeroHeadCfg();//读取英雄头像//
	INT32			LoadGoodsCfg();
	INT32			LoadShopCfg();
	INT32			LoadMapCfg();
	INT32			LoadMapData(UINT32 un32MapID);
	INT32			LoadMapLogicCfg();
	INT32			LoadVoipCfg();
	INT32			LoadFilterWordsCfg();
	INT32			LoadNewSkillCfg();
	INT32			LoadSkillModelAccountCfg();
	INT32			LoadSkillModelLeadingCfg();
	INT32			LoadSkillModelEmitCfg();
	INT32			LoadSkillModelRangeCfg();
	INT32			LoadSkillModelBufCfg();
	INT32			LoadSkillModelSummonCfg();
	INT32			LoadSkillModelMoveCfg();
	INT32			LoadSkillModelSwitchCfg();
	INT32			LoadSkillModelPurifactionCfg();
	INT32			LoadSkillModelLinkCfg();
	INT32			LoadPassitiveSkillCfg();
	INT32			LoadPassitiveEffectBloodSeekCfg();
	INT32			LoadPassitiveEffectReboundCfg();
	INT32			LoadPassitiveEffectReliveCfg();
	INT32			LoadAiRobotName();
	INT32			LoadDailyTaskConfig();
	INT32			LoadInfiniteTaskConfig();

private:
	void			SpilitEventStr(std::string & str, std::vector<SSNextSkillEffectCfg>& tSNSE );
	void			DoEventStr(std::string & str, SSNextSkillEffectCfg  *asSkillModelList,INT32 num);

	////////////////////////////////////////////////////////////////////////////////////////////////
public:
	INT32						LoadUserCfg();
	INT32						ResetUserCfg();

	const SRunesCfg*			GetRunesCfg(UINT32 eOT) const;
	map<INT32, SRunesCfg>&		GetRunesMap(){return m_cRunesCfgMap;}
	const SRunesComposeInfo*	GetRunesComposeInfo(UINT8 un8Level) const;
	const INT32					GetUserLvUpExp(UINT16 un16UserLv) const;
	map<UINT32, SHeroBuyCfg>&   GetHeroBuyCfgmap(){ return m_HeroBuyCfgMap;}

	SHeroBuyCfg*				GetHeroBuyCfg(UINT32 HeroGoodsID);
	SHeroBuyCfg*				GetHeroClientMatchCfg(UINT32 HeroGoodsID);

	INT32						GetUserMaxLv();
	SLoginReward*				GetLoginReward(UINT32 undays);
	INT32						GetComsume(EGoodsType goodsType, EComsumeType consumeType, UINT32 un32GoodsID);

	void						GetGuideAwardInfo( INT32 mapId,    CSGuideAward &cfg);

	SOtherItem*					GetOtherItemCfg(UINT32 item_id);


	UserDbSaveConfig&			GetCSGlobalCfg()	{ return m_UserDbSaveCfg;}

	INT32						GetLoginRewardLen()	{ return m_cLoginRewardCfgMap.size();}
	map<UINT32, SDiscountCfg>&	GetDisCountMapByType(EGoodsType goodsType){return m_cDiscountCfgMap;}
	vector<UINT32>&		GetNewCfgVec(){return m_NewGoodsCfgVec;}
	vector<UINT32>&		GetHotGoodsCfg(){return m_HotGoodsCfgVec;}

	SDBCfg	&					GetDBCfg(EDBType dt);

	SRunesCfg*		GetRanmRuneFromLevel(UINT16 level);
	SRunesCfg*		GetNextLevelRunesFromFixedRunesID(UINT32 id);

	bool			CheckInvalidWorld( const char* word);
	STaskConfig*	GetTaskConfig(UINT32 taskId);

private:
	INT32			LoadRunesCfg();
	INT32			LoadRunesSlotCfg();
	INT32			AddRunesComposeInfo();
	INT32			LoadUserExpCfg();
	INT32			LoadHeroBuyCfg();
	INT32			LoadDiscountCfg();
	INT32			LoadLoginRewardCfg();
	INT32			LoadOtherItemCfg();

	INT32			LoadInvalidWordCfg();

	INT32			LoadGuideBattleEndAward();

	INT32			LoadUserDBSaveCfg();

	//About Goods
	INT32			LoadHotAndNewGoodsCfg();	

	INT32			LoadSDBCfg();
private:
	map<INT32, SRunesCfg>				m_cRunesCfgMap;
	map<INT32, SRunesSlotCfg>			m_cRunesSlotCfgMap;
	map<UINT16, SUserLvUpExpCfg>		m_cUserCfgMap;

	typedef std::unordered_map<UINT8, vector<UINT32>> RunesComposeInfoMap;
	RunesComposeInfoMap				m_RunesComposeInfoMap;
	map<UINT32, UINT32>				m_UperRunesComposeMap;

	map<UINT32, SHeroBuyCfg>		m_HeroBuyCfgMap;
	map<UINT32, SHeroBuyCfg>		m_HeroClientMatchMap;
	map<UINT32, SDiscountCfg>		m_cDiscountCfgMap;
	map<UINT32, SLoginReward>		m_cLoginRewardCfgMap;

	vector<UINT32>					m_HotGoodsCfgVec;
	vector<UINT32>					m_NewGoodsCfgVec;
	//屏蔽字
	std::vector<std::string>        m_invalidWorld; 

	//新手引导结束奖励
	std::map<UINT32/*mapId*/,  CSGuideAward   >   m_GuideEndAward;

	//用户数据保存(设置为定时)
	UserDbSaveConfig				m_UserDbSaveCfg;
	//SDBCfg							m_DbCfg;
	std::default_random_engine		m_Generator;

	std::map<EDBType,SDBCfg>		m_mapDBCfg;
	std::map<UINT32, SOtherItem>	m_OtherItemMap;
    //当前可执行程序目录
    std::string                     m_strExeDir;
};

}
