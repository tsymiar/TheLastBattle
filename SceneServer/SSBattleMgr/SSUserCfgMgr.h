#pragma once
#include "stdafx.h"
#include <unordered_map>

namespace SceneServer{ 

class CSSUserCfgMgr
{
	friend class CSSBattleMgr;
public:
	CSSUserCfgMgr();
	~CSSUserCfgMgr();

	INT32						Initialize();
	INT32						Uninitialize();

//	const SRunesCfg*			GetRunesCfg(EObjectType eOT) const;
//	const SRunesComposeInfo*	GetRunesComposeInfo(UINT8 un8Level) const;
//
//private:
//	INT32						AddRunesCfg(const SRunesCfg& sRunesCfg);
//	INT32						AddRunesSlotCfg(const SRunesSlotCfg& sRunesSlotCfg);
//
//private:
//	map<EObjectType, SRunesCfg>						m_cRunesCfgMap;
//	typedef std::unordered_map<UINT8, SRunesComposeInfo> RunesComposeInfoMap;
//	RunesComposeInfoMap								m_cRunesComposeInfoMap;
//	map<EObjectType, SRunesSlotCfg>					m_cRunesSlotCfgMap;
};

}
