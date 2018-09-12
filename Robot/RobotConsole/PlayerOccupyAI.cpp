#include "StdAfx.h"
#include "PlayerOccupyAI.h"
#include "Robot.h"
#include "RobotMgr.h"
#include "PlayerMoveAI.h"
#include "PlayerAtkAI.h"
#include "Player.h"
#include "GameUnit.h"
#include "AbsorbAI.h"
#include "CfgMgr.h"

namespace ReBot
{
	CPlayerOccupyAI::CPlayerOccupyAI():m_eOccupyState(eOccupyState_Hesitation)
		, m_pPlayerMoveAI(NULL)
		, m_tStateBeginMilsec(0)
		, m_pPlayerAtkAI(NULL)
		, m_n32CurNodeIdx(0)
		, m_tLastWatchEnemyMilsec(0)
		, m_pCurEnemy(NULL)
		, m_LastBuyGoodsTime(0){

	}

	CPlayerOccupyAI::~CPlayerOccupyAI(){
		if (m_pPlayerAtkAI){
			delete m_pPlayerAtkAI;
			m_pPlayerAtkAI = nullptr;
		}
		if (m_pPlayerMoveAI){
			delete m_pPlayerMoveAI;
			m_pPlayerMoveAI = nullptr;
		}
	}

	INT32 CPlayerOccupyAI::Begin(PlayerAIData& sPlayerAIData){
		m_MoveType = 0;
		m_tLastMoveMilsec = 0;
		m_StayPosTime.sPos.Clear();
		m_StayPosTime.sTime = gRobotMgr->GetTickCount();
		m_pPlayer = sPlayerAIData.pPlayer;
		if (m_pPlayerAtkAI){
			m_pPlayerAtkAI->End();
		}

		if (m_pPlayerMoveAI){
			m_pPlayerMoveAI->End();
		}
		m_RebotAIState = eRebotAIState_Running;
		m_pPlayer->GetClient()->AskAlwaysAutoAtk(1);
		m_pPlayer->GetClient()->AskMovePos(CVector3D(9836.0, 6006.0, 9831.0));
		m_tStateBeginMilsec = gRobotMgr->GetTickCount();
		m_StayPosTime.sTime = m_tStateBeginMilsec;
		return eNormal;
	}

	INT32 CPlayerOccupyAI::Clear(){
		m_eOccupyState = eOccupyState_Hesitation;
		m_pPlayer = NULL;
		if (m_pPlayerMoveAI){
			m_pPlayerMoveAI->Clear();
			CPlayerAI::FreeMoveAI(m_pPlayerMoveAI);
			m_pPlayerMoveAI = NULL;
		}
		
		if (m_pPlayerAtkAI){
			m_pPlayerAtkAI->End();
			CPlayerAI::FreeAtkAI(m_pPlayerAtkAI);
			m_pPlayerAtkAI = NULL;
		}
		
		m_n32CurNodeIdx = 0;
		m_tStateBeginMilsec = 0;
		m_RebotAIState = eRebotAIState_Cleared;
		m_tLastWatchEnemyMilsec = 0;
		m_pCurEnemy = NULL;
		return eNormal;
	}

	INT32 CPlayerOccupyAI::TryToUpdatePathNode(){
		vector<CVector3D>& linePathVec = m_pPlayer->m_pCClient->m_pRobotMgr->GetLinePath();
		return eNormal;
	}

	INT32 CPlayerOccupyAI::End(){
		m_RebotAIState = eRebotAIState_Cleared;
		return eNormal;
	}

	INT32 CPlayerOccupyAI::Update(TIME_MILSEC tCurTime){
		if (tCurTime - m_StayPosTime.sTime > 10 * 1000 && m_pPlayer->GetState() != eOAS_Running){
			switch (m_MoveType)
			{
			case 0:
				m_pPlayer->GetClient()->AskMovePos(m_pPlayer->m_pCClient->m_pRobotMgr->GetRandPathNode());
				break;
			case 1:
				m_pPlayer->GetClient()->AskMoveDir(m_pPlayer->m_CurDir);
				break;
			case 2:
				m_pPlayer->GetClient()->AskUseSkill(100001);
				break;
			}

			m_MoveType = (m_MoveType + 1)% 3;
			m_StayPosTime.sTime  = tCurTime;
			m_tLastMoveMilsec = tCurTime;
			return eNormal;
		}

		m_pPlayer->FindNearestEnemy(FALSE);
		m_pPlayer->GetClient()->AskAutoAtk();
		if (tCurTime - m_tLastWatchEnemyMilsec> 3 * 1000 && tCurTime - m_tLastMoveMilsec < 5*1000){
			RobotSkillData* pSkill = m_pPlayer->GetRandSkill();
			if (NULL != pSkill){
				m_pPlayer->m_pCClient->AskUseSkill(pSkill->cpsCfg->un32SkillID);
			}
			m_tLastWatchEnemyMilsec = tCurTime;
			return eNormal;
		}

		if (tCurTime - m_LastBuyGoodsTime > 1000 * 60){
			if (m_pPlayer->m_BuyGoods.size() >= 6){
				return 0;
			}
			INT32 n32NowCP = m_pPlayer->GetCP();
			auto& goodsMap = CCfgMgr::getInstance().GetGoodsMap();
			for (auto iter = goodsMap.begin(); iter != goodsMap.end(); ++iter){
				SGoodsCfg& goodCfg = iter->second;
				if (m_pPlayer->m_TryToBuGoods.find(goodCfg.eOT) == m_pPlayer->m_TryToBuGoods.end() && m_pPlayer->m_BuyGoods.find(goodCfg.eOT) == m_pPlayer->m_BuyGoods.end() && n32NowCP >= goodCfg.n32CPCost){
					m_pPlayer->GetClient()->AskBuyGoods(goodCfg.eOT);
					m_pPlayer->m_TryToBuGoods.insert(goodCfg.eOT);
					break;
				}
			}

			m_LastBuyGoodsTime = tCurTime;
		}

		//if (NULL != m_pCurEnemy && m_pCurEnemy->GetState() != eOAS_Dead){
		//	TIME_MILSEC timeDiff = tCurTime - m_tLastWatchEnemyMilsec;

		//	if (timeDiff > 10 * 1000 && (m_pPlayer->GetState() != eOAS_Running)){
		//		RobotSkillData* pSkill = m_pPlayer->CanUseSkill();
		//		if (NULL != pSkill){
		//			m_pPlayer->GetClient()->AskUseSkill(pSkill->cpsCfg->un32SkillID);
		//			return eNormal;
		//		}

		//		m_tLastWatchEnemyMilsec = tCurTime;
		//	}

		//	m_pPlayer->TestGoods();
		//	return eNormal;
		//}

		//m_pCurEnemy = m_pPlayer->FindNearestEnemy(FALSE);
		//if (NULL == m_pCurEnemy){
		//	TIME_MILSEC timeDiff = tCurTime - m_tStateBeginMilsec;
		//	if (timeDiff > 5 * 1000 && (m_pPlayer->GetState() != eOAS_Running)){
		//		m_pPlayer->GetClient()->AskMovePos(CRobotMgr::GetRandPathNode());
		//		m_tStateBeginMilsec = tCurTime;
		//		m_pPlayer->m_StateChangeTime = tCurTime;
		//	}
		//}

		return eNormal;
	}

	INT32 CPlayerOccupyAI::BeginAtkAI(TIME_MILSEC tCurTime){
		CClientGameObject* pEnemy = m_pPlayer->FindRandEnemyAndLock();
		m_tLastWatchEnemyMilsec = tCurTime;
		if (NULL != pEnemy){
			if (NULL == m_pPlayerAtkAI){
				m_pPlayerAtkAI = CPlayerAI::GetAtkAI();
			}
			else{
				m_pPlayerAtkAI->Clear();
			}

			PlayerAIData sData;
			sData.pPlayer = m_pPlayer;
			INT32 n32Res = m_pPlayerAtkAI->Begin(sData);
			if (eNormal == n32Res){
				m_eOccupyState = eOccupyState_Attack;
				return eNormal;
			}
		}else{
			return eEC_BeginAIFailed;
		}

		return eNormal;
	}

	INT32 CPlayerOccupyAI::BeginMoveAI(TIME_MILSEC tCurTime){
		if (NULL == m_pPlayerMoveAI){
			m_pPlayerMoveAI = CPlayerAI::GetMoveAI();
		}
		else{
			m_pPlayerMoveAI->End();
		}

		vector<CVector3D>& linePathVec = m_pPlayer->m_pCClient->m_pRobotMgr->GetLinePath();

		PlayerAIData sData;
		sData.pPlayer = m_pPlayer;
		sData.sTarPos = linePathVec[m_n32CurNodeIdx];
		INT32 n32Res = m_pPlayerMoveAI->Begin(sData);
		if (eNormal == n32Res){
			m_eOccupyState = eOccupyState_Walk;
			return eNormal;
		}

		return eEC_BeginAIFailed;
	}

	void CPlayerOccupyAI::OnDead(){
		vector<CVector3D>& linePathVec = m_pPlayer->m_pCClient->m_pRobotMgr->GetLinePath();
		m_pPlayer->GetClient()->AskMovePos(linePathVec[0]);
	}

}
