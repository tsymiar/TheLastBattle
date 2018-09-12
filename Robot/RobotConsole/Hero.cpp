#include "StdAfx.h"
#include "Hero.h"
#include "CfgMgr.h"
#include "Robot.h"
#include "RobotMgr.h"

namespace ReBot
{
	CRebotHero::CRebotHero():m_un32Level(0)
		, m_n32Fury(0)
		, m_IsLocalPlayer(FALSE)
		, m_n32Exp(0)
		, m_n32MonsterID1(0)
		, m_n32MonsterID2(0){
			for (INT32 i = 0; i < n32MaxRobotPlayerSkillNum; ++i){
				m_asSkilLRunTimeData[i].Clear();
			}

			for (INT32 i = 0; i < c_un32MaxGridNum; ++i){
				m_asGoodsRunTimeData[i].Clear();
			}
			m_bIfAbsorb = FALSE;
	}


	CRebotHero::~CRebotHero(void){
	}

	INT32 CRebotHero::Clear(){
		m_un32Level = 0;
		m_n32Fury = 0;
		m_IsLocalPlayer = FALSE;
		m_n32Exp = 0;
		return eNormal;
	}

	void CRebotHero::UpdateGoods(UINT8 un8Pos, INT32 n32Num, UINT32 un32TypeID, INT32 n32State, UINT32 un32StateTime){
		SGoodsRunTimeData& sGoodsData = m_asGoodsRunTimeData[un8Pos];
		sGoodsData.sGoodsCfg = CCfgMgr::getInstance().GetGoodsCfg(un32TypeID);
		sGoodsData.sGoodsState = (eGoodsState)n32State;
		sGoodsData.tStateBeginMilsec = un32StateTime;
	}

	void CRebotHero::UpdateAbsorb(UINT32 un32ID1, UINT32 un32ID2){
		SNPCCfg* pNPCCfg1 = CCfgMgr::getInstance().GetNPCCfg(un32ID1);
		if (NULL != pNPCCfg1){
			RobotSkillData& sAbsordData1 = m_asSkilLRunTimeData[n32AbsorbSkillID1];
			sAbsordData1.cpsCfg = CCfgMgr::getInstance().GetSkillCfg(pNPCCfg1->aun32SkillList[2]);
			
		}

		SNPCCfg* pNPCCfg2 = CCfgMgr::getInstance().GetNPCCfg(un32ID2);
		if (NULL != pNPCCfg2){
			RobotSkillData& sAbsordData2 = m_asSkilLRunTimeData[n32AbsorbSkillID2];
			sAbsordData2.cpsCfg = CCfgMgr::getInstance().GetSkillCfg(pNPCCfg2->aun32SkillList[2]);
		}
	}

	INT32 CRebotHero::LoadHeroCfg(const SHeroCfg* pcCfg){
		m_cpsHeroCfg = pcCfg;

		for (INT32 i = 0; i < c_n32MaxHeroSkillNum; ++i){
			m_asSkilLRunTimeData[i].Clear();
		}

		for (INT32 i = 0; i < c_n32MaxHeroSkillNum; i++){
			UINT32 un32SkillID = m_cpsHeroCfg->aun32SkillList[i];
			if (0 == un32SkillID){
				continue;
			}
			const SSNewSkillCfg *cpsCfg = CCfgMgr::getInstance().GetSkillCfg(un32SkillID);
			if (NULL == cpsCfg){
				ELOG(LOG_WARNNING, " un32SkillID %d cano not find Cfg info.", un32SkillID);
				continue;
			}
			RobotSkillData& cSkill = m_asSkilLRunTimeData[i];
			cSkill.cpsCfg = cpsCfg;
			cSkill.eSkillState = eSkillState_Free;
			cSkill.tCooldownMilsec = 0;
		}

		return eNormal;
	}

	void CRebotHero::UpdateSkill(UINT32 n32SkillID, UINT32 un32StateTime){
		for (INT32 i = 0; i < n32MaxRobotPlayerSkillNum; ++i){
			RobotSkillData& sSkill = m_asSkilLRunTimeData[i];
			if (sSkill.cpsCfg && sSkill.cpsCfg->un32SkillID == n32SkillID){
				sSkill.tCooldownMilsec = un32StateTime;
				sSkill.tBeginTime = gRobotMgr->GetTickCount();
				break;
			}
		}
	}

}
