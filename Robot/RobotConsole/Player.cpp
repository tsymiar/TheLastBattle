#include "StdAfx.h"
#include "Player.h"
#include "CfgMgr.h"
#include "Robot.h"
#include "RobotMgr.h"
#include "PlayerAtkAI.h"
#include "PlayerMoveAI.h"
#include "PlayerOccupyAI.h"
#include "RobotNPC.h"
#include "AbsorbAI.h"
#include "PlayerAI.h"

namespace ReBot
{
	CRobotPlayer::CRobotPlayer()
		: m_n32CurRoadIndex(0)
		, m_pCClient(NULL)
		, m_SendMoveTarTime(0)
		, m_EPlayerState(ePlayerState_Free)
		, m_LockEnemy(NULL)
		, m_pPlayerOccupyAI(new CPlayerOccupyAI())
		, m_pAbsorbAI(new AbsorbAI())
		, m_IfAbsorbed1(false)
		, m_IfAbsorbed2(false)
		, m_IfRepostSolider(false){
			m_AbsorbTimes = 0;
	}


	CRobotPlayer::~CRobotPlayer(){
		if (m_pPlayerOccupyAI){
			delete m_pPlayerOccupyAI;
			m_pPlayerOccupyAI = NULL;
		}
		if (m_pAbsorbAI){
			delete m_pAbsorbAI;
			m_pAbsorbAI = nullptr;
		}
	}

	INT32 CRobotPlayer::OnHeartBeat(TIME_MILSEC tUTCMilsec){
		for (INT32 i = 0; i < c_n32MaxHeroSkillNum; ++i){
			RobotSkillData& sData = m_asSkilLRunTimeData[i];
			if (sData.tBeginTime > 0 && tUTCMilsec - sData.tBeginTime > sData.tCooldownMilsec){
				sData.tCooldownMilsec = 0;
				sData.tBeginTime = 0;
			}
		}

		if (m_GOActionState == eOAS_Running){
			RunState(tUTCMilsec);
			m_pPlayerOccupyAI->m_StayPosTime.sTime = tUTCMilsec;
		}

		if (m_pPlayerOccupyAI->GetAIState() == eRebotAIState_Running){
			m_pPlayerOccupyAI->Update(tUTCMilsec);
		}

		if (m_pAbsorbAI->GetAIState() == eRebotAIState_Running){
			m_pAbsorbAI->Update(tUTCMilsec);
		}
		else if (m_un32Level >= 3){
			do {
				if (m_un32Level < 10){
					RobotSkillData& sAbsordData1 = m_asSkilLRunTimeData[n32AbsorbSkillID1];
					if (!sAbsordData1.cpsCfg){
						CRobotNPC* pNPC = FindNearestAbosrbNPC();
						if (NULL != pNPC){
							m_pCClient->AskAlwaysAutoAtk(0);
							m_pCClient->AskLockTar(pNPC->GetObjectGUID());

							PlayerAIData sPlayerAIData;
							sPlayerAIData.pPlayer = this;
							INT32 n32Res = m_pAbsorbAI->Begin(sPlayerAIData);
							++m_AbsorbTimes;
							if (eNormal == n32Res){
								m_pPlayerOccupyAI->End();
							}
						}
					}
				}
				else{
					RobotSkillData& sAbsordData1 = m_asSkilLRunTimeData[n32AbsorbSkillID2];
					if (!sAbsordData1.cpsCfg){
						CRobotNPC* pNPC = FindNearestAbosrbNPC();
						if (NULL != pNPC){
							m_pCClient->AskAlwaysAutoAtk(0);
							m_pCClient->AskLockTar(pNPC->GetObjectGUID());

							PlayerAIData sPlayerAIData;
							sPlayerAIData.pPlayer = this;
							INT32 n32Res = m_pAbsorbAI->Begin(sPlayerAIData);
							++m_AbsorbTimes;
							if (eNormal == n32Res){
								m_pPlayerOccupyAI->End();
							}
						}
					}
				}
			} while (false);
		}

		if (m_pAbsorbAI->GetAIState() != eRebotAIState_Running && m_pPlayerOccupyAI->GetAIState() != eRebotAIState_Running){
			PlayerAIData sPlayerAIData;
			sPlayerAIData.pPlayer = this;
			m_pCClient->AskAlwaysAutoAtk(1);
			m_pPlayerOccupyAI->Begin(sPlayerAIData);
		}
		return eNormal;
	}

	void CRobotPlayer::SetRobot(CCClient* pClient){
		m_pCClient = pClient;
		m_LockTime = 0;
		m_AbsorbTimes = 0;
		Clear();
		if (NULL == m_pPlayerOccupyAI){
			m_pPlayerOccupyAI = CPlayerAI::GetOccupyAI();
			PlayerAIData sPlayerAIData;
			sPlayerAIData.pPlayer = this;
			m_pPlayerOccupyAI->Begin(sPlayerAIData);
		}
	}

	INT32 CRobotPlayer::Clear(){
		m_EPlayerState = ePlayerState_Free;
		m_LockEnemy = NULL;
		return eNormal;
	}

	CClientGameObject* CRobotPlayer::FindRandEnemyAndLock(){
		TIME_MILSEC tCurTime = gRobotMgr->GetTickCount();
		if (tCurTime - m_LockTime < 2*1000){
			return m_LockEnemy;
		}
		m_LockTime = tCurTime;

		GOMap& sGOMap = m_pCClient->GetGOMap();

		INT32 n32Index = m_pCClient->m_pRobotMgr->GetRandIndexOnce(sGOMap.size() - 1);
		INT32 n32TempNum = 0;
		GOMap::iterator iter = sGOMap.begin();
		for (; iter != sGOMap.end(); ++iter){
			CClientGameObject* pGO = iter->second;
			if (n32TempNum == n32Index){
				m_LockEnemy = pGO;
				break;
			}
			++n32TempNum;
		}

		if (NULL == m_LockEnemy || m_LockEnemy->GetState() == eOAS_Dead){
			GOMap::iterator iter = sGOMap.begin();
			for (iter = sGOMap.begin(); iter != sGOMap.end(); ++iter){
				CClientGameObject* pGO = iter->second;
				if (pGO->GetState() != eOAS_Dead){
					m_LockEnemy = pGO;
					break;
				}
			}
		}

		m_pCClient->AskLockTar(m_LockEnemy->GetObjectGUID());
		m_pCClient->AskAutoAtk();
		return m_LockEnemy;
	}

	vector<RobotSkillData*> CRobotPlayer::GetCanUseSkillVec(){
		vector<RobotSkillData*> resVec;
		if (m_GOActionState >= eOAS_PreparingSkill){
			return resVec;
		}

		for (INT32 i = 0; i < n32MaxRobotPlayerSkillNum; ++i){
			RobotSkillData& sSkillData = m_asSkilLRunTimeData[i];
			if (sSkillData.cpsCfg == NULL){
				continue;
			}

			if (sSkillData.tCooldownMilsec > 0){
				continue;
			}

			if (sSkillData.cpsCfg->bIfNomalAttack){
				continue;
			}
			if (GetCP() < sSkillData.cpsCfg->n32UseCP){
				continue;
			}
			if (GetMP() < sSkillData.cpsCfg->n32UseMP){
				continue;
			}
			if (GetHP() < sSkillData.cpsCfg->n32UseHP){
				continue;
			}

			resVec.push_back(&sSkillData);
		}

		return resVec;
	}

	BOOLEAN CRobotPlayer::IfInReleaseSkillRange(const SSNewSkillCfg& sCfg, CClientGameObject& pGO){
		return GetCurPos().CanWatch(sCfg.fReleaseDist, pGO.GetCurPos());
	}

	void CRobotPlayer::ClearLockedTar(){
		m_pCClient->EraseObj(m_LockEnemy->GetObjectGUID());
		m_LockEnemy = NULL;
		FindNearestEnemy(true);
	}

	CCClient* CRobotPlayer::GetClient(){
		return m_pCClient;
	}

	string sStateStr[] = {
		"eOAS_Free",
		"eOAS_Running",
		"eOAS_Flying",
		"eOAS_Swimming",
		"eOAS_PreparingSkill",
		"eOAS_ReleasingSkill",
		"eOAS_PassiveState",
		"eOAS_Controlled",
		"eOAS_Reliving",
		"eOAS_Dead",
		"eOAS_End",
	};
	void CRobotPlayer::UpdateState(EGOActionState state, const CVector3D& curPos, const CVector3D& dir){
		TIME_MILSEC tCime = gRobotMgr->GetTickCount();
		m_GOActionState = state;
		if (m_GOActionState == eOAS_Running){
			m_tLastSyncMoveStateMilsec = tCime;
		}

		//ELOG(LOG_INFO, "Player(%u) State ChangeTo %s, ClientPos(%f,%f):ServerPos(%f, %f)", m_pCClient->m_un32RobotID, sStateStr[state].c_str()
		//	, m_CurPos.m_fX, m_CurPos.m_fZ, curPos.m_fX, curPos.m_fZ);

		m_CurPos = curPos;
		m_CurDir = dir;

		if (m_GOActionState == eOAS_Dead){
			m_LockEnemy = NULL;
			m_pPlayerOccupyAI->OnDead();
		}

		m_StateChangeTime = tCime;
	}

	void CRobotPlayer::UpdateHP(INT32 n32Val){
		//if (m_n32CurHP > n32Val && n32Val > 0){
		//	FindNearestEnemy(true);
		//	
		//	RobotSkillData* pSkill = GetRandSkill();
		//	if (NULL != pSkill){
		//		m_pCClient->AskUseSkill(pSkill->cpsCfg->un32SkillID);
		//	}
		//}
		m_n32CurHP = n32Val;
	}

	CClientGameObject* CRobotPlayer::FindNearestEnemy(BOOLEAN bIfHpEvent){
		GOMap& sGOMap = m_pCClient->GetGOMap();

		TIME_MILSEC tCurTime = gRobotMgr->GetTickCount();
		if (!bIfHpEvent){
			if (m_LockEnemy && sGOMap.find(m_LockEnemy->GetObjectGUID()) != sGOMap.end()){
				return m_LockEnemy;
			}

			if (tCurTime - m_LockTime < 2*1000){
				return m_LockEnemy;
			}
			m_LockTime = tCurTime;
		}
		
		m_LockTime = tCurTime;
		
		GOMap::iterator iter = sGOMap.begin();
		m_LockEnemy = NULL;
		for (; iter != sGOMap.end(); ++iter){
			CClientGameObject* pGO = iter->second;
			if (pGO->GetState() == eOAS_Dead){
				continue;
			}

			if (CFunction::IfEnemy(pGO->GetCampID(), GetCampID())){
				m_LockEnemy = pGO;
				break;
			}
		}

		if (NULL == m_LockEnemy){
			return NULL;
		}

		m_pCClient->AskLockTar(m_LockEnemy->GetObjectGUID());
		return m_LockEnemy;
	}

	RobotSkillData* CRobotPlayer::GetRandSkill(){
		vector<RobotSkillData*> pSkill = GetCanUseSkillVec();
		if (pSkill.empty()){
			return NULL;
		}

		INT32 n32Index = m_pCClient->m_pRobotMgr->GetRandIndexOnce(pSkill.size()-1);
		if (n32Index > 8){
			m_pCClient->AskUseGoods(pSkill[n32Index]->goodsIndex);
			return NULL;
		}
		return pSkill[n32Index];
	}

	CRobotNPC* CRobotPlayer::FindNearestAbosrbNPC(){
		CRobotNPC* lResMonster = NULL;
		GOMap& sGOMap = m_pCClient->GetGOMap();
		GOMap::iterator iter = sGOMap.begin();
		//float fDist = 9999999;
		for (; iter != sGOMap.end(); ++iter){
			CClientGameObject* pGO = iter->second;
			if (pGO->GetState() == eOAS_Dead){
				continue;
			}

			//if (!CFunction::IfTypeNPC(pGO->GetObjectGUID())){
				//continue;
			//}
			CRobotNPC* pMonster = (CRobotNPC*)pGO;
			if (!pMonster->GetNPCCfg()){
				continue;
			}

			//if (!pMonster->GetNPCCfg()->bIfCanControl){
				//continue;
			//}

			lResMonster = pMonster;
			break;
			//float fDirstTemp = pGO->GetCurPos().GetWatchDistSqr(m_CurPos);
			//if (fDirstTemp < fDist){
			//	lResMonster = pMonster;
			//	fDist = fDirstTemp;
			//}
		}

		if (NULL == lResMonster){
			return NULL;
		}

		m_LockEnemy = lResMonster;
		return lResMonster;
	}

	void CRobotPlayer::UpdateAbsorb(UINT32 un32ID1, UINT32 un32ID2){
		SNPCCfg* pNPCCfg1 = CCfgMgr::getInstance().GetNPCCfg(un32ID1);
		if (NULL != pNPCCfg1){
			m_pAbsorbAI->End();
			RobotSkillData& sAbsordData1 = m_asSkilLRunTimeData[n32AbsorbSkillID1];
			sAbsordData1.cpsCfg = CCfgMgr::getInstance().GetSkillCfg(pNPCCfg1->aun32SkillList[2]);
			m_IfAbsorbed1 = true;
		}

		SNPCCfg* pNPCCfg2 = CCfgMgr::getInstance().GetNPCCfg(un32ID2);
		if (NULL != pNPCCfg2){
			m_pAbsorbAI->End();
			RobotSkillData& sAbsordData2 = m_asSkilLRunTimeData[n32AbsorbSkillID2];
			sAbsordData2.cpsCfg = CCfgMgr::getInstance().GetSkillCfg(pNPCCfg2->aun32SkillList[2]);
			m_IfAbsorbed2 = true;
		}
		m_n32MonsterID1 = un32ID1;
		m_n32MonsterID2 = un32ID2;
		if (!m_IfRepostSolider){
			if (m_n32MonsterID1 > 0){
				m_IfRepostSolider = true;
				m_pCClient->AskReportAltarStrawSolder(m_n32MonsterID1);
			}
			else if (m_n32MonsterID2 > 0){
				m_pCClient->AskReportAltarStrawSolder(m_n32MonsterID2);
			}
		}
	}

	void CRobotPlayer::UpdateGoodsState(GSToGC::NotifyGoodsInfo_GoodsInfo& pInfo){
		m_TryToBuGoods.erase(pInfo.tyepid());
		m_BuyGoods.insert(pInfo.tyepid());

		 SGoodsRunTimeData& sGoodsRunTimeData = m_asGoodsRunTimeData[pInfo.pos()];
		 sGoodsRunTimeData.n32Num = pInfo.num();
		 sGoodsRunTimeData.sGoodsCfg = CCfgMgr::getInstance().GetGoodsCfg(pInfo.tyepid());
		 if (!sGoodsRunTimeData.sGoodsCfg){
			 ELOG(LOG_WARNNING, "null cfg with id:%d", pInfo.tyepid());
			 return;
		 }

		 const SSNewSkillCfg* pNewSkillCfg = CCfgMgr::getInstance().GetSkillCfg(sGoodsRunTimeData.sGoodsCfg->un32SkillID);
		 bool bIfNewGoods = true;
		 for (int i = 9; i < n32MaxRobotPlayerSkillNum; ++i){
			 if (m_asSkilLRunTimeData[i].cpsCfg == pNewSkillCfg){
				 if (sGoodsRunTimeData.n32Num == 0){
					 m_asSkilLRunTimeData[i].Clear();
					 m_BuyGoods.erase(pInfo.tyepid());
					 return;
				 }
				 m_asSkilLRunTimeData[i].tCooldownMilsec = pInfo.statetime();
				 m_asSkilLRunTimeData[i].goodsIndex = pInfo.pos();
				 bIfNewGoods = false;
				 break;
			 }
		 }

		 if (bIfNewGoods){
			 for (int i = 9; i < n32MaxRobotPlayerSkillNum; ++i){
				 if (!m_asSkilLRunTimeData[i].cpsCfg){
					 m_asSkilLRunTimeData[i].cpsCfg = pNewSkillCfg;
					 m_asSkilLRunTimeData[i].tCooldownMilsec = pInfo.statetime();
					 m_asSkilLRunTimeData[i].goodsIndex = pInfo.pos();
					 break;
				 }
			 }
		 }
	}

}

