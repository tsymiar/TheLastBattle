#include "stdafx.h"
#include "SSUserCfgMgr.h"

#include "SSLoggingHeader.h"
Begin_JT_FBAll_SceneServer_SSUserMgr

CSSUserCfgMgr g_cCSSUserCfgMgr;

CSSUserCfgMgr::CSSUserCfgMgr()
{

}

CSSUserCfgMgr::~CSSUserCfgMgr()
{

}

INT32 CSSUserCfgMgr::Initialize()
{
	return eNormal;
}

INT32 CSSUserCfgMgr::Uninitialize()
{
	//m_cRunesCfgMap.clear();
	//m_cRunesComposeInfoMap.clear();
	return eNormal;
}

//const SRunesCfg* CSSUserCfgMgr::GetRunesCfg(EObjectType eOT) const
//{
//	map<EObjectType, SRunesCfg>::const_iterator iter = m_cRunesCfgMap.find(eOT);
//	if (m_cRunesCfgMap.end() != iter)
//	{
//		return &iter->second;
//	}
//
//	return NULL;
//}
//
//INT32 CSSUserCfgMgr::AddRunesCfg(const SRunesCfg& sRunesCfg)
//{
//	m_cRunesCfgMap[sRunesCfg.eOT] = sRunesCfg;
//
//	if (FALSE == sRunesCfg.bIsCanComposed)
//	{
//		return eNormal;
//	}
//
//	RunesComposeInfoMap::iterator iter = m_cRunesComposeInfoMap.find(sRunesCfg.un8Level);
//	if (m_cRunesComposeInfoMap.end() == iter)
//	{
//		SRunesComposeInfo sRunesComposeInfo;
//		sRunesComposeInfo.eOTMin = sRunesCfg.eOT;
//		sRunesComposeInfo.eOTMax = sRunesCfg.eOT;
//	}
//	else
//	{
//		if (iter->second.eOTMin > sRunesCfg.eOT)
//		{
//			iter->second.eOTMin = sRunesCfg.eOT;
//		}
//		else if (iter->second.eOTMax < sRunesCfg.eOT)
//		{
//			iter->second.eOTMax = sRunesCfg.eOT;
//		}
//	}
//
//	return eNormal;
//}
//
//const SRunesComposeInfo* CSSUserCfgMgr::GetRunesComposeInfo(UINT8 un8Level) const
//{
//	RunesComposeInfoMap::const_iterator iter = m_cRunesComposeInfoMap.find(un8Level);
//	if (m_cRunesComposeInfoMap.end() != iter)
//	{
//		return &iter->second;
//	}
//
//	return NULL;
//}
//
//INT32 CSSUserCfgMgr::AddRunesSlotCfg(const SRunesSlotCfg& sRunesSlotCfg)
//{
//	m_cRunesSlotCfgMap[sRunesSlotCfg.eOT] = sRunesSlotCfg;
//
//	return eNormal;
//}

End_JT_FBAll_SceneServer_SSUserMgr