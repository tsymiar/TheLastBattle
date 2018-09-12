#pragma once
#include "stdafx.h"
#include <unordered_map>

namespace ReBot{

	class CChineseCode{
	public:

		static void UTF_8ToUnicode(wchar_t* pOut,char *pText);  // 把UTF-8转换成Unicode
		static void UnicodeToUTF_8(char* pOut,wchar_t* pText);  //Unicode 转换成UTF-8
		static void UnicodeToGB2312(char* pOut,wchar_t uData);  // 把Unicode 转换成 GB2312 
		static void Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer);// GB2312 转换成　Unicode
		static void GB2312ToUTF_8(string& pOut,char *pText, int pLen);//GB2312 转为 UTF-8
		static void UTF_8ToGB2312(string &pOut, char *pText, int pLen);//UTF-8 转为 GB2312
	};

class CCfgMgr
{
public:
	CCfgMgr();
	~CCfgMgr();
	static CCfgMgr &getInstance(){
		static CCfgMgr cfg;
		return cfg;
	}

public:
	INT32			Initalize();
	INT32			Uninitialize();

public:
	std::set<UINT32>&			GetMapCfgMap(){return m_cMapCfgSet;}
	const CHAR*					GetVoipServer();
	INT32						GetVoipPort();
	SMapLogicCfg*				GetMapLogicCfg(UINT32 id) { return m_cMapLogicCfg[id]; }
	vector<UINT32>&				GetHeroHeadMap() { return m_cHeroHeadMap; }
	vector<string>&				GetAiRobotNameMap() { return m_cAiRobotNameMap; }
	map<string,bool>&			GetAiRobotNameMapForCheck() { return m_cAiRobotNameMapForCheck; }
	INT32						LoadUserCfg();
	INT32						ResetUserCfg();
	const SRunesComposeInfo*	GetRunesComposeInfo(UINT8 un8Level) const;
	const INT32					GetUserLvUpExp(UINT16 un16UserLv) const;
	INT32						GetUserMaxLv();
	INT32						GetGuideAwardByType(EGuideAward eType, INT32 mapId);
	vector<UINT32>&				GetNewCfgVec(){return m_NewGoodsCfgVec;}
	vector<UINT32>&				GetHotGoodsCfg(){return m_HotGoodsCfgVec;}
	SDBCfg&						GetDBCfg(EDBType dt);
	bool						CheckInvalidWorld( const char* word);
	void						SpilitEventStr(std::string & str, std::vector<SSNextSkillEffectCfg>& tSNSE );
	void						DoEventStr(std::string & str, SSNextSkillEffectCfg  *asSkillModelList,INT32 num);
	SNPCCfg*					GetNPCCfg(INT32 key);
	SHeroCfg*					GetHeroCfg(INT32 key);
	SGoodsCfg*					GetGoodsCfg(UINT32 key);
	SSNewSkillCfg*				GetSkillCfg(UINT32 un32SkillID);
	std::map<INT32, SGoodsCfg>& GetGoodsMap(){return m_cGoodsCfgMap;}

public:
	INT32			LoadFilterWordsCfg();
	INT32			LoadNPCCfg();
	INT32			LoadHeroChooseCfg();
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
	INT32			LoadPassitiveSkillCfg();
	INT32			LoadPassitiveEffectBloodSeekCfg();
	INT32			LoadPassitiveEffectReboundCfg();
	INT32			LoadPassitiveEffectReliveCfg();
	INT32			LoadAiRobotName();
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
	INT32			LoadHotAndNewGoodsCfg();	
	INT32			LoadSDBCfg();

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
	map<UINT32, SSPassiveSkilllCfg>			m_cPassitiveSkillCfgMap;
	map<UINT32, EPassiveSkillType>			m_cPassitiveSkillTypeMap;
	map<UINT32, SSPassiveEffectCfg_BloodSeek>	m_cPassitiveEffectBloodSeekCfgMap;
	map<UINT32, SSPassiveEffectCfg_Rebound>	m_cPassitiveEffectReboundCfgMap;
	map<UINT32, SSPassiveEffectCfg_Relive>	m_cPassitiveEffectReliveCfgMap;
	vector<string>							m_cAiRobotNameMap;
	map<string,bool>						m_cAiRobotNameMapForCheck;
	map<INT32, SRunesSlotCfg>				m_cRunesSlotCfgMap;
	map<UINT16, SUserLvUpExpCfg>			m_cUserCfgMap;
	typedef std::unordered_map<UINT8, vector<UINT32>> RunesComposeInfoMap;
	RunesComposeInfoMap						m_RunesComposeInfoMap;
	map<UINT32, UINT32>						m_UperRunesComposeMap;
	vector<UINT32>							m_HotGoodsCfgVec;
	vector<UINT32>							m_NewGoodsCfgVec;
	std::vector<std::string>				m_invalidWorld; 
	std::map<EDBType,SDBCfg>				m_mapDBCfg;
};

}
