#include "stdafx.h"
#include "SSHero.h"
#include "SSBattleMgr.h"
#include "SSCfgMgr.h"
#include "SSBattle.h"
#include "SSNPC.h"
#include "SSPassiveSkill.h"
#include "SSPassiveSkillMgr.h"
#include "SSEffectMgr.h"
//#include "SSGuide.h"
#include "boost/spirit.hpp"
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string.hpp"
#include "SSWorkThreadMgr.h"
#include "SSUser.h"

#include "log/SSGameLogMgr.h"
#include "SSAI_Hero.h"
#include "SSAI_HeroRobot.h"
#include "SSSkill.h"

namespace SceneServer{

const INT32	n32PersentDevide = 1000;
const INT32 n32MaxAtkSpeed = 5000;

CSSHero::CSSHero(UINT64 guid, EGameObjectCamp sCamp, UINT32 objectID, UINT64 sMasterGUID, CSSBattle* pBattle, CSSBattleMgr* pMgr, bool IfAI):
	CSSGameUnit(guid, sCamp, objectID, sMasterGUID, pBattle, pMgr)
, m_un8BuyRebornTimes(1)
, m_eKillTitel(eNoneTitel)
, m_cpsHeroCfg(NULL)
, m_bIfAI(IfAI)
, m_eFuryState(eFuryNullState)
, m_tFuryMilsec(0)
, m_bCheckFuryTimeTag(FALSE)
, m_bStartRecoverCpTag(FALSE)
, m_tRecoverCpMilsec(0)
, m_n16ContinuousKill(0)
, m_tConKillMilTime(0)
, m_n16ContinuousDead(0)
, m_tConDeadMilTime(0)
, m_eHeroKillState(eNoneKill)
, m_bAutoAttack(FALSE)
, m_tHeroLastDeadMilsec(0)
//, m_guide(new CSSGuide())
, m_n64KillNPC(0)
, m_piUser(NULL)
, m_bAlwaysAutoAtk(FALSE)
, m_tClearHurtRecordTime(0)
, m_heroBornPos(0,0,0){
	for (INT32 i = 0; i < c_un32MaxGridNum; ++i){
		m_asGoodsRunTimeData[i] = SGoodsRunTimeData();
	}

	for (INT32 i = 0; i < c_n32MaxHeroSkillNum; ++i){
		m_aspSkillArr[i] = new CSSSkill();
	}

	for (INT32 i = 0; i < c_n32MaxAbsorbSkillNum; ++i){
		m_asAbsorbRuntimeInfo[i] = AbsorbRuntimeInfo();
		m_asAbsorbRuntimeInfo[i].pSkill = new CSSSkill();
	}
}


CSSHero::~CSSHero(void){
	for (INT32 i = 0; i < c_n32MaxHeroSkillNum; ++i){
		delete m_aspSkillArr[i];
		m_aspSkillArr[i] = nullptr;
	}

	for (INT32 i = 0; i < c_n32MaxAbsorbSkillNum; ++i){
		delete m_asAbsorbRuntimeInfo[i].pSkill;
		m_asAbsorbRuntimeInfo[i].pSkill = nullptr;
	}

	for(auto iter = m_asAllGoodsSkillDataMap.begin(); iter != m_asAllGoodsSkillDataMap.end(); ){
		CSSSkill* pSkill = iter->second;
		iter = m_asAllGoodsSkillDataMap.erase(iter);
		delete pSkill;
		pSkill = NULL;
	}

	m_sRecentlyHurtbyHeroMap.clear();
	m_hurtRecordVec.clear();

	//delete m_guide;
}

INT32	CSSHero::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	OnGameUnitHeartBeat(tUTCMilsec, tTickSpan);
	if(m_bCheckFuryTimeTag){
		CheckFuryRunEnd(tUTCMilsec,tTickSpan);
	}
	if(m_bStartRecoverCpTag){
		RecoverPlayerCp(tUTCMilsec,tTickSpan);
	}
	//清理受伤害列表
	if(tUTCMilsec - m_tClearHurtRecordTime > 2000){
		m_tClearHurtRecordTime = tUTCMilsec;
		for(auto iter = m_hurtRecordVec.begin(); iter != m_hurtRecordVec.end();){
			SHurtRecord* pRecord = iter._Ptr;
			if(tUTCMilsec - pRecord->tHurtTime > 10000){
				iter = m_hurtRecordVec.erase(iter);
			}
			else{
				break;
			}
		}
	}

	return eNormal;
}

INT32 CSSHero::GetCP(ECPCostType eType){ 
	switch (eType){
	case eCPCP_Personnal:
		return m_sServiceData.PersonCP;
	case eCPCP_Team:
		return m_sServiceData.TeamCP;
	}
	return 0;
}

INT32	CSSHero::GetBuyRebornCost()
{
	if (GetHeroRebornTimes() == 0){
		return 0;
	}

	if (1 == m_un8BuyRebornTimes){
		if (NULL != GetUser() && 0 < GetUser()->GetVipLevel()){
			return c_VipBuyRebornCost;
		}
		else{
			return c_FirBuyRebornCost;
		}
	}
	else if (2 == m_un8BuyRebornTimes){
		return c_SecBuyRebornCost;
	}
	else if (3 == m_un8BuyRebornTimes){
		return c_ThrBuyRebornCost;
	}
	else
		return 0;
}

INT32 CSSHero::ChangeCP(ECPCostType type, INT32 n32CP,  bool bIfIncomByKill){
	switch (type)
	{
	case eCPCP_Personnal:
		{
			INT32 n32FinalCP = m_sServiceData.PersonCP + n32CP;
			if (n32FinalCP < 0){
				return eEC_AbsentCP;
			}
			if (0 < n32CP){
				m_sServiceData.TotalPersonCP += n32CP;
			}
			m_sServiceData.PersonCP = n32FinalCP;
		}
		break;
	case eCPCP_Team:
		{
			INT32 n32FinalCP = m_sServiceData.TeamCP + n32CP;
			if (n32FinalCP < 0){
				return eEC_AbsentCP;
			}
			if (0 < n32CP){
				m_sServiceData.TotalTeamCP += n32CP;
			}
			m_sServiceData.TeamCP = n32FinalCP;
		}
		break;
	default:
		ELOG(LOG_ERROR, "failed");
		return eNormal;
	}

	SyncCpToCl(); 

	if (bIfIncomByKill){
		GSToGC::PersonCPChange sCPChange;
		sCPChange.set_guid(GetObjGUID());
		sCPChange.set_cp(n32CP);
		GetCurBattle()->SendMsgToSelfAndOB(sCPChange, sCPChange.msgid(), m_piUser);
	} 

	return eNormal;
}

INT32 CSSHero::GetBuyGoodsCp(UINT32 un32GoodsTypeID){
	SGoodsCfg* pOriGoodsCfg = (SGoodsCfg*)CSSCfgMgr::GetSSCfgInstance().GetGoodsCfg((EObjectType)un32GoodsTypeID);
	if (NULL == pOriGoodsCfg){
		return -1;
	}
	INT32 n32ComsumeCP = pOriGoodsCfg->n32CPCost;
	set<UINT8> toDecGirdSet;
	TryDownCompose(pOriGoodsCfg, n32ComsumeCP, toDecGirdSet);
	return n32ComsumeCP;
}

INT32 CSSHero::AskBuyGoods(UINT32 un32GoodsTypeID,INT32 &goodsId){
	SGoodsCfg* pOriGoodsCfg = (SGoodsCfg*)CSSCfgMgr::GetSSCfgInstance().GetGoodsCfg((EObjectType)un32GoodsTypeID);
	if (NULL == pOriGoodsCfg){
		return eEC_NULLCfg;
	}

	if (IsDead() && !pOriGoodsCfg->bIfCanBuyWhenDeath){
		return eEC_CannotBuygoodsWhenDeath;
	}

	ECPCostType consumeType = ConvertCPType(pOriGoodsCfg);
	if (eCPCP_Unlegal == consumeType){
		return eEC_AskBuyGoodsFailForInvalidCPType;
	}

	INT32 buyRes = eNormal;
	SGoodsCfg* newGoodsCfg = NULL;
	INT32 n32ComsumeCP = pOriGoodsCfg->n32CPCost;
	UINT8 un8IsComposed = 0;
	set<UINT8> toDecGirdSet;

	do{
		bool bDownCompose = TryDownCompose(pOriGoodsCfg, n32ComsumeCP, toDecGirdSet);
		if (TRUE == bDownCompose){
			if (GetCP((ECPCostType)pOriGoodsCfg->n32CPCostType) < n32ComsumeCP){
				if (eCPCP_Personnal == pOriGoodsCfg->n32CPCostType){
					buyRes = eEC_AskBuyGoodsFailForLackCP;
				}
				else{
					buyRes = eEC_AskBuyGoodsFailForLackTeamCP;
				}
				break;
			}

			newGoodsCfg = pOriGoodsCfg;
			un8IsComposed = 1;
			SGoodsCfg* sNextUpComposeCfg = TryUpCompose(pOriGoodsCfg, toDecGirdSet);
			if (NULL != sNextUpComposeCfg){
				newGoodsCfg = sNextUpComposeCfg;
			}
			break;
		}
		else{
			n32ComsumeCP = pOriGoodsCfg->n32CPCost;
			if (GetCP((ECPCostType)pOriGoodsCfg->n32CPCostType) < n32ComsumeCP){
				if (eCPCP_Personnal == pOriGoodsCfg->n32CPCostType){
					buyRes = eEC_AskBuyGoodsFailForLackCP;
				}
				else{
					buyRes = eEC_AskBuyGoodsFailForLackTeamCP;
				}
				break;
			}

			newGoodsCfg = TryUpCompose(pOriGoodsCfg, toDecGirdSet);
			if (NULL != newGoodsCfg){
				un8IsComposed = 1;
			}
			else{
				newGoodsCfg = pOriGoodsCfg;
			}
		}

		if (NULL == newGoodsCfg){
			buyRes = eEC_rInvalidGoodsNum;
			break;
		}

		if (newGoodsCfg->bUsedAfterBuyed && newGoodsCfg->bUsedDestory){
			buyRes = GetCurBattle()->GetEffectMgr()->UseSkillDirect(newGoodsCfg->un32SkillID, this, this);
			if (eNormal == buyRes){
				ChangeCP(ECPCostType(newGoodsCfg->n32CPCostType), -n32ComsumeCP, false);

				for (UINT8 i = 0; i < c_un32MaxGridNum; ++i){
					SGoodsRunTimeData sInsideGoodsRunTimeData = m_asGoodsRunTimeData[i];
					if (sInsideGoodsRunTimeData.n32Num > 0 && NULL != sInsideGoodsRunTimeData.sGoodsCfg && newGoodsCfg->eOT == sInsideGoodsRunTimeData.sGoodsCfg->eOT){
						return SynGoodsUseStateToGC(i, 0);
					}
				}
				goodsId = newGoodsCfg->un32GoodsID ;
				return eNormal;
			}
		}
	} while (FALSE);

	if (eNormal == buyRes && NULL != newGoodsCfg){
		for(set<UINT8>::iterator iter = toDecGirdSet.begin(); iter != toDecGirdSet.end(); ++iter){
			m_asGoodsRunTimeData[*iter].bIsComposing = TRUE;
		}

		if (newGoodsCfg->bUniqueID && NULL != HasSameEOTIDGoods(newGoodsCfg->eOT)){
			for(set<UINT8>::iterator iter = toDecGirdSet.begin(); iter != toDecGirdSet.end(); ++iter){
				m_asGoodsRunTimeData[*iter].bIsComposing = FALSE;
			}
			return eEC_AskBuyGoodsFailForSole;
		}

		if (newGoodsCfg->un32UniqueTypeID && NULL != HasSameTypeIDGoods(newGoodsCfg->un32UniqueTypeID)){
			for(set<UINT8>::iterator iter = toDecGirdSet.begin(); iter != toDecGirdSet.end(); ++iter){
				m_asGoodsRunTimeData[*iter].bIsComposing = FALSE;
			}

			return eEC_AskBuyGoodsFailForHasSameTypeID;
		}

		for(set<UINT8>::iterator iter = toDecGirdSet.begin(); iter != toDecGirdSet.end(); ++iter){
			m_asGoodsRunTimeData[*iter].bIsComposing = FALSE;
			DecreaseOneGoods(*iter, EDecreaseReason_Compose);
		}

		UINT8 un8GridID = 0;
		SGoodsRunTimeData* newGoodsRuntime = InsertOneGoods(newGoodsCfg, 1, un8GridID, buyRes, un8IsComposed);
		if(NULL != newGoodsRuntime){
			ChangeCP(ECPCostType(newGoodsCfg->n32CPCostType), -n32ComsumeCP, false);

			if (TRUE == newGoodsCfg->bUsedAfterBuyed){
				buyRes = GetCurBattle()->GetEffectMgr()->UseSkillDirect(newGoodsCfg->un32SkillID, this, this);
				if (eNormal == buyRes){
					DecreaseOneGoods(un8GridID, EDecreaseReason_Use);
					SynGoodsUseStateToGC(un8GridID);
				}
			} 

			RefreshGoodsSkillMap();
			goodsId = newGoodsCfg->un32GoodsID ;
			return eNormal;
		}
	}
	RefreshGoodsSkillMap();
	ELOG(LOG_WARNNING, "failed for errorcode:%d", buyRes);
	
	return buyRes;
}

INT32 CSSHero::AskUseGoods(UINT8 un8GridID){
	if (FALSE == IsValidGrid(un8GridID)){
		return eEC_InvaildGridID;
	}

	SGoodsRunTimeData& sGoodsRunTimeData = m_asGoodsRunTimeData[un8GridID];
	if (FALSE == IsStillHasGoods(un8GridID) || sGoodsRunTimeData.eGoodsSpecificaFlag == eGoodsSpecificaFlag_InvalidButNotDestory){
		return eEC_InvaildGridID;
	}

	if (NULL == sGoodsRunTimeData.pSkill || NULL == sGoodsRunTimeData.pSkill->cpsCfg){
		return eEC_NULLCfg;
	}

	if(GetFPData(eEffectCate_Silence) > 0){
		return eEC_UseGoodslFailForSilenced;
	}

	INT32 n32Res = AskUseSkillInGoods(*sGoodsRunTimeData.sGoodsCfg, sGoodsRunTimeData.pSkill->cpsCfg);
	if (eNormal != n32Res){
		if (eEC_UseSkillFailForBuildingNullity == n32Res){
			return eEC_UseGoodsFailForBuildingNullity;
		}
		if (eEC_UseSkillFailForNULLTarget == n32Res){
			return eEC_UseGoodsFailForNULLTarget;
		}
		if (eEC_UseSkillFailForDizziness == n32Res){
			return eEC_UseGoodsFailForDizziness;
		}
		if (eEC_UseSkillFailForSilenced == n32Res){
			return eEC_UseGoodslFailForSilenced;
		}
		if (eEC_UseSkillTargetWrongCamp == n32Res){
			return eEC_UseGoodslFailForErrorCamp;
		}
		ELOG(LOG_ERROR, "useskill failed with errorcode:%d with gridid:%d", n32Res, un8GridID);
		return n32Res;
	}

	INT32 nret = DecreaseOneGoods(un8GridID, EDecreaseReason_Use);
	if(!IfAI()&& !this->GetCurBattle()->IsGuideBattle()){//log
		stringstream mystream;
		mystream << this->GetObjType() << LOG_SIGN;
		mystream << this->GetLevel() << LOG_SIGN;
		mystream << ((INT32)un8GridID) << LOG_SIGN;
		mystream << sGoodsRunTimeData.pSkill->cpsCfg->un32SkillID;
		CSSGameLogMgr::GetInstance().AddBattleLog(eLog_BattleSkillEquip,this->GetCurBattleID(),this->GetCurBattle()->GetBattleMapId(),mystream.str());
	}
	 return nret;
}


INT32 CSSHero::AskSelGoods(UINT8 un8GridID){
	if (FALSE == IsValidGrid(un8GridID)){
		return eEC_InvaildGridID;
	}

	if (FALSE == IsStillHasGoods(un8GridID)){
		return eEC_InvaildGridID;
	}
	SGoodsRunTimeData& sGoodsRunTimeData = m_asGoodsRunTimeData[un8GridID];
	if (IsDead() && !sGoodsRunTimeData.sGoodsCfg->bIfCanBuyWhenDeath){
		return eEC_CannotSellgoodsWhenDeath;
	}

	INT32 n32Res = DecreaseOneGoods(un8GridID, EDecreaseReason_Sell);
	if (eNormal == n32Res){
		SGoodsRunTimeData& sGoodsRunTimeData = m_asGoodsRunTimeData[un8GridID];
		ChangeCP((ECPCostType)sGoodsRunTimeData.sGoodsCfg->n32CPCostType, sGoodsRunTimeData.sGoodsCfg->n32CPCost / 2, false);
	}
	RefreshGoodsSkillMap();

	return eNormal;
}

INT32 CSSHero::AskMoveGoods(UINT8 un8FromGridID, UINT8 un8ToGridID){
	if (FALSE == IsValidGrid(un8FromGridID) || FALSE == IsValidGrid(un8ToGridID)){
		return eEC_InvaildGridID;
	}

	if (FALSE == IsStillHasGoods(un8FromGridID)){
		return eEC_InvaildGridID;
	}

	SGoodsRunTimeData& spFromGoods = m_asGoodsRunTimeData[un8FromGridID];
	SGoodsRunTimeData& sToGoods = m_asGoodsRunTimeData[un8ToGridID];

	std::swap(spFromGoods, sToGoods);

	SynGoodsUseStateToGC(un8FromGridID, 0);
	return SynGoodsUseStateToGC(un8ToGridID, 0);
}

ECPCostType CSSHero::ConvertCPType(const SGoodsCfg* pGoodsCfg){
	switch (pGoodsCfg->n32CPCostType){
	case  1:
		return eCPCP_Personnal;
	case 2:
		return eCPCP_Team;
	default:
		return eCPCP_Unlegal;
	}
}

SGoodsCfg* CSSHero::TryUpCompose(SGoodsCfg* pGoodsCfg, set<UINT8>& toDecGirdSet){
	if (NULL == pGoodsCfg){
		return NULL;
	}

	SGoodsCfg* pNewGoods = NULL;

	GetUpComposeRunTimsGoods(pGoodsCfg, toDecGirdSet, pNewGoods);

	return pNewGoods;
}

const SGoodsCfg* CSSHero::GetUpComposeRunTimsGoods(const SGoodsCfg* pGoodsCfg, set<UINT8>& un8GridIdSet, SGoodsCfg*& pNewGoodsCfg){
	if (NULL == pGoodsCfg){
		return NULL;
	}

	const SUpInfo* sUpInfo = pGoodsCfg->sGoodsCompose.sUpInfo;
	for (UINT8 i = 0; i < c_un32MaxGoodsDownInfoNum; ++i){
		const SGoodsCfg* pBrotherGoodsCfg = CSSCfgMgr::GetSSCfgInstance().GetGoodsCfg((EObjectType)sUpInfo[i].un32BrotherType);
		if (NULL != pBrotherGoodsCfg){
			UINT8 un8GridID = 0;
			SGoodsRunTimeData* sRuntimeData = HasSameEOTIDGoods(sUpInfo[i].un32BrotherType, un8GridID);
			if(NULL != sRuntimeData && NULL != sRuntimeData->sGoodsCfg){
				const SGoodsCfg* pParentGoodsCfg = CSSCfgMgr::GetSSCfgInstance().GetGoodsCfg((EObjectType)sUpInfo[i].un32ParentType);
				if (NULL == pParentGoodsCfg || pParentGoodsCfg->n32CombineCPCost > 0){
					return NULL;
				}
				pNewGoodsCfg = (SGoodsCfg*)pParentGoodsCfg;
				un8GridIdSet.insert(un8GridID);
				return GetUpComposeRunTimsGoods(pParentGoodsCfg, un8GridIdSet, pNewGoodsCfg);
			}
		}
	}
	return NULL;
}

void CSSHero::GetDownComposeRunTimsGoods(const SGoodsCfg* pGoodsCfg, set<UINT8>& un8GridIdList){
	if (NULL == pGoodsCfg){
		return;
	}

	const SDownInfo& sDownInfo = pGoodsCfg->sGoodsCompose.sDownInfo;
	UINT8 un8GridID = 0;

	if (0 != sDownInfo.un32Child1Type){
		const SGoodsCfg* pBrotherGoodsCfg1 = CSSCfgMgr::GetSSCfgInstance().GetGoodsCfg((EObjectType)sDownInfo.un32Child1Type);
		if (NULL != pBrotherGoodsCfg1){
			set<UINT8> un8IDSet;
			HasSameEOTIDGoods(sDownInfo.un32Child1Type, un8IDSet);
			bool bIfDoNext = TRUE;
			if (FALSE == un8IDSet.empty()){
				for (set<UINT8>::iterator iter = un8IDSet.begin(); iter != un8IDSet.end(); ++iter){
					if(un8GridIdList.end() == un8GridIdList.find(*iter)){
						un8GridIdList.insert(*iter);
						bIfDoNext = FALSE;
						break;
					}
				}
			}

			if (TRUE == bIfDoNext){
				GetDownComposeRunTimsGoods(pBrotherGoodsCfg1, un8GridIdList);
			}
		}
	}

	if (0 != sDownInfo.un32Child2Type){
		const SGoodsCfg* pBrotherGoodsCfg2 = CSSCfgMgr::GetSSCfgInstance().GetGoodsCfg((EObjectType)sDownInfo.un32Child2Type);
		if (NULL != pBrotherGoodsCfg2){
			set<UINT8> un8IDSet;
			HasSameEOTIDGoods(sDownInfo.un32Child2Type, un8IDSet);
			bool bIfDoNext = TRUE;
			if (FALSE == un8IDSet.empty()){
				for (set<UINT8>::iterator iter = un8IDSet.begin(); iter != un8IDSet.end(); ++iter){
					if(un8GridIdList.end() == un8GridIdList.find(*iter)){
						un8GridIdList.insert(*iter);
						bIfDoNext = FALSE;
						break;
					}
				}
			}

			if (TRUE == bIfDoNext){
				GetDownComposeRunTimsGoods(pBrotherGoodsCfg2, un8GridIdList);
			}
		}
	}
}

SGoodsRunTimeData* CSSHero::InsertOneGoods(SGoodsCfg* psCfg, INT32 n32GoodsNum, UINT8& un8GridID, INT32& n32Reflag, UINT8 bIsComposed){
	if (NULL == psCfg || n32GoodsNum < 1){
		return NULL;
	}

	bool bIsFind = FALSE;

	for (UINT8 i = 0; i < c_un32MaxGridNum; ++i){
		SGoodsRunTimeData& aSGoodsRunTimeData = m_asGoodsRunTimeData[i];
		if (aSGoodsRunTimeData.eGoodsSpecificaFlag != eGoodsSpecificaFlag_NoSpecifical){
			continue;
		}
		if (aSGoodsRunTimeData.n32Num > 0 && psCfg == aSGoodsRunTimeData.sGoodsCfg && aSGoodsRunTimeData.n32Num < aSGoodsRunTimeData.sGoodsCfg->n32OverlapTimes){
			bIsFind = TRUE;
			un8GridID = i;
			++aSGoodsRunTimeData.n32Num;
			break;
		}
	}

	if (FALSE == bIsFind){
		for (UINT8 i = 0; i < c_un32MaxGridNum; ++i){
			SGoodsRunTimeData& aSGoodsRunTimeData = m_asGoodsRunTimeData[i];
			if (aSGoodsRunTimeData.eGoodsSpecificaFlag != eGoodsSpecificaFlag_NoSpecifical){
				continue;
			}

			if (0 == aSGoodsRunTimeData.n32Num){
				bIsFind = TRUE;
				un8GridID = i;
				aSGoodsRunTimeData.sGoodsCfg = psCfg;
				const SSNewSkillCfg* pNewSkillCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillCfg(psCfg->un32SkillID);

				AllGoodsSkillDataMap::iterator iter = m_asAllGoodsSkillDataMap.find(psCfg->un32SkillID);
				if (iter == m_asAllGoodsSkillDataMap.end()){
					CSSSkill* pSkill = new CSSSkill();
					pSkill->cpsCfg = pNewSkillCfg;
					pSkill->pcMasterGU = (CSSGameUnit*)this;
					pSkill->eSkillState = eSkillState_Free;
					pSkill->tStateMilsec = 0;
					m_asAllGoodsSkillDataMap.insert(std::make_pair(psCfg->un32SkillID, pSkill));
				}
				aSGoodsRunTimeData.pSkill = m_asAllGoodsSkillDataMap[psCfg->un32SkillID];

				if (eGoodsSpecificaFlag_InfiniteUse == psCfg->n32UseTimes){
					aSGoodsRunTimeData.eGoodsSpecificaFlag = eGoodsSpecificaFlag_InfiniteUse;
					aSGoodsRunTimeData.n32Num = eGoodsSpecificaFlag_InfiniteUse;
				} else if (psCfg->n32UseTimes > 0){
					aSGoodsRunTimeData.n32Num = psCfg->n32UseTimes;
				}
				else if (0 == psCfg->bUsedDestory){
					aSGoodsRunTimeData.eGoodsSpecificaFlag = eGoodsSpecificaFlag_InvalidButNotDestory;
					aSGoodsRunTimeData.n32Num = 1;
				}
				else{
					aSGoodsRunTimeData.n32Num = 1;
				}

				aSGoodsRunTimeData.sGoodsState = eGoodsState_Free;
				break;
			}
		}
	}

	if (TRUE == bIsFind){
		SGoodsRunTimeData& aSGoodsRunTimeData = m_asGoodsRunTimeData[un8GridID];

		for (INT32 i = 0; i < c_un32MaxItemEffectNum; ++i){
			SGoodsEffect& sGoodsEffect = aSGoodsRunTimeData.sGoodsCfg->sGoodsEffect[i];
			if (sGoodsEffect.n32Atrr >= eEffectCate_PhyAttack){
				ChangeFPData((EEffectCate)(sGoodsEffect.n32Atrr), sGoodsEffect.n32Val, sGoodsEffect.n32Percent);
			}
		}

		SynGoodsUseStateToGC(un8GridID, bIsComposed);


		//旧的物品添加被动效果逻辑
		/*
		for(int i = 0; i < c_n32MaxNextSkillEffectCfgNum; ++i){
			INT32 un32PassiveSkillID = aSGoodsRunTimeData.sGoodsCfg->lun32PassiveSkillID[i];
			if(0 != un32PassiveSkillID){
				aSGoodsRunTimeData.ln32PassSkillUniqueID[i] = GetCurBattle()->GetPassiveSkillMng()->AddPassiveSkill(this,un32PassiveSkillID);
			}
		}
		*/
		//新的物品添加被动效果逻辑。如果有重复的被动ID，则不添加
		for(int i = 0; i < c_n32MaxNextSkillEffectCfgNum; ++i){
			INT32 un32PassiveSkillID = aSGoodsRunTimeData.sGoodsCfg->lun32PassiveSkillID[i];
			if(0 != un32PassiveSkillID){
				//检查是否有重复
				bool ifRepeate = FALSE;
				for (UINT8 j = 0; j < c_un32MaxGridNum; ++j){
					if(ifRepeate) break;
					if( j == un8GridID) continue;
					SGoodsRunTimeData& aTempSGoodsRunTimeData = m_asGoodsRunTimeData[j];
					if(aTempSGoodsRunTimeData.n32Num <= 0) continue;
					for( int k = 0; k< c_n32MaxNextSkillEffectCfgNum; ++k){
						INT32 tempPassiveSkillID = aTempSGoodsRunTimeData.sGoodsCfg->lun32PassiveSkillID[k];
						//如果有重复ID，则不添加被动
						if(un32PassiveSkillID == tempPassiveSkillID){
							ifRepeate = TRUE;
							break;
						}
					}
				}
				if(FALSE == ifRepeate){
					INT32 uniID = GetCurBattle()->GetPassiveSkillMgr()->AddPassiveSkill(this,un32PassiveSkillID);
					if(uniID > 0)
						aSGoodsRunTimeData.ln32PassSkillUniqueID[i] = uniID;
				}
			}
		}
		
		return &aSGoodsRunTimeData;
	}

	n32Reflag = eEC_AskBuyGoodsFailForBagFull;
	return NULL;
}

INT32 CSSHero::DecreaseOneGoods(UINT8 un8GridID, EDecreaseReason eReseon){
	if (FALSE == IsValidGrid(un8GridID)){
		return eEC_InvaildGridID;
	}
	if (FALSE == IsStillHasGoods(un8GridID)){
		return eEC_InvaildGridID;
	}

	SGoodsRunTimeData& sGoodsRunTimeData = m_asGoodsRunTimeData[un8GridID];

	if (sGoodsRunTimeData.eGoodsSpecificaFlag == eGoodsSpecificaFlag_NoSpecifical){
		--sGoodsRunTimeData.n32Num;
	}
	else {
		switch (eReseon){
		case EDecreaseReason_Use:
			break;
		case EDecreaseReason_Sell:
		case EDecreaseReason_Compose:
			sGoodsRunTimeData.eGoodsSpecificaFlag = eGoodsSpecificaFlag_NoSpecifical;
			sGoodsRunTimeData.n32Num = 0;
			//新的物品添加被动效果逻辑。如果有重复的被动ID，则不移除
			for(int i = 0; i < c_n32MaxNextSkillEffectCfgNum; i++){
				INT32 un32PassiveSkillUniqueID = sGoodsRunTimeData.ln32PassSkillUniqueID[i];
				INT32 un32PassiveSkillID = sGoodsRunTimeData.sGoodsCfg->lun32PassiveSkillID[i];
				if(0 != un32PassiveSkillUniqueID){
					//检查是否有重复
					bool ifRepeate = FALSE;
					for (UINT8 j = 0; j < c_un32MaxGridNum; ++j){
						if(ifRepeate) break;
						if( j == un8GridID) continue;
						SGoodsRunTimeData& aTempSGoodsRunTimeData = m_asGoodsRunTimeData[j];
						if(aTempSGoodsRunTimeData.n32Num <= 0) continue;
						for(int k = 0; k< c_n32MaxNextSkillEffectCfgNum; ++k){
							INT32 tempPassiveSkillID = aTempSGoodsRunTimeData.sGoodsCfg->lun32PassiveSkillID[k];
							//如果有重复ID，则不添加被动
							if(un32PassiveSkillID == tempPassiveSkillID){
								ifRepeate = TRUE;
								//将id搞到新的重复物品上
								for(int l = 0; l < c_n32MaxNextSkillEffectCfgNum ; ++l){
									if(aTempSGoodsRunTimeData.ln32PassSkillUniqueID[l] == 0){
										aTempSGoodsRunTimeData.ln32PassSkillUniqueID[l] = un32PassiveSkillUniqueID;
										break;
									}
								}
								break;
							}
						}
					}
					if(FALSE == ifRepeate){
						GetCurBattle()->GetPassiveSkillMgr()->RevmovePassiveSkill(un32PassiveSkillUniqueID);
					}
				}
			}
			//新的物品添加被动效果逻辑结束

			if(sGoodsRunTimeData.pSkill && TRUE == sGoodsRunTimeData.pSkill->bIfOpen){
				sGoodsRunTimeData.pSkill->bIfCanCooldown = FALSE;
				sGoodsRunTimeData.pSkill->bIfOpen = FALSE;
				for(int i=0; i<c_n32MaxNextSkillEffectCfgNum; i++){
					INT32 un32PassiveSkillID = sGoodsRunTimeData.pSkill->un32PassitiveSkillArr[i];
					if(0 != un32PassiveSkillID){
						GetCurBattle()->GetPassiveSkillMgr()->RevmovePassiveSkill(un32PassiveSkillID);
					}
				}
			}
			break;
		}
	}

	SynGoodsUseStateToGC(un8GridID, 0);

	if (sGoodsRunTimeData.eGoodsSpecificaFlag != eGoodsSpecificaFlag_InfiniteUse){
		for (INT32 i = 0; i < c_un32MaxItemEffectNum; ++i){
			ChangeFPData((EEffectCate)(sGoodsRunTimeData.sGoodsCfg->sGoodsEffect[i].n32Atrr), sGoodsRunTimeData.sGoodsCfg->sGoodsEffect[i].n32Val, sGoodsRunTimeData.sGoodsCfg->sGoodsEffect[i].n32Percent,FALSE);
		}
	}
	//log
	if(!IfAI()&& !this->GetCurBattle()->IsGuideBattle()){
		stringstream mystream; 
		mystream <<((GetUTCMiliSecond() - this->GetCurBattle()->GetBattleStateStartTime()) / 1000 )<<LOG_SIGN;
		mystream << this->GetUser()->GetGUID() << LOG_SIGN;
		mystream << sGoodsRunTimeData.sGoodsCfg->un32GoodsID ;
		CSSGameLogMgr::GetInstance().AddBattleLog( eLog_BattleSelGoods,this->GetCurBattleID(),this->GetCurBattle()->GetBattleMapId(),mystream.str() );
	}
	return eNormal;
}

SGoodsRunTimeData* CSSHero::HasSameTypeIDGoods(UINT32 un32Type){
	if (0 == un32Type){
		return NULL;
	}

	for (UINT8 i = 0; i < c_un32MaxGridNum; ++i){
		if (FALSE == IsStillHasGoods(i)){
			return NULL;
		}
		SGoodsRunTimeData& aSGoodsRunTimeData = m_asGoodsRunTimeData[i];
		if (NULL != aSGoodsRunTimeData.sGoodsCfg && aSGoodsRunTimeData.sGoodsCfg->un32UniqueTypeID == un32Type){
			return &aSGoodsRunTimeData;
		}
	}

	return NULL;
}

SGoodsRunTimeData* CSSHero::HasSameFunTypeGoods(UINT32 un32Type){
	for (UINT8 i = 0; i < c_un32MaxGridNum; ++i){
		if (FALSE == IsStillHasGoods(i)){
			return NULL;
		}
		SGoodsRunTimeData& aSGoodsRunTimeData = m_asGoodsRunTimeData[i];
		if (NULL != aSGoodsRunTimeData.sGoodsCfg && aSGoodsRunTimeData.sGoodsCfg->un8FunctionType == un32Type){
			return &aSGoodsRunTimeData;
		}
	}

	return NULL;
}

bool CSSHero::IsValidGrid(UINT8 un8GirdID){
	return un8GirdID < c_un32MaxGridNum;
}

bool CSSHero::IsEmptyGrid(UINT8 un8GirdID){
	return NULL == m_asGoodsRunTimeData[un8GirdID].sGoodsCfg ? true : false;
}

bool	CSSHero::IfDamage(GSToGC::HPMPChangeReason eHPCR, INT32 n32HP)
{
	switch(eHPCR){
	case GSToGC::NormalHurt:
	case GSToGC::BloastHurt:
	case GSToGC::SkillHurt:
		return true;
	case GSToGC::RecoverHurt:
	case GSToGC::SkillConsume:
	case GSToGC::BuffEffectReason:{
		if (n32HP < 0){
			return true;
		}
		else{
			return false;
		}
	}
	default:
		return false;
	}
}

CSSSkill*	CSSHero::GetSkillByID(UINT32 un32SkillID){
	if (un32SkillID == 0){
		return NULL;
	}

	CSSSkill *psData = NULL;
	for (INT32 i = 0 ;i < c_n32MaxHeroSkillNum; i++){
		const SSNewSkillCfg *cpsTempSkilCfg = m_aspSkillArr[i]->cpsCfg;
		if (NULL != cpsTempSkilCfg && cpsTempSkilCfg->un32SkillID == un32SkillID){
			return m_aspSkillArr[i];
		}
	}
	for (INT32 i = 0; i < 2; ++i){
		AbsorbRuntimeInfo& sAbsorbRuntimeInfo = m_asAbsorbRuntimeInfo[i];
		if (sAbsorbRuntimeInfo.pSkill->cpsCfg && sAbsorbRuntimeInfo.pSkill->cpsCfg->un32SkillID == un32SkillID){
			return sAbsorbRuntimeInfo.pSkill;
		}
	}
	for (INT32 i = 0; i < c_un32MaxGridNum; ++i){
		SGoodsRunTimeData& sGoodsRuntimeData = m_asGoodsRunTimeData[i];
		if ((sGoodsRuntimeData.n32Num > 0 ||  sGoodsRuntimeData.eGoodsSpecificaFlag ==  eGoodsSpecificaFlag_InfiniteUse)
			&& NULL != sGoodsRuntimeData.pSkill
			&& NULL != sGoodsRuntimeData.pSkill->cpsCfg 
			&& un32SkillID == sGoodsRuntimeData.pSkill->cpsCfg->un32SkillID){
				return sGoodsRuntimeData.pSkill;
		}
	}
	return NULL;
}

INT32	CSSHero::LoadHeroCfg(const SHeroCfg* pcCfg){
	m_cpsHeroCfg = pcCfg;

	for (INT32 i = 0; i < c_n32MaxHeroSkillNum; ++i){
		m_aspSkillArr[i]->Clear();
	}

	for (INT32 i = 0; i < c_n32MaxHeroSkillNum; i++){
		UINT32 un32SkillID = m_cpsHeroCfg->aun32SkillList[i];
		if (0 == un32SkillID){
			continue;
		}
		const SSNewSkillCfg *cpsCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillCfg(un32SkillID);
		if (NULL == cpsCfg){
			ELOG(LOG_WARNNING, " un32SkillID %d cano not find Cfg info.", un32SkillID);
			continue;
		}

		m_aspSkillArr[i]->cpsCfg = cpsCfg;
		m_aspSkillArr[i]->pcMasterGU = (CSSGameUnit*)this;
		m_aspSkillArr[i]->eSkillState = eSkillState_Free;
		m_aspSkillArr[i]->tStateMilsec = 0;

		if (cpsCfg->bIfNomalAttack){
			m_pNormalAttackSkill = m_aspSkillArr[i];
		}
	}

	m_n32CollideRadius = pcCfg->n32CollideRadius;
	if (m_n32CollideRadius < c_n32MiniCollideRadius){
		m_n32CollideRadius = c_n32MiniCollideRadius;
	}

	//加入基础属性
	m_sFpMgr.AddBaseGroup(m_cpsHeroCfg->sBaseFP,TRUE);
	m_sFpMgr.SetValueChangeCallBack(eEffectCate_AttackSpeed,m_pNormalAttackSkill);
	//加满血蓝
	FullHP();
	FullMP();
	return eNormal;
};

void CSSHero::LoadPassitiveSkill(){
	for(int i=0; i < c_n32MaxHeroPassitiveSkillNum; i++){
		UINT32 u32PSkillID = m_cpsHeroCfg->aun32PassitiveSkillList[i];
		if(0 != u32PSkillID){
			GetCurBattle()->GetPassiveSkillMgr()->AddPassiveSkill(this,u32PSkillID);
		}
	}
}

INT32	CSSHero::SyncSkillStateToGC(  UINT32 un32SkillID){
	 
	bool bIsSyned = FALSE;  
	if (0 != un32SkillID){  
		for (UINT32 i = 0; i < (UINT32)c_n32MaxHeroSkillNum; i++){
			if (NULL != m_aspSkillArr[i]->cpsCfg && m_aspSkillArr[i]->cpsCfg->un32SkillID == un32SkillID){
				SyncSkillStateToGC(m_aspSkillArr[i], i);
				bIsSyned = TRUE;
				break;
			}
		}

		if (FALSE == bIsSyned){  
			for (INT32 j = 0; j < 2; ++j){
				if (m_asAbsorbRuntimeInfo[j].pSkill->cpsCfg && m_asAbsorbRuntimeInfo[j].pSkill->cpsCfg->un32SkillID == un32SkillID){
						SyncSkillStateToGC(m_asAbsorbRuntimeInfo[j].pSkill, j + eSkillSlotIdx_SuiteSkill1);
						bIsSyned = TRUE;
						break;
				}
			}
		}
	}else{  
		for (UINT32 i = 0; i < (UINT32)c_n32MaxHeroSkillNum; i++){
			if (NULL != m_aspSkillArr[i]->cpsCfg){
				SyncSkillStateToGC(m_aspSkillArr[i], i);
				bIsSyned = TRUE;
			}
		}

		if (!bIsSyned)
		{
			for (INT32 j = 0; j < 2; ++j){
				if (m_asAbsorbRuntimeInfo[j].pSkill->cpsCfg){
					SyncSkillStateToGC(m_asAbsorbRuntimeInfo[j ].pSkill,  j + eSkillSlotIdx_SuiteSkill1 );
				}
			}
		} 
	}
	return eNormal;
}

INT32	CSSHero::SyncSkillStateToGC(CSSSkill *psSkill, UINT32 un32SkillSlotIdx){
	if (NULL == psSkill){
		return eEC_InvalidSkillID;
	}

	if (NULL == m_piUser){
		return eEC_NullUser;
	}

	GSToGC::NotifySkillInfo sSkillEntityInfo;
	sSkillEntityInfo.set_guid(GetObjGUID());
	sSkillEntityInfo.set_skillslot(un32SkillSlotIdx);
	if ( m_eFuryState == eFuryRunState) 
	{
		if (un32SkillSlotIdx == eSkillSlotIdx_NormalSkill1) 
		{
			sSkillEntityInfo.set_skillid(m_aspSkillArr[eSkillSlotIdx_GasSkill1]->cpsCfg->un32SkillID);
		}else if (un32SkillSlotIdx == eSkillSlotIdx_NormalSkill2) 
		{
			sSkillEntityInfo.set_skillid(m_aspSkillArr[eSkillSlotIdx_GasSkill2]->cpsCfg->un32SkillID);
		}
		else{
			sSkillEntityInfo.set_skillid(psSkill->cpsCfg->un32SkillID);
		}
	}else{
		sSkillEntityInfo.set_skillid(psSkill->cpsCfg->un32SkillID);
	}

	INT64 tDiff = psSkill->tCooldownMilsec - GetUTCMiliSecond();
	if (tDiff < 0){
		tDiff = 0;
	}
	sSkillEntityInfo.set_time(tDiff);
	GetCurBattle()->SendMsgToSelfAndOB(sSkillEntityInfo, sSkillEntityInfo.msgid(), m_piUser);
	return eNormal;
}

FLOAT	CSSHero::GetSight(){
	return 1500.0f;
}

INT32	CSSHero::DoUpgrade(){
	SetLevel(m_sServiceData.HeroLevel + 1);
	m_sFpMgr.AddBaseGroup(m_cpsHeroCfg->sGrowthFP);
	return eNormal;
}

INT32	CSSHero::CheckUpgradeSkill(){
	for (INT32 i = 0; i < c_n32MaxHeroSkillNum; i++){
		if (NULL == m_aspSkillArr[i]->cpsCfg){
			continue;
		}

		UINT32 un32ThisSkillID = m_aspSkillArr[i]->cpsCfg->un32SkillID;

		const SSNewSkillCfg *cpsUpSkillCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillFromOriSkillAndNewLevel(un32ThisSkillID, m_sServiceData.HeroLevel);
		if (NULL != cpsUpSkillCfg && cpsUpSkillCfg != m_aspSkillArr[i]->cpsCfg){
			m_aspSkillArr[i]->cpsCfg = cpsUpSkillCfg;
			GSToGC::NotifySkillUpLv sNotifySkillUpLv;
			sNotifySkillUpLv.set_guid(GetObjGUID());
			sNotifySkillUpLv.set_skillpos(i);
			sNotifySkillUpLv.set_skillid(m_aspSkillArr[i]->cpsCfg->un32SkillID);
			GetCurBattle()->SendMsgToSelfAndOB(sNotifySkillUpLv, sNotifySkillUpLv.msgid(), m_piUser);
		}
	}
#if 0
	//AbsorbRuntimeInfo			m_asAbsorbRuntimeInfo[c_n32MaxAbsorbSkillNum];
	for (INT32 j =0; j < c_n32MaxAbsorbSkillNum; j++)
	{ 
		if (m_asAbsorbRuntimeInfo[j].sSkillRunTimeData.cpsCfg == NULL)
		{
			continue;
		}
		UINT32 nSkillId = m_asAbsorbRuntimeInfo[j].sSkillRunTimeData.cpsCfg->un32SkillID;
		const SSNewSkillCfg *cpsUpSkillCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillFromOriSkillAndNewLevel(nSkillId, m_un32Level);
		if (NULL != cpsUpSkillCfg && cpsUpSkillCfg != m_asAbsorbRuntimeInfo[j].sSkillRunTimeData.cpsCfg){
			m_asAbsorbRuntimeInfo[j].sSkillRunTimeData.cpsCfg = cpsUpSkillCfg;
		
			DefineMsgBuilder_Mini
			tMsgBuilder.GetMsgHeader()->n32ProtocolID = eMsgToGCFromGS_NotifySkillUpLv;
			tMsgBuilder.AddData(&GetObjGUID(),sizeof(SGUID_KEY));
			tMsgBuilder.AddINT32(j);
			tMsgBuilder.AddINT32(m_asAbsorbRuntimeInfo[j].sSkillRunTimeData.cpsCfg->un32SkillID); 
			//////GetCurBattle()->SendMsgToSelfAndOB(tMsgBuilder.GetMsgHeader(), m_piWatchUser);
 		}
	}
#endif

	for(int i = EPassiveSkillTriggerType_HeartBeat; i< EPassiveSkillTriggerType_Max; ++i){
		PassiveSkillArr& passiveVecBefore = GetPassiveSkillVec((EPassiveSkillTriggerType)i);
		for (INT32* pEle = passiveVecBefore.Begin(); pEle != passiveVecBefore.End(); pEle = passiveVecBefore.Next()){
			GetCurBattle()->GetPassiveSkillMgr()->TryUpgradeSkill(*pEle, m_sServiceData.HeroLevel);
		}
	}
	return eNormal;	
}

INT32	CSSHero::ChangeExp(INT32 n32ExpNum){
	if (m_sServiceData.HeroLevel >= c_un32MaxHeroLevel){
		return eNormal;
	}

	if (IsDead()){
		return eNormal;
	}
	m_sServiceData.CurLvExp += n32ExpNum;
	INT32 n32UpgradeExp = m_cpsHeroCfg->n32BaseExp + (m_sServiceData.HeroLevel - 1) * m_cpsHeroCfg->n32ExpGrowth;
	while (m_sServiceData.CurLvExp >= n32UpgradeExp){
		m_sServiceData.CurLvExp -= n32UpgradeExp;
		DoUpgrade();
		n32UpgradeExp = m_cpsHeroCfg->n32BaseExp + (m_sServiceData.HeroLevel - 1) * m_cpsHeroCfg->n32ExpGrowth;
	}

	SyncExpInfoToGC();
	return eNormal;
}

INT32	CSSHero::SyncExpInfoToGC(){
	if (NULL == m_piUser){
		return eEC_NullMasterUser;
	}

	GSToGC::Exp sExp;
	sExp.set_guid(GetObjGUID());
	sExp.set_exp(m_sServiceData.CurLvExp);
	GetCurBattle()->SendMsgToSelfAndOB(sExp, sExp.msgid(), m_piUser); 
	return eNormal;
}

INT32	CSSHero::BroadcastLvInfoToGC(){
	GSToGC::LevelInfo sLevelInfo;
	sLevelInfo.set_guid(GetObjGUID());
	sLevelInfo.set_level(m_sServiceData.HeroLevel);
	GetCurBattle()->BroadMsgToGC(sLevelInfo, sLevelInfo.msgid());
	return eNormal;
}

INT32 CSSHero::AskRemoveAbsorbSkill(UINT32 un32MonsterID){
	size_t i = 0;
	for (; i < c_n32MaxAbsorbSkillNum; ++i){
		if (NULL != m_asAbsorbRuntimeInfo[i].sNPCCfg && un32MonsterID == m_asAbsorbRuntimeInfo[i].sNPCCfg->eOT){
			
			ChangeFPInAbsorb(*m_asAbsorbRuntimeInfo[i].sNPCCfg, FALSE);

			UINT32 un32TmpMonsterIDArr[2] = {0};
			AbsorbRuntimeInfo& curAbsorbRuntimeInfo = m_asAbsorbRuntimeInfo[i];
			if (m_asAbsorbRuntimeInfo[1-i].sNPCCfg){
				un32TmpMonsterIDArr[1-i] = m_asAbsorbRuntimeInfo[1-i].sNPCCfg->eOT;
			}
			GetCurBattle()->GetEffectMgr()->RemoveAllEffectBySkillID(curAbsorbRuntimeInfo.pSkill->cpsCfg->un32SkillID);
			curAbsorbRuntimeInfo.Clear();

			return SynAbsobInfoToGC(1, un32TmpMonsterIDArr[0], un32TmpMonsterIDArr[1]);
		}
	}

	if (c_n32MaxAbsorbSkillNum == i){
		return eEC_RemoveAbsorbSkillFailed;
	}
	
	return eNormal;
}

INT32 CSSHero::ChangeFPInAbsorb(const SNPCCfg& pNPCCfg, bool bAdd){
	for (INT32 i = eEffectCate_None; i < eEffectCate_Restrain + 1; ++i){
		INT32 n32ChangeNum = pNPCCfg.n32AbsorbAttriChange[i];
		INT32 n32ConsumePer = pNPCCfg.n32ConsumePer[i];
		ChangeFPData((EEffectCate)i, n32ChangeNum, n32ConsumePer, bAdd);
	}
	return eNormal;
}


void	CSSHero::ChangeFuryValue(EFuryCostType eType, INT32 n32FURY){ 
	switch (eType)
	{
	case eFuryUseAll:
		m_sServiceData.CurFuryValue = 0;
		break;
	case eFuryAdd:
		{
			if(eFuryNullState != m_eFuryState)
				return;
			m_sServiceData.CurFuryValue += n32FURY;
			if(m_sServiceData.CurFuryValue > c_un32PerFuryValue)
				m_sServiceData.CurFuryValue = c_un32PerFuryValue;
			if(c_un32PerFuryValue == m_sServiceData.CurFuryValue)
				ChangeFuryState(eFuryFullState);
		}
		break;
	default:
		break;
	}
	SyncFuryValueToGC(); 
}

INT32	CSSHero::SyncFuryValueToGC(){
	GSToGC::FuryVal sFury;
	sFury.set_guid(GetObjGUID());
	sFury.set_fury(m_sServiceData.CurFuryValue);

	GetCurBattle()->SendMsgToSelfAndOB(sFury, sFury.msgid(), m_piUser);
	return eNormal;
}


void CSSHero::ChangeFuryState(EFuryState state){
	if(m_eFuryState != state){
		m_eFuryState = state;
		if(m_eFuryState == eFuryRunState){
			m_bCheckFuryTimeTag = TRUE;
			ResetSelfSkillCoolDown();
			m_tFuryMilsec = GetUTCMiliSecond();
		}
		
		NotifyFuryStateToCl(); 
	}
} 


INT32 CSSHero::GasExpersion(  ){
	if(m_eFuryState == eFuryRunState){
		return eEC_UseSkillGasHasInRunState;
	}
	if(m_sServiceData.CurFuryValue < c_un32PerFuryValue){
		return eEC_GasExplosionNotFull;
	}

	ChangeFuryState(eFuryRunState);
	ChangeFuryValue(eFuryUseAll,0); 
 
	SyncSkillStateToGC(0); 
	return eNormal;
}

void CSSHero::CheckFuryRunEnd(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan)
{
	TIME_TICK tTick = tUTCMilsec - m_tFuryMilsec;
	if(tTick >= c_un32FuryRunTime){
		ResetSelfSkillCoolDown();
		ChangeFuryState(eFuryNullState);
		m_sServiceData.CurFuryValue = 0;
		m_bCheckFuryTimeTag = FALSE;
		
	}
}

bool CSSHero::IsGasSkill( UINT32 skillId )
{
	const SSNewSkillCfg	*cpsCfg1 = m_aspSkillArr[eSkillSlotIdx_GasSkill1]->cpsCfg;
	if (cpsCfg1 && cpsCfg1->un32SkillID == skillId){
		return TRUE;
	}
	const SSNewSkillCfg	*cpsCfg2 = m_aspSkillArr[eSkillSlotIdx_GasSkill2]->cpsCfg;
	if (cpsCfg2 && cpsCfg2->un32SkillID == skillId){
		return TRUE;
	}

	return FALSE;
}

 EFuryState CSSHero::GetFuryState()
{
	return m_eFuryState;
}

EHeroKillTitle CSSHero::GetCurTitel()
{
	ELOG(LOG_DBBUG, "%d.",m_sServiceData.ContTitelKills);
	switch(m_sServiceData.ContTitelKills){
	case 0:
	case 1:
	case 2:
		return eNoneTitel;
	case 3:
		return eKillingSpree;
	case 4:
		return eDominating;
	case 5:
		return eMegaKill;
	case 6:
		return eUnstoppable;
	case 7:
		return eWhickedSick;
	case 8:
		return eMonsterKill;
	case 9:
		return eGodLike;
	case 10:
		return eHolyShit;
	default:
		return eHolyShit;
	}
}

INT32	CSSHero::GetEndTitleCP(EHeroKillTitle title)
{
	switch(title){
	case eNoneTitel:
		return 0;
	case eKillingSpree:
		return c_n32EndKillingSpreeReward;
	case eDominating:
		return c_n32EndDomanatingReward;
	case eMegaKill:
		return c_n32EndMegaKillReward;
	case eUnstoppable:
		return c_n32EndUnstoppableReward;
	case eWhickedSick:
		return c_n32EndWhickedSickReward;
	case eMonsterKill:
		return c_n32EndMonsterKillReward;
	case eGodLike:
		return c_n32EndGodLikeReward;
	case eHolyShit:
		return c_n32EndHolyShitReward;
	default:
		ELOG(LOG_ERROR, "Unknow EHeroKillTitle.");
		return 0;
	}
}

CSSHero* CSSHero::GetLastValidHeroHurt()
{
	CSSHero* tempHero = NULL;
	UINT64 tempTime = c_MaxValidBetween;
	TIME_MILSEC CurTime = GetUTCMiliSecond();
	for (map<UINT64, sRecentlyHurt>::iterator iter = m_sRecentlyHurtbyHeroMap.begin();
		iter != m_sRecentlyHurtbyHeroMap.end();iter++){
			ELOG(LOG_DBBUG, "CurTIme:%d, HurtTime:%d.", CurTime, iter->second.tHurtTime);
			if (tempTime > CurTime - iter->second.tHurtTime){
					tempTime = CurTime - iter->second.tHurtTime;
					tempHero = iter->second.pHero;
			}
	}
	return tempHero;
}

 INT32 CSSHero::CheckUseGasSkill( UINT32 skillId )
 {  
	if(m_eFuryState != eFuryRunState)
		return eEC_UseSkillGasNotInRunState;
	 return eNormal;
 }

 INT32 CSSHero::SynAbsobInfoToGC(UINT8 un8Res, UINT32 un32MonsterID1, UINT32 un32MonsterID2){
	CSSBattle *pcCurBattle = GetCurBattle();
	if (NULL != pcCurBattle){
		GSToGC::AbsorbRes sMsg;
		sMsg.set_guid(GetObjGUID());
		sMsg.set_res(un8Res);
		sMsg.set_absorb1(un32MonsterID1);
		sMsg.set_absorb2(un32MonsterID2);
	 
		pcCurBattle->SyncMsgToGC(sMsg, sMsg.msgid(), GetObjGUID(), this, NULL);  
	}

	return eNormal;
 }

 UINT32 CSSHero::GetAbsorbMonsterID(INT32 n32Index){

	 AbsorbRuntimeInfo& sAbsorbRuntimeInfo = m_asAbsorbRuntimeInfo[n32Index];
	 if (sAbsorbRuntimeInfo.sNPCCfg){
		 return sAbsorbRuntimeInfo.sNPCCfg->eOT;
	 }

	 return 0;
 }

 void CSSHero::ResetSelfSkillCoolDown()
 {
	 for (UINT32 i = 0; i < 4; i++){
		 if (NULL != m_aspSkillArr[i]->cpsCfg && eSkillState_End == m_aspSkillArr[i]->eSkillState ){
			 m_aspSkillArr[i]->eSkillState = eSkillState_Free;
			 SyncSkillStateToGC(m_aspSkillArr[i]->cpsCfg->un32SkillID);
		 }			
	 }
 }
  

 void CSSHero::RecoverPlayerCp( TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan )
 {
	 TIME_TICK timeTick = tUTCMilsec - m_tRecoverCpMilsec;
	 if(timeTick >= c_un32RecoverCpTime){
		 m_tRecoverCpMilsec = tUTCMilsec;
 		 ChangeCP(eCPCP_Personnal, m_cpsHeroCfg->n32CPRecover, false);
	 }
 }

void CSSHero::StartRecoverCp(){
	m_bStartRecoverCpTag = TRUE;
	m_tRecoverCpMilsec = GetUTCMiliSecond();
}

INT32 CSSHero::SynGoodsUseStateToGC(UINT8 un8GridID, UINT8 un8IfComposed){
	 SGoodsRunTimeData& oriGoodRuntimeData = m_asGoodsRunTimeData[un8GridID];
	 TIME_MILSEC curTime = GetUTCMiliSecond();
	 INT64 n32RemainCoolTime = 0;
	 if (oriGoodRuntimeData.pSkill){
		 n32RemainCoolTime = oriGoodRuntimeData.pSkill->tCooldownMilsec - curTime;
		 if (n32RemainCoolTime < 0){
			 n32RemainCoolTime = 0; 
		 }
	 }

	 UINT32 un32TypeId = 0;
	 if (oriGoodRuntimeData.sGoodsCfg){
		 un32TypeId = oriGoodRuntimeData.sGoodsCfg->eOT;
	 }

	 GSToGC::NotifyGoodsInfo sMsg;
	 sMsg.set_guid(GetObjGUID());
	 for (UINT8 i = 0; i < c_un32MaxGridNum; ++i){
		 if (i != un8GridID && IsStillHasGoods(i) && oriGoodRuntimeData.sGoodsCfg && m_asGoodsRunTimeData[i].sGoodsCfg->un32SkillID == oriGoodRuntimeData.sGoodsCfg->un32SkillID){
			 SGoodsRunTimeData& sGoodsRunTimeData = m_asGoodsRunTimeData[i];
			 GSToGC::NotifyGoodsInfo_GoodsInfo* pInfo = sMsg.add_info();
			 pInfo->set_pos(i);
			 pInfo->set_ifcomposed(0);
			 pInfo->set_num(sGoodsRunTimeData.n32Num);
			 pInfo->set_tyepid(m_asGoodsRunTimeData[i].sGoodsCfg->eOT);
			 pInfo->set_statetime(n32RemainCoolTime);
		 }
	 }

	 GSToGC::NotifyGoodsInfo_GoodsInfo* pInfo = sMsg.add_info();
	 pInfo->set_pos(un8GridID);
	 pInfo->set_ifcomposed(un8IfComposed);
	 pInfo->set_num(oriGoodRuntimeData.n32Num);
	 pInfo->set_tyepid(un32TypeId);
	 pInfo->set_statetime(n32RemainCoolTime);

	 ELOG(LOG_DBBUG, "un8GridID:%d, num:%d, eOT:%d n32RemainCoolTime:%d", un8GridID, oriGoodRuntimeData.n32Num, un32TypeId, n32RemainCoolTime);
	 
	GetCurBattle()->SendMsgToSelfAndOB(sMsg, sMsg.msgid(), m_piUser);
	return eNormal;
 }

 bool CSSHero::IsStillHasGoods(UINT8 un8GridID){
	SGoodsRunTimeData& pGoodsRuntime = m_asGoodsRunTimeData[un8GridID];
	if (NULL == pGoodsRuntime.sGoodsCfg){
		return FALSE;
	}

	if (pGoodsRuntime.bIsComposing){
		return pGoodsRuntime.n32Num > 1;
	}

	return !(pGoodsRuntime.eGoodsSpecificaFlag == eGoodsSpecificaFlag_NoSpecifical && pGoodsRuntime.n32Num == 0);
 }

 INT32 CSSHero::AskUseSkillInGoods(SGoodsCfg& sCfg, const SSNewSkillCfg* pSkillCfg){
	return AskUseSkill(sCfg.un32SkillID, eGOOrderPriority_UserAsk);
 }

 SGoodsRunTimeData* CSSHero::HasSameEOTIDGoods(UINT32 eOT, UINT8& un8GridID){
	 if (0 == eOT){
		 return NULL;
	 }

	 for (UINT8 i = 0; i < c_un32MaxGridNum; ++i){
		 SGoodsRunTimeData& aSGoodsRunTimeData = m_asGoodsRunTimeData[i];
		 if (IsStillHasGoods(i) && NULL != aSGoodsRunTimeData.sGoodsCfg && aSGoodsRunTimeData.sGoodsCfg->eOT == eOT){
			 un8GridID = i;
			 return &aSGoodsRunTimeData;
		 }
	 }

	 return NULL;
 }

 SGoodsRunTimeData* CSSHero::HasSameEOTIDGoods(UINT32 eOT){
	 if (0 == eOT){
		 return NULL;
	 }

	 for (UINT8 i = 0; i < c_un32MaxGridNum; ++i){
		 SGoodsRunTimeData& aSGoodsRunTimeData = m_asGoodsRunTimeData[i];
		 if (IsStillHasGoods(i) && aSGoodsRunTimeData.sGoodsCfg && aSGoodsRunTimeData.sGoodsCfg->eOT == eOT){
			 return &aSGoodsRunTimeData;
		 }
	 }

	 return NULL;
 }

void CSSHero::HasSameEOTIDGoods(UINT32 eOT, set<UINT8>& un8GridIDSet){
	 if (0 == eOT){
		 return;
	 }

	 for (UINT8 i = 0; i < c_un32MaxGridNum; ++i){
		 SGoodsRunTimeData& aSGoodsRunTimeData = m_asGoodsRunTimeData[i];
		 if (IsStillHasGoods(i) && NULL != aSGoodsRunTimeData.sGoodsCfg && aSGoodsRunTimeData.sGoodsCfg->eOT == eOT){
				un8GridIDSet.insert(i);
		 }
	 }
 }

 void	CSSHero::SetHeroKillState()
 {
	 TIME_MILSEC tempTime = GetUTCMiliSecond();
	 if (0 == m_tConKillMilTime){
		 m_tConKillMilTime  = tempTime;
		 AddContinuousKill();
	 }
	 else{
		 TIME_MILSEC tSpanTime = tempTime - m_tConKillMilTime;
		 if(tSpanTime < c_MaxConKillBetween ){
			 //如果在时间段内连续击杀多个，则为最高连续击杀
			 if (m_n16ContinuousKill < c_MaxContinueKills){
				 AddContinuousKill();
			 }
		 }
		 else{
			 ResetContinuousKill();
		 }
		 m_tConKillMilTime = tempTime;
	 }
	 CSSBattle *pcCurBattle =GetCurBattle();
	 if (pcCurBattle->GetIfFirsetBlood())
	 {
		 pcCurBattle->SetIfFirstBloodState(false);
		 m_eHeroKillState = eFirstBoold;
	 }
	 else{
		 if (1 == m_n16ContinuousKill)
		 {
			 m_eHeroKillState = eNormalKill;
		 }
		 else if (2 == m_n16ContinuousKill)
		 {
			 m_eHeroKillState = eDoubleKill;
		 }
		 else if (3 == m_n16ContinuousKill)
		 {
			 m_eHeroKillState = eTripleKill;
		 }
	 }
 }

bool CSSHero::TryDownCompose(SGoodsCfg* pGoodsCfg, INT32& n32CPCost, set<UINT8>& toDecGirdVec){
	if (NULL == pGoodsCfg){
		return FALSE;
	}

	 GetDownComposeRunTimsGoods(pGoodsCfg, toDecGirdVec);

	 if(TRUE != toDecGirdVec.empty()){
		 INT32 n32CPCostReduce = 0;

		 for (set<UINT8>::iterator iter = toDecGirdVec.begin(); iter != toDecGirdVec.end(); ++iter){
			 SGoodsRunTimeData& aSGoodsRunTimeData = m_asGoodsRunTimeData[*iter];
			 n32CPCostReduce += aSGoodsRunTimeData.sGoodsCfg->n32CPCost;
		 }
		 n32CPCost = pGoodsCfg->n32CPCost - n32CPCostReduce;

		 return TRUE;
	 }

	 return FALSE;
 }

void CSSHero::InitLevelByUserName(const char* name)
{
#ifdef USE_TEST
	if (!strcmp(name,"test3") || !strcmp(name,"test4")) m_un32Level = 7;
	else if (!strcmp(name,"test5") || !strcmp(name,"test6")) m_un32Level = 15;
#endif
}
void CSSHero::SetLevel(UINT32 lv){
	if (m_sServiceData.HeroLevel >= lv){
		return;
	}

	m_sServiceData.HeroLevel = lv;

	BroadcastLvInfoToGC();

	CheckUpgradeSkill();
}
 

void		CSSHero::AddKillNPC()
{
	++m_n64KillNPC;
	GSToGC::NotifyKillNPC sMsg;
	sMsg.set_guid(GetObjGUID());
	sMsg.set_killnum(m_n64KillNPC);

	GetCurBattle()->SendMsgToSelfAndOB(sMsg, sMsg.msgid(), m_piUser);
}


 
 
void		CSSHero::NotifyFuryStateToCl()
{
	CSSBattle *pcCurBattle = GetCurBattle();
	GSToGC::FuryState sMsg;
	sMsg.set_guid(GetObjGUID());
	sMsg.set_state(m_eFuryState);
	pcCurBattle->SyncMsgToGC(sMsg, sMsg.msgid(), GetObjGUID(),this, NULL); 
}

INT32	CSSHero::UserAskReconnect()
{
	GetHeroGoodsInfo();
	SyncSkillStateToGC(0);
	SyncFuryValueToGC();
	NotifyFuryStateToCl();
	
	INT32 n32MosterId[2] = {0};
	for (INT32 i = 0; i < c_n32MaxAbsorbSkillNum; i ++){
		if(NULL != m_asAbsorbRuntimeInfo[i].sNPCCfg){
			n32MosterId[i] = m_asAbsorbRuntimeInfo[i].sNPCCfg->eOT;
		}
	}

	SynAbsobInfoToGC(1, n32MosterId[0], n32MosterId[1]);
	return eNormal;
}

INT32	CSSHero::GetHeroGoodsInfo()
{
	CSSBattle* pBattle = GetCurBattle();
	INT32 tSpanTime = GetUTCMiliSecond() - pBattle->GetBattleStateStartTime();
	GSToGC::NotifyGoodsInfo sMsg;
	sMsg.set_guid(GetObjGUID());
	 
	TIME_MILSEC curTime = GetUTCMiliSecond();
	TIME_MILSEC n64RemainCoolTime = 0;
	for(INT32 i = 0; i < c_un32MaxGridNum; i ++){
		if (NULL != m_asGoodsRunTimeData[i].sGoodsCfg){
			n64RemainCoolTime = m_asGoodsRunTimeData[i].pSkill->tCooldownMilsec - curTime;
			if (n64RemainCoolTime < 0){
				n64RemainCoolTime = 0;
			}
			GSToGC::NotifyGoodsInfo::GoodsInfo* pInfo = sMsg.add_info();
			pInfo->set_pos(i);
			pInfo->set_tyepid(m_asGoodsRunTimeData[i].sGoodsCfg->eOT);
			pInfo->set_num(m_asGoodsRunTimeData[i].n32Num);
			pInfo->set_statetime(n64RemainCoolTime);
		}
	}
	GetCurBattle()->SendMsgToSelfAndOB(sMsg, sMsg.msgid(), m_piUser);
	return eNormal;
}

CSSGameUnit* CSSHero::LookForEnemy(){
	CSSBattle *pcCurBattle = GetCurBattle();

	FLOAT fGOSight = GetSight();
	CSSSkill* pSkill = GetNormalAttackSkill_NEW();
	if (NULL != pSkill){
		fGOSight = GetFPData(eEffectCAte_AttackDist);
	}

	CSSGameUnit *pcLockedGO = pcCurBattle->GetGameUnitByGUID(GetLockedTargetGUID());
	if (NULL == pcLockedGO){
		return NULL;
	}

	if (GetCurPos().CanWatch(fGOSight, pcLockedGO->GetCurPos())){
		return pcLockedGO;
	}

	return NULL;
}

INT32 CSSHero::ChangeCurHP(CSSGameUnit *pcReasonGO, GSToGC::HPMPChangeReason eHPCR, INT32 n32ChangeValue,INT32 skillId , EEffectCate	eE){
	//如果伤害来源是英雄，将其加入到伤害来源列表中
	CSSHero* pReasonHero = NULL;
	if (NULL != pcReasonGO){
		pReasonHero = pcReasonGO->TryCastToHero();
		if (NULL == pReasonHero && pcReasonGO->IfNPC() && FALSE == pcReasonGO->GetFatherHeroGUID() > 0){
			CSSGameUnit* pGO = pcReasonGO->GetCurBattle()->GetGameUnitByGUID(pcReasonGO->GetFatherHeroGUID());
			if (NULL != pGO){
				pReasonHero = pGO->TryCastToHero();
			}
		}

		if (IfDamage(eHPCR, n32ChangeValue) && IfEnemy(pcReasonGO)){
			if(NULL != pReasonHero){
				sRecentlyHurt temp;
				temp.tHurtTime = GetUTCMiliSecond();
				ELOG(LOG_SpecialDebug, "Hero hit %d.", temp.tHurtTime);
				temp.pHero = pReasonHero;
				m_sRecentlyHurtbyHeroMap[pReasonHero->GetObjGUID()] = temp;
			}
			//刘路加入，记录伤害原因
			SHurtRecord hurtRecord;
			hurtRecord.pHurterGUID = pcReasonGO->GetObjGUID();
			hurtRecord.tHurtTime = GetUTCMiliSecond();
			hurtRecord.n32HurtValue = -n32ChangeValue;
			m_hurtRecordVec.push_back(hurtRecord);
		}
	}

	INT32 n32Res = DoChangeHP(n32ChangeValue, eHPCR, pcReasonGO,this);
	if (eEC_NullPointer == n32Res){
		return eNormal;
	}
	if (eEC_GUDead != n32Res){
		return eNormal;
	}

	AddSSendData_DeadTimes();
	m_n16ContinuousKill = 0;

	if (NULL == pReasonHero || pReasonHero->GetObjGUID() == GetObjGUID()){
		pReasonHero = GetLastValidHeroHurt();
	}

	if (pReasonHero == NULL)
	{
		ELOG(LOG_WARNNING, "Not Find killer hero.");
	}

	INT8 n8HeroKillstate = 0;
	INT32 n32HeroKillTitle = 0;
	INT32 n32GotCP =  1000 * (c_n32BaseCP + (GetLevel() -1) * c_n32CoefficientCP);
	if (pReasonHero && IfEnemy(pReasonHero)){
		NotifyKillHeroMsgToGC(pReasonHero->GetObjGUID(), pReasonHero);
		pReasonHero->SetHeroKillState();
		pReasonHero->AddTitelkills();
		pReasonHero->ChangeCP(eCPCP_Personnal, n32GotCP, TRUE);
		pReasonHero->ChangeFuryValue(eFuryAdd, c_un32HeroGivenFury);
		n8HeroKillstate = pReasonHero->GetHeroKillState();
		n32HeroKillTitle = pReasonHero->GetCurTitel();
		ELOG(LOG_SpecialDebug, "User CurTitle:%d.", n32HeroKillTitle);

		INT32 n32EndtitleCP = GetEndTitleCP(GetCurTitel());
		if (0 < n32EndtitleCP){
			pReasonHero->ChangeCP(eCPCP_Personnal, n32EndtitleCP, TRUE);
			ELOG(LOG_SpecialDebug, "Got End title CP:%d.", n32EndtitleCP);
		}
	}

	ResetTitelkills();
	INT8 n8Aced = GetCurBattle()->CheckIfAced(GetCampID());
	EGameObjectCamp sReasonCamp = pcReasonGO->GetCampID();
	if (pReasonHero){
		sReasonCamp = pReasonHero->GetCampID();
	}

	GSToGC::CurDeadTimes sCurDeadTimes;
	sCurDeadTimes.set_objguid(GetObjGUID());
	sCurDeadTimes.set_deadtimes(GetSSendData().un32DeadTimes);
	sCurDeadTimes.set_killer_camp(sReasonCamp);
	sCurDeadTimes.set_ifaced(n8Aced);
	sCurDeadTimes.set_camp(GetCampID());
	if (pReasonHero){
		sCurDeadTimes.set_reasonheroguid(pReasonHero->GetObjGUID());
	}
	sCurDeadTimes.set_herostate(n8HeroKillstate);
	sCurDeadTimes.set_herotitle(n32HeroKillTitle);
	GetCurBattle()->BroadMsgToGC(sCurDeadTimes, sCurDeadTimes.msgid());

	CalculateKillResult(pReasonHero);
	CalculateAssit(pReasonHero);


	//log
	if (n32ChangeValue < 0 && !IfAI())
	{
		CelObjHurtLog( pcReasonGO,   skillId,n32ChangeValue, eE);
	} 

	return eNormal;
}

INT32	CSSHero::CalculateKillResult(CSSHero* pcReason)
{
	INT32 n32GotTotalExp = c_n32BaseExp + c_n32CoefficientExp * pow((GetLevel() - 1), c_fLvPowerExp);
	set<CSSHero*> sHeroSet;
	CSSHero *apcHeroList[c_un32MaxUserInBattle];
	INT32 n32EnemyAround  = GetCurBattle()->GetAroundEnemyHero(GetCurPos(), 2500, GetCampID(), apcHeroList, c_un32MaxUserInBattle);
	bool bIfHasCaculatedReasonHero = FALSE;
	for (INT32  i = 0; i < n32EnemyAround; i++){
		sHeroSet.insert(apcHeroList[i]);
	}

	if (NULL != pcReason){
		sHeroSet.insert(pcReason);
	}

	INT32 n32GotExp = n32GotTotalExp / FLOAT(sHeroSet.size());
	for (set<CSSHero*>::iterator iter = sHeroSet.begin(); iter != sHeroSet.end(); ++iter){
		CSSHero* pTempEnemyHero = *iter;
		pTempEnemyHero->ChangeExp(n32GotExp);
	}

	if (NULL != pcReason){
		sHeroSet.erase(pcReason);
	}

	if (sHeroSet.empty()){
		return eNormal;
	}

	INT32 n32GotCP = 0;
	if (sHeroSet.size() == 1){
		n32GotCP = 1000 * (c_n32RangeOneBaseCP + (GetLevel() -1) * c_n32RangeOneCoefCP);
	}
	else{
		n32GotCP = 1000 * (c_n32RangeTwoBaseCP + (GetLevel() -1) * c_n32RangeTwoCoefCP);
	}

	for (set<CSSHero*>::iterator iter = sHeroSet.begin(); iter != sHeroSet.end(); ++iter){
		//(*iter)->ChangeExp(n32GotExp);
		(*iter)->ChangeCP(eCPCP_Personnal, n32GotCP, TRUE);
	}

	return eNormal;
}

INT32	CSSHero::BeginAction_Dead(bool bIfNotifyGC, CSSGameUnit *pcReasonGO){
	m_sCurOASI.eOAS = eOAS_Dead;
	m_sCurOASI.tASUTCMilsec = GetUTCMiliSecond();
	CSSBattle *pBattle = GetCurBattle(); 
	pBattle->SetCaseDeadReasonInfo(this, pcReasonGO); 

	if (NULL == pcReasonGO){ 
		return eNormal;
	}

	if (eSSBS_Finished != pBattle->GetBattleState()){
		SetHeroLastDeadMilsec(GetUTCMiliSecond());

		GSToGC::RebornTimes sMsg;
		sMsg.set_masterguid(GetObjGUID());
		sMsg.set_sendtimes(GetHeroLastDeadMilsec());
		sMsg.set_gold(GetBuyRebornCost());
		sMsg.set_reborn_time(GetHeroRebornTimes());
		sMsg.set_remain_times(GetFPData(eEffectCate_ReliveTime));

		GetCurBattle()->SendMsgToSelfAndOB(sMsg, sMsg.msgid(), m_piUser);
	}
	if(bIfNotifyGC)
	{
		pBattle->SyncState(this);
	} 

	m_CurEnemyGUID = 0;
	m_CurAtkGUID = 0;   
	
	//log
	CelObjDeadLog(pcReasonGO);
	return eNormal;
}

void	CSSHero::CalculateAssit(CSSHero* pHero)
{
	if (NULL != pHero){
		map<UINT64, sRecentlyHurt>::iterator iter_f =	m_sRecentlyHurtbyHeroMap.find(pHero->GetObjGUID());
		if (m_sRecentlyHurtbyHeroMap.end() != iter_f){
			m_sRecentlyHurtbyHeroMap.erase(iter_f);
		}
	}
	TIME_MILSEC tempTime = GetUTCMiliSecond();
	map<UINT64, sRecentlyHurt>::iterator iter =	m_sRecentlyHurtbyHeroMap.begin();
	for (; iter != m_sRecentlyHurtbyHeroMap.end(); iter ++){
		if (c_MaxAssKillBetween > tempTime - iter->second.tHurtTime){
			iter->second.pHero->AddAssistKill();
			GSToGC::HeroAssist Assit;
			Assit.set_guid(iter->second.pHero->GetObjGUID());
			Assit.set_assist(iter->second.pHero->GetAssistNum());
			if(iter->second.pHero->GetUser() != NULL)
				CSSWorkThreadMgr::GetInstance().PostMsgToGC(iter->second.pHero->GetUser()->GetUserNetInfo(), Assit, Assit.msgid());
		}
	}
}

void	CSSHero::SyncCpToCl( )
{   
	GSToGC::CurCP sCurCP;
	sCurCP.set_targetguid(GetObjGUID());
	sCurCP.set_person_cp(m_sServiceData.PersonCP);
	sCurCP.set_team_cp(m_sServiceData.TeamCP);
	GetCurBattle()->SendMsgToSelfAndOB(sCurCP, sCurCP.msgid(), m_piUser);
}

void CSSHero::OnAppearInSight(std::vector<CSSUser*>& cTempUserMap){
	SendAppearMsg(cTempUserMap, m_cpsHeroCfg->eOT);

	SendHPMPMsg(cTempUserMap);

	GSToGC::NotifyHeroInfo sNotifyHeroInfo;
	sNotifyHeroInfo.set_guid(GetObjGUID());
	sNotifyHeroInfo.set_level(GetLevel());
	sNotifyHeroInfo.set_fury(GetFuryState());
	sNotifyHeroInfo.set_exp(GetExp());
	sNotifyHeroInfo.set_absorb1(GetAbsorbMonsterID(0));
	sNotifyHeroInfo.set_absorb2(GetAbsorbMonsterID(1));

	SendMsgToWatchersByType(eSightType_All, sNotifyHeroInfo, sNotifyHeroInfo.msgid(), cTempUserMap);
	GetCurBattle()->SyncState(this,cTempUserMap);	//英雄突然出现Bug修正，出现前同步State。ByCH20140925
}

void CSSHero::OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap){
	//特殊处理：如果是英雄死亡，则不发Disappear，让其躺尸
	if(IsDead()){
		return;
	}

	SendDisppearMsg(cTempUserMap);
}
 
bool		CSSHero::IsSkillFromNPC(INT32 nSkillId)
{ 
	for (INT32 i = 0; i < 2; ++i){
		AbsorbRuntimeInfo& sAbsorbRuntimeInfo = m_asAbsorbRuntimeInfo[i];
		if (sAbsorbRuntimeInfo.pSkill->cpsCfg && sAbsorbRuntimeInfo.pSkill->cpsCfg->un32SkillID == nSkillId){
			return true;
		}
	} 
	return false;
}
 

void CSSHero::CheckDeadState(){
	INT32 tMilsecSpan = m_pCSSBattleMgr->GetHeartBeatUTCMilsec() - GetGOActionStateInfo().tASUTCMilsec;
	if (tMilsecSpan >= GetFPData(eEffectCate_ReliveTime)){
		GetCurBattle()->AskRebornHero((CSSHero*)this);
	}
}

INT32 CSSHero::SetUser(CSSUser *piUser){
	if(piUser != NULL){
		piUser->SetHero(this);
		m_piUser = piUser;
	}
	return eNormal;
}

//CSSGameLogMgr* CSSHero::GetSSUserBattleLogMgr(){
//	if (CSSCfgMgr::GetSSCfgInstance().getMapGuideSwitch(this->GetCurBattle()->GetBattleMapId())){
//		return NULL;
//	}
//	if (m_piUser){
//		return &m_piUser->GetCSSUserBattleLogMgr();
//	}
//	return NULL;
//}

 INT32 CSSHero::AskLockTar(CSSGameUnit *pcTarGU){
	const SGOActionStateInfo &crsOSI = GetGOActionStateInfo();
	if (eOAS_PassiveState < crsOSI.eOAS){
		return eEC_AbsentOrderPriority;
	}

	if (NULL == pcTarGU){
		return eEC_NullGameUnit;
	}

	CSSBattle* psCurBattle = pcTarGU->GetCurBattle();

	if (psCurBattle != GetCurBattle()){
		return eEC_NullGameUnit;
	}

	if (pcTarGU->IsDead()){
		return eEC_TargetIsDead;
	}

	m_sLockedTargetGUID = pcTarGU->GetObjGUID(); 
	return eNormal;
}

SGUID_KEY& CSSHero::GetLockedTargetGUID(){
	if (FALSE == m_bAlwaysAutoAtk){
		return m_sLockedTargetGUID;
	}

	bool bIfExsit = TRUE;
	CSSGameObject* pOriGO = GetCurBattle()->GetGameUnitByGUID(m_sLockedTargetGUID);
	if (NULL == pOriGO || pOriGO->GetExpire() || !pOriGO->IfCanBeTarget()){
		bIfExsit = FALSE;
	}

	if (FALSE == bIfExsit){
		vector<CSSGameObject*> enemyVec;
		GameObjectMap& sGOMap = GetCurBattle()->GetBattleGameObjectList();
		GameObjectMap::iterator iter = sGOMap.begin();
		CSSGameObject* pEnemy =  NULL;
		FLOAT fDist = 99999999999.0f;
		for (; iter != sGOMap.end(); ++iter){
			CSSGameObject* pGO = iter->second;
			if (FALSE == pGO->GetExpire() && pGO->IfEnemy(this) && GetCurPos().CanWatch(3000, pGO->GetCurPos())){	
				float fTempDist = GetCurPos().GetWatchDistSqr(pGO->GetCurPos());
				if (fTempDist < fDist){
					fDist = fTempDist;
					pEnemy = pGO;
				}
			}
		}

		if (pEnemy){
			m_sLockedTargetGUID = pEnemy->GetObjGUID();
		}
	}


	return m_sLockedTargetGUID;
}

void CSSHero::SetAlwaysAutoAtk(INT8 n8Flag){
	if (0 == n8Flag){
		m_bAlwaysAutoAtk = FALSE;
	}
	else{
		m_bAlwaysAutoAtk = TRUE;
	}
}

void CSSHero::ResetAI(){
	if(m_pAI != NULL){
		delete m_pAI;
		m_pAI = NULL;
	}
	if(TRUE == IfAI()){
		ERobotAIType eAiType = eRobotAIType_Guide;
		EBattleType eBattleType = GetCurBattle()->GetBattleType();
		if(eBattleType == eBattleType_Free || eBattleType == eBattleType_Room){
			eAiType = eRobotAIType_PvE;
		}
		else if(eBattleType == eBattleType_Match){
			eAiType = eRobotAIType_Match;
		}
		else{
			eAiType = eRobotAIType_Guide;
		}
		m_pAI = new CSSAI_HeroRobot(this,eAiType);
	}
	else{
		m_pAI = new CSSAI_Hero(this);
	}
	//测试用代码，打开可以将自己也替换成机器人
	//delete m_pAI;
	//m_pAI = new CSSAI_HeroRobot(this);
	m_pAI->SetNormalAttackSkill(m_pNormalAttackSkill);
}

INT32	CSSHero::AskHeroAttributes()
{
	GSToGC::HeroAttributes Attr;
	Attr.set_guid(GetObjGUID());
	Attr.set_playerspeed(GetFPData(eEffectCate_MoveSpeed));
	INT32 t_n32 = GetFPData(eEffectCate_AttackSpeed);

	CSSSkill *temp_skillcfg = GetSkillByID(m_cpsHeroCfg->aun32SkillList[4]);
	INT32 n32ReleaseMilsec = temp_skillcfg->cpsCfg->n32ReleaseMilsec
		* GetFPData(eEffectCate_AttackSpeed) / 1000;
	INT32 n32StandCoolDownMilsec = (temp_skillcfg->cpsCfg->n32CoolDown + temp_skillcfg->cpsCfg->n32ReleaseMilsec ) 
		* GetFPData(eEffectCate_AttackSpeed) / 1000;
	double d_t = (double)1000 / n32StandCoolDownMilsec;

	Attr.set_attackinterval((INT32)(d_t * 1000));
	Attr.set_attackrange(GetFPData(eEffectCAte_AttackDist));
	Attr.set_resurgencetime(GetFPData(eEffectCate_ReliveTime));
	Attr.set_physicattack(GetFPData(eEffectCate_PhyAttack));
	Attr.set_spellsattack(GetFPData(eEffectCate_MagicAttack));
	Attr.set_physicdef(GetFPData(eEffectCate_PhyDefense));
	Attr.set_spellsdef(GetFPData(eEffectCate_MagicDefense));
	if(GetUser() != NULL)
		CSSWorkThreadMgr::GetInstance().PostMsgToGC(GetUser()->GetUserNetInfo(), Attr, Attr.msgid());
	return eNormal;
}

void		CSSHero::GetHurtMeList(UINT32 time,map<CSSGameUnit*,INT32> *pMap){
	int64_t now = GetUTCMiliSecond();
	int index = 0;
	GetObjGUID();
	for(auto iter = m_hurtRecordVec.begin(); iter != m_hurtRecordVec.end();iter++){
		SHurtRecord* pRecord = iter._Ptr;
		if(now - pRecord->tHurtTime > time) continue;
		CSSGameUnit* pGU = GetCurBattle()->GetGameUnitByGUID(pRecord->pHurterGUID);
		if(NULL == pGU) continue;
		auto mapIter = pMap->find(pGU);
		if(mapIter == pMap->end()){
			pMap->insert(map<CSSGameUnit*,INT32>::value_type(pGU,pRecord->n32HurtValue));
		}
		else{
			pMap->insert(map<CSSGameUnit*,INT32>::value_type(pGU,pRecord->n32HurtValue + mapIter->second));
		}
	}
}

void		CSSHero::RefreshCD(){
	for (INT32 i = 0 ;i < c_n32MaxHeroSkillNum; i++){
		const SSNewSkillCfg *cpsTempSkilCfg = m_aspSkillArr[i]->cpsCfg;
		if (NULL != cpsTempSkilCfg){
			m_aspSkillArr[i]->ReFreshCD();
			SyncSkillStateToGC(m_aspSkillArr[i]->cpsCfg->un32SkillID);
		}
		
	}
	for (INT32 i = 0; i < 2; ++i){
		AbsorbRuntimeInfo& sAbsorbRuntimeInfo = m_asAbsorbRuntimeInfo[i];
		if (sAbsorbRuntimeInfo.pSkill->cpsCfg){
			sAbsorbRuntimeInfo.pSkill->ReFreshCD();
			SyncSkillStateToGC(sAbsorbRuntimeInfo.pSkill->cpsCfg->un32SkillID);
		}
	}
	for (INT32 i = 0; i < c_un32MaxGridNum; ++i){
		SGoodsRunTimeData& sGoodsRuntimeData = m_asGoodsRunTimeData[i];
		if ((sGoodsRuntimeData.n32Num > 0 ||  sGoodsRuntimeData.eGoodsSpecificaFlag ==  eGoodsSpecificaFlag_InfiniteUse)
			&& NULL != sGoodsRuntimeData.pSkill
			&& NULL != sGoodsRuntimeData.pSkill->cpsCfg){
				sGoodsRuntimeData.pSkill->ReFreshCD();
				SynGoodsUseStateToGC(i);
		}
	}
}

void		CSSHero::RefreshGoodsSkillMap(){
	m_goodsSkillMap.clear();
	for (INT32 i = 0; i < c_un32MaxGridNum; ++i){
		SGoodsRunTimeData& sGoodsRuntimeData = m_asGoodsRunTimeData[i];
		if ((sGoodsRuntimeData.n32Num > 0 ||  sGoodsRuntimeData.eGoodsSpecificaFlag ==  eGoodsSpecificaFlag_InfiniteUse)
			&& NULL != sGoodsRuntimeData.pSkill
			&& NULL != sGoodsRuntimeData.pSkill->cpsCfg){
				m_goodsSkillMap.insert(make_pair(sGoodsRuntimeData.pSkill,i));
		}
	}
}

}