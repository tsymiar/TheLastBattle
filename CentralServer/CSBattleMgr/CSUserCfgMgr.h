#pragma once
#include "stdafx.h"
#include <unordered_map>
#include <random>

namespace CentralServer{
class CCSUserMgr;

typedef map<UINT32, SGoodsBuyCfg> GoodsCfgMap;
class CCSUSerCfgMgr
{
private:
	CCSUSerCfgMgr();

public:
	~CCSUSerCfgMgr();

	static CCSUSerCfgMgr &getInstance(){
		static CCSUSerCfgMgr cfg;
		return cfg;
	}

	INT32							Initalize();
	INT32							Uninitialize();

	const SRunesCfg*			GetRunesCfg(UINT32 eOT) const;
	const SRunesComposeInfo*	GetRunesComposeInfo(UINT8 un8Level) const;
	const INT32					GetUserLvUpExp(UINT16 un16UserLv) const;
	map<UINT32, SHeroBuyCfg>&   GetHeroBuyCfgmap(){ return m_HeroBuyCfgMap;}

	SHeroBuyCfg*				GetHeroBuyCfg(UINT32 heroid);

	INT32						GetUserMaxLv();
	void						GetLoginReward(UINT32 undays, SLoginReward& temp);
	INT32						GetComsume(EGoodsType goodsType, EComsumeType consumeType, UINT32 un32GoodsID);

	INT32						GetGuideAwardByType(EGuideAward eType, INT32 mapId);


	UserDbSaveConfig&			GetCSGlobalCfg()	{ return m_UserDbSaveCfg;}
	 
	INT32						GetLoginRewardLen()	{ return m_cLoginRewardCfgMap.size();}
	map<UINT32, SDiscountCfg>&	GetDisCountMapByType(EGoodsType goodsType){return m_cDiscountCfgMap;}
	vector<UINT32>&		GetNewCfgVec(){return m_NewGoodsCfgVec;}
	vector<UINT32>&		GetHotGoodsCfg(){return m_HotGoodsCfgVec;}

	SDBCfg	&					GetDBCfg()				{return								m_DbCfg;}
	SRunesCfg*		GetRanmRuneFromLevel(UINT16 level);
	SRunesCfg*		GetNextLevelRunesFromFixedRunesID(UINT32 id);

	bool			CheckInvalidWorld( const char* word);

private:
	INT32			LoadRunesCfg();
	INT32			LoadRunesSlotCfg();
	INT32			AddRunesComposeInfo();
	INT32			LoadUserExpCfg();
	INT32			LoadHeroBuyCfg();
	INT32			LoadDiscountCfg();
	INT32			LoadLoginRewardCfg();
	
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
	map<UINT32, SDiscountCfg>		m_cDiscountCfgMap;
	map<UINT32, SLoginReward>		m_cLoginRewardCfgMap;

	vector<UINT32>				m_HotGoodsCfgVec;
	vector<UINT32>				m_NewGoodsCfgVec;
	//屏蔽字
	std::vector<std::string>        m_invalidWorld; 

	//新手引导结束奖励
	std::map<UINT32/*mapId*/,std::map<EGuideAward, INT32 > >   m_GuideEndAward;

	//用户数据保存(设置为定时)
	UserDbSaveConfig				m_UserDbSaveCfg;
	SDBCfg							m_DbCfg;
	std::default_random_engine		m_Generator;
};

}
