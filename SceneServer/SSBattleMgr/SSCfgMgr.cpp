#include "stdafx.h"
#include "SSCfgMgr.h"
#include <random>
 
#include "SSBattle_Script.h"
#include "boost/spirit.hpp"
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/typeof/typeof.hpp"
#include "boost/foreach.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"

using namespace boost::property_tree;

namespace SceneServer{

CSSCfgMgr::CSSCfgMgr(void) {
    char szPath[MAX_PATH];
    ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
    for (int i = strlen(szPath) - 1; i >= 0; --i)
    {
        if (szPath[i] == '\\')
        {
            szPath[i] = 0;
            break;
        }
    }

    m_strExeDir = szPath;
    m_strExeDir += '\\';

    
    m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyNPCCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyNPCCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyHeroCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyHeroCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyGoodsCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyGoodsCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyShopCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyShopCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyRobotAICfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyRobotAICfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyRobotAIPathCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyRobotAIPathCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyNewSkillCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyNewSkillCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelAccountCfgCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelAccountCfgCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelEmitCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelEmitCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelLeadingCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelLeadingCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelRangeCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelRangeCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelBufCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelBufCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelSummonCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelSummonCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelMoveCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelMoveCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelSwitchCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelSwitchCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelPurificationCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelPurificationCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySkillModelLinkCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifySkillModelLinkCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyPassitiveSkillCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyPassitiveSkillCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyPassitiveEffectBloodSeekCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyPassitiveEffectBloodSeekCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyPassitiveEffectReboundCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyPassitiveEffectReboundCfgList);
	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifyPassitiveEffectReliveCfgList] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyPassitiveEffectReliveCfgList);

	m_asCSMsgHandler[CSToSS::eMsgToSSFromCS_NotifySSGameSwitch] = (PFCSMsgHandler)(&CSSCfgMgr::OnMsgFromCS_NotifyGameSwitchCfg);
}

CSSCfgMgr::~CSSCfgMgr(void){
	Uninitialize();
}

INT32	CSSCfgMgr::Initialize(){
	LoadMapObjCfg();
	
	LoadMapCfg();
	LoadMapStaticData();

	LoadSSHeroBornCfg();

	LoadSSMapPathPointCfg(); 
	LoadSSMapSolderInfoCfg();
	LoadSSMapAltarSolderInfoCfg();
	LoadSSMapFunDelayCfg();
	LoadSSMapWildMonsterCfg();

	LoadSSGuideSolderCfg();	
#if 0
	LoadSSGuideDelayCfg();	
	LoadSSGuideMonsterCfg();
#endif
	LoadSSGuideHeroCfg();
	
	LoadSSGuideUserHeroCfg();
	LoadSSGuideHurtObjCfg();

	m_IfInitAll = false;
	return  eNormal;
}

INT32	CSSCfgMgr::Uninitialize(){
	m_cNPCCfgMap.clear();
	m_cHeroCfgMap.clear();	
	m_cGoodsCfgMap.clear();
	
	map<UINT32,		 SMapCfg*>::iterator iterMapCfg = m_cMapCfgMap.begin();
	while (m_cMapCfgMap.end() != iterMapCfg){
		delete iterMapCfg->second;
		iterMapCfg++;
	}
	m_cMapCfgMap.clear();
	m_cSkillCfgMap.clear();
	m_cSkillModelTypeMap.clear();
	m_cSkillModelAccountCfgMap.clear();

	mapHeroBornPosCfg.clear();
	mapWMLvDelayTimeCfg.clear();
	mapWMCDMilsecCfg.clear();
	mapAltarBornDelayNPCCfg.clear();
	mapAltarBornNPCCfg.clear();
	mapOBSystem.clear();
	mapGuideSwitch.clear();
	m_asHeroIDCfg.clear();

	m_FuncSwitch.clear();
	return eNormal;
}

const SGoodsCfg*	CSSCfgMgr::GetGoodsCfg(EObjectType eOT){
	map<EObjectType, SGoodsCfg>::iterator iterGoods = m_cGoodsCfgMap.find(eOT);
	if (m_cGoodsCfgMap.end() != iterGoods){
		return &iterGoods->second;
	}
	return NULL;
}

const SNPCCfg*	CSSCfgMgr::GetNPCCfg(EObjectType eOT){
	map<EObjectType, SNPCCfg>::iterator iterNPC = m_cNPCCfgMap.find(eOT);
	if (m_cNPCCfgMap.end() != iterNPC){
		return &iterNPC->second;
	}
	return NULL;
}

const SHeroCfg*	CSSCfgMgr::GetHeroCfg(EObjectType eOT){
	map<EObjectType, SHeroCfg>::iterator iterHero = m_cHeroCfgMap.find(eOT);
	if (m_cHeroCfgMap.end() != iterHero){
		return &iterHero->second;
	}
	return NULL;
}

const SSNewSkillCfg*	CSSCfgMgr::GetSkillCfg(UINT32 un32SkillID){
	map<UINT32, SSNewSkillCfg>::iterator iterSkill = m_cSkillCfgMap.find(un32SkillID);
	if (m_cSkillCfgMap.end() != iterSkill){
		return &iterSkill->second;
	}
	return NULL;
}

const SSkillModelAccountCfg*	CSSCfgMgr::GetSkillModelAccountCfg(UINT32 un32SkillModelID){
	map<UINT32, SSkillModelAccountCfg>::iterator iter = m_cSkillModelAccountCfgMap.find(un32SkillModelID);
	if (m_cSkillModelAccountCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

const SSkillModelLeadingCfg*	CSSCfgMgr::GetSkillModelLeadingCfg(UINT32 un32SkillModelID){
	map<UINT32, SSkillModelLeadingCfg>::iterator iter = m_cSkillModelLeadingCfgMap.find(un32SkillModelID);
	if (m_cSkillModelLeadingCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

const SSkillModelEmitCfg*	CSSCfgMgr::GetSkillModelEmitCfg(UINT32 un32SkillModelID){
	map<UINT32, SSkillModelEmitCfg>::iterator iter = m_cSkillModelEmitCfgMap.find(un32SkillModelID);
	if (m_cSkillModelEmitCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

const SSkillModelRangeCfg*	CSSCfgMgr::GetSkillModelRangeCfg(UINT32 un32SkillModelID){
	map<UINT32, SSkillModelRangeCfg>::iterator itr = m_cSkillModelRangeCfgMap.find(un32SkillModelID);
	if (m_cSkillModelRangeCfgMap.end() != itr){
		return &itr->second;
	}
	return NULL;
}

const SSkillModelBufCfg* CSSCfgMgr::GetSkillModelBuffCfg(UINT32 un32SkillModelID){
	map<UINT32, SSkillModelBufCfg>::iterator itr = m_cSkillModelBufCfgMap.find(un32SkillModelID);
	if (m_cSkillModelBufCfgMap.end() != itr){
		return &itr->second;
	}
	return NULL;
}

const SSkillModuleSummonCfg* CSSCfgMgr::GetSkillModelSummonCfg(UINT32 un32SkillModelID)
{
	map<UINT32, SSkillModuleSummonCfg>::iterator itr = m_cSkillModelSummonCfgMap.find(un32SkillModelID);
	if (m_cSkillModelSummonCfgMap.end() != itr){
		return &itr->second;
	}
	return NULL;
}

const SSkillModelMoveCfg*	CSSCfgMgr::GetSkillModelMoveCfg(UINT32 un32SkillModelID){
	map<UINT32, SSkillModelMoveCfg>::iterator iter = m_cSkillModelMoveCfgMap.find(un32SkillModelID);
	if (m_cSkillModelMoveCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

const SSkillModelSwitchCfg*	CSSCfgMgr::GetSkillModelSwitchCfg(UINT32 un32SkillModelID){
	map<UINT32, SSkillModelSwitchCfg>::iterator iter = m_cSkillModelSwitchCfgMap.find(un32SkillModelID);
	if (m_cSkillModelSwitchCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

const SSkillModelPurificationCfg*	CSSCfgMgr::GetSkillModelPurificationCfg(UINT32 un32SkillModelID){
	map<UINT32, SSkillModelPurificationCfg>::iterator iter = m_cSkillModelPurificationCfgMap.find(un32SkillModelID);
	if (m_cSkillModelPurificationCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

const SSkillModelLinkCfg*	CSSCfgMgr::GetSkillModelLinkCfg(UINT32 un32SkillModelID){
	map<UINT32, SSkillModelLinkCfg>::iterator iter = m_cSkillModelLinkCfgMap.find(un32SkillModelID);
	if (m_cSkillModelLinkCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}


const ESkillEffectType	CSSCfgMgr::GetSkillModelType(UINT32 un32SkillModelID){
	map<UINT32, ESkillEffectType>::iterator iter = m_cSkillModelTypeMap.find(un32SkillModelID);
	if (m_cSkillModelTypeMap.end() != iter){
		return iter->second;
	}
	return eSkillEffectType_None;
}

const SSPassiveSkilllCfg*	CSSCfgMgr::GetPassitiveSkillCfg(UINT32 un32PassitiveSkillID){
	map<UINT32, SSPassiveSkilllCfg>::iterator iter = m_cPassitiveSkillCfgMap.find(un32PassitiveSkillID);
	if (m_cPassitiveSkillCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

const EPassiveSkillType	CSSCfgMgr::GetPassitiveSkillType(UINT32 un32PassitiveEffectID){
	map<UINT32, EPassiveSkillType>::iterator iter = m_cPassitiveTypeMap.find(un32PassitiveEffectID);
	if (m_cPassitiveTypeMap.end() != iter){
		return iter->second;
	}
	return EPassiveSkillType_None;
}

const SSPassiveEffectCfg_BloodSeek*	CSSCfgMgr::GetPassitiveEffectBloodSeekCfg(UINT32 un32PassitiveSkillID){
	map<UINT32, SSPassiveEffectCfg_BloodSeek>::iterator iter = m_cPassitiveEffectBloodSeekCfgMap.find(un32PassitiveSkillID);
	if (m_cPassitiveEffectBloodSeekCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

const SSPassiveEffectCfg_Rebound*	CSSCfgMgr::GetPassitiveEffectReboundCfg(UINT32 un32PassitiveSkillID){
	map<UINT32, SSPassiveEffectCfg_Rebound>::iterator iter = m_cPassitiveEffectReboundCfgMap.find(un32PassitiveSkillID);
	if (m_cPassitiveEffectReboundCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

const SSPassiveEffectCfg_Relive*	CSSCfgMgr::GetPassitiveEffectReliveCfg(UINT32 un32PassitiveSkillID){
	map<UINT32, SSPassiveEffectCfg_Relive>::iterator iter = m_cPassitiveEffectReliveCfgMap.find(un32PassitiveSkillID);
	if (m_cPassitiveEffectReliveCfgMap.end() != iter){
		return &iter->second;
	}
	return NULL;
}

INT32	CSSCfgMgr::AddSkillCfg(const SSNewSkillCfg &crsSkillCfg){
	m_cSkillCfgMap[crsSkillCfg.un32SkillID] = crsSkillCfg;

	const UINT32 un32LevelSkillID = GetLevelSkillIDFromOriSkillAndLevel(crsSkillCfg.un32SkillID, crsSkillCfg.n32UpgradeLevel);
	m_cpSkillLevelMap[un32LevelSkillID] = &m_cSkillCfgMap[crsSkillCfg.un32SkillID];
	return eNormal;
}

INT32	CSSCfgMgr::AddSkillModelAccountCfg(const SSkillModelAccountCfg &crsSkillModelCfg){
	m_cSkillModelAccountCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Caculate;
	return eNormal;
}

INT32	CSSCfgMgr::AddSkillModelLeadingCfg(const SSkillModelLeadingCfg &crsSkillModelCfg){
	m_cSkillModelLeadingCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Leading;
	return eNormal;
}

INT32	CSSCfgMgr::AddSkillModelEmitCfg(const SSkillModelEmitCfg &crsSkillModelCfg){
	m_cSkillModelEmitCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Emit;
	return eNormal;
}

INT32	CSSCfgMgr::AddNPCCfg(const SNPCCfg &crsNPCCfg){
	m_cNPCCfgMap[crsNPCCfg.eOT] = crsNPCCfg;
	return eNormal;
}

INT32	CSSCfgMgr::AddHeroCfg(const SHeroCfg &crsHeroCfg){
	m_cHeroCfgMap[crsHeroCfg.eOT] = crsHeroCfg;
	if(crsHeroCfg.eOT >= 10000 && crsHeroCfg.eOT < 15000){
		m_robotChooseHeroVec.push_back(crsHeroCfg.eOT);
	}
	return eNormal;
}

INT32 CSSCfgMgr::AddGoodsCfg(const SGoodsCfg &crsGoodsCfg){
	m_cGoodsCfgMap[crsGoodsCfg.eOT] = crsGoodsCfg;
	return eNormal;
}

INT32 CSSCfgMgr::AddShopCfg(const SShopCfg &crsShopCfg)
{
	m_cShopMap[crsShopCfg.eOT] = crsShopCfg;
	return eNormal;
}

INT32 CSSCfgMgr::AddRobotAICfg(const SRobotAICfg &crsRobotAICfg)
{
	m_cRobotAIVec.push_back(crsRobotAICfg);
	return eNormal;
}

INT32 CSSCfgMgr::AddRobotAIPathCfg(const SRobotAIPathCfg &crsRobotAIPathCfg)
{
	//首先根据mapID来获取该map的配置
	auto robotMapIter = m_cRobotAIPathMap.find(crsRobotAIPathCfg.n32MapID);
	if(robotMapIter == m_cRobotAIPathMap.end()){
		SRobotMapPath robotMapPath;
		robotMapPath.mapID = crsRobotAIPathCfg.n32MapID;
		m_cRobotAIPathMap[crsRobotAIPathCfg.n32MapID] = robotMapPath;
		robotMapIter = m_cRobotAIPathMap.find(crsRobotAIPathCfg.n32MapID);
	}
	SRobotMapPath* pRobotMapPath = &robotMapIter->second;
	//根据分组(阵营)获取地图下该组的信息
	auto robotGrpIter = pRobotMapPath->grpPathMap.find(crsRobotAIPathCfg.n32CampID);
	if(robotGrpIter == pRobotMapPath->grpPathMap.end()){
		SRobotGrpPath robotGrpPath;
		robotGrpPath.grpID = crsRobotAIPathCfg.n32CampID;
		pRobotMapPath->grpPathMap[crsRobotAIPathCfg.n32CampID] = robotGrpPath;
		robotGrpIter = pRobotMapPath->grpPathMap.find(crsRobotAIPathCfg.n32CampID);
	}
	SRobotGrpPath* pRobotGrpPath = &robotGrpIter->second;
	//根据路径ID获取其中的某一条路
	auto robotPathIter = pRobotGrpPath->pathMap.find(crsRobotAIPathCfg.n32PahtID);
	if(robotPathIter == pRobotGrpPath->pathMap.end()){
		SRobotPath robotPath;
		robotPath.pathID = crsRobotAIPathCfg.n32PahtID;
		pRobotGrpPath->pathMap[crsRobotAIPathCfg.n32PahtID] = robotPath;
		robotPathIter = pRobotGrpPath->pathMap.find(crsRobotAIPathCfg.n32PahtID);
	}
	SRobotPath* pRobotPath = &robotPathIter->second;
	//添加路点
	CVector3D pos(crsRobotAIPathCfg.fX,crsRobotAIPathCfg.fY,crsRobotAIPathCfg.fZ);
	pRobotPath->path.push_back(pos);
	return eNormal;
}

const SShopCfg* CSSCfgMgr::GetShopCfg(EObjectType eOT)
{
	map<EObjectType, SShopCfg>::iterator iterShop = m_cShopMap.find(eOT);
	if (m_cShopMap.end() != iterShop){
		return &iterShop->second;
	}
	return NULL;
}

const SMapCfg*	CSSCfgMgr::GetMapCfg(UINT32 un32MapID){
	map<UINT32,	SMapCfg*>::iterator iterMap = m_cMapCfgMap.find(un32MapID);
	if (m_cMapCfgMap.end() == iterMap){
		return NULL;
	}
	return iterMap->second;
}

bool	CSSCfgMgr::IfNPCCanControlByType(EObjectType et)
{
	map<EObjectType, SNPCCfg>::const_iterator itr =	m_cNPCCfgMap.find(et);
	if ((m_cNPCCfgMap.end() != itr) && itr->second.bIfCanControl){
		return true;
	}
	return false;
}

bool		CSSCfgMgr::IfNPCSameGroupByType(EObjectType et , EWMGroup		egroup)
{
	map<EObjectType, SNPCCfg>::const_iterator itr =	m_cNPCCfgMap.find(et);
	if ((m_cNPCCfgMap.end() != itr) && (itr->second.eWMGroupCate == egroup)){
		return true;
	}
	return false;
}

INT32	CSSCfgMgr::AddSkillModelRangeCfg(const SSkillModelRangeCfg &crsSkillModelCfg)
{
	m_cSkillModelRangeCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Range;
 
	return eNormal;
}

INT32		CSSCfgMgr::AddSkillModelBufgCfg(const SSkillModelBufCfg &crsSkillModelCfg)
{
	m_cSkillModelBufCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Buf;

	return eNormal;
}

INT32		CSSCfgMgr::AddSkillModelSummongCfg(const SSkillModuleSummonCfg &crsSkillModelCfg)
{
	m_cSkillModelSummonCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Summon;

	return eNormal;
}

INT32	CSSCfgMgr::AddSkillModelMoveCfg(const SSkillModelMoveCfg &crsSkillModelCfg)
{
	m_cSkillModelMoveCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Move;

	return eNormal;
}

INT32	CSSCfgMgr::AddSkillModelSwitchCfg(const SSkillModelSwitchCfg &crsSkillModelCfg)
{
	m_cSkillModelSwitchCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Switch;

	return eNormal;
}

INT32	CSSCfgMgr::AddSkillModelPurificationCfg(const SSkillModelPurificationCfg &crsSkillModelCfg)
{
	m_cSkillModelPurificationCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Purification;

	return eNormal;
}

INT32	CSSCfgMgr::AddSkillModelLinkCfg(const SSkillModelLinkCfg &crsSkillModelCfg)
{
	m_cSkillModelLinkCfgMap[crsSkillModelCfg.un32SkillModelID] = crsSkillModelCfg;
	m_cSkillModelTypeMap[crsSkillModelCfg.un32SkillModelID] = eSkillEffectType_Link;

	return eNormal;
}

INT32	CSSCfgMgr::AddPassitiveSkillCfg(const SSPassiveSkilllCfg &crsSkillCfg)
{
	m_cPassitiveSkillCfgMap[crsSkillCfg.un32PassiveSkillID] = crsSkillCfg;
	return eNormal;
}

INT32	CSSCfgMgr::AddPassitiveEffectBloodSeekCfg(const SSPassiveEffectCfg_BloodSeek &crsSkillCfg)
{
	m_cPassitiveEffectBloodSeekCfgMap[crsSkillCfg.n32PassitiveEffectID] = crsSkillCfg;
	m_cPassitiveTypeMap[crsSkillCfg.n32PassitiveEffectID] = EPassiveSkillType_BloodSeek;
	return eNormal;
}

INT32	CSSCfgMgr::AddPassitiveEffectReboundCfg(const SSPassiveEffectCfg_Rebound &crsSkillCfg)
{
	m_cPassitiveEffectReboundCfgMap[crsSkillCfg.n32PassitiveEffectID] = crsSkillCfg;
	m_cPassitiveTypeMap[crsSkillCfg.n32PassitiveEffectID] = EPassiveSkillType_Rebound;
	return eNormal;
}

INT32	CSSCfgMgr::AddPassitiveEffectReliveCfg(const SSPassiveEffectCfg_Relive &crsSkillCfg)
{
	m_cPassitiveEffectReliveCfgMap[crsSkillCfg.n32PassitiveEffectID] = crsSkillCfg;
	m_cPassitiveTypeMap[crsSkillCfg.n32PassitiveEffectID] = EPassiveSkillType_Relive;
	return eNormal;
}

UINT32 CSSCfgMgr::GetLevelSkillIDFromOriSkillAndLevel(UINT32 un32OriSkillID, INT32 n32Level){
	INT32 n32Temp = un32OriSkillID / 10.0f;
	return n32Temp * 10 + n32Level;
}

const INT32	c_n32MaxSkillUpgradeLevel = 3;
const SSNewSkillCfg* CSSCfgMgr::GetSkillFromOriSkillAndNewLevel(UINT32 un32OriSkillID, INT32 n32NewLevel){
	for (INT32 i = c_n32MaxSkillUpgradeLevel; i >= 0; --i){
		UINT32 un32SkillID = GetLevelSkillIDFromOriSkillAndLevel(un32OriSkillID, i);
		map<UINT32, SSNewSkillCfg>::iterator iter = m_cSkillCfgMap.find(un32SkillID);
		if (m_cSkillCfgMap.end() != iter){
			SSNewSkillCfg& pNewSkillCfg =  iter->second;
			if (pNewSkillCfg.n32UpgradeLevel <= n32NewLevel){
				return &pNewSkillCfg;
			}
		}
	}

	return NULL;
} 
//
//void CSSCfgMgr::addHeroBornPos(UINT64 mapId, INT32 seatPos, const CVector3D & bornPos)
//{
//	ELOG(LOG_DBBUG, "mapid:%d %d", mapId, seatPos);
//	auto itr =	mapHeroBornPosCfg.find(mapId); 
//	if ( itr != mapHeroBornPosCfg.end())
//	{
//		std::map<INT32/*seatPos*/,CVector3D> & cMapSeatPosCfg = itr->second;
//		auto  itrSeatPos = cMapSeatPosCfg.find(seatPos);
//		if (itrSeatPos == cMapSeatPosCfg.end())
//		{
//			cMapSeatPosCfg[seatPos] = bornPos; 
//		}
//		else
//		{ 
//			ELOG(LOG_WARNNING, "already addHeroBornPos cfg! "); 
//		}
//	}else
//	{
//		std::map<INT32/*seatPos*/,CVector3D>   cMapSeatPosCfg;
//		{
//			cMapSeatPosCfg[seatPos] = bornPos; 
//		}
//		mapHeroBornPosCfg[mapId] = cMapSeatPosCfg;
//	}	
//}



void CSSCfgMgr::addVMNPCBornDelayTimeCfg(UINT64 mapId, EWMLevel elv,INT32 timeCfg)
{
	auto itr = mapWMLvDelayTimeCfg.find(mapId);
	if (itr != mapWMLvDelayTimeCfg.end())
	{
		std::map<EWMLevel,INT64> & childMap = itr->second;
		if (childMap.find(elv) == childMap.end())
		{
			childMap[elv] = timeCfg;
		} 
		else
		{ 
			//ELOG(LOG_ERROR,"samething addVMNPCBornDelayTimeCfg cfg! "); 
		}
	}else
	{
		std::map<EWMLevel,INT64> childMap;
		{
			childMap[elv] = timeCfg;
		}
		mapWMLvDelayTimeCfg[mapId] = childMap;
	}
}

 void  CSSCfgMgr::getVMNPCBornDelayTimeCfg(UINT64 mapId, map<EWMLevel,INT64> & v)
{ 
	auto itr = mapWMLvDelayTimeCfg.find(mapId);
	if (itr != mapWMLvDelayTimeCfg.end())
	{
		v = itr->second;  
	}
	 
}

void CSSCfgMgr::addWMCDMilsecCfg(UINT64 mapId, INT32 nPoint, const SWMRefrshCfg & vcfg)
{
	auto itr = mapWMCDMilsecCfg.find(mapId);
	if (itr != mapWMCDMilsecCfg.end())
	{
		map<INT32, SWMRefrshCfg> & childMap = itr->second;
		if (childMap.find(nPoint) == childMap.end())
		{
			childMap[nPoint] = vcfg;
		}
		else
		{ 
			//ELOG(LOG_ERROR,"samething addVMNPCBornDelayTimeCfg cfg! "); 
		}
	}else
	{
		map<INT32, SWMRefrshCfg>   childMap;
		{
			childMap[nPoint] = vcfg;
		}
		mapWMCDMilsecCfg[mapId] = childMap;
	}
}
void CSSCfgMgr::getWMCDMilsecCfg(UINT64 mapId  ,map<INT32, SWMRefrshCfg> & v)
{ 
	auto itr = mapWMCDMilsecCfg.find(mapId);
	if (itr != mapWMCDMilsecCfg.end())
	{
		v = itr->second; 
	}
 
}

void CSSCfgMgr::addAltarBornNPCCfg(UINT64 mapId,INT32 altarIdx ,INT32 tSpace,INT32 delayTime, INT32 num)
{
	auto itr = mapAltarBornNPCCfg.find(mapId);
	if (itr != mapAltarBornNPCCfg.end() )
	{
		std::map<UINT32, tagBornSolderCfg> &childeMap = itr->second;
		if ( childeMap.find(altarIdx) == childeMap.end())
		{
			tagBornSolderCfg tb(tSpace,num);
			childeMap[altarIdx] = tb;
		}else
		{
			//ELOG(LOG_ERROR,"error addAltarBornNPCCfg-->mapAltarBornNPCCfg cfg!");
		}
	}else
	{
		std::map<UINT32, tagBornSolderCfg> child;
		{
			tagBornSolderCfg tb(tSpace,num);
			child[altarIdx] = tb;
		}
		mapAltarBornNPCCfg[mapId] = child;
	}

	auto itr_ = mapAltarBornDelayNPCCfg.find(mapId);
	if(itr_ != mapAltarBornDelayNPCCfg.end())
	{
		std::map<UINT32, UINT32> &childMap = itr_->second;
		if (childMap.find(altarIdx) == childMap.end())
		{
			childMap[altarIdx] = delayTime;
		} 
		else
		{
			//ELOG(LOG_ERROR,"error addAltarBornNPCCfg-->mapAltarBornDelayNPCCfg cfg!");
		}
	} else
	{
		std::map<UINT32, UINT32> child;
		{
			child[altarIdx] = delayTime;
		}
		mapAltarBornDelayNPCCfg[mapId] = child;
	}
}
void CSSCfgMgr::getAltarBornNPCCfg(UINT64 mapId, std::map<UINT32, tagBornSolderCfg> & vSolder)
{ 
	auto itr = mapAltarBornNPCCfg.find(mapId);
	if (itr != mapAltarBornNPCCfg.end() )
	{
		vSolder = itr->second; 
	} 
}
void  CSSCfgMgr::getAltarBornNPCDelayCfg(UINT64 mapId , std::map<UINT32, UINT32> &  adelayCfg)
{ 
	auto itr_ = mapAltarBornDelayNPCCfg.find(mapId);
	if (itr_ != mapAltarBornDelayNPCCfg.end())
	{
		adelayCfg = itr_->second; 
	} 
}
 
//////////////////////////////////////////////////////////////////////////
//OB    cfg
//////////////////////////////////////////////////////////////////////////
void CSSCfgMgr::addOBSystemSwitchCfg(UINT64 mapId, bool b)
{
	if (mapId < 0)
	{
		return;
	}
	auto itr = mapOBSystem.find(mapId);
	if (itr == mapOBSystem.end())
	{
		mapOBSystem[mapId] = b;
	}else
	{
		//ELOG(LOG_ERROR,"error ob cfg-->addOBSystemSwitchCfg!");
	}
}
bool CSSCfgMgr::getOBSystemSwitchCfgByMapId(UINT64 mapId)
{
	if (mapId < 0)
	{
		return false;
	}
	auto itr = mapOBSystem.find(mapId);
	if (itr != mapOBSystem.end())
	{
		return itr->second;
	}
	return  false;
}


//////////////////////////////////////////////////////////////////////////
//guide  function  cfg
//////////////////////////////////////////////////////////////////////////
void CSSCfgMgr::addGuideHeroIdCfg(UINT32  idx)
{
	m_asHeroIDCfg.push_back(idx);
}
INT32 CSSCfgMgr::getGuideCfgHeroId()
{
	if (m_asHeroIDCfg.empty()){
		return 0;
	}
	INT32 Idex = CFunction::GetGameRandomN(m_Generator, 0, m_asHeroIDCfg.size() - 1); 
	if (Idex < 0)
	{
		Idex = 0;
	}
	return m_asHeroIDCfg[Idex];	
}

void CSSCfgMgr::addMapGuideSwitch(UINT64 mapId, bool b)
{
	auto itr =	mapGuideSwitch.find(mapId);
	if (itr == mapGuideSwitch.end())
	{
		mapGuideSwitch[mapId] = b;
	}else
	{
		//ELOG(LOG_ERROR,"error ob cfg-->addMapGuideSwitch(%llu,%d)!",mapId,(INT32)b);
	}
}

bool	CSSCfgMgr::getMapGuideSwitch(UINT64 mapId )
{
	auto itr = mapGuideSwitch.find(mapId);
	if (itr != mapGuideSwitch.end() )
	{
		return itr->second;
	}
	return false;
}

 

void	CSSCfgMgr::addNPCBornDelayCfg(UINT64 mapId, INT32 dealyTime)
{
	if(mapNPCBornDelayTime.find(mapId) == mapNPCBornDelayTime.end())
	{
		mapNPCBornDelayTime[mapId] = dealyTime;
	}
}

INT32	CSSCfgMgr::getNPCBornDelayCfg(UINT64 mapId )
{
	auto itr = mapNPCBornDelayTime.find(mapId);
	if(itr != mapNPCBornDelayTime.end())
	{
		return  itr->second;
	}
	return 0;
}

INT32 CSSCfgMgr::LoadMapCfg(){
	string strExtFilename;
	string strBaseName;
	string strFullName;
	string strLuaFileName;

	try{
		boost::filesystem::directory_iterator pathdirectoryiter(std::string(m_strExeDir + "CSBattleMgr/Map").c_str());
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
				strLuaFileName = strFullName;
				/*boost::replace_last(strLuaFileName, ".map", ".lua");
				if (!boost::filesystem::exists(strLuaFileName)){
					ELOG(LOG_ERROR, "map file %s, but can not find %s.", strFullName.c_str(), strLuaFileName.c_str());
					continue;
				}*/
				LoadMapData(un32MapID, strFullName, strLuaFileName);
			}

			++ pathdirectoryiter;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " load Map data file fail.");
		return eEC_OpenCfgFileFail;
	}

	ELOG(LOG_DBBUG, " load %d map.", (UINT32)m_cMapCfgMap.size());

	return eNormal;
}

INT32 CSSCfgMgr::LoadMapData(UINT32 un32MapID, string &strMapDataFileName, string &strLuaFileName){
	FILE *pcMapDataStream = NULL;
	pcMapDataStream = fopen(strMapDataFileName.c_str(),"rb");
	if (NULL == pcMapDataStream){
		ELOG(LOG_ERROR, "can not open file: %s.",strMapDataFileName.c_str());
		return eEC_OpenCfgFileFail;
	}
	fseek(pcMapDataStream,0,SEEK_END);
	INT32 n32MapDataLen = ftell(pcMapDataStream);

	vector<SMapObjCfg*>	cTempMapObjCfgVec;

	map<UINT32, SMapObjCfg>::iterator iterMapObj = m_cMapObjCfgMap.begin();
	while (m_cMapObjCfgMap.end() != iterMapObj){
		if (iterMapObj->second.un32MapID == un32MapID){
			cTempMapObjCfgVec.push_back(&iterMapObj->second);
		}
		iterMapObj++;
	}

	UINT32 un32TotalSize = (UINT32)(n32MapDataLen + sizeof(SMapCfg) +cTempMapObjCfgVec.size() * sizeof(SMapObjCfg) + 10);
	BYTE *pbBuff = new BYTE[un32TotalSize];
	memset(pbBuff, 0, un32TotalSize);
	BYTE *pbDataPos = pbBuff;
	SMapCfg *psMapCfg = (SMapCfg*)pbBuff;	
	SMapDataHeader *psMapHeader = (SMapDataHeader*)pbDataPos;
	pbDataPos += sizeof(SMapCfg);
	psMapCfg->un32MapID = un32MapID;
	psMapCfg->un32MapCfgSize = un32TotalSize;
	//read map data.
	psMapCfg->un32HeaderDataOffset = (UINT32)(pbDataPos - pbBuff);
	fseek(pcMapDataStream, 0, SEEK_SET);
	fread(pbDataPos, n32MapDataLen, 1, pcMapDataStream);
	pbDataPos += n32MapDataLen;
	fclose(pcMapDataStream);

	psMapCfg->un32MapObjNum = (UINT32)cTempMapObjCfgVec.size();
	psMapCfg->un32MapObjOffset = (UINT32)(pbDataPos - pbBuff);
	for (UINT32 i = 0; i < (UINT32)cTempMapObjCfgVec.size(); i++){
		memcpy(pbDataPos, cTempMapObjCfgVec[i], sizeof(SMapObjCfg));
		pbDataPos += sizeof(SMapObjCfg);
	}

	psMapCfg->psMapHeader = (SMapDataHeader*)(pbBuff + psMapCfg->un32HeaderDataOffset);
	if (0 < psMapCfg->un32MapObjNum){
		psMapCfg->psMapObjCfg = (SMapObjCfg *)(pbBuff + psMapCfg->un32MapObjOffset);
	}

	//psMapCfg->luaFileStr = strLuaFileName;

	m_cMapCfgMap[un32MapID] = psMapCfg;
	return eNormal;
}

INT32 CSSCfgMgr::LoadMapObjCfg(){
	std::string cpszfilename = m_strExeDir + "CSBattleMgr/MapObjCfg.xml";
	ptree cPtree;
	ptree cHeroCfg;
	try{
		read_xml(cpszfilename.c_str(), cPtree, xml_parser::trim_whitespace);
		cHeroCfg = cPtree.get_child("MapObjCfg");
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " open cfg file fail.");
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

			SMapObjCfg sTempCfg;
			memset(&sTempCfg, 0, sizeof(sTempCfg));
			sTempCfg.un32ID = (EObjectType)iterInfo->second.get<INT32>("<xmlattr>.un32ID", 0);
			if (0 == sTempCfg.un32ID){
				continue;
			}
			sTempCfg.un32MapID = iterInfo->second.get<UINT32>("un32MapID", 0);
			sTempCfg.eObjTypeID = (EObjectType)iterInfo->second.get<UINT32>("eObjectTypeID", 0);
			sTempCfg.un32ObjIdx = iterInfo->second.get<UINT32>("un32ObjIdx", 0);
			sTempCfg.n32CampID = iterInfo->second.get<INT32>("n32Camp", 0);
			cTempStr = iterInfo->second.get<string>("cBornPos", "0,0,0");
			FLOAT afPos[3];
			CFunction::SplitFloat(cTempStr.c_str(), afPos, 3);
			sTempCfg.cBornPos.m_fX = afPos[0];
			sTempCfg.cBornPos.m_fY = afPos[1];
			sTempCfg.cBornPos.m_fZ = afPos[2];
			cTempStr = iterInfo->second.get<string>("cBornDir", "0,0,0");
			CFunction::SplitFloat(cTempStr.c_str(), afPos, 3);
			sTempCfg.cBorDir.m_fX = afPos[0];
			sTempCfg.cBorDir.m_fY = afPos[1];
			sTempCfg.cBorDir.m_fZ = afPos[2];

			sTempCfg.n32Visibility = iterInfo->second.get<INT32>("n32Visibility", 0);
			sTempCfg.eAttackMode = (EAttackMode)iterInfo->second.get<INT32>("eAttackMode", 0);

			m_cMapObjCfgMap[sTempCfg.un32ID] = sTempCfg;
		}
	}catch(std::exception &cException){
		ELOG(LOG_ERROR, " read cfg file fail.");
		return eEC_ReadCfgFileFail;
	}

	ELOG(LOG_DBBUG, " Load %d map obj cfg.", (UINT32)m_cMapObjCfgMap.size());
	return eNormal;
}

CSSCfgMgr& CSSCfgMgr::GetSSCfgInstance(){
	static CSSCfgMgr lCSSCfgMgr;
	return lCSSCfgMgr;
}

INT32 CSSCfgMgr::LoadMapStaticData(){
	for (map<UINT32, SMapCfg*>::iterator iter = m_cMapCfgMap.begin(); iter != m_cMapCfgMap.end(); ++iter){
		SMapCfg* m_cpsMapCfg = iter->second;
		if (NULL != GetMapStaticDataByMapID(iter->first)){
			continue;
		}
		MapStaticData lMapStaticData;
		//caculate area information.
		lMapStaticData.m_n32XAreaNum = m_cpsMapCfg->psMapHeader->n32XCellNum / c_n32AreaCellNum;
		if (0 < m_cpsMapCfg->psMapHeader->n32XCellNum % c_n32AreaCellNum){
			lMapStaticData.m_n32XAreaNum++;
		}
		lMapStaticData.m_n32ZAreaNum = m_cpsMapCfg->psMapHeader->n32ZCellNum / c_n32AreaCellNum;
		if (0 < m_cpsMapCfg->psMapHeader->n32ZCellNum % c_n32AreaCellNum){
			lMapStaticData.m_n32ZAreaNum++;
		}
		lMapStaticData.m_n32TotalAreaNum = lMapStaticData.m_n32XAreaNum * lMapStaticData.m_n32ZAreaNum;
		//init region list.
		lMapStaticData.m_n32XRegionNum = m_cpsMapCfg->psMapHeader->n32XCellNum;
		lMapStaticData.m_n32ZRegionNum = m_cpsMapCfg->psMapHeader->n32ZCellNum;
		lMapStaticData.m_n32TotalRegionNum = lMapStaticData.m_n32XRegionNum * lMapStaticData.m_n32ZRegionNum;

		lMapStaticData.m_pcRegionList = new SBattleRegion[lMapStaticData.m_n32TotalRegionNum];
		BYTE *pbBlockList = (BYTE*)(m_cpsMapCfg->psMapHeader + 1);
		for (INT32 i = 0; i < lMapStaticData.m_n32ZRegionNum; i++){
			for (INT32 j = 0; j < lMapStaticData.m_n32XRegionNum; j++){
				INT32 n32Idx = j + i * lMapStaticData.m_n32XRegionNum;
				SBattleRegion &rsRegion = lMapStaticData.m_pcRegionList[n32Idx];
				rsRegion.un16XRegionIdx = j;
				rsRegion.un16ZRegionIdx = i;
				rsRegion.bStaticBlock = pbBlockList[n32Idx];

				rsRegion.apsAroundRegion[eIntDir_Up] = GetRegionByDir(&rsRegion, eIntDir_Up, lMapStaticData);
				rsRegion.apsAroundRegion[eIntDir_UpRight] = GetRegionByDir(&rsRegion, eIntDir_UpRight, lMapStaticData);
				rsRegion.apsAroundRegion[eIntDir_Right] = GetRegionByDir(&rsRegion, eIntDir_Right, lMapStaticData);
				rsRegion.apsAroundRegion[eIntDir_BelowRight] = GetRegionByDir(&rsRegion, eIntDir_BelowRight, lMapStaticData);
				rsRegion.apsAroundRegion[eIntDir_Below] = GetRegionByDir(&rsRegion, eIntDir_Below, lMapStaticData);
				rsRegion.apsAroundRegion[eIntDir_BelowLeft] = GetRegionByDir(&rsRegion, eIntDir_BelowLeft, lMapStaticData);
				rsRegion.apsAroundRegion[eIntDir_Left] = GetRegionByDir(&rsRegion, eIntDir_Left, lMapStaticData);
				rsRegion.apsAroundRegion[eIntDir_UpLeft] = GetRegionByDir(&rsRegion, eIntDir_UpLeft, lMapStaticData);
			}
		}
		//刘路测试添加，动态创建三角面
		for(int i=0; i<lMapStaticData.m_n32XRegionNum*lMapStaticData.m_n32ZRegionNum;i++){
			SBattleRegion &rsRegion = lMapStaticData.m_pcRegionList[i];
			if(TRUE == rsRegion.bStaticBlock) continue;
			UINT8 nearStaticBlockNum = 0;
			if(rsRegion.apsAroundRegion[eIntDir_Up] != NULL && TRUE == rsRegion.apsAroundRegion[eIntDir_Up]->bStaticBlock) nearStaticBlockNum += 1;
			if(rsRegion.apsAroundRegion[eIntDir_Right] != NULL && TRUE == rsRegion.apsAroundRegion[eIntDir_Right]->bStaticBlock) nearStaticBlockNum += 2;
			if(rsRegion.apsAroundRegion[eIntDir_Below] != NULL && TRUE == rsRegion.apsAroundRegion[eIntDir_Below]->bStaticBlock) nearStaticBlockNum += 4;
			if(rsRegion.apsAroundRegion[eIntDir_Left] != NULL && TRUE == rsRegion.apsAroundRegion[eIntDir_Left]->bStaticBlock) nearStaticBlockNum += 8;
			if(nearStaticBlockNum == 3){rsRegion.bStaticTriBlock = TRUE; rsRegion.eTriBlockDir = eIntDir_BelowLeft;}
			if(nearStaticBlockNum == 6){rsRegion.bStaticTriBlock = TRUE; rsRegion.eTriBlockDir = eIntDir_UpLeft;}
			if(nearStaticBlockNum == 12){rsRegion.bStaticTriBlock = TRUE; rsRegion.eTriBlockDir = eIntDir_UpRight;}
			if(nearStaticBlockNum == 9){rsRegion.bStaticTriBlock = TRUE; rsRegion.eTriBlockDir = eIntDir_BelowRight;}
		}

		lMapStaticData.m_pbBlockList = (BYTE*)(m_cpsMapCfg->psMapHeader + 1);
		m_MapStaticDataMap[iter->first] = lMapStaticData;

		ELOG(LOG_INFO, "Add %d MapStaticData", iter->first);
	}

	return 0;
}

SBattleRegion* CSSCfgMgr::GetRegionByDir(const SBattleRegion *cpsRegion, EIntDir eDir, MapStaticData& sMapStaticData){
	if (NULL == cpsRegion){
		return NULL;
	}
	UINT16 un16CurXIdx = cpsRegion->un16XRegionIdx;
	UINT16 un16CurZIdx = cpsRegion->un16ZRegionIdx;
	if (eIntDir_Up == eDir){
		un16CurZIdx += 1;
	}else if (eIntDir_UpRight == eDir){
		un16CurXIdx += 1;
		un16CurZIdx += 1;
	}else if (eIntDir_Right == eDir){
		un16CurXIdx += 1;
	}else if (eIntDir_BelowRight == eDir){
		un16CurXIdx += 1;
		un16CurZIdx -= 1;
	}else if (eIntDir_Below == eDir){
		un16CurZIdx -= 1;
	}else if (eIntDir_BelowLeft == eDir){
		un16CurXIdx -= 1;
		un16CurZIdx -= 1;
	}else if (eIntDir_Left == eDir){
		un16CurXIdx -= 1;
	}else if (eIntDir_UpLeft == eDir){
		un16CurXIdx -= 1;
		un16CurZIdx += 1;
	}else{
		return NULL;
	}

	if (0 > un16CurXIdx || 0 > un16CurZIdx){
		return NULL;
	}
	if (un16CurXIdx >= sMapStaticData.m_n32XRegionNum || un16CurZIdx >= sMapStaticData.m_n32ZRegionNum){
		return NULL;
	}
	INT32 n32Idx = un16CurXIdx + un16CurZIdx * sMapStaticData.m_n32XRegionNum;
	return &sMapStaticData.m_pcRegionList[n32Idx];
}

MapStaticData* CSSCfgMgr::GetMapStaticDataByMapID(UINT32 un32MapID){
	MapStaticDataMap::iterator iter = m_MapStaticDataMap.find(un32MapID);
	if (m_MapStaticDataMap.end() != iter){
		return &iter->second;
	}

	return NULL;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyNPCCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyNPCCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyNPCCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SNPCCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddNPCCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyHeroCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyHeroCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyHeroCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SHeroCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddHeroCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyGoodsCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyGoodsCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyGoodsCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SGoodsCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddGoodsCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyShopCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyShopCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyShopCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SShopCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddShopCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyRobotAICfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyRobotAICfgList> pMsg = ParseProtoMsg<CSToSS::NotifyRobotAICfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SRobotAICfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddRobotAICfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyRobotAIPathCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyRobotAIPathCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyRobotAIPathCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SRobotAIPathCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddRobotAIPathCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyNewSkillCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyNewSkillCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyNewSkillCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSNewSkillCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelAccountCfgCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifySkillModelAccountCfgCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelAccountCfgCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModelAccountCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelAccountCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());  
	return eNormal; 
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelEmitCfgList(const char* pData, int n32DataLength){	
	boost::shared_ptr<CSToSS::NotifySkillModelEmitCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelEmitCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModelEmitCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelEmitCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelRangeCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifySkillModelRangeCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelRangeCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModelRangeCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelRangeCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelLeadingCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifySkillModelLeadingCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelLeadingCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModelLeadingCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelLeadingCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelBufCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifySkillModelBufCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelBufCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModelBufCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelBufgCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelSummonCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifySkillModelSummonCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelSummonCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModuleSummonCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelSummongCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelMoveCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifySkillModelMoveCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelMoveCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModelMoveCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelMoveCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelSwitchCfgList(const char* pData, int n32DataLength)
{
	boost::shared_ptr<CSToSS::NotifySkillModelSwitchCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelSwitchCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModelSwitchCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelSwitchCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelPurificationCfgList(const char* pData, int n32DataLength)
{
	boost::shared_ptr<CSToSS::NotifySkillModelPurificationCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelPurificationCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModelPurificationCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelPurificationCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifySkillModelLinkCfgList(const char* pData, int n32DataLength)
{
	boost::shared_ptr<CSToSS::NotifySkillModelLinkCfgList> pMsg = ParseProtoMsg<CSToSS::NotifySkillModelLinkCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSkillModelLinkCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddSkillModelLinkCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyPassitiveSkillCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyPassitiveSkillCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyPassitiveSkillCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSPassiveSkilllCfg sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddPassitiveSkillCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyPassitiveEffectBloodSeekCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyPassitiveEffectBloodSeekCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyPassitiveEffectBloodSeekCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSPassiveEffectCfg_BloodSeek sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddPassitiveEffectBloodSeekCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyPassitiveEffectReboundCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyPassitiveEffectReboundCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyPassitiveEffectReboundCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSPassiveEffectCfg_Rebound sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddPassitiveEffectReboundCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());
	return eNormal;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyPassitiveEffectReliveCfgList(const char* pData, int n32DataLength){
	boost::shared_ptr<CSToSS::NotifyPassitiveEffectReliveCfgList> pMsg = ParseProtoMsg<CSToSS::NotifyPassitiveEffectReliveCfgList>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}

	for (UINT32 i = 0; i < pMsg->cfg_size(); i++){
		SSPassiveEffectCfg_Relive sTempCfg;
		memcpy(&sTempCfg, pMsg->cfg(i).c_str(), pMsg->cfg(i).size());
		CSSCfgMgr::GetSSCfgInstance().AddPassitiveEffectReliveCfg(sTempCfg);
	}
	ELOG(LOG_INFO, "Load %u Cfgs.", pMsg->cfg_size());

	return eNormal;
}

void CSSCfgMgr::HandleMsg(const char* pMsg, int n32MsgLength, int n32MsgID){
	CSMsgHandlerMap::iterator iter = m_asCSMsgHandler.find(n32MsgID);
	if (m_asCSMsgHandler.end() != iter){
		return (this->*iter->second)(pMsg, n32MsgLength);
	}
	ELOG(LOG_WARNNING, "Can't handle %d", n32MsgID);
}

const SRobotGrpPath* CSSCfgMgr::GetRobotAIPath(INT32 mapID, INT32 grpID){
	//首先根据mapID来获取该map的配置
	auto robotMapIter = m_cRobotAIPathMap.find(mapID);
	if(robotMapIter == m_cRobotAIPathMap.end()){
		return NULL;
	}
	SRobotMapPath* pRobotMapPath = &robotMapIter->second;
	//根据分组(阵营)获取地图下该组的信息
	auto robotGrpIter = pRobotMapPath->grpPathMap.find(grpID);
	if(robotGrpIter == pRobotMapPath->grpPathMap.end()){
		return NULL;
	}
	return &robotGrpIter->second;
}

INT32 CSSCfgMgr::OnMsgFromCS_NotifyGameSwitchCfg(const char* pMsg, int n32MsgLength){
	boost::shared_ptr<CSToSS::SSGameFuncSwitch> pbMsg = ParseProtoMsgWithNewMemory<CSToSS::SSGameFuncSwitch>(pMsg, n32MsgLength);
	if (!pbMsg) return 0; 

	SSFuncSwitch func;
	func.logSwitch = pbMsg->log_witch() > 0 ? true : false;
	func.gmSwitch = pbMsg->gm_witch() > 0 ? true : false;
	func.solderSwitch = pbMsg->solderwitch() > 0 ? true : false;
	func.monsterSwitch = pbMsg->monsterwitch()>0 ? true : false;
	func.altarSwitch = pbMsg->altarsolderwitch() >0? true : false;
	func.logRefreshTime = pbMsg->logrefshtime();
	SetSSFuncSwitch(func);
	ELOG(LOG_INFO,"--set this ss gamelog switch .....ok!--");
	return eNormal;
}
}