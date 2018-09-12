#include "stdafx.h"
#include "CSCfgMgr.h"
#include "boost/format.hpp"
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include <boost/property_tree/xml_parser.hpp>
 
#include "tinyxml.h"

#include <fstream>
using namespace boost::property_tree;

namespace CentralServer{ 

typedef vector< string > split_vector_type;

INT32 CCSCfgMgr::LoadRunesCfg()
{
	std::string cpszfilename = m_strExeDir + "./CSBattleMgr/IAPShop/RuneCfg.xml";
	ptree cPtree;
	ptree cRunesCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cRunesCfg = cPtree.get_child("RuneCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "%s open cfg file fail for %s.", __FUNCTION__, cException.what());
		return eEC_OpenCfgFileFail;
	}

	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	SRunesCfg sTempCfg;
	memset(&sTempCfg, 0, sizeof(sTempCfg));

	try{
		for (ptree::iterator iterInfo = cRunesCfg.begin(); iterInfo != cRunesCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			memset(&sTempCfg, 0, sizeof(sTempCfg));

			sTempCfg.eOT = (EObjectType)iterInfo->second.get<INT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.eOT){
				continue;
			}

			sTempCfg.un8Level = iterInfo->second.get<UINT8>("un8Level");
			sTempCfg.un8EffectID = iterInfo->second.get<UINT8>("un8EffectID");
			sTempCfg.fEffectVal = iterInfo->second.get<FLOAT>("n32EffectVal");
			sTempCfg.fEffectPer = iterInfo->second.get<FLOAT>("fEffectPer");
			sTempCfg.bIsCanComposed = iterInfo->second.get<bool>("bIsCanRandomComposed");
			sTempCfg.n32ComposedSubID = iterInfo->second.get<INT32>("n32ComposedSubID");
			sTempCfg.bIfShowInShop = iterInfo->second.get<bool>("bIsShowInShop");

			split_vector_type sConsumeTypeVec;
			string sConsumeType = iterInfo->second.get<string>("un8ConsumeType");
			boost::algorithm::split(sConsumeTypeVec, sConsumeType, boost::algorithm::is_any_of(","));

			split_vector_type sCPVec;
			string sCP = iterInfo->second.get<string>("n32PriceSeries");
			boost::algorithm::split(sCPVec, sCP, boost::algorithm::is_any_of(","));

			if (sCPVec.size() != sConsumeTypeVec.size()){
				break;
			}

			for (INT32 i = 0; i < sCPVec.size(); ++i){
				ConsumeStruct sConsumeStruct(atoi(sConsumeTypeVec[i].c_str()), atoi(sCPVec[i].c_str()));
				sTempCfg.sConsumeList.AddElement(sConsumeStruct);
			}

			m_cRunesCfgMap[sTempCfg.eOT] = sTempCfg;

			if (!sTempCfg.bIsCanComposed){
				continue;
			}

			if (sTempCfg.n32ComposedSubID != 0){
				m_UperRunesComposeMap[sTempCfg.n32ComposedSubID] = sTempCfg.eOT;
			}

			RunesComposeInfoMap::iterator iter = m_RunesComposeInfoMap.find(sTempCfg.un8Level);
			if (m_RunesComposeInfoMap.end() == iter){
				vector<UINT32> runesSet;
				runesSet.push_back(sTempCfg.eOT);
				m_RunesComposeInfoMap.insert(std::make_pair(sTempCfg.un8Level, runesSet));
			}
			else{
				iter->second.push_back(sTempCfg.eOT);
			}
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s(eOT:%d).", cException.what(), sTempCfg.eOT);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d runes cfg.", (UINT32)m_cRunesCfgMap.size());
	return eNormal;
}

INT32 CCSCfgMgr::LoadUserExpCfg()
{
	std::string cpszfilename = m_strExeDir + "./CSBattleMgr/UserAccountCfg.xml";
	ptree cPtree;
	ptree cUserExpCfg;
	try{
		read_xml(cpszfilename.c_str(), cPtree, xml_parser::trim_whitespace);
		cUserExpCfg = cPtree.get_child("WildMonsterCfg");   // WildMonsterCfg
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail for %s.", cException.what());
		return eEC_OpenCfgFileFail;
	}
	SUserLvUpExpCfg sTempCfg;
	try
	{
		for (ptree::iterator iterInfo = cUserExpCfg.begin(); iterInfo != cUserExpCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.un16UserLv = (EObjectType)iterInfo->second.get<INT32>("<xmlattr>.UserLevel", 0);
			sTempCfg.un32LvUpExp = iterInfo->second.get<UINT32>("LevelUpExp", 0);
			m_cUserCfgMap[sTempCfg.un16UserLv] = sTempCfg;
		}

	}catch(std::exception &cException){
		ELOG(LOG_DBBUG, "read cfg file fail for %s(UseLv:%d).", cException.what(), sTempCfg.un16UserLv);
		return eEC_ReadCfgFileFail;
	}
	return eNormal;
}

INT32 CCSCfgMgr::LoadLoginRewardCfg()
{
	std::string cpszfilename = m_strExeDir + "./CSBattleMgr/IAPShop/dailybonus.xml";
	ptree cPtree;
	ptree cDailybonusCfg;
	try{
		read_xml(cpszfilename.c_str(), cPtree, xml_parser::trim_whitespace);
		cDailybonusCfg = cPtree.get_child("DailyBonus");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail for %s.", cException.what());
		return eEC_OpenCfgFileFail;
	}

	SLoginReward sTempCfg;
	UINT32 un32Temp = 0;
	try
	{
		for (ptree::iterator iterInfo = cDailybonusCfg.begin(); iterInfo != cDailybonusCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.n32Days = iterInfo->second.get<UINT32>("<xmlattr>.n32Days", 0);

			string strType = iterInfo->second.get<string>("n32Type", "");
			string strItemType = iterInfo->second.get<string>("n32ItemID", "");
			string strItemNum = iterInfo->second.get<string>("n32ItemNum", "");

			vector<string> typeVec;
			boost::algorithm::split(typeVec, strType, boost::algorithm::is_any_of(","));
			vector<string> itemtypeVec;
			boost::algorithm::split(itemtypeVec, strItemType, boost::algorithm::is_any_of(","));
			vector<string> itemnumVec;
			boost::algorithm::split(itemnumVec, strItemNum, boost::algorithm::is_any_of(","));

			if (typeVec.size() != itemtypeVec.size() || typeVec.size() != itemnumVec.size() || typeVec.size() > MAX_REWARD){
				ELOG(LOG_ERROR, "read cfg file fail for size un equal.");
				return eEC_ReadCfgFileFail;
			}

			for (INT32 i = 0; i < typeVec.size(); ++i){
				sTempCfg.eRewardType[i] = (ERewardType)atoi(typeVec[i].c_str());
				sTempCfg.eItemType[i] = (ELoginRewardItemType)atoi(itemtypeVec[i].c_str());
				sTempCfg.un32num[i] = atoi(itemnumVec[i].c_str());
			}

			m_cLoginRewardCfgMap.insert(make_pair(sTempCfg.n32Days, sTempCfg));
		}

	}catch(std::exception &cException){
		ELOG(LOG_DBBUG, "read cfg file fail for %s(un32ID:%d).", cException.what(), un32Temp);
		return eEC_ReadCfgFileFail;
	}
	return eNormal;
}

INT32 CCSCfgMgr::LoadOtherItemCfg()
{
	std::string cpszfilename = m_strExeDir + "./CSBattleMgr/IAPShop/OtherItemCfg.xml";
	ptree cPtree;
	ptree OtherItemCfg;
	try{
		read_xml(cpszfilename.c_str(), cPtree, xml_parser::trim_whitespace);
		OtherItemCfg = cPtree.get_child("OtherItemCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail for %s.", cException.what());
		return eEC_OpenCfgFileFail;
	}

	SOtherItem sTempCfg;
	try
	{
		for (ptree::iterator iterInfo = OtherItemCfg.begin(); iterInfo != OtherItemCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.item_id = iterInfo->second.get<UINT32>("<xmlattr>.ID", 0);

			sTempCfg.effect_type = (ItemEffect)iterInfo->second.get<UINT32>("EffectID", 0);
			sTempCfg.effect_value = iterInfo->second.get<UINT32>("EffectValue", 0);
			sTempCfg.consume_type = iterInfo->second.get<INT32>("ConsumeType", 0);
			sTempCfg.price_series = iterInfo->second.get<INT32>("PriceSeries", 0);
			sTempCfg.bIsGetUse = iterInfo->second.get<bool>("bIsGetUse", false);
			sTempCfg.b_inshop = iterInfo->second.get<UINT8>("bIsShowInShop", 0);
			sTempCfg.item_duration = iterInfo->second.get<INT32>("Time", 0);

			m_OtherItemMap[sTempCfg.item_id] = sTempCfg;
		}

	}catch(std::exception &cException){
		ELOG(LOG_DBBUG, "read cfg file fail for %s(ID:%d).", cException.what(), sTempCfg.item_id);
		return eEC_ReadCfgFileFail;
	}
	return eNormal;
}

INT32 CCSCfgMgr::LoadUserCfg(){
	INT32	n32LoadRet = LoadRunesCfg(); 

	if (eNormal == n32LoadRet)
	{
		n32LoadRet = LoadRunesSlotCfg();
	}

	if (eNormal == n32LoadRet){
		n32LoadRet = LoadUserExpCfg();
	}

	if (eNormal == n32LoadRet){
		n32LoadRet = LoadHeroBuyCfg();
	}

	if (eNormal == n32LoadRet){
		n32LoadRet = LoadDiscountCfg();
	}

	if (eNormal == n32LoadRet){
		n32LoadRet = LoadLoginRewardCfg();
	}

	if (eNormal == n32LoadRet)
	{
		n32LoadRet = LoadInvalidWordCfg();
	}

	 
	if (eNormal == n32LoadRet){
		n32LoadRet = LoadUserDBSaveCfg();
	}  
	
	if (eNormal == n32LoadRet)
	{
		n32LoadRet = LoadOtherItemCfg();
	}

	if (eNormal == n32LoadRet)
	{
		n32LoadRet = LoadHotAndNewGoodsCfg();
	}

	if (eNormal == n32LoadRet)
	{
		n32LoadRet = LoadSDBCfg();
	}

	if (eNormal == n32LoadRet)
	{
		n32LoadRet = LoadGuideBattleEndAward();
	}

	return n32LoadRet;
}

INT32 CCSCfgMgr::ResetUserCfg()
{
	m_cRunesCfgMap.clear();
	m_cLoginRewardCfgMap.clear();
	m_cRunesSlotCfgMap.clear();
	m_invalidWorld.clear();

	m_mapDBCfg.clear();
	m_GuideEndAward.clear();
	return eNormal;
}

INT32 CCSCfgMgr::LoadRunesSlotCfg()
{
	std::string cpszfilename = m_strExeDir + "./CSBattleMgr/RuneSlotCfg.xml";
	ptree cPtree;
	ptree cRunesSlotCfg;
	try{
		read_xml(cpszfilename.c_str(), cPtree, xml_parser::trim_whitespace);
		cRunesSlotCfg = cPtree.get_child("RuneSlotCfg");
	}catch(std::exception &cException){
		ELOG(LOG_DBBUG, "open cfg file fail for %s.", cException.what());
		return eEC_OpenCfgFileFail;
	}

	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	SRunesSlotCfg sTempCfg;
	memset(&sTempCfg, 0, sizeof(sTempCfg));

	try{
		for (ptree::iterator iterInfo = cRunesSlotCfg.begin(); iterInfo != cRunesSlotCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			memset(&sTempCfg, 0, sizeof(sTempCfg));

			sTempCfg.eOT = (EObjectType)iterInfo->second.get<INT32>("<xmlattr>.un32TypeID", 0);
			if (0 == sTempCfg.eOT){
				continue;
			}

			sTempCfg.un8Level = iterInfo->second.get<UINT8>("un8SlotLevel");
			sTempCfg.un8OpenLevel = iterInfo->second.get<UINT8>("un8SlotOpenLevel");

			m_cRunesSlotCfgMap[sTempCfg.eOT] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s(eOT:%d).", cException.what(), sTempCfg.eOT);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d runesslot cfg.", (UINT32)m_cRunesSlotCfgMap.size());
	return eNormal;
}

const SRunesCfg* CCSCfgMgr::GetRunesCfg(UINT32 eOT) const
{
	map<INT32, SRunesCfg>::const_iterator iter = m_cRunesCfgMap.find(eOT);
	if (m_cRunesCfgMap.end() != iter)
	{
		return &iter->second;
	}

	return NULL;
}

const INT32 CCSCfgMgr::GetUserLvUpExp(UINT16 un16UserLv) const{
	UINT32	cpsUserExpfCfg = 0;
	map<UINT16, SUserLvUpExpCfg>::const_iterator iter = m_cUserCfgMap.find(un16UserLv);
	if(m_cUserCfgMap.end() != iter)
	{
		cpsUserExpfCfg = iter->second.un32LvUpExp;
	}
	return cpsUserExpfCfg;
}

INT32	CCSCfgMgr::GetUserMaxLv(){
	INT32 n32MaxLv = 0;
	map<UINT16, SUserLvUpExpCfg>::iterator iter = m_cUserCfgMap.begin();
	for (; iter != m_cUserCfgMap.end(); iter ++){
		if (n32MaxLv < iter->second.un16UserLv){
			n32MaxLv = iter->second.un16UserLv;
		}
	}
	return n32MaxLv;
}

SLoginReward* CCSCfgMgr::GetLoginReward(UINT32 undays)
{
	map<UINT32, SLoginReward>::iterator iter = m_cLoginRewardCfgMap.find(undays);
	if (m_cLoginRewardCfgMap.end() != iter)
	{
		return &(iter->second);
	}
	return NULL;
}

INT32 CCSCfgMgr::LoadHeroBuyCfg(){
	std::string cpszfilename = m_strExeDir + "./CSBattleMgr/IAPShop/HeroBuyCfg.xml";
	ptree cPtree;
	ptree cHeroBuyCfg;
	try{
		read_xml(cpszfilename.c_str(), cPtree, xml_parser::trim_whitespace);
		cHeroBuyCfg = cPtree.get_child("HeroBuyCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "%s open cfg file fail for %s.", __FUNCTION__, cException.what());
		return eEC_OpenCfgFileFail;
	}

	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	SHeroBuyCfg sTempCfg;

	try{
		for (ptree::iterator iterInfo = cHeroBuyCfg.begin(); iterInfo != cHeroBuyCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			memset(&sTempCfg, 0, sizeof(sTempCfg));

			sTempCfg.un32CommondityID =  iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32CommondityID){
				continue;
			}

			sTempCfg.un32HeroID =  iterInfo->second.get<UINT32>("UnlockHeroID", 0);

			split_vector_type sHeroKindVec;
			string sHeroKind = iterInfo->second.get<string>("HeroKind");
			boost::algorithm::split(sHeroKindVec, sHeroKind, boost::algorithm::is_any_of(","));
			for (INT32 i = 0; i < sHeroKindVec.size(); ++i){
				sTempCfg.eHeroKind[i] = (EHeroKind)atoi(sHeroKindVec[i].c_str());
			}

			split_vector_type sConsumeTypeVec;
			string sConsumeType = iterInfo->second.get<string>("un8ConsumeType");
			boost::algorithm::split(sConsumeTypeVec, sConsumeType, boost::algorithm::is_any_of(","));

			split_vector_type sCPVec;
			string sCP = iterInfo->second.get<string>("n32Price");
			boost::algorithm::split(sCPVec, sCP, boost::algorithm::is_any_of(","));

			if (sCPVec.size() != sConsumeTypeVec.size()){
				break;
			}
			for (int i = 0; i < sCPVec.size(); ++i){
				ConsumeStruct sConsumeStruct((EConsumeType)atoi(sConsumeTypeVec[i].c_str()), atoi(sCPVec[i].c_str()));
				sTempCfg.sConsumeList.AddElement(sConsumeStruct);
			}

			sTempCfg.bIsShowHot = (bool)iterInfo->second.get<INT32>("bIsShowHot", 0);
			sTempCfg.eDefaultSkin = (ESkinType)iterInfo->second.get<INT32>("DefaultSkin", 0);
			sTempCfg.eOnSaleSkins = (ESkinType)iterInfo->second.get<INT32>("OnSaleSkins", 0);
			sTempCfg.useTimeSpan = iterInfo->second.get<int64_t>("Time", 0);
// 			if (sTempCfg.useTimeSpan > 0)
// 			{
// 				sTempCfg.useTimeSpan *= 3600;
// 			}
			sTempCfg.bIfShowInShop = iterInfo->second.get<bool>("bIsShowInShop");
			m_HeroBuyCfgMap[sTempCfg.un32CommondityID] = sTempCfg;

			if (sTempCfg.un32CommondityID < c_MaxHeroClientMatch){
				m_HeroClientMatchMap[sTempCfg.un32HeroID] = sTempCfg;
			}
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s(eOT:%d).", cException.what(), sTempCfg.un32HeroID);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d hero buy cfg.", (UINT32)m_HeroBuyCfgMap.size());
	return eNormal;
}

INT32 CCSCfgMgr::LoadDiscountCfg(){
	std::string cpszfilename = m_strExeDir + "./CSBattleMgr/IAPShop/DiscountsCfg.xml";
	ptree cPtree;
	ptree cDiscountCfg;
	try{
		read_xml(cpszfilename.c_str(), cPtree, xml_parser::trim_whitespace);
		cDiscountCfg = cPtree.get_child("DiscountsCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "%s open cfg file fail for %s.", __FUNCTION__, cException.what());
		return eEC_OpenCfgFileFail;
	}

	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	SDiscountCfg sTempCfg;
	memset(&sTempCfg, 0, sizeof(sTempCfg));

	try{
		for (ptree::iterator iterInfo = cDiscountCfg.begin(); iterInfo != cDiscountCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			memset(&sTempCfg, 0, sizeof(sTempCfg));

			sTempCfg.un32CommdityID = iterInfo->second.get<INT32>("<xmlattr>.un32ForeignKey", 0);
			if (0 == sTempCfg.un32CommdityID){
				continue;
			}

			split_vector_type sConsumeTypeVec;
			string sConsumeType = iterInfo->second.get<string>("un8ConsumeType");
			boost::algorithm::split(sConsumeTypeVec, sConsumeType, boost::algorithm::is_any_of(","));

			split_vector_type sCPVec;
			string sCP = iterInfo->second.get<string>("n32Price");
			boost::algorithm::split(sCPVec, sCP, boost::algorithm::is_any_of(","));

			if (sCPVec.size() != sConsumeTypeVec.size()){
				break;
			}

			
			for (INT32 i = 0; i < sCPVec.size(); ++i){
				ConsumeStruct sConsumeStruct(atoi(sConsumeTypeVec[i].c_str()), atoi(sCPVec[i].c_str()));
				sTempCfg.sConsumeList.AddElement(sConsumeStruct);
			}

			
			m_cDiscountCfgMap.insert(std::make_pair(sTempCfg.un32CommdityID, sTempCfg));
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s(eOT:%d).", cException.what(), sTempCfg.un32CommdityID);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %u discount cfg.", m_cDiscountCfgMap.size());
	return eNormal;
}

INT32 CCSCfgMgr::GetComsume(EGoodsType goodsType, EComsumeType consumeType, UINT32 un32GoodsID){
	auto discoontIter = m_cDiscountCfgMap.find(un32GoodsID);
	if (discoontIter != m_cDiscountCfgMap.end()){
		SDiscountCfg& pCfg = discoontIter->second;
		for (auto consumeIter = pCfg.sConsumeList.Begin(); consumeIter != pCfg.sConsumeList.End(); consumeIter = pCfg.sConsumeList.Next()){
			ConsumeStruct& sConsumeStruct = *consumeIter;
			if (sConsumeStruct.type == consumeType){
				return sConsumeStruct.price;
			}
		}
	}
	if (goodsType == eGoodsType_Hero){
		
		auto iter = m_HeroBuyCfgMap.find(un32GoodsID);
		if (iter != m_HeroBuyCfgMap.end()){
			SHeroBuyCfg& pCfg = iter->second;
			for (auto consumeIter = pCfg.sConsumeList.Begin(); consumeIter != pCfg.sConsumeList.End(); consumeIter = pCfg.sConsumeList.Next()){
				ConsumeStruct& sConsumeStruct = *consumeIter;
				if (sConsumeStruct.type == consumeType){
					return sConsumeStruct.price;
				}
			}
		}

		return 0;
	}
	else 	if (goodsType == eGoodsType_Runes){
		auto iter = m_cRunesCfgMap.find(un32GoodsID);
		if (iter != m_cRunesCfgMap.end()){
			SRunesCfg& pCfg = iter->second;
			for (auto consumeIter = pCfg.sConsumeList.Begin(); consumeIter != pCfg.sConsumeList.End(); consumeIter = pCfg.sConsumeList.Next()){
				ConsumeStruct& sConsumeStruct = *consumeIter;
				if (sConsumeStruct.type == consumeType){
					return sConsumeStruct.price;
				}
			}
		}

		return 0;
	}
	return eNormal;
}


INT32	CCSCfgMgr::LoadInvalidWordCfg()
{
	m_invalidWorld.clear(); 

	std::string cpszfilename = m_strExeDir + "./CSConfig/NamedFilterWorld.ini";

	char szWord[128]={0};

	ifstream    FileStream; 

	FileStream.open( cpszfilename.c_str() ); 

	if( !FileStream.is_open() )
	{ 
		ELOG(LOG_DBBUG,"--read file: %s -- failed!",cpszfilename);
		return eEC_ReadCfgFileFail;
	}
	while( !FileStream.eof() )
	{
		FileStream  >> szWord;
		m_invalidWorld.push_back( std::string(szWord) );
	}

	FileStream.close(); 
	return eNormal;
}
bool	CCSCfgMgr::CheckInvalidWorld(const   char* word)
{
	std::string strWorld(word); 

	int  total = m_invalidWorld.size(); 
	for(int i=0; i< total; i++)
	{
		std::string wordCfg = m_invalidWorld[i];
		int npos = strWorld.find( wordCfg.data() );
		if(npos != -1)
		{
			return true;
		}
	}
	return false;
}


INT32	CCSCfgMgr::LoadGuideBattleEndAward()
{
	m_GuideEndAward.clear(); 
	std::string cfg = m_strExeDir + "./CSBattleMgr/CS_Award.xml"; 

	TiXmlDocument doc(cfg.c_str()); 
	if(doc.LoadFile())
	{
		TiXmlHandle dochandle(&doc);
		TiXmlElement *father =  dochandle.FirstChild("Award").FirstChild("info").ToElement();
		while(father) 
		{   
			CSGuideAward cfg;

			INT32 mapId = atoi(father->Attribute("mapid"));

			TiXmlElement *tid  = father->FirstChildElement("id")->ToElement(); 
			cfg.taskId =  atoi(tid->GetText());

			TiXmlElement *award  = father->FirstChildElement("cp")->ToElement(); 
			cfg.gold =   atoi(award->GetText());

			TiXmlElement *dia  = father->FirstChildElement("rmb")->ToElement(); 
			cfg.diamond =  atoi(dia->GetText()); 

			TiXmlElement *hero  = father->FirstChildElement("hero")->ToElement(); 
			cfg.heroid =  atoi(hero->GetText()); 

			TiXmlElement *rune  = father->FirstChildElement("Rune")->ToElement(); 
			cfg.runeid =  atoi(rune->GetText());  

			TiXmlElement *skin  = father->FirstChildElement("skin")->ToElement(); 
			cfg.skinid =  atoi(skin->GetText());  

			TiXmlElement *exp  = father->FirstChildElement("exp")->ToElement(); 
			cfg.exp =  atoi(exp->GetText());
			 
			m_GuideEndAward[mapId] = cfg; 

			father = father->NextSiblingElement(); 
		}
	}
	else
	{
		ELOG(LOG_ERROR,"can not find file: %s",cfg);
	}  
	return eNormal;
}

void	CCSCfgMgr::GetGuideAwardInfo(  INT32 mapId,   CSGuideAward &cfg)
{
	auto itr = m_GuideEndAward.find(mapId);
	if (itr != m_GuideEndAward.end())
	{ 
			cfg = itr->second; 
	} 
}

SOtherItem*	CCSCfgMgr::GetOtherItemCfg(UINT32 item_id)
{
	auto iter = m_OtherItemMap.find(item_id);
	if (m_OtherItemMap.end() != iter)
	{
		return &iter->second;
	}

	return NULL;
}

INT32	CCSCfgMgr::LoadUserDBSaveCfg()
{
	m_UserDbSaveCfg.clear();
	std::string cpszfilename = m_strExeDir + "./CSConfig/CSGlobalCfg.cfg";
	ifstream    FileStream; 

	FileStream.open( cpszfilename.c_str() ); 

	if( !FileStream.is_open() )
	{ 
		ELOG(LOG_DBBUG,"--read file: %s -- failed!",cpszfilename);
		return eEC_ReadCfgFileFail;
	}
	std::string strData("");

	{
		FileStream >> strData >> m_UserDbSaveCfg.dbSaveTimeSpace
			>> strData >>m_UserDbSaveCfg.delayDelFromCacheTime 
			>> strData >>m_UserDbSaveCfg.dbCSLogSwitch  
			>> strData >>m_UserDbSaveCfg.m_MaxUserMailN
			>> strData >>m_UserDbSaveCfg.m_IfDelAtLooked
			>> strData >>m_UserDbSaveCfg.m_ifSortAtLooked
			//ss--
			>> strData >>m_UserDbSaveCfg.dbSSLogSwitch
			>> strData >>m_UserDbSaveCfg.logRefreshTime
			>> strData >>m_UserDbSaveCfg.sSGMCmdSwitch
			>> strData >>m_UserDbSaveCfg.ssSolderBornSwitch
			>> strData >>m_UserDbSaveCfg.ssMonsterBornSwitch
			>> strData >>m_UserDbSaveCfg.ssAltarSolderSwitch;
	}

	FileStream.close(); 

	return eNormal;
}

INT32 CCSCfgMgr::LoadHotAndNewGoodsCfg(){
	std::string cpszfilename = m_strExeDir + "./CSBattleMgr/IAPShop/HotAndNewCfg.xml";
	ptree cPtree;
	ptree cHeroBuyCfg;
	try{
		read_xml(cpszfilename.c_str(), cPtree, xml_parser::trim_whitespace);
		cHeroBuyCfg = cPtree.get_child("DiscountsCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail for %s.", cException.what());
		return eEC_OpenCfgFileFail;
	}

	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;

	try{
		for (ptree::iterator iterInfo = cHeroBuyCfg.begin(); iterInfo != cHeroBuyCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			if (iterInfo->second.get<string>("<xmlattr>.un8TypeName") == "New"){
				HotGoodsCfg sTempCfg;
				memset(&sTempCfg, 0, sizeof(sTempCfg));

				const string heroHeroStr = iterInfo->second.get<string>("un32HeroGoodsID");
				split_vector_type sHeroVec;
				boost::algorithm::split(sHeroVec, heroHeroStr, boost::algorithm::is_any_of(","));
				for (auto iter = sHeroVec.begin(); iter != sHeroVec.end(); ++iter){
					string& herpStr = *iter;
					m_NewGoodsCfgVec.push_back(atoi(herpStr.c_str()));
				}
			}
			else if (iterInfo->second.get<string>("<xmlattr>.un8TypeName") == "Hot"){
				NewGoodsCfg sTempCfg;
				memset(&sTempCfg, 0, sizeof(sTempCfg));

				const string heroNewStr = iterInfo->second.get<string>("un32HeroGoodsID");
				split_vector_type sHeroVec;
				boost::algorithm::split(sHeroVec, heroNewStr, boost::algorithm::is_any_of(","));
				for (auto iter = sHeroVec.begin(); iter != sHeroVec.end(); ++iter){
					string& herpStr = *iter;
					m_HotGoodsCfgVec.push_back(atoi(herpStr.c_str()));
				}
			}
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s.", cException.what());
		return eEC_ReadCfgFileFail;
	}


	ELOG(LOG_DBBUG, "Load NewCfg:%d and HotCfg:%d", m_NewGoodsCfgVec.size(), m_HotGoodsCfgVec.size());
	return eNormal;
}

INT32	CCSCfgMgr::LoadSDBCfg()
{
	std::string cfg = m_strExeDir + "./CSConfig/DBConfig.xml";
	TiXmlDocument doc(cfg.c_str()); 
	if(doc.LoadFile())
	{
		TiXmlHandle dochandle(&doc);
		TiXmlElement *father =  dochandle.FirstChild("DBCfg").FirstChild("DB").ToElement();
		while(father) 
		{
			SDBCfg  cfg; 

			TiXmlElement *dt  = father->FirstChildElement("type")->ToElement(); 
			EDBType dtype = (EDBType)atoi(dt->GetText());

			TiXmlElement *ip = father->FirstChildElement("ip")->ToElement();
			strcpy(cfg.aszDBHostIP , ip->GetText() );

			TiXmlElement *port = father->FirstChildElement("Port")->ToElement();
			cfg.un32DBHostPort =  atoi(port->GetText());

			TiXmlElement *user = father->FirstChildElement("user")->ToElement();
			strcpy(cfg.aszDBUserName , user->GetText());

			TiXmlElement *pwd = father->FirstChildElement("pwd")->ToElement();
			strcpy(cfg.aszDBUserPwd , pwd->GetText());

			TiXmlElement *dbname = father->FirstChildElement("dbname")->ToElement();
			strcpy(cfg.aszDBName , dbname->GetText()); 

			m_mapDBCfg[dtype] = cfg;

			father = father->NextSiblingElement();
		}
	}
	return eNormal;
}

SHeroBuyCfg* CCSCfgMgr::GetHeroBuyCfg(UINT32 HeroGoodsID){
	auto iter = m_HeroBuyCfgMap.find(HeroGoodsID);
	if (m_HeroBuyCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

SHeroBuyCfg* CCSCfgMgr::GetHeroClientMatchCfg(UINT32 HeroID)
{
	auto iter = m_HeroClientMatchMap.find(HeroID);
	if (m_HeroClientMatchMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

SRunesCfg* CCSCfgMgr::GetRanmRuneFromLevel(UINT16 level){
	auto iter = m_RunesComposeInfoMap.find(level);
	if (iter == m_RunesComposeInfoMap.end()){
		return NULL;
	}

	vector<UINT32>& runesVec = iter->second;
	std::uniform_int_distribution<int> distribution(0, runesVec.size()-1);
	int randIndex = distribution(m_Generator);
	int randRunesID = runesVec[randIndex];
	return (SRunesCfg*)GetRunesCfg(randRunesID);
}

SRunesCfg* CCSCfgMgr::GetNextLevelRunesFromFixedRunesID(UINT32 id){
	auto iter = m_UperRunesComposeMap.find(id);
	if (iter == m_UperRunesComposeMap.end()){
		return NULL;
	}

	UINT32 runesID = iter->second;
	return (SRunesCfg*)GetRunesCfg(runesID);
}


SDBCfg	&	CCSCfgMgr::GetDBCfg(EDBType dt)
{
	SDBCfg cfg;
	auto itr =		m_mapDBCfg.find(dt);
	if (itr != m_mapDBCfg.end())
	{
		cfg = itr->second;
	}
	return cfg;
}
}