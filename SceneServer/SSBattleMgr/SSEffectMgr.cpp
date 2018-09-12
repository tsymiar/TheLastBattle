#include "stdafx.h"
#include "SSEffectMgr.h"
#include "SSSkillEffect.h"
#include "SSGameUnit.h"
#include "SSSkillEffect_Fly.h"
#include "SSSkillEffect_Caculate.h"
#include "SSBattleMgr.h" 
#include "SSSkillEffect_Range.h" 
#include "SSSkillEffect_Leading.h"
#include "SSSkillEffect_Buf.h"
#include "SSSkillEffect_Summon.h" 
#include "SSSkillEffect_Move.h"
#include "SSSkillEffect_Switch.h"
#include "SSSkillEffect_Purification.h"
#include "SSSkillEffect_Link.h"
#include "SSBattle.h"
#include "SSCfgMgr.h"
#include "SSBattleMgr.h"
#include "SSSkill.h"
 

namespace SceneServer{

CSSEffectMgr::CSSEffectMgr():
	m_pCSSBattleMgr(NULL)
{
	
}

CSSEffectMgr::~CSSEffectMgr(){
	for (auto iter = m_WaitingEffectMap.begin(); iter != m_WaitingEffectMap.end(); ++iter){
		CSSSkillEffect* spEffect = iter->second;
		DestroyAFreeSkillEffect(spEffect);
	}
	for (auto iter = m_UpdateEffectMap.begin(); iter != m_UpdateEffectMap.end(); ++iter){
		CSSSkillEffect* spEffect = iter->second;
		DestroyAFreeSkillEffect(spEffect);
	}
}

INT32 CSSEffectMgr::AddEffect(CSSSkillEffect* pEffect){
	if (NULL == pEffect){
		return eEC_AddEffectFailed;
	}

	if (TRUE == pEffect->IsForceStop()){
		return eEC_AddEffectFailed;
	}
	//如果一个效果有设置等待时间，将其加入到等待列表，直到等待时间结束
	if(pEffect->m_EffectDelayTime > 0){
		m_WaitingEffectMap[pEffect->m_un32UniqueID] = pEffect;
	}
	else{
		//尝试使用效果的启动方法。如果启动成功，将其加入正在运行的效果列表中
		if(eNormal == pEffect->Begin()){
			//if (FALSE == pEffect->m_pCfg->bIsCanMove){
				pEffect->AddSelfToUsingList();
			//}
			m_UpdateEffectMap[pEffect->m_un32UniqueID] = pEffect;
		}
		//如果效果启动失败，检查其是否会开始计算技能CD，然后结束效果。
		else{
			pEffect->CheckCooldown();
			pEffect->End();
			DestroyAFreeSkillEffect(pEffect);
			return eEC_AddEffectFailed;
		}
	}
	pEffect->CheckCooldown();
	return eNormal;
}

INT32 CSSEffectMgr::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if (FALSE == m_WaitingEffectMap.empty()){
		for (EffectMap::iterator iter = m_WaitingEffectMap.begin(); iter != m_WaitingEffectMap.end(); ){
			CSSSkillEffect* pEffect = iter->second;
			if (NULL == pEffect){
				iter = m_WaitingEffectMap.erase(iter);
				continue;
			}
			//检查变量的可靠性
			if (pEffect->IfParaInvalid()){
				pEffect->CSSSkillEffect::Clear();
				DestroyAFreeSkillEffect(pEffect);
				iter = m_WaitingEffectMap.erase(iter);
				continue;
			}
			//检查等待时间
			if (pEffect->IfNeedWait(tUTCMilsec)){
				++iter;
				continue;
			}
			//等待时间结束，开始运行begin方法，并根据结果将其加入运行列表或者移除
			if(eNormal == pEffect->Begin()){
				//if (FALSE == pEffect->m_pCfg->bIsCanMove){
					pEffect->AddSelfToUsingList();
				//}
				m_UpdateEffectMap[pEffect->m_un32UniqueID] = pEffect;
			}
			else{
				DestroyAFreeSkillEffect(pEffect);
			}
			iter = m_WaitingEffectMap.erase(iter);
		}
	}

	if (FALSE == m_UpdateEffectMap.empty()){
		for (EffectMap::iterator iter = m_UpdateEffectMap.begin(); iter != m_UpdateEffectMap.end(); ){
			CSSSkillEffect* pEffect = iter->second;
			if (NULL == pEffect){
				iter = m_UpdateEffectMap.erase(iter);
				continue;
			}

			if (pEffect->IfParaInvalid()){
				if (pEffect->GetUniqueID() != 0){
					//ELOG(LOG_WARNNING, "EffetID:%u", pEffect->GetUniqueID());
				}
				pEffect->CSSSkillEffect::Clear();
				DestroyAFreeSkillEffect(pEffect);
				iter = m_UpdateEffectMap.erase(iter);
				continue;
			}
			//运行列表中的效果，每一帧需要运行Update方法。如果运行返回不是Normal，则将其终止并移除
			if (eNormal != pEffect->Update(tUTCMilsec, tTickSpan)){
				pEffect->StopDependedEffect();
				pEffect->End();
				pEffect->CSSSkillEffect::Clear();
				DestroyAFreeSkillEffect(pEffect);
				iter = m_UpdateEffectMap.erase(iter);
				continue;
			}

			++iter;
		}
	}

	return eNormal;
}

INT32 CSSEffectMgr::AddEffectsFromCfg(SSNextSkillEffectCfg* pEffectCfg, CSSGameUnit* pMaster, CSSGameUnit* pTarget, CVector3D cPos, CVector3D cDir
	,  CSSSkill* pSkill, TIME_MILSEC beginTime, CSSGameUnit* pStartSkillGU){
	if (NULL == pEffectCfg || NULL == pMaster){
		return eNormal;
	}
	//临时的依赖效果列表，存储配置中的依赖效果配置。
	CSSSkillEffect* pBeDependedEffect = NULL;
	INT32 dependEffectIdList[c_n32MaxDependSkillEffectCfgNum] = {0};
	INT32 n32DependNum = 0;

	for(INT32 i = 0; i < c_n32MaxNextSkillEffectCfgNum; ++i){
		SSNextSkillEffectCfg& spEffectCfg = pEffectCfg[i];
		if(spEffectCfg.un32SkillEffectID == 0){
			continue;
		}
		//创建效果
		CSSSkillEffect* pEffect = CreateSkillEffect(spEffectCfg.un32SkillEffectID);
		if(NULL == pEffect){
			continue;
		} 
		//设置变量
		if(pMaster != NULL && pMaster->GetFatherHeroGUID() > 0 && m_pCSSBattle->GetGameUnitByGUID(pMaster->GetFatherHeroGUID()) != NULL){
			pMaster = m_pCSSBattle->GetGameUnitByGUID(pMaster->GetFatherHeroGUID());
		}

		pEffect->SetBattleMgr(m_pCSSBattleMgr);
		pEffect->m_pSkillRuntimeData = pSkill;
		pEffect->m_pcMasterGO = pMaster;
		pEffect->m_pcTarGU = pTarget;
		pEffect->m_pcStartSkillGU = pStartSkillGU;
		pEffect->m_EffectDelayTime = spEffectCfg.n32Delay;
		pEffect->m_cDir = cDir;
		pEffect->m_cPos = cPos;
		pEffect->m_tBeginMilsec = beginTime;
		pEffect->m_IsForceStop = FALSE;
		pEffect->m_un32UniqueID = pMaster->GetCurBattle()->GetUniqueEffectID();
		pEffect->m_pBattle = pMaster->GetCurBattle();
		
		for (INT32 j = 0; j < c_n32MaxDependSkillEffectCfgNum; ++j){
			pEffect->m_lDependEffect[j] = 0;
		}
		//将依赖的效果ID加入到该效果的列表中。只存储ID
		if(0 != spEffectCfg.n32DependedArr[0]){
			memcpy(dependEffectIdList, spEffectCfg.n32DependedArr, sizeof(INT32) * c_n32MaxDependSkillEffectCfgNum);
			pBeDependedEffect = pEffect;
			n32DependNum = 0;
		}
		//将依赖的效果指针存到被依赖的效果中
		if(pBeDependedEffect != NULL){
			for(int j = 0; j < c_n32MaxDependSkillEffectCfgNum; j++){
				if(dependEffectIdList[j] == spEffectCfg.un32SkillEffectID){
					pBeDependedEffect->m_lDependEffect[n32DependNum++] = pEffect;
					break;
				}
			}
		}
		//添加效果到管理器中
		AddEffect(pEffect); 
	}

	return eNormal;
}

CSSSkillEffect* CSSEffectMgr::CreateSkillEffect(INT32 n32SkillEffectId)
{
	//根据不同的效果类型，实例化对应的效果。
	CSSSkillEffect* pEffect = NULL;
	const SSkillEffectlBaseCfg* pCfg = NULL; 
	ESkillEffectType effectType = CSSCfgMgr::GetSSCfgInstance().GetSkillModelType(n32SkillEffectId);
	switch(effectType){
	case eSkillEffectType_Caculate:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelAccountCfg(n32SkillEffectId);
			if(NULL == pCfg) {
				break;
			}

			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}

			pEffect = new CSSSkillEffect_Caculate;
		}
		break;
	case eSkillEffectType_Emit:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelEmitCfg(n32SkillEffectId);
			if(NULL == pCfg){
				break;
			}
			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}

			pEffect = new CSSSkillEffect_Fly;
		}
		break;
 
	case eSkillEffectType_Range:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelRangeCfg(n32SkillEffectId);
			if (NULL == pCfg){
				return NULL;
			}
			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}

			pEffect = new CSSSkillEffect_Range;
		}	
		break; 
	case eSkillEffectType_Leading:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelLeadingCfg(n32SkillEffectId);
			if(NULL == pCfg){
				break;
			}

			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}

			pEffect = new CSSSkillEffect_Leading;
		}
		break;
	case eSkillEffectType_Summon:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelSummonCfg(n32SkillEffectId);
			if(NULL == pCfg){
				break;
			}

			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}

			pEffect = new CSSSkillEffect_Summon;
		}break;
	case eSkillEffectType_Buf:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelBuffCfg(n32SkillEffectId);
			if(NULL == pCfg){
				break;
			}
			
			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}
			
			pEffect = new CSSSkillEffect_Buf;
		}
		break;
	case eSkillEffectType_Move:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelMoveCfg(n32SkillEffectId);
			if(NULL == pCfg){
				break;
			}

			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}

			pEffect = new CSSSkillEffect_Move;
		}
		break;
	case eSkillEffectType_Switch:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelSwitchCfg(n32SkillEffectId);
			if(NULL == pCfg){
				break;
			}

			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}

			pEffect = new CSSkillModel_Switch;
		}
		break;
	case eSkillEffectType_Purification:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelPurificationCfg(n32SkillEffectId);
			if(NULL == pCfg){
				break;
			}

			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}

			pEffect = new CSSSkillEffect_Purification;
		}
		break;
	case eSkillEffectType_Link:
		{
			pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelLinkCfg(n32SkillEffectId);
			if(NULL == pCfg){
				break;
			}

			if (FALSE == CanEffectBeTriggered(m_pCSSBattle->GetRandomEngine(),pCfg->n32TriggerRate)){
				return NULL;
			}

			pEffect = new CSSSkillEffect_Link;
		}
		break;
	default:
		ELOG(LOG_WARNNING, "swith type(%d) error", effectType);
		return NULL;
	}

	if (NULL == pEffect){
		return NULL;
	}

	pEffect->m_SkillEffectType = effectType;
	pEffect->m_pCfg = pCfg;
	return pEffect;
}

void CSSEffectMgr::Clear(){
	
	m_WaitingEffectMap.clear();
	m_UpdateEffectMap.clear();
}

INT32 CSSEffectMgr::DestroyAFreeSkillEffect(CSSSkillEffect* pcEffect)
{
	delete pcEffect;
	return eNormal;
}

bool CSSEffectMgr::CanEffectBeTriggered(std::default_random_engine& engine, INT32 n32TriggerRate, INT32 n32TriggerBase){
	INT32 n32RandInt = CFunction::GetGameRandomN(engine,0,n32TriggerBase);
	return n32RandInt <= n32TriggerRate;
}

INT32 CSSEffectMgr::DirectAddBuff(INT32 n32BuffEffectId, CSSGameUnit* pReleaser, CSSGameUnit* pTarget){
	if (NULL == pReleaser){
		return eNormal;
	}

	CSSSkillEffect* pEffect = CreateSkillEffect(n32BuffEffectId);
	if(NULL == pEffect){
		return eNormal;
	}

	if(pEffect->m_SkillEffectType != eSkillEffectType_Buf){
		return eNormal;
	}

	//设置通用的成员变量
	pEffect->m_pSkillRuntimeData = NULL;
	if(pReleaser != NULL && pReleaser->GetFatherHeroGUID() > 0 && m_pCSSBattle->GetGameUnitByGUID(pReleaser->GetFatherHeroGUID()) != NULL){
		pReleaser = m_pCSSBattle->GetGameUnitByGUID(pReleaser->GetFatherHeroGUID());
	}
	pEffect->m_pcMasterGO = pReleaser;
	pEffect->m_pcTarGU = pTarget;
	pEffect->m_pcStartSkillGU = NULL;
	pEffect->m_EffectDelayTime = 0;
	pEffect->m_cDir = pReleaser->GetCurDir();
	pEffect->m_cPos = pReleaser->GetCurPos();
	pEffect->m_tBeginMilsec = GetUTCMiliSecond();
	pEffect->m_un32UniqueID = pReleaser->GetCurBattle()->GetUniqueEffectID();
	pEffect->m_pBattle = pReleaser->GetCurBattle();
	for (INT32 i = 0; i < c_n32MaxDependSkillEffectCfgNum; ++i){
		pEffect->m_lDependEffect[i] = nullptr;
	}

	if (eNormal == AddEffect(pEffect)){
		return pEffect->m_un32UniqueID;
	}
	
	return eNormal;
}


INT32 CSSEffectMgr::RemoveEffect(UINT32 un32EffectUniqueID){
	if (0 == un32EffectUniqueID){
		return eEC_RemoveEffectFailed;
	}
	//移除效果时，要注意需要将其强制停止
	CSSSkillEffect* spEffect = NULL;
	EffectMap::iterator iter = m_WaitingEffectMap.find(un32EffectUniqueID);
	if (m_WaitingEffectMap.end() != iter){
		spEffect = iter->second;
		if (NULL != spEffect){
			spEffect->ForceStop();
		}
		DestroyAFreeSkillEffect(spEffect);
		m_WaitingEffectMap.erase(iter);
		return eNormal;
	}

	iter = 	m_UpdateEffectMap.find(un32EffectUniqueID);
	if (m_UpdateEffectMap.end() != iter){
		spEffect = iter->second;
		if (NULL != spEffect){
			spEffect->ForceStop();
		}
		DestroyAFreeSkillEffect(spEffect);
		m_UpdateEffectMap.erase(iter);
		return eNormal;
	}

	return eEC_RemoveEffectFailed;
}


CSSSkillEffect* CSSEffectMgr::GetEffect(UINT32 un32EffectUniqueID){
	if (0 == un32EffectUniqueID){
		return NULL;
	}

	CSSSkillEffect* spEffect = NULL;
	EffectMap::iterator iter = m_WaitingEffectMap.find(un32EffectUniqueID);
	if (m_WaitingEffectMap.end() != iter){
		return iter->second;
	}

	iter = 	m_UpdateEffectMap.find(un32EffectUniqueID);
	if (m_UpdateEffectMap.end() != iter){
		return iter->second;
	}

	return NULL; 
}

CSSEffectMgr::EffectMap& CSSEffectMgr::GetEffectMap(){
	return m_UpdateEffectMap;
}

INT32 CSSEffectMgr::UseSkillDirect(UINT32 un32SkillID, CSSGameUnit* pReleaser, CSSGameUnit* pTarget){
	if (NULL == pReleaser){
		return eEC_NullHero;
	}

	SSNewSkillCfg* pSkill = (SSNewSkillCfg*)CSSCfgMgr::GetSSCfgInstance().GetSkillCfg(un32SkillID);
	if (NULL == pSkill){
		return eEC_NULLCfg;
	}

	for(INT32 i = 0; i < c_n32MaxNextSkillEffectCfgNum; ++i){
		SSNextSkillEffectCfg& spEffectCfg = pSkill->asSkillModelList[i];
		if(spEffectCfg.un32SkillEffectID == 0){
			continue;
		}

		ESkillEffectType effectType = CSSCfgMgr::GetSSCfgInstance().GetSkillModelType(spEffectCfg.un32SkillEffectID);
		//只对结算类效果生效。
		if (eSkillEffectType_Caculate == effectType){
			const SSkillModelAccountCfg* pCfg = CSSCfgMgr::GetSSCfgInstance().GetSkillModelAccountCfg(spEffectCfg.un32SkillEffectID);
			if (NULL != pCfg){
				CSSSkillEffect_Caculate::CaculateSkillEffectOnce(pReleaser, pTarget, pCfg->sEffectInfo, un32SkillID, FALSE);
				 
			}
		}
	}

	return eNormal;
}

void CSSEffectMgr::SetBattleMgr(CSSBattleMgr* pMgr, CSSBattle* pBattle){
	m_pCSSBattleMgr = pMgr;
	m_pCSSBattle = pBattle;
}

void CSSEffectMgr::RemoveAllEffectBySkillID(INT32 skillID){
	for (auto iter = m_WaitingEffectMap.begin(); iter != m_WaitingEffectMap.end();){
		CSSSkillEffect* spEffect = iter->second;
		if(spEffect->m_pSkillRuntimeData != NULL && spEffect->m_pSkillRuntimeData->cpsCfg->un32SkillID == skillID){
			spEffect->ForceStop();
			DestroyAFreeSkillEffect(spEffect);
			iter = m_WaitingEffectMap.erase(iter);
		}
		else{
			++iter;
		}
	}
	for (auto iter = m_UpdateEffectMap.begin(); iter != m_UpdateEffectMap.end();){
		CSSSkillEffect* spEffect = iter->second;
		if(spEffect->m_pSkillRuntimeData != NULL && spEffect->m_pSkillRuntimeData->cpsCfg->un32SkillID == skillID){
			spEffect->ForceStop();
			DestroyAFreeSkillEffect(spEffect);
			iter = m_UpdateEffectMap.erase(iter);
		}
		else{
			++iter;
		}
	}
}

}