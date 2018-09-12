#include "stdafx.h"
#include "CfgMgr.h"
#include <unordered_map>
#include "boost/spirit.hpp"
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/typeof/typeof.hpp"
#include "boost/foreach.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"'
using namespace boost;
using namespace boost::property_tree;

namespace ReBot{

CCfgMgr::CCfgMgr(){
	m_voipServerStr = NULL;
}

CCfgMgr::~CCfgMgr(){
	if (NULL != m_voipServerStr){
		m_voipServerStr = NULL;
	}
}

INT32	CCfgMgr::Initalize(){
	INT32 n32LoadRet = eNormal;
	if (eNormal == n32LoadRet) n32LoadRet = LoadFilterWordsCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadNewSkillCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSkillModelAccountCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSkillModelEmitCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSkillModelRangeCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSkillModelLeadingCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSkillModelBufCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSkillModelSummonCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSkillModelMoveCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSkillModelSwitchCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSkillModelPurifactionCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadPassitiveSkillCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadPassitiveEffectBloodSeekCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadPassitiveEffectReboundCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadPassitiveEffectReliveCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadNPCCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadHeroCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadGoodsCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadShopCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadMapCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadVoipCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadHeroChooseCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadRobotAICfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadRobotAIPathCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadMapLogicCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadUserCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadHeroHeadCfg();
	if (eNormal == n32LoadRet) n32LoadRet = LoadAiRobotName();
	Assert(eNormal==n32LoadRet);
	return n32LoadRet;
}

INT32	CCfgMgr::Uninitialize(){
	m_cNPCCfgMap.clear();
	m_cHeroCfgMap.clear();
	m_cGoodsCfgMap.clear();
	m_cShopCfgMap.clear();
	m_cMapCfgSet.clear();
	if (NULL != m_voipServerStr){
		delete m_voipServerStr;
		m_voipServerStr = NULL;
	}
	m_voipPort = 0;
	
	ResetUserCfg();

	return eNormal;
}

INT32	CCfgMgr::LoadNPCCfg(){
	m_cNPCCfgMap.clear();

	const char *cpszfilename = "./CSBattleMgr/NPCCfg.xml";
	ptree cPtree;
	ptree cNPCCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cNPCCfg = cPtree.get_child("NPCCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NPCNum = 0;
	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	INT32		an32ConsumeAtt[100];
	INT32		an32ConumeValue[100];
	INT32		an32ConsumePer[100];
	try{
		for (ptree::iterator iterInfo = cNPCCfg.begin(); iterInfo != cNPCCfg.end(); iterInfo++){
			n32NPCNum++;
			if ("info" != iterInfo->first){
				continue;
			}

			SNPCCfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));

			sTempCfg.eOT = (EObjectType)iterInfo->second.get<INT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.eOT){
				continue;
			}

			sTempCfg.eRace = (ERace)iterInfo->second.get<INT32>("eRace", 0);
			sTempCfg.eAttendantCate = (EAttendantCate)iterInfo->second.get<INT32>("eAttendantCate", 0);
			sTempCfg.eMagicCate = (EMagicCate)iterInfo->second.get<INT32>("eMagicCate", 0);
			sTempCfg.eNPCCate = (ENPCCate)iterInfo->second.get<INT32>("eNpcType", 1);
			sTempCfg.eNPCCateChild = (ENPCCateChild)iterInfo->second.get<INT32>("eNpcChildType",1);
			sTempCfg.sBaseFP.n32AttackDist = iterInfo->second.get<INT32>("n32AttackDist", 0);
			sTempCfg.eAICate = (EAICate)iterInfo->second.get<INT32>("eAICate", 0);
			sTempCfg.un32AITarID = iterInfo->second.get<UINT32>("un32AITarID", 0);
			sTempCfg.n32PursueDist = iterInfo->second.get<INT32>("n32PursueDist", 0);
			sTempCfg.n32GuardDist = iterInfo->second.get<INT32>("n32GuardDist", 0);
			sTempCfg.eAttMode = (EAttackMode)iterInfo->second.get<INT32>("eAttackMode", 0);
			sTempCfg.eAttackWay = (EAttackWay)iterInfo->second.get<INT32>("eAttackWay", 0);
			sTempCfg.sBaseFP.n32PhyAttPower = iterInfo->second.get<INT32>("n32AttackPower", 0);
			sTempCfg.sBaseFP.n32MagicAttPower = iterInfo->second.get<INT32>("n32MagAttackPower", 0);
			sTempCfg.sBaseFP.n32PhyDefAbility = iterInfo->second.get<INT32>("n32DefenseAbility", 0);
			sTempCfg.sBaseFP.n32MagicDefAbility = iterInfo->second.get<INT32>("n32MagDefenseAbility", 0);
			sTempCfg.sBaseFP.n32MoveSpeed = iterInfo->second.get<INT32>("n32MoveSpeed", 0);
			sTempCfg.sBaseFP.n32AttackSpeed = iterInfo->second.get<INT32>("n32AttackSpeed", 0);
			sTempCfg.sBaseFP.n32MaxHP = iterInfo->second.get<INT32>("n32MaxHP", 0);
			sTempCfg.sBaseFP.n32MaxMP = iterInfo->second.get<INT32>("n32MaxMP", 0);
			sTempCfg.n32GotExp = iterInfo->second.get<INT32>("n32GotExp", 0);
			sTempCfg.n32ConsumeCP = (INT32)(iterInfo->second.get<INT32>("n32ConsumeCP", 0)*1000);
			sTempCfg.n32KillGotCP = (INT32)(iterInfo->second.get<INT32>("n32KillCP", 0)*1000);
			sTempCfg.bIfCanControl = iterInfo->second.get<INT8>("bIfCanControl", 0);
			sTempCfg.sBaseFP.n32HPRecover = (INT32)(iterInfo->second.get<INT32>("n32HPRecover", 0) * 1000);
			sTempCfg.sBaseFP.n32MPRecover = (INT32)(iterInfo->second.get<INT32>("n32MPRecover", 0) * 1000);
			sTempCfg.n32CollideRadius = iterInfo->second.get<INT32>("n32CollideRadius", 0);
			sTempCfg.aun32SkillList[0] = iterInfo->second.get<INT32>("un32SkillType1", 0);
			sTempCfg.aun32SkillList[1] = iterInfo->second.get<INT32>("un32SkillType2", 0);
			sTempCfg.aun32SkillList[2] = iterInfo->second.get<INT32>("un32SkillType3", 0);
			sTempCfg.aun32SkillList[3] = iterInfo->second.get<INT32>("un32SkillType4", 0);
			sTempCfg.aun32SkillList[4] = iterInfo->second.get<INT32>("un32SkillType5", 0);

			string sPassitiveEvent = iterInfo->second.get<string>("un32PassSkillID");
			typedef vector< string > split_vector_type;
			split_vector_type sPassitiveVec;
			boost::algorithm::split( sPassitiveVec, sPassitiveEvent, boost::algorithm::is_any_of(",") );

			for (INT32 i = 0; i < sPassitiveVec.size(); ++i){
				sTempCfg.lun32PassiveSkillID[i] = atoi(sPassitiveVec[i].c_str());
			}

			sTempCfg.n32CampID = iterInfo->second.get<INT32>("eCamp", 0);
			sTempCfg.un32ShopID = iterInfo->second.get<INT32>("un32ShopID", 0);
			sTempCfg.bIsLocked = (iterInfo->second.get<INT32>("n32IsLocked", 0) > 0) ? true : false;

			sTempCfg.eWMGroupCate = (EWMGroup)iterInfo->second.get<INT32>("n32GroupID", 0);

			memset(an32ConsumeAtt, 0, sizeof(an32ConsumeAtt));
			memset(an32ConumeValue, 0, sizeof(an32ConumeValue));

			if (eNPCCate_WideAnimal == sTempCfg.eNPCCate || eNPCCate_Solider == sTempCfg.eNPCCate){
				cTempStr = iterInfo->second.get<string>("eConsumeAtt", "");
				CFunction::SplitInt(cTempStr.c_str(), an32ConsumeAtt, 100);

				cTempStr = iterInfo->second.get<string>("n32ConsumeValue", "");
				CFunction::SplitInt(cTempStr.c_str(), an32ConumeValue, 100);

				cTempStr = iterInfo->second.get<string>("n32ConsumePer", "");
				CFunction::SplitInt(cTempStr.c_str(), an32ConsumePer, 100);

				for (size_t i = 0; i < 100; ++i){
					if (0 != an32ConsumeAtt[i]){
						sTempCfg.n32AbsorbAttriChange[an32ConsumeAtt[i]] = an32ConumeValue[i];
						sTempCfg.n32ConsumePer[an32ConsumeAtt[i]] = an32ConsumePer[i];
					}
				}
			}

			std::string strEmitPos = iterInfo->second.get<string>("EmitPos","");
			if(FALSE == strEmitPos.empty()){
				vector<string> eEmitPosVec;
				boost::algorithm::split(eEmitPosVec, strEmitPos, is_any_of(","));
				if(eEmitPosVec.size() == 3){
					sTempCfg.cEmitPos.m_fX = (float)atoi(eEmitPosVec[0].c_str()) / 1000;
					sTempCfg.cEmitPos.m_fY = (float)atoi(eEmitPosVec[1].c_str()) / 1000;
					sTempCfg.cEmitPos.m_fZ = (float)atoi(eEmitPosVec[2].c_str()) / 1000;
				}
			}
			std::string strOnHitPos = iterInfo->second.get<string>("HitPos","");
			if(FALSE == strOnHitPos.empty()){
				vector<string> eOnHitPosVec;
				boost::algorithm::split(eOnHitPosVec, strOnHitPos, is_any_of(","));
				if(eOnHitPosVec.size() == 3){
					sTempCfg.cOnHitPos.m_fX = (float)atoi(eOnHitPosVec[0].c_str()) / 1000;
					sTempCfg.cOnHitPos.m_fY = (float)atoi(eOnHitPosVec[1].c_str()) / 1000;
					sTempCfg.cOnHitPos.m_fZ = (float)atoi(eOnHitPosVec[2].c_str()) / 1000;
				}
			}


			m_cNPCCfgMap[sTempCfg.eOT] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s npcnum:%d.", cException.what(), n32NPCNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d NPC cfg.", (UINT32)m_cNPCCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadHeroCfg(){
	m_cHeroCfgMap.clear();

	const char *cpszfilename = "./CSBattleMgr/HeroCfg.xml";
	ptree cPtree;
	ptree cHeroCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cHeroCfg = cPtree.get_child("herocfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NPCNum = 0;
	//UINT32 un32ID = 0;
	//CHAR *cpszName = NULL;
	string cTempStr;
	try{
		for (ptree::iterator iterInfo = cHeroCfg.begin(); iterInfo != cHeroCfg.end(); iterInfo++){
			n32NPCNum++;
			if ("info" != iterInfo->first){
				continue;
			}

			SHeroCfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));

			sTempCfg.eOT = (EObjectType)iterInfo->second.get<INT32>("szNOStr", 0);
			if (0 == sTempCfg.eOT){
				continue;
			}
			sTempCfg.eMagicCate = (EMagicCate)iterInfo->second.get<INT32>("eMagicCate", 0);
			sTempCfg.eAttackWay = (EAttackWay)iterInfo->second.get<INT32>("eAttackWay", 0);
			sTempCfg.sBaseFP.n32AttackDist = iterInfo->second.get<INT32>("n32AttackDist", 0);
			sTempCfg.n32BaseExp = iterInfo->second.get<INT32>("n32BaseExp", 0);
			sTempCfg.sBaseFP.n32PhyAttPower = iterInfo->second.get<INT32>("n32BasePhyAttPower", 0);
			sTempCfg.sBaseFP.n32MagicAttPower = iterInfo->second.get<INT32>("n32BaseMagAttPower", 0);
			sTempCfg.sBaseFP.n32PhyDefAbility = iterInfo->second.get<INT32>("n32BasePhyDef", 0);
			sTempCfg.sBaseFP.n32MagicDefAbility = iterInfo->second.get<INT32>("n32BaseMagDef", 0);
			sTempCfg.sBaseFP.n32MoveSpeed = iterInfo->second.get<INT32>("n32BaseMoveSpeed", 0);
			sTempCfg.sBaseFP.n32AttackSpeed = iterInfo->second.get<INT32>("n32BaseAttackSpeed", 0);
			sTempCfg.sBaseFP.n32MaxHP = iterInfo->second.get<INT32>("n32BaseMaxHP", 0);
			sTempCfg.sBaseFP.n32MaxMP = iterInfo->second.get<INT32>("n32BaseMaxMP", 0);
			sTempCfg.sBaseFP.n32HPRecover = (INT32)(iterInfo->second.get<INT32>("n32BaseHPRecover", 0));
			sTempCfg.sBaseFP.n32MPRecover = (INT32)(iterInfo->second.get<INT32>("n32BaseMPRecover", 0));
			sTempCfg.sBaseFP.n32ReliveSecond = iterInfo->second.get<INT32>("n32BaseReliveTime", 0);
			sTempCfg.n32ExpGrowth = iterInfo->second.get<INT32>("n32ExpGrowth", 0);
			sTempCfg.sGrowthFP.n32PhyAttPower = iterInfo->second.get<INT32>("n32PhyAttGrowth", 0);
			sTempCfg.sGrowthFP.n32MagicAttPower = iterInfo->second.get<INT32>("n32MagAttGrowth", 0);
			sTempCfg.sGrowthFP.n32PhyDefAbility = iterInfo->second.get<INT32>("n32PhyDefGrowth", 0);
			sTempCfg.sGrowthFP.n32MagicDefAbility = iterInfo->second.get<INT32>("n32MagDefGrowth", 0);
			sTempCfg.sGrowthFP.n32MaxHP = iterInfo->second.get<INT32>("n32MaxHPGrowth", 0);
			sTempCfg.sGrowthFP.n32MaxMP = iterInfo->second.get<INT32>("n32MaxMPGrowth", 0);
			sTempCfg.sGrowthFP.n32HPRecover = (INT32)(iterInfo->second.get<INT32>("n32HPRecoverGrowth", 0));
			sTempCfg.sGrowthFP.n32MPRecover = (INT32)(iterInfo->second.get<INT32>("n32MPRecoverGrowth", 0));
			sTempCfg.sGrowthFP.n32ReliveSecond = (INT32)(iterInfo->second.get<INT32>("n32ReliveGrowth", 0));
			sTempCfg.sGrowthFP.n32MoveSpeed = iterInfo->second.get<INT32>("n32MoveSpeedGrowth", 0);
			sTempCfg.sGrowthFP.n32AttackSpeed = iterInfo->second.get<INT32>("n32AttackSpeedGrowth", 0);
			sTempCfg.n32CPRecover = (INT32)(iterInfo->second.get<FLOAT>("n32CPRecover", 0) * 1000);
			sTempCfg.n32CollideRadius = iterInfo->second.get<INT32>("n32CollideRadious", 0);			
			sTempCfg.aun32SkillList[0] = iterInfo->second.get<INT32>("un32SkillType1", 0);
			sTempCfg.aun32SkillList[1] = iterInfo->second.get<INT32>("un32SkillType2", 0);
			sTempCfg.aun32SkillList[2] = iterInfo->second.get<INT32>("un32SkillType3", 0);
			sTempCfg.aun32SkillList[3] = iterInfo->second.get<INT32>("un32SkillType4", 0);
			sTempCfg.aun32SkillList[4] = iterInfo->second.get<INT32>("un32SkillType5", 0);
			sTempCfg.aun32SkillList[5] = iterInfo->second.get<INT32>("un32SkillType6", 0);
			sTempCfg.aun32SkillList[6] = iterInfo->second.get<INT32>("un32SkillType7", 0);
			sTempCfg.aun32PassitiveSkillList[0] = iterInfo->second.get<INT32>("un32SkillTypeP", 0);

			std::string strEmitPos = iterInfo->second.get<string>("EmitPos","");
			if(FALSE == strEmitPos.empty()){
				vector<string> eEmitPosVec;
				boost::algorithm::split(eEmitPosVec, strEmitPos, is_any_of(","));
				if(eEmitPosVec.size() == 3){
					sTempCfg.cEmitPos.m_fX = (float)atoi(eEmitPosVec[0].c_str()) / 1000;
					sTempCfg.cEmitPos.m_fY = (float)atoi(eEmitPosVec[1].c_str()) / 1000;
					sTempCfg.cEmitPos.m_fZ = (float)atoi(eEmitPosVec[2].c_str()) / 1000;
				}
			}
			std::string strOnHitPos = iterInfo->second.get<string>("HitPos","");
			if(FALSE == strOnHitPos.empty()){
				vector<string> eOnHitPosVec;
				boost::algorithm::split(eOnHitPosVec, strOnHitPos, is_any_of(","));
				if(eOnHitPosVec.size() == 3){
					sTempCfg.cOnHitPos.m_fX = (float)atoi(eOnHitPosVec[0].c_str()) / 1000;
					sTempCfg.cOnHitPos.m_fY = (float)atoi(eOnHitPosVec[1].c_str()) / 1000;
					sTempCfg.cOnHitPos.m_fZ = (float)atoi(eOnHitPosVec[2].c_str()) / 1000;
				}
			}

			//推荐购买物品列表
			int itemIndex = 0;
			std::string strPreItem = iterInfo->second.get<string>("un32PreItem","");
			if(FALSE == strPreItem.empty()){
				vector<string> itemVec;
				boost::algorithm::split(itemVec, strPreItem, is_any_of(","));
				for(auto itemIter = itemVec.begin();itemIter != itemVec.end(); itemIter++){
					sTempCfg.aun32BuyItemList[itemIndex++] = atoi((*itemIter).c_str());
				}
			}
			std::string strMidItem = iterInfo->second.get<string>("un32MidItem","");
			if(FALSE == strMidItem.empty()){
				vector<string> itemVec;
				boost::algorithm::split(itemVec, strMidItem, is_any_of(","));
				for(auto itemIter = itemVec.begin();itemIter != itemVec.end(); itemIter++){
					sTempCfg.aun32BuyItemList[itemIndex++] = atoi((*itemIter).c_str());
				}
			}
			std::string strProItem = iterInfo->second.get<string>("un32ProItem","");
			if(FALSE == strProItem.empty()){
				vector<string> itemVec;
				boost::algorithm::split(itemVec, strProItem, is_any_of(","));
				for(auto itemIter = itemVec.begin();itemIter != itemVec.end(); itemIter++){
					sTempCfg.aun32BuyItemList[itemIndex++] = atoi((*itemIter).c_str());
				}
			}

			m_cHeroCfgMap[sTempCfg.eOT] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail.");
		return eEC_ReadCfgFileFail;
	}
	
	ELOG(LOG_DBBUG, " Load %d hero cfg.", (UINT32)m_cHeroCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadHeroHeadCfg()
{
	const char *cpszfilename = "./CSBattleMgr/HeadSelect.xml";
	ptree cPtree;
	ptree cLogicCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cLogicCfg = cPtree.get_child("HeadSelect");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NPCNum = 0;
	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	try{
		for (ptree::iterator mapit = cLogicCfg.begin(); mapit != cLogicCfg.end(); mapit++){
			if ("info" != mapit->first) continue;
			UINT32 headid = mapit->second.get<UINT32>("<xmlattr>.HeadID", 0);
			UINT32 headAtlas = mapit->second.get<int>("Atlas_Num",0);
			m_cHeroHeadMap.push_back(headAtlas);
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " load HeadSelect.xml error.");
		return eEC_OpenCfgFileFail;
	}
	return eNormal;
}

INT32	CCfgMgr::LoadMapCfg()
{
	string strExtFilename;
	string strBaseName;
	string strFullName;
	string strLuaFileName;

	try{
		boost::filesystem::directory_iterator pathdirectoryiter("./CSBattleMgr/Map");
		boost::filesystem::directory_iterator end_iter;
		while(pathdirectoryiter != end_iter){
			if (boost::filesystem::is_directory(*pathdirectoryiter)){
				++ pathdirectoryiter;
				continue;
			}

			strExtFilename = boost::filesystem::extension(*pathdirectoryiter);
			strBaseName = boost::filesystem::basename(*pathdirectoryiter);
			strFullName = pathdirectoryiter->path().generic_string();
			if (strExtFilename == ".map"){
				UINT32 un32MapID = atoi(strBaseName.c_str());
				LoadMapData(un32MapID);
			}

			++ pathdirectoryiter;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " load Map data file fail.");
		return eEC_OpenCfgFileFail;
	}

	ELOG(LOG_DBBUG, " load %d map.", (UINT32)m_cMapCfgSet.size());

	return eNormal;
}

INT32	CCfgMgr::LoadMapData(UINT32 un32MapID){
	m_cMapCfgSet.insert(un32MapID);
	return eNormal;
}

INT32 CCfgMgr::LoadMapLogicCfg(){
	const char *cpszfilename = "./CSBattleMgr/MapLoadCfg.xml";
	ptree cPtree;
	ptree cLogicCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cLogicCfg = cPtree.get_child("MapLoadCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NPCNum = 0;
	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	try{
		for (ptree::iterator mapit = cLogicCfg.begin(); mapit != cLogicCfg.end(); mapit++){
			if ("info" != mapit->first) continue;
			UINT32 mapId = mapit->second.get<UINT32>("<xmlattr>.MapID", 0);
			SMapLogicCfg* &pMapInfo = m_cMapLogicCfg[mapId];
			if (pMapInfo==NULL) pMapInfo = new SMapLogicCfg;
			pMapInfo->MapId = mapId;
			pMapInfo->MapName = mapit->second.get<string>("NameCn","");
			pMapInfo->MapPic = mapit->second.get<int>("ShowPic",0);
			pMapInfo->SeatCount = mapit->second.get<int>("PlayerNum",0);
			string model = mapit->second.get<string>("PlayerMode","");
			int teamcount[10] = {0};
			int totalcount = 0;
			CFunction::SplitInt(model.c_str(), teamcount, 10);
			for (int i=0;i<10;++i){
				if (teamcount[i]==0) break;
				totalcount += teamcount[i];
				pMapInfo->PlayerModel.push_back(teamcount[i]);
			}
			Assert(pMapInfo->SeatCount==totalcount);
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " load Map logic file fail.");
		return eEC_OpenCfgFileFail;
	}
	return eNormal;
}

INT32 CCfgMgr::LoadGoodsCfg()
{
	m_cGoodsCfgMap.clear(); 

	std::unordered_map<INT32, SDownInfo> lGoodsDownComposeMap;
	std::multimap<UINT32, SUpInfo> lGoodsUpComposeMap;

	{
		const char *cpszCombineFilename = "./CSBattleMgr/CombineCfg.xml";
		ptree cPtree;
		ptree cComposeCfg;
		try{
			read_xml(cpszCombineFilename, cPtree, xml_parser::trim_whitespace);
			cComposeCfg = cPtree.get_child("CombineCfg");
		}catch(std::exception &cException){
			ELOG(LOG_ERROR, "open CombineCfgcfg file fail.");
			return eEC_OpenCfgFileFail;
		}

		try{
			for (ptree::iterator iterInfo = cComposeCfg.begin(); iterInfo != cComposeCfg.end(); iterInfo++){
				if ("info" != iterInfo->first){
					continue;
				}

				EObjectType eOT = (EObjectType)iterInfo->second.get<UINT32>("<xmlattr>.un32ItemID");
				if (0 == eOT){
					continue;
				}

				UINT32 un32Child1 = iterInfo->second.get<UINT32>("un32ChildID1");
				UINT32 un32Child2 = iterInfo->second.get<UINT32>("un32ChildID2");
				lGoodsDownComposeMap[eOT] = SDownInfo(un32Child1, un32Child2);

				if (0 != un32Child1 && 0 != un32Child2){
					if (un32Child2 == un32Child1){
						lGoodsUpComposeMap.insert(std::make_pair(un32Child1, SUpInfo(eOT, un32Child2)));
					}
					else{
						lGoodsUpComposeMap.insert(std::make_pair(un32Child1, SUpInfo(eOT, un32Child2)));
						lGoodsUpComposeMap.insert(std::make_pair(un32Child2, SUpInfo(eOT, un32Child1)));
					}
				}
			}
		}catch(std::exception &cException){
			ELOG(LOG_ERROR, "read CombineCfg file fail for %s", cException.what());
			return eEC_ReadCfgFileFail;
		}
	}

	const char *cpszfilename = "./CSBattleMgr/ItemCfg.xml";
	ptree cPtree;
	ptree cGoodsCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cGoodsCfg = cPtree.get_child("ItemCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open ItemCfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	SGoodsCfg sTempCfg;
	memset(&sTempCfg, 0, sizeof(sTempCfg));

	try{
		for (ptree::iterator iterInfo = cGoodsCfg.begin(); iterInfo != cGoodsCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			memset(&sTempCfg, 0, sizeof(sTempCfg));
			
			sTempCfg.un32GoodsID = iterInfo->second.get<INT32>("<xmlattr>.un32GoodsID", 0);
			if (0 == sTempCfg.un32GoodsID){
				continue;
			}
			sTempCfg.eOT = (EObjectType)sTempCfg.un32GoodsID;
			if (0 == sTempCfg.eOT){
				continue;
			}

			sTempCfg.un8TypeID = iterInfo->second.get<INT8>("un8TypeID");

			typedef vector< string > split_vector_type;

			string sAtrr = iterInfo->second.get<string>("attributeKeyList");
			split_vector_type sAtrrVec;
			boost::algorithm::split( sAtrrVec, sAtrr, boost::algorithm::is_any_of(",") );

			string sVal = iterInfo->second.get<string>("attributeValList");
			split_vector_type sValVec;
			boost::algorithm::split( sValVec, sVal, boost::algorithm::is_any_of(",") );

			string sPersent = iterInfo->second.get<string>("attributePerList");
			split_vector_type sPersentVec;
			boost::algorithm::split( sPersentVec, sPersent, boost::algorithm::is_any_of(",") );

			if (sValVec.size() != sAtrrVec.size() || sValVec.size() != sPersentVec.size()){
				ELOG(LOG_ERROR, "sValVec.size() != sAtrrVec.size()");
				continue;
			}

			for (INT32 i = 0; i < sAtrrVec.size(); ++i){
				sTempCfg.sGoodsEffect[i].n32Atrr = atoi(sAtrrVec[i].c_str());
				sTempCfg.sGoodsEffect[i].n32Val = atoi(sValVec[i].c_str());
				sTempCfg.sGoodsEffect[i].n32Percent = atoi(sPersentVec[i].c_str());
			}
			
			sTempCfg.n32UseTimes = iterInfo->second.get<INT32>("un8UseTimes");	
			sTempCfg.un32SkillID = iterInfo->second.get<UINT32>("un32SkillID");

			string sPassitiveEvent = iterInfo->second.get<string>("un32PassSkillID");
			typedef vector< string > split_vector_type;
			split_vector_type sPassitiveVec;
			boost::algorithm::split( sPassitiveVec, sPassitiveEvent, boost::algorithm::is_any_of(",") );

			for (INT32 i = 0; i < sPassitiveVec.size(); ++i){
				sTempCfg.lun32PassiveSkillID[i] = atoi(sPassitiveVec[i].c_str());
			}
			sTempCfg.bUsedAfterBuyed = iterInfo->second.get<bool>("bUsedAfterBuyed");
			sTempCfg.un32CDTime = iterInfo->second.get<UINT32>("un32CdTime") * 1000;
			sTempCfg.bUsedDestory = iterInfo->second.get<bool>("bDestroyAfterUsed");
			sTempCfg.bUniqueID = iterInfo->second.get<bool>("bUniqueID");
			sTempCfg.n32OverlapTimes = iterInfo->second.get<INT32>("un8OverlapTimes");
			sTempCfg.n32CPCostType = iterInfo->second.get<INT32>("un8CPCostType");
			sTempCfg.n32CPCost = (INT32)(iterInfo->second.get<INT32>("n32CPCost")*1000);
			sTempCfg.un8FunctionType = iterInfo->second.get<UINT8>("un8FunctionType");
			sTempCfg.un32UniqueTypeID = iterInfo->second.get<UINT32>("un32UniqueTypeID");
			sTempCfg.n32CombineCPCost = iterInfo->second.get<UINT32>("n32CombineCPCost");
			sTempCfg.bIfCanBuyWhenDeath = iterInfo->second.get<bool>("bIsCanDeathToBuy");
			std::unordered_map<INT32, SDownInfo>::iterator iter = lGoodsDownComposeMap.find(sTempCfg.eOT);
			if (lGoodsDownComposeMap.end() != iter)
				sTempCfg.sGoodsCompose.sDownInfo = iter->second;

			INT32 i = 0;
			for (std::multimap<UINT32, SUpInfo>::iterator iter = lGoodsUpComposeMap.begin(); iter != lGoodsUpComposeMap.end(); ++iter){
				if (iter->first == sTempCfg.eOT){
					if (i == c_un32MaxGoodsDownInfoNum-1){
						break;
					}
					sTempCfg.sGoodsCompose.sUpInfo[i] = iter->second;
					++i;
				}
			}
			
			m_cGoodsCfgMap[sTempCfg.eOT] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s(eOT:%d).", cException.what(), sTempCfg.eOT);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d goods cfg.", (UINT32)m_cGoodsCfgMap.size());
	return eNormal;
}

INT32 CCfgMgr::LoadShopCfg(){
	m_cShopCfgMap.clear();

	const char *cpszfilename = "./CSBattleMgr/ShopCfg.xml";
	ptree cPtree;
	ptree cShopCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cShopCfg = cPtree.get_child("ShopCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	UINT32 un32ShopID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	SShopCfg sTempCfg;

	memset(&sTempCfg, 0, sizeof(sTempCfg));
	try{
		for (ptree::iterator iterInfo = cShopCfg.begin(); iterInfo != cShopCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.eOT = (EObjectType)iterInfo->second.get<INT32>("<xmlattr>.n32ShopID", 0);
			if (0 == sTempCfg.eOT){
				continue;
			}
			
			INT32 n32CPIndex = 0;
			typedef vector< string > split_vector_type;

			for (INT32 n32PageNum = 1; n32PageNum <= 5; ++n32PageNum)
			{
				char temp[64];
				_snprintf(temp, sizeof(temp), "%d", n32PageNum);
				string postfix(temp);
				postfix = "Page" + postfix;

				string sItemIDPage = "un32ItemID" + postfix;
				string sCostTypePage = "un8CostType"+ postfix;
				string sCPCostPage = "n32CPCost"+ postfix;

				split_vector_type sItemIDVec;
				string sItemID = iterInfo->second.get<string>(sItemIDPage);
				boost::algorithm::split( sItemIDVec, sItemID, boost::algorithm::is_any_of(",") );

				split_vector_type sCostTypeVec;
				string sCostType = iterInfo->second.get<string>(sCostTypePage);
				boost::algorithm::split( sCostTypeVec, sCostType, boost::algorithm::is_any_of(",") );

				split_vector_type sCPCostVec;
				string sCPCost = iterInfo->second.get<string>(sCPCostPage);
				boost::algorithm::split( sCPCostVec, sCPCost, boost::algorithm::is_any_of(",") );

				if (sItemIDVec.size() != sCostTypeVec.size() || sItemIDVec.size() != sCPCostVec.size()){
					break;
				}

				for (INT32 i = 0; i < sItemIDVec.size(); ++i){
					sTempCfg.sGoodCPInfo[n32CPIndex].un32GoodsID = atoi(sItemIDVec[i].c_str());
					sTempCfg.sGoodCPInfo[n32CPIndex].un8CPType = atoi(sCostTypeVec[i].c_str());
					sTempCfg.sGoodCPInfo[n32CPIndex].n32CPCost = atoi(sCPCostVec[i].c_str());
				}
				 
				++n32CPIndex;
			}

			m_cShopCfgMap[sTempCfg.eOT] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s(eOT:%d).", cException.what(), sTempCfg.eOT);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d shop cfg.", (UINT32)m_cShopCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadVoipCfg(){
	m_voipServerStr = NULL;

	const char *cpszfilename = "./CSBattleMgr/VoipCfg.xml";
	ptree cPtree;
	ptree cCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cCfg = cPtree.get_child("VoipCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	CHAR *cpszName = NULL;
	string cTempStr;
	try{
		cTempStr = cCfg.get<string>("server","");
		m_voipServerStr = new CHAR[cTempStr.length() + 1];
		strcpy(m_voipServerStr,cTempStr.c_str());
		m_voipPort = cCfg.get<INT32>("port",0);
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail.");
		return eEC_ReadCfgFileFail;
	}

	return eNormal;
}

const CHAR* CCfgMgr::GetVoipServer(){
	return m_voipServerStr;
}

INT32 CCfgMgr::GetVoipPort(){
	return m_voipPort;
}

#define  SKILL_NEW_SWITCH_CFG 0

INT32 CCfgMgr::LoadNewSkillCfg(){
	m_cNewSkillCfgMap.clear();


	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_manager.xml";
	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_manager");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			n32NewSkillNum++;
			if ("info" != iterInfo->first){
				continue;
			}

			SSNewSkillCfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.un32SkillID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillID){
				continue;
			}
			sTempCfg.n32UpgradeLevel = iterInfo->second.get<INT32>("n32UpgradeLevel", 0);
			sTempCfg.bIfNomalAttack = iterInfo->second.get<INT32>("bIfNormalAttack", 0);
			sTempCfg.bIsConsumeSkill = iterInfo->second.get<INT32>("bIsConsumeSkill", 0);
			sTempCfg.eReleaseWay = (ESkillReleaseWay)iterInfo->second.get<INT32>("eReleaseWay", 0);
			sTempCfg.n32UseMP = iterInfo->second.get<INT32>("n32UseMP", 0);
			sTempCfg.n32UseHP = iterInfo->second.get<INT32>("n32UseHP", 0);
			sTempCfg.n32UseCP = (INT32)(iterInfo->second.get<INT32>("n32UseCP", 0)*1000);
			sTempCfg.n32PrepareMilsec= iterInfo->second.get<INT32>("n32PrepareTime", 0);

			sTempCfg.n32ReleaseMilsec= iterInfo->second.get<INT32>("n32ReleaseTime", 0);
			sTempCfg.n32SkillLastTime= iterInfo->second.get<INT32>("n32SkillLastTime", 0);

			sTempCfg.n32CoolDown = iterInfo->second.get<INT32>("n32CoolDown", 0);
			sTempCfg.fReleaseDist = iterInfo->second.get<FLOAT>("n32ReleaseDistance", 0);
			sTempCfg.eUseWay = (ESkillUseWay)iterInfo->second.get<INT32>("eUseWay", 0);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.bIfAffectBuilding = iterInfo->second.get<bool>("bIfAffectBuilding", 0);
			sTempCfg.bIfAffectHero = iterInfo->second.get<bool>("bIfAffectHero", 0);
			sTempCfg.bIfAffectMonster = iterInfo->second.get<bool>("bIfAffectMonster", 0);
			sTempCfg.eSkillTargetCate = (ESkillTargetCate)iterInfo->second.get<INT32>("eTargetType", 0);
			sTempCfg.bIfCheckTarState = iterInfo->second.get<bool>("bIsCheckTargetSta", 0);

			string sVal = iterInfo->second.get<string>("EventID", ""); 
			DoEventStr(sVal, sTempCfg.asSkillModelList, c_n32MaxNextSkillEffectCfgNum); 
			m_cNewSkillCfgMap[sTempCfg.un32SkillID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d Skill cfg.", (UINT32)m_cNewSkillCfgMap.size());
	return eNormal;
}

INT32 CCfgMgr::LoadSkillModelAccountCfg(){
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_account.xml";
	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_account");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			n32NewSkillNum++;
			if ("info" != iterInfo->first){
				continue;
			}

			SSkillModelAccountCfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.eSkillModelType = eSkillEffectType_Caculate;
			sTempCfg.un32SkillModelID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillModelID){
				continue;
			}

			sTempCfg.bIsCoolDown = iterInfo->second.get<bool>("bIsCoolDown", 0);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.bIfAffectBuilding = iterInfo->second.get<bool>("bIfAffectBuilding", 0);
			sTempCfg.bIfAffectHero = iterInfo->second.get<bool>("bIfAffectHero", 0);
			sTempCfg.bIfAffectMonster = iterInfo->second.get<bool>("bIfAffectMonster", 0);
			sTempCfg.bIsCanMove = iterInfo->second.get<bool>("bIsCanMove", 1);
			//sTempCfg.bIsCanBreak = iterInfo->second.get<bool>("bIsCanBreak", 1);
			//sTempCfg.eTargetType = (ESkillModelTargetCate)iterInfo->second.get<INT32>("eTargetType", 0);
			//sTempCfg.sEffectInfo.eEffectCate = (EEffectCate)iterInfo->second.get<INT32>("n32EffectID", 0);
			//sTempCfg.sEffectInfo.n32EffectValue = iterInfo->second.get<INT32>("eEffectValue", 0);
			//sTempCfg.sEffectInfo.n32EffectRate = iterInfo->second.get<INT32>("eEffectRate", 0);
			//sTempCfg.sEffectInfo.bIfBlastAttack = iterInfo->second.get<bool>("bEffectIsCritical", 0);
			//sTempCfg.sEffectInfo.n32CriticalPercent = iterInfo->second.get<INT32>("eEffectCriticalPosb", 0);
			//sTempCfg.sEffectInfo.n32BlastAttackRate = iterInfo->second.get<INT32>("eEffectCriticalPercent", 0);

			string sVal = iterInfo->second.get<string>("n32EventID", ""); 
			DoEventStr(sVal, sTempCfg.asSkillModelList,c_n32MaxNextSkillEffectCfgNum); 

			m_cSkillModelAccountCfgMap[sTempCfg.un32SkillModelID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d Skill cfg.", (UINT32)m_cSkillModelAccountCfgMap.size());
	return eNormal;
}

INT32 CCfgMgr::LoadSkillModelLeadingCfg(){
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_leading.xml";
	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_leading");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			n32NewSkillNum++;
			if ("info" != iterInfo->first){
				continue;
			}

			SSkillModelLeadingCfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.eSkillModelType = eSkillEffectType_Leading;
			sTempCfg.un32SkillModelID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillModelID){
				continue;
			}

			sTempCfg.bIsCoolDown = iterInfo->second.get<bool>("bIsCoolDown", 0);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.bIfAffectBuilding = iterInfo->second.get<bool>("bIfAffectBuilding", 0);
			sTempCfg.bIfAffectHero = iterInfo->second.get<bool>("bIfAffectHero", 0);
			sTempCfg.bIfAffectMonster = iterInfo->second.get<bool>("bIfAffectMonster", 0);
			sTempCfg.bIsCanMove = iterInfo->second.get<bool>("bIsCanMove", 1);
			sTempCfg.bIsCanBreak = iterInfo->second.get<bool>("bIsCanBreak", 1);
			sTempCfg.eTargetType = (ESkillModelTargetCate)iterInfo->second.get<INT32>("eTargetType", 0);

			sTempCfg.eLeadingTime = (EEffectCate)iterInfo->second.get<INT32>("eLeadingTime", 0);
			sTempCfg.bIsCheckTargetSta = iterInfo->second.get<bool>("bIsCheckTargetSta", 0);

			string sVal = iterInfo->second.get<string>("EventID", ""); 
			DoEventStr(sVal, sTempCfg.asSkillModelList,c_n32MaxNextSkillEffectCfgNum); 

			m_cSkillModelLeadingCfgMap[sTempCfg.un32SkillModelID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d Skill cfg.", (UINT32)m_cSkillModelLeadingCfgMap.size());
	return eNormal;
}

INT32 CCfgMgr::LoadSkillModelEmitCfg(){
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_emit.xml";
	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_emit");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSkillModelEmitCfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.eSkillModelType = eSkillEffectType_Emit;
			sTempCfg.un32SkillModelID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillModelID){
				continue;
			}

			sTempCfg.bIsCoolDown = iterInfo->second.get<bool>("bIsCoolDown", 0);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.bIfAffectBuilding = iterInfo->second.get<bool>("bIfAffectBuilding", 0);
			sTempCfg.bIfAffectHero = iterInfo->second.get<bool>("bIfAffectHero", 0);
			sTempCfg.bIfAffectMonster = iterInfo->second.get<bool>("bIfAffectMonster", 0);
			sTempCfg.bIsCanMove = iterInfo->second.get<bool>("bIsCanMove", 1);
			sTempCfg.bIsCanBreak = iterInfo->second.get<bool>("bIsCanBreak", 1);
			sTempCfg.bIfAffectMonster = iterInfo->second.get<bool>("bIfAffectMonster", 0);
			sTempCfg.eTargetType = (ESkillModelTargetCate)iterInfo->second.get<INT32>("EmitTarget", 0);

			sTempCfg.eEmitType = (ESkillModelEmitType)iterInfo->second.get<INT32>("EmitType", 0);
			sTempCfg.n32ProjFlySpeed = iterInfo->second.get<INT32>("FlySpeed", 0);
			sTempCfg.n32FlyPar1 = iterInfo->second.get<INT32>("FlyPar1", 0);
			sTempCfg.n32FlyPar2 = iterInfo->second.get<INT32>("FlyPar2", 0);
			sTempCfg.n32FlyPar3 = iterInfo->second.get<INT32>("FlyPar3", 0);
			sTempCfg.n32ProjectileNum = iterInfo->second.get<INT32>("n32ProjectileNum", 0);
			sTempCfg.n32ProjectileAngle = iterInfo->second.get<INT32>("n32ProjEmitInter", 0);
			sTempCfg.bIsPenetrate = iterInfo->second.get<bool>("bIsPenetrate", 0);
			sTempCfg.n32ProjectileCollideRadius = iterInfo->second.get<INT32>("eCollideRadius", 0);
			sTempCfg.n32ProjectileLifeTime = iterInfo->second.get<INT32>("eLifeTime", 0);
			
			string sVal = iterInfo->second.get<string>("EventID", ""); 
			DoEventStr(sVal, sTempCfg.asSkillModelList,c_n32MaxNextSkillEffectCfgNum); 

			m_cSkillModelEmitCfgMap[sTempCfg.un32SkillModelID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d Skill cfg.", (UINT32)m_cSkillModelEmitCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadSkillModelRangeCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_area.xml";
	
	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_area");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSkillModelRangeCfg sTempCfg;
			 sTempCfg.eSkillModelType = eSkillEffectType_Range;
			sTempCfg.un32SkillModelID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillModelID){
				continue;
			}
			sTempCfg.n32UseMp = iterInfo->second.get<INT32>("n32UseMP", 0);
			sTempCfg.n32UseHp = iterInfo->second.get<INT32>("n32UseHP", 0);
			sTempCfg.n32UseCP = iterInfo->second.get<INT32>("n32UseCP", 0);

			sTempCfg.bIsCoolDown = iterInfo->second.get<bool>("bIsCoolDown", 0);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.bIfAffectBuilding = iterInfo->second.get<bool>("bIfAffectBuilding", 0);
			sTempCfg.bIfAffectHero = iterInfo->second.get<bool>("bIfAffectHero", 0);
			sTempCfg.bIfAffectMonster = iterInfo->second.get<bool>("bIfAffectMonster", 0);
			sTempCfg.bIsCanMove = iterInfo->second.get<bool>("bIsCanMove", 1);
			sTempCfg.bIsCanBreak = iterInfo->second.get<bool>("bIsCanBreak", 1);
			sTempCfg.eTargetType = (ESkillModelTargetCate)iterInfo->second.get<INT32>("eTargetType", 0);

			sTempCfg.eSkillAOECate = (ESkillAOECate)iterInfo->second.get<INT32>("eAoeType", 0);

			sTempCfg.n32RangeInterval = iterInfo->second.get<INT32>("n32RangeInterval", 0);
			sTempCfg.n32RangeTimes = iterInfo->second.get<INT32>("n32RangeTimes", 0);
			sTempCfg.eSkillShapeType = (ESkillShapeType)iterInfo->second.get<INT32>("n32Range", 0);
			sTempCfg.n32RangePar1 = iterInfo->second.get<INT32>("n32RangePar1", 0);
			sTempCfg.n32RangePar2 = iterInfo->second.get<INT32>("n32RangePar2", 0);
			sTempCfg.n32MaxEffectObj = iterInfo->second.get<INT32>("n32EffectObjNum", 0);
			sTempCfg.n32LifeTime = iterInfo->second.get<INT32>("eLifeTime", 0); 

			sTempCfg.n32ReleaseDist = iterInfo->second.get<INT32>("n32ReleaseDistance", 0); 

			sTempCfg.n32ReleaseTimeDelay  = iterInfo->second.get<INT32>("n32ReleaseTimeDelay", 0); 

			string sEffect = iterInfo->second.get<string>("attackEffect", "");  
			strcpy(sTempCfg.szattackEffect,sEffect.data() );

			string sVal = iterInfo->second.get<string>("EventID", ""); 
			DoEventStr(sVal, sTempCfg.asSkillModelList,c_n32MaxNextSkillEffectCfgNum); 

			sTempCfg.bIfHasEffect = FALSE == (sEffect == "0" || sEffect.empty());

			m_cSkillModelRangeCfgMap[sTempCfg.un32SkillModelID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d Skill cfg.", (UINT32)m_cSkillModelRangeCfgMap.size());
	 
	return eNormal;
}

INT32	CCfgMgr::LoadSkillModelBufCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_buff.xml";

	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_buff");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSkillModelBufCfg sTempCfg;
			
			sTempCfg.eSkillModelType = eSkillEffectType_Buf;
			sTempCfg.un32SkillModelID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillModelID){
				continue;
			}
			sTempCfg.n32UseMp = iterInfo->second.get<INT32>("n32UseMP", 0);
			sTempCfg.n32UseHp = iterInfo->second.get<INT32>("n32UseHP", 0);
			sTempCfg.n32UseCP = iterInfo->second.get<INT32>("n32UseCP", 0);

			sTempCfg.bIsCoolDown = iterInfo->second.get<bool>("bIsCoolDown", 0);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.bIfAffectBuilding = iterInfo->second.get<bool>("bIfAffectBuilding", 0);
			sTempCfg.bIfAffectHero = iterInfo->second.get<bool>("bIfAffectHero", 0);
			sTempCfg.bIsCanMove = iterInfo->second.get<bool>("bIsCanMove", 1);
			sTempCfg.bIsCanBreak = iterInfo->second.get<bool>("bIsCanBreak", 1);
			sTempCfg.bIfAffectMonster = iterInfo->second.get<bool>("bIfAffectMonster", 0); 
			sTempCfg.eBuffType =  (EBuffType)iterInfo->second.get<INT32>("eBuffType", 0); 
			sTempCfg.n32EffectLastTick =  iterInfo->second.get<INT32>("eEffect0LastTick", 0); 
			sTempCfg.n32EffectInterval =  iterInfo->second.get<INT32>("eEffect0EffectInterval", 0);

			//sTempCfg.sEffectInfo.eEffectCate = (EEffectCate)iterInfo->second.get<INT32>("n32EffectID", 0);
			//sTempCfg.sEffectInfo.n32EffectValue = iterInfo->second.get<INT32>("eEffectValue", 0);
			//sTempCfg.sEffectInfo.n32EffectRate = iterInfo->second.get<INT32>("eEffectRate", 0);
			//sTempCfg.sEffectInfo.bIfBlastAttack = iterInfo->second.get<bool>("bEffectIsCritical", 0);
			//sTempCfg.sEffectInfo.n32CriticalPercent = iterInfo->second.get<INT32>("eEffectCriticalPosb", 0);
			//sTempCfg.sEffectInfo.n32BlastAttackRate = iterInfo->second.get<INT32>("eEffectCriticalPercent", 0);

			sTempCfg.eTargetType = (ESkillModelTargetCate)iterInfo->second.get<INT32>("eTargetType", 0);
			sTempCfg.eBuffTarget = (ESkillBuffTarCate)iterInfo->second.get<INT32>("eBuffTarget", 0);

			sTempCfg.eReplaceType = (EReplaceType)iterInfo->second.get<INT32>("n32ReplaceType", 0);
			sTempCfg.n32ReplaceTimes = iterInfo->second.get<INT32>("n32RepeatTimes", 0);
			sTempCfg.n32RejectId =  iterInfo->second.get<INT32>("n32RejectID", 0);
			sTempCfg.n32ReplaceId = iterInfo->second.get<INT32>("n32ReplaceID", 0);

			sTempCfg.bIfClearAtDie =  (iterInfo->second.get<INT32>("bEffect0ClearWhenDie", 0) > 0 ) ? true : false;

			string sStartEvent = iterInfo->second.get<string>("StartEventID", ""); 
			DoEventStr(sStartEvent, sTempCfg.asSkillStartModelList,c_n32MaxNextSkillEffectCfgNum); 

			string sIntervalEvent = iterInfo->second.get<string>("IntervalEventID", ""); 
			DoEventStr(sIntervalEvent, sTempCfg.asSkillIntervalModelList,c_n32MaxNextSkillEffectCfgNum); 

			string sEndEvent = iterInfo->second.get<string>("EndEventID", ""); 
			DoEventStr(sEndEvent, sTempCfg.asSkillEndModelList, c_n32MaxNextSkillEffectCfgNum); 

			m_cSkillModelBufCfgMap[sTempCfg.un32SkillModelID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d Skill cfg.", (UINT32)m_cSkillModelBufCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadSkillModelSummonCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_summon.xml"; 
	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_summon");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSkillModuleSummonCfg sTempCfg;

			sTempCfg.un32SkillModelID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillModelID){
				continue;
			}
			sTempCfg.n32UseMp = iterInfo->second.get<INT32>("n32UseMP", 0);
			sTempCfg.n32UseHp = iterInfo->second.get<INT32>("n32UseHP", 0);
			sTempCfg.n32UseCP = iterInfo->second.get<INT32>("n32UseCP", 0);

			sTempCfg.bIsCoolDown = iterInfo->second.get<bool>("bIsCoolDown", 0);
			sTempCfg.bIsCanMove = iterInfo->second.get<bool>("bIsCanMove", 1);
			sTempCfg.bIsCanBreak = iterInfo->second.get<bool>("bIsCanBreak", 1);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.eSummonType = (ESummonType )iterInfo->second.get<INT32>("eSummonType", 0);
			sTempCfg.eSummonWayType = (ESummonWayType )iterInfo->second.get<INT32>("eSummonWayType", 0);
			sTempCfg.n32LifeTime = iterInfo->second.get<INT32>("eLifeTime", 0); 
			sTempCfg.n32Distance = iterInfo->second.get<INT32>("n32Distance", 0); 

			string sSummonNum = iterInfo->second.get<string>("n32SummonNum"); 
			INT32 nPos = sSummonNum.find(",");
			if (-1 == nPos)
			{
				sTempCfg.n32SummonNum[0] = (INT32)atoi( sSummonNum.data());
			}
			else
			{
				vector<string> tVec; 
				tVec.clear(); 
				boost::algorithm::split( tVec, sSummonNum, boost::algorithm::is_any_of(",") ); 
				for (int i=0; i< tVec.size(); i++)
				{
					INT32  tSummonNum = (INT32)atoi(tVec[i].data());
					if ( tSummonNum > 0)
					{
						sTempCfg.n32SummonNum[i] = tSummonNum;
					}
					if (i >= n_c32NPCNum)
					{
						break;
					}
				}
			}

			string sSummonId = iterInfo->second.get<string>("summonID"); 
			nPos = sSummonId.find(",");
			if (-1 == nPos)
			{
				sTempCfg.nNPCId[0] = (INT32)atoi( sSummonId.data());
			}
			else
			{
				vector<string> tVec; 
				tVec.clear(); 
				boost::algorithm::split( tVec, sSummonId, boost::algorithm::is_any_of(",") ); 
				for (int i=0; i< tVec.size(); i++)
				{
					INT32  tSummonID = (INT32)atoi(tVec[i].data());
					if ( tSummonID > 0)
					{
						sTempCfg.nNPCId[i] = tSummonID;
					}
					if (i >= n_c32NPCNum)
					{
						break;
					}
				}
			}

			string sVal = iterInfo->second.get<string>("EventID", ""); 
			DoEventStr(sVal, sTempCfg.asSkillModelList, c_n32MaxNextSkillEffectCfgNum); 

			m_cSSkillModuleSummonCfgMap[sTempCfg.un32SkillModelID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d Skill cfg.", (UINT32)m_cSSkillModuleSummonCfgMap.size());

	return eNormal;
}

INT32	CCfgMgr::LoadSkillModelMoveCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_move.xml";

	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_move");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSkillModelMoveCfg sTempCfg;

			sTempCfg.eSkillModelType = eSkillEffectType_Move;
			sTempCfg.un32SkillModelID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillModelID){
				continue;
			}
			sTempCfg.n32UseMP = iterInfo->second.get<INT32>("n32UseMP", 0);
			sTempCfg.n32UseHP = iterInfo->second.get<INT32>("n32UseHP", 0);
			sTempCfg.n32UseCP = iterInfo->second.get<INT32>("n32UseCP", 0);

			sTempCfg.bIsCoolDown = iterInfo->second.get<bool>("bIsCoolDown", 0);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);

			sTempCfg.bIsCanMove = iterInfo->second.get<bool>("bIsCanMove", 1);
			sTempCfg.bIsCanBreak = iterInfo->second.get<bool>("bIsCanBreak", 1);

			sTempCfg.eMoveType = (ESkillEffectMoveType)iterInfo->second.get<INT32>("eMoveType", 0);
			sTempCfg.eMovedTargetType = (ESkillEffectMovedTargetType)iterInfo->second.get<INT32>("eMovedTarget", 0);
			sTempCfg.eMoveToTargetType = (ESkillEffectMoveToTargetType)iterInfo->second.get<INT32>("eMoveToTarget", 0);

			sTempCfg.n32Angle = iterInfo->second.get<INT32>("n32Angle", 0);
			sTempCfg.n32Distance =  iterInfo->second.get<INT32>("n32Distance", 0);
			sTempCfg.n32Speed = iterInfo->second.get<INT32>("n32Speed", 0);

			string sEvent = iterInfo->second.get<string>("EventID", ""); 
			DoEventStr(sEvent, sTempCfg.asSkillModelList, c_n32MaxNextSkillEffectCfgNum); 

			m_cSkillModuleMoveCfgMap[sTempCfg.un32SkillModelID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d Skill cfg.", (UINT32)m_cSkillModuleMoveCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadSkillModelSwitchCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_switch.xml";

	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_switch");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSkillModelSwitchCfg sTempCfg;

			sTempCfg.eSkillModelType = eSkillEffectType_Switch;
			sTempCfg.un32SkillModelID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillModelID){
				continue;
			}
			sTempCfg.n32UseMP = iterInfo->second.get<INT32>("n32UseMP", 0);
			sTempCfg.n32UseHP = iterInfo->second.get<INT32>("n32UseHP", 0);
			sTempCfg.n32UseCP = iterInfo->second.get<INT32>("n32UseCP", 0);

			sTempCfg.bIsCoolDown = iterInfo->second.get<bool>("bIsCoolDown", 0);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.bIsCanMove = iterInfo->second.get<bool>("bIsCanMove", 1);
			sTempCfg.bIsCanBreak = iterInfo->second.get<bool>("bIsCanBreak", 1);

			string sPassitiveEvent = iterInfo->second.get<string>("PassEventID");
			typedef vector< string > split_vector_type;
			split_vector_type sPassitiveVec;
			boost::algorithm::split( sPassitiveVec, sPassitiveEvent, boost::algorithm::is_any_of(",") );

			for (INT32 i = 0; i < sPassitiveVec.size(); ++i){
				sTempCfg.lPassitiveEffectList[i] = atoi(sPassitiveVec[i].c_str());
			}

			m_cSkillModuleSwitchCfgMap[sTempCfg.un32SkillModelID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d Skill cfg.", (UINT32)m_cSkillModuleMoveCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadSkillModelPurifactionCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_purification.xml";

	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_purification");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSkillModelPurificationCfg sTempCfg;

			sTempCfg.eSkillModelType = eSkillEffectType_Purification;
			sTempCfg.un32SkillModelID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32SkillModelID){
				continue;
			}
			sTempCfg.n32UseMP = iterInfo->second.get<INT32>("n32UseMP", 0);
			sTempCfg.n32UseHP = iterInfo->second.get<INT32>("n32UseHP", 0);
			sTempCfg.n32UseCP = iterInfo->second.get<INT32>("n32UseCP", 0);

			sTempCfg.bIsCoolDown = iterInfo->second.get<bool>("bIsCoolDown", 0);
			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.bIfAffectBuilding = iterInfo->second.get<bool>("bIfAffectBuilding", 0);
			sTempCfg.bIfAffectHero = iterInfo->second.get<bool>("bIfAffectHero", 0);
			sTempCfg.bIfAffectMonster = iterInfo->second.get<bool>("bIfAffectMonster", 0); 
			sTempCfg.eTargetType = (ESkillModelTargetCate)iterInfo->second.get<bool>("eTargetType", 0); 

			sTempCfg.ePurType = (EPurificationType)iterInfo->second.get<INT32>("ePurifyWay", 0);
			sTempCfg.n32MaxNum = (EPurificationType)iterInfo->second.get<INT32>("n32PurifyMaxCount", 0);

			typedef vector< string > split_vector_type;
			string sAtrr = iterInfo->second.get<string>("ParameterList");
			split_vector_type sAtrrVec;
			boost::algorithm::split( sAtrrVec, sAtrr, boost::algorithm::is_any_of(",") );
			for (INT32 i = 0; i < sAtrrVec.size(); ++i){
				sTempCfg.lPurParameters[i] = atoi(sAtrrVec[i].c_str());
			}

			m_cSkillModulePurificationCfgMap[sTempCfg.un32SkillModelID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d Skill cfg.", (UINT32)m_cSkillModulePurificationCfgMap.size());
	return eNormal;
}


INT32	CCfgMgr::LoadPassitiveSkillCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_passitive.xml";

	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_passitive");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSPassiveSkilllCfg sTempCfg;

			sTempCfg.un32PassiveSkillID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32PassiveSkillID){
				continue;
			}
			sTempCfg.eReleaseWay = (ESkillReleaseWay)iterInfo->second.get<INT32>("eReleaseWay", 0);
			sTempCfg.n32UpgradeLevel = iterInfo->second.get<INT32>("n32UpgradeLevel", 0);

			sTempCfg.n32UseMP = iterInfo->second.get<INT32>("n32UseMP", 0);
			sTempCfg.n32UseHP = iterInfo->second.get<INT32>("n32UseHP", 0);
			sTempCfg.n32UseCP = iterInfo->second.get<INT32>("n32UseCP", 0);

			sTempCfg.n32Cooldown = iterInfo->second.get<INT32>("n32CoolDown", 0);

			string sTriggerEvent = iterInfo->second.get<string>("eTriggerWay");
			typedef vector< string > split_vector_type;
			split_vector_type sPassitiveTriggerVec;
			boost::algorithm::split( sPassitiveTriggerVec, sTriggerEvent, boost::algorithm::is_any_of(",") );
			for (INT32 i = 0; i < sPassitiveTriggerVec.size(); ++i){
				sTempCfg.lePassiveSkillTriggerType[i] = (EPassiveSkillTriggerType)atoi(sPassitiveTriggerVec[i].c_str());
			}

			sTempCfg.n32TriggerRate = iterInfo->second.get<INT32>("n32TriggerRate", 0);
			sTempCfg.n32TriggerInterval = iterInfo->second.get<INT32>("n32TriggerInterval", 0);

			sTempCfg.bIfAffectBuilding = iterInfo->second.get<bool>("bIfTriggerByBuilding", 0);
			sTempCfg.bIfAffectHero = iterInfo->second.get<bool>("bIfTriggerByHero", 0);
			sTempCfg.bIfAffectMonster = iterInfo->second.get<bool>("bIfTriggerByMonster", 0); 

			sTempCfg.eRangeType = (EPassiveRangeType)iterInfo->second.get<INT32>("eEffectiveAttackWay", 0);
			sTempCfg.ePassiveSkillTargetType = (EPassiveSkillTargetType)iterInfo->second.get<INT32>("eTargetType", 0);
			sTempCfg.bIfEffectOnDis = iterInfo->second.get<bool>("bIfRemoveWhenDie", 0); 

			string sStartEvent = iterInfo->second.get<string>("StartEventID", ""); 
			DoEventStr(sStartEvent, sTempCfg.asStartSkillModelList, c_n32MaxNextSkillEffectCfgNum); 

			string sEndEvent = iterInfo->second.get<string>("EndEventID", ""); 
			DoEventStr(sEndEvent, sTempCfg.asEndSkillModelList, c_n32MaxNextSkillEffectCfgNum); 

			string sEvent = iterInfo->second.get<string>("EventID", ""); 
			DoEventStr(sEvent, sTempCfg.asSkillModelList, c_n32MaxNextSkillEffectCfgNum); 

			string sPassitiveEvent = iterInfo->second.get<string>("PassitiveEventID");
			typedef vector< string > split_vector_type;
			split_vector_type sPassitiveVec;
			boost::algorithm::split( sPassitiveVec, sPassitiveEvent, boost::algorithm::is_any_of(",") );

			for (INT32 i = 0; i < sPassitiveVec.size(); ++i){
				sTempCfg.lPassitiveEffectList[i] = atoi(sPassitiveVec[i].c_str());
			}

			//特效部分
			sTempCfg.bIfHasStartEffect = iterInfo->second.get<string>("StartEffect") != "0"; 
			sTempCfg.bIfHasReleaseEffect = iterInfo->second.get<string>("n32ReleaseAction") != "0" || iterInfo->second.get<string>("n32ReleaseSound") != "0" || iterInfo->second.get<string>("ReleaseEffect") != "0"; 

			sTempCfg.bIfSendColdDown = iterInfo->second.get<bool>("bIsShowColdDown", 0);

			m_cPassitiveSkillCfgMap[sTempCfg.un32PassiveSkillID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d Skill cfg.", (UINT32)m_cPassitiveSkillCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadPassitiveEffectBloodSeekCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_bloodseek.xml";

	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_bloodseek");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSPassiveEffectCfg_BloodSeek sTempCfg;

			sTempCfg.n32PassitiveEffectID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.n32PassitiveEffectID){
				continue;
			}
			
			sTempCfg.n32BaseValue = iterInfo->second.get<INT32>("n32BaseValue", 0);
			sTempCfg.n32Percent = iterInfo->second.get<INT32>("n32HarmRate", 0);
			sTempCfg.n32TargetHealthPercent = iterInfo->second.get<INT32>("n32TargetLifeRate", 0);
			sTempCfg.n32SelfHealthPercent = iterInfo->second.get<INT32>("n32SelfLifeRate", 0);


			m_cPassitiveEffectBloodSeekCfgMap[sTempCfg.n32PassitiveEffectID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d Skill cfg.", (UINT32)m_cPassitiveEffectBloodSeekCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadPassitiveEffectReboundCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_rebound.xml";

	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_rebound");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSPassiveEffectCfg_Rebound sTempCfg;

			sTempCfg.n32PassitiveEffectID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.n32PassitiveEffectID){
				continue;
			}

			sTempCfg.n32BaseValue = iterInfo->second.get<INT32>("n32BaseValue", 0);
			sTempCfg.n32Percent = iterInfo->second.get<INT32>("n32HarmRate", 0);
			sTempCfg.n32TargetHealthPercent = iterInfo->second.get<INT32>("n32TargetLifeRate", 0);
			sTempCfg.n32SelfHealthPercent = iterInfo->second.get<INT32>("n32SelfLifeRate", 0);


			m_cPassitiveEffectReboundCfgMap[sTempCfg.n32PassitiveEffectID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d Skill cfg.", (UINT32)m_cPassitiveEffectReboundCfgMap.size());
	return eNormal;
}

INT32	CCfgMgr::LoadPassitiveEffectReliveCfg()
{
	const char *cpszfilename = "./CSBattleMgr/NewSkill/SkillCfg_relive.xml";

	ptree cPtree;
	ptree cSkillCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cSkillCfg = cPtree.get_child("SkillCfg_relive");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NewSkillNum = 0;
	try{
		for (ptree::iterator iterInfo = cSkillCfg.begin(); iterInfo != cSkillCfg.end(); iterInfo++){
			if ("info" != iterInfo->first){
				continue;
			}

			n32NewSkillNum++;

			SSPassiveEffectCfg_Relive sTempCfg;

			sTempCfg.n32PassitiveEffectID = iterInfo->second.get<UINT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.n32PassitiveEffectID){
				continue;
			}

			sTempCfg.n32WaitMilSec = iterInfo->second.get<INT32>("n32WaitMilSec", 0);
			sTempCfg.n32BaseHP = iterInfo->second.get<INT32>("n32BaseHP", 0);
			sTempCfg.n32PercentHP = iterInfo->second.get<INT32>("n32PercentHP", 0);
			sTempCfg.n32BaseMP = iterInfo->second.get<INT32>("n32BaseMP", 0);
			sTempCfg.n32PercentMP = iterInfo->second.get<INT32>("n32PercentMP", 0);


			m_cPassitiveEffectReliveCfgMap[sTempCfg.n32PassitiveEffectID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail for %s (%d).", cException.what(), n32NewSkillNum);
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d Skill cfg.", (UINT32)m_cPassitiveEffectReliveCfgMap.size());
	return eNormal;
}


void	CCfgMgr::DoEventStr(std::string & str, SSNextSkillEffectCfg *asSkillModelList,INT32 num)
{
	if (str.length() < 1)
	{
		return;
	}

	std::vector<SSNextSkillEffectCfg> tSNSE;
	tSNSE.clear();	 
	SpilitEventStr(str,tSNSE); 
	{
		INT32 ntotal = tSNSE.size();
		for (int i=0; i< ntotal; ++i)
		{
			if (asSkillModelList[i].un32SkillEffectID == 0)
			{
				asSkillModelList[i] = tSNSE[i];
			}

			if (i > num)
			{
				ELOG(LOG_DBBUG, "---error config %d", num);
				break;
			}
		} 
	}
}
//解析字符串：20025:1000(20026:0,20017:500);20030:1000
//解析字符串：20025:1000;20030:1000
//注:不可能有多层伊奈，约定最多1层伊奈(最多一层())
#define SPILIT_STR_FEATURE_SEM ";"
#define SPILIT_STR_FEATURE_COM "," 

void  CCfgMgr::SpilitEventStr(std::string & str, std::vector<SSNextSkillEffectCfg>& tSNSE )
{
 	vector<string> tokens_Lv_1;
 	tokens_Lv_1.clear();  
	SSNextSkillEffectCfg snecfg;  
	memset(&snecfg,0,sizeof(snecfg));

	int tPos = str.find(":");
	if (-1 == tPos)
	{
		snecfg.un32SkillEffectID = (UINT32)atoi(str.data());
		tSNSE.push_back(snecfg);
		return;
	}

#define DOTAGPARAM(tempSNSE,moduleId) {   \
	if (tempSNSE.un32SkillEffectID > 0) {							  \
		for (int l=0; l < c_n32MaxDependSkillEffectCfgNum ;l++)	{     \
			if (tempSNSE.n32DependedArr[l] == 0)					{	 \
			tempSNSE.n32DependedArr[l] = moduleId ;					 \
			break;													\
			}}	} }

#define  DoAddVec(tSNSE, snecfg,moduleId, pValue) {						\
				memset(&snecfg,0,sizeof(snecfg));  \
				snecfg.un32SkillEffectID = moduleId;		\
				snecfg.n32Delay = pValue;			\
				tSNSE.push_back(snecfg);		}

	boost::algorithm::split( tokens_Lv_1, str, boost::algorithm::is_any_of(SPILIT_STR_FEATURE_SEM) ); 
	for (int i=0; i< tokens_Lv_1.size(); ++i)
	{  
		string strChild( tokens_Lv_1[i] );
		tPos = strChild.find("(");	//是否存在关系模块
		if (-1 == tPos)
		{
			vector<string> tokens_Lv_2;
			tokens_Lv_2.clear(); 
			boost::algorithm::split( tokens_Lv_2, strChild, boost::algorithm::is_any_of(":") );
			{
				DoAddVec(tSNSE,snecfg,(INT32)atoi(tokens_Lv_2[0].c_str()),(INT32)atoi(tokens_Lv_2[1].c_str())) 
			} 
		}else
		{
			SSNextSkillEffectCfg  tempSNSE;
			memset(&tempSNSE,0,sizeof(tempSNSE));

			bool bState = false;
			tPos = strChild.find("(");
			if (-1 != tPos)//限定只有1层关系
			{
				vector<string> tokens_Lv_2;
				tokens_Lv_2.clear(); 
				boost::algorithm::split( tokens_Lv_2, strChild, boost::algorithm::is_any_of("(") ); 
				for (int j =0; j < tokens_Lv_2.size(); j++)
				{
					strChild =  tokens_Lv_2[j];
					tPos = strChild.find(")");
					if (tPos != -1)
					{
						strChild =strChild.replace(tPos,tPos +1, "");
						vector<string> tokens_Lv_3;
						tokens_Lv_3.clear(); 
						boost::algorithm::split( tokens_Lv_3, strChild, boost::algorithm::is_any_of(",") ); 
						for (int p =0; p < tokens_Lv_3.size(); p++)
						{
							vector<string> tokens_Lv_4;
							tokens_Lv_4.clear(); 
							strChild = tokens_Lv_3[p];

							boost::algorithm::split( tokens_Lv_4, strChild, boost::algorithm::is_any_of(":") ); 
							
							DoAddVec(tSNSE,snecfg,(INT32)atoi(tokens_Lv_4[0].c_str()),(INT32)atoi(tokens_Lv_4[1].c_str()))

							DOTAGPARAM(tempSNSE, snecfg.un32SkillEffectID )  
						} 
					}
					else
					{
						vector<string> tokens_Lv_3;
						tokens_Lv_3.clear(); 
						boost::algorithm::split( tokens_Lv_3, strChild, boost::algorithm::is_any_of(":") ); 

						DoAddVec(tSNSE,snecfg,(INT32)atoi(tokens_Lv_3[0].c_str()),(INT32)atoi(tokens_Lv_3[1].c_str()))

						DOTAGPARAM(tempSNSE, snecfg.un32SkillEffectID ) 

						tempSNSE = snecfg;
					}
				}
			}
			else
			{
				vector<string> tokens_Lv_2;
				tokens_Lv_2.clear(); 
				boost::algorithm::split( tokens_Lv_2, strChild, boost::algorithm::is_any_of(":") ); 
 
				DoAddVec(tSNSE,snecfg,(INT32)atoi(tokens_Lv_2[0].c_str()),(INT32)atoi(tokens_Lv_2[1].c_str()))

				tempSNSE = snecfg;
			} 

			std::vector<SSNextSkillEffectCfg>::iterator itr = tSNSE.begin();
			for (;itr != tSNSE.end(); ++itr)
			{
				if (itr->un32SkillEffectID == tempSNSE.un32SkillEffectID)
				{
					*itr = tempSNSE;
					break;
				}
			}
		}
	} 
}

INT32 CCfgMgr::LoadFilterWordsCfg(){
	const string fileName = "./CSConfig/Chatfilter.txt";
	std::ifstream in(fileName, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		
		string utf8Res;
		CChineseCode::GB2312ToUTF_8(utf8Res, (char*)contents.c_str(), contents.size());
		typedef vector<string> split_vector_type;
		split_vector_type sAtrrVec;
		boost::algorithm::split(sAtrrVec, utf8Res, boost::algorithm::is_any_of(","));

		//CCSWordFilter::GetWordFilterInstance().AddWords(sAtrrVec);

		ELOG(LOG_INFO, "Success!");

		return eNormal;
	}

	return eEC_LoadFilterCfgFailed;
}

INT32 CCfgMgr::LoadHeroChooseCfg(){
	m_cHeroChooseCfg.clear();

	const char *cpszfilename = "./CSBattleMgr/HeroChooseCfg.xml";
	ptree cPtree;
	ptree cHeroCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cHeroCfg = cPtree.get_child("herochoose");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NPCNum = 0;
	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	try{
		for (ptree::iterator iterInfo = cHeroCfg.begin(); iterInfo != cHeroCfg.end(); iterInfo++){
			n32NPCNum++;
			if ("info" != iterInfo->first){
				continue;
			}

			SHeroChooseCfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));

			sTempCfg.un32HeroID = (EObjectType)iterInfo->second.get<UINT32>("<xmlattr>.number", 0);
			if (0 == sTempCfg.un32HeroID){
				continue;
			}
			sTempCfg.n32IfGuide = iterInfo->second.get<INT32>("newsguidehero", 0);
			m_cHeroChooseCfg[sTempCfg.un32HeroID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail.");
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d herochoose cfg.", (UINT32)m_cHeroChooseCfg.size());
	return eNormal;
}

INT32 CCfgMgr::LoadRobotAICfg(){
	m_cRobotAICfg.clear();

	const char *cpszfilename = "./CSBattleMgr/RobotAICfg.xml";
	ptree cPtree;
	ptree cHeroCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cHeroCfg = cPtree.get_child("SkillCfg_RobotAI");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NPCNum = 0;
	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	try{
		for (ptree::iterator iterInfo = cHeroCfg.begin(); iterInfo != cHeroCfg.end(); iterInfo++){
			n32NPCNum++;
			if ("info" != iterInfo->first){
				continue;
			}

			SRobotAICfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.n32ID = iterInfo->second.get<INT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.n32ID){
				continue;
			}
			sTempCfg.n32ModelID = iterInfo->second.get<INT32>("n32FunctionID");
			sTempCfg.n32Type = iterInfo->second.get<INT32>("n32NodeID");
			sTempCfg.n32ParentID = iterInfo->second.get<INT32>("n32ExNodeID");

			typedef vector< string > split_vector_type;
			string sAtrr = iterInfo->second.get<string>("n32Value");
			split_vector_type sAtrrVec;
			boost::algorithm::split( sAtrrVec, sAtrr, boost::algorithm::is_any_of(",") );
			for (INT32 i = 0; i < sAtrrVec.size(); ++i){
				sTempCfg.an32Parameters[i] = atoi(sAtrrVec[i].c_str());
			}

			m_cRobotAICfg.push_back(sTempCfg);
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail.");
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d RobotAI cfg.", (UINT32)m_cRobotAICfg.size());
	return eNormal;
}

INT32 CCfgMgr::LoadRobotAIPathCfg(){
	m_cRobotAIPathCfg.clear();

	const char *cpszfilename = "./CSBattleMgr/RobotPath.xml";
	ptree cPtree;
	ptree cHeroCfg;
	try{
		read_xml(cpszfilename, cPtree, xml_parser::trim_whitespace);
		cHeroCfg = cPtree.get_child("SkillCfg_RobotPath");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "open cfg file fail.");
		return eEC_OpenCfgFileFail;
	}

	INT32 n32NPCNum = 0;
	UINT32 un32ID = 0;
	CHAR *cpszName = NULL;
	string cTempStr;
	try{
		for (ptree::iterator iterInfo = cHeroCfg.begin(); iterInfo != cHeroCfg.end(); iterInfo++){
			n32NPCNum++;
			if ("info" != iterInfo->first){
				continue;
			}

			SRobotAIPathCfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.un32ID = iterInfo->second.get<INT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32ID){
				continue;
			}
			sTempCfg.n32MapID = iterInfo->second.get<INT32>("n32MapID");
			sTempCfg.n32CampID = iterInfo->second.get<INT32>("n32CampID");
			sTempCfg.n32PahtID = iterInfo->second.get<INT32>("n32NodeID");
			sTempCfg.fX = iterInfo->second.get<FLOAT>("n32NodeX");
			sTempCfg.fY = iterInfo->second.get<FLOAT>("n32NodeY");
			sTempCfg.fZ = iterInfo->second.get<FLOAT>("n32NodeZ");

			m_cRobotAIPathCfg[sTempCfg.un32ID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, "read cfg file fail.");
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, "Load %d RobotAI cfg.", (UINT32)m_cRobotAIPathCfg.size());
	return eNormal;
}

INT32 CCfgMgr::LoadAiRobotName()
{
	const string fileName = "./CSConfig/AiRobotName.txt";
	std::ifstream in(fileName, std::ios::in | std::ios::binary);
	if (!in.is_open()) return eEC_ReadCfgFileFail;
	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();
	string utf8Res;
	CChineseCode::GB2312ToUTF_8(utf8Res, (char*)contents.c_str(), contents.size());
	m_cAiRobotNameMap.clear();
	boost::algorithm::split(m_cAiRobotNameMap, utf8Res, boost::algorithm::is_any_of(","));
	Assert(m_cAiRobotNameMap.size()>10);
	for (auto it=m_cAiRobotNameMap.begin();it!=m_cAiRobotNameMap.end();++it){
		m_cAiRobotNameMapForCheck.insert(make_pair((*it),false));
	}
	ELOG(LOG_INFO, "LoadAiRobotName Success!");
	return eNormal;
}

SNPCCfg* CCfgMgr::GetNPCCfg( INT32 key )
{
	auto it = m_cNPCCfgMap.find(key);
	return it==m_cNPCCfgMap.end()?NULL:&(it->second);
}

SHeroCfg* CCfgMgr::GetHeroCfg( INT32 key )
{
	auto it = m_cHeroCfgMap.find(key);
	return it==m_cHeroCfgMap.end()?NULL:&(it->second);
}

SSNewSkillCfg* CCfgMgr::GetSkillCfg( UINT32 un32SkillID )
{
	auto it = m_cNewSkillCfgMap.begin();
	return it==m_cNewSkillCfgMap.end()?NULL:&(it->second);
}

SGoodsCfg* CCfgMgr::GetGoodsCfg( UINT32 key )
{
	auto it = m_cGoodsCfgMap.begin();
	return it==m_cGoodsCfgMap.end()?NULL:&(it->second);
}

INT32 CCfgMgr::LoadUserCfg()
{
	INT32 n32LoadRet = eNormal;
	//if (eNormal == n32LoadRet) n32LoadRet = LoadRunesCfg(); 
	//if (eNormal == n32LoadRet) n32LoadRet = LoadRunesSlotCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadUserExpCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadHeroBuyCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadDiscountCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadLoginRewardCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadInvalidWordCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadUserDBSaveCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadOtherItemCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadHotAndNewGoodsCfg();
	//if (eNormal == n32LoadRet) n32LoadRet = LoadSDBCfg();
	return n32LoadRet;
}

INT32 CCfgMgr::ResetUserCfg()
{
	return eNormal;
}

void CChineseCode::UTF_8ToUnicode(wchar_t* pOut,char *pText){
	char* uchar = (char *)pOut;
	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
	return;
}

void CChineseCode::UnicodeToUTF_8(char* pOut,wchar_t* pText){
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pText;
	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));
	return;
}

void CChineseCode::UnicodeToGB2312(char* pOut, wchar_t uData){
#ifdef _WINDOWS_
	WideCharToMultiByte(CP_ACP,NULL, &uData, 1, pOut, sizeof(wchar_t), NULL, NULL);
#else
	//iconv_t cd = iconv_open("gb2312", "unicode");
	//size_t pLen = 1;
	//size_t pOutLen = 0;
	//iconv(cd, &uData, &pLen, ,pOut, &pOutLen);
	//iconv_close(cd);
#endif
	return;
}     

void CChineseCode::Gb2312ToUnicode(wchar_t* pOut, char *gbBuffer){
#ifdef _WINDOWS_
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);
#else
	//iconv_t cd = iconv_open("unicode","gb2312");
	//size_t pLen = 2;
	//size_t pOutLen = 1;
	//iconv(cd, &gbBuffer, &pLen, pOut, &pOutLen);
	//iconv_close(cd);
#endif
	return ;
}

void CChineseCode::GB2312ToUTF_8(string& pOut,char *pText, int pLen){
	char buf[4];
	int nLength = pLen * 3;
	char* rst = new char[nLength];

	memset(buf,0,4);
	memset(rst,0,nLength);

	int i = 0;
	int j = 0;      
	while(i < pLen){
		if( *(pText + i) >= 0){
			rst[j++] = pText[i++]; 
		}else{
			wchar_t pbuffer;
			Gb2312ToUnicode(&pbuffer, pText+i);
			UnicodeToUTF_8(buf, &pbuffer);
			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];    
			j += 3;
			i += 2;
		}
	}

	rst[j] = '/0';
	pOut = rst;             
	delete []rst;     
	return;
}

void CChineseCode::UTF_8ToGB2312(string &pOut, char *pText, int pLen){
	char * newBuf = new char[pLen];
	char Ctemp[4];
	memset(Ctemp,0,4);
	int i =0;
	int j = 0;

	while(i < pLen){

		if(pText > 0){
			newBuf[j++] = pText[i++];                       
		}else{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);
			UnicodeToGB2312(Ctemp, Wtemp);
			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}

	newBuf[j] = '/0';
	pOut = newBuf;
	delete []newBuf;
	return; 
}

}