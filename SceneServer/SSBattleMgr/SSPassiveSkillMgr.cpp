#include "SSPassiveSkillMgr.h"
#include "SSPassiveSkill.h"
#include "SSBattle.h"
#include "SSGameUnit.h"
#include "SSBattleMgr.h" 

namespace SceneServer{

const UINT32	c_un32DefaultSkillBlockNum = 1024;

CSSPassiveSkillMgr::CSSPassiveSkillMgr(): m_pCSSBattleMgr(NULL){

}

CSSPassiveSkillMgr::~CSSPassiveSkillMgr(){
	for (auto iter = m_PassiveSkillMap.begin(); iter != m_PassiveSkillMap.end(); ++iter){
		CSSPassiveSkill* pSkill = iter->second;
		if(NULL != pSkill){
			delete pSkill;
		}
	}
}

CSSPassiveSkill* CSSPassiveSkillMgr::GetPassiveSkill(INT32 n32PassiveSkillID){
	PassiveSkillMap::iterator iter = m_PassiveSkillMap.find(n32PassiveSkillID);
	if(iter != m_PassiveSkillMap.end()){
		return iter->second;
	}
	return NULL;
}

INT32 CSSPassiveSkillMgr::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if (FALSE == m_DelayDeleteVec.empty()){
		for(auto iter = m_DelayDeleteVec.begin(); iter != m_DelayDeleteVec.end();){
			INT32 pSkillID = *iter;
			auto skillIter = m_PassiveSkillMap.find(pSkillID);
			if(skillIter == m_PassiveSkillMap.end()) continue;
			CSSPassiveSkill* pSkill = skillIter->second;
			if(NULL != pSkill->m_pcMasterGO){
				for(int i=0;i<n_cPassiveSkillTriggerNum;i++){
					EPassiveSkillTriggerType eType = pSkill->m_pCfg->lePassiveSkillTriggerType[i];
					if(eType != EPassiveSkillTriggerType_None){
						pSkill->m_pcMasterGO->RemovePassiveSkill(eType,pSkill->m_un32UniqueID);
					}
				}
			}
			m_PassiveSkillMap.erase(pSkillID);
			iter = m_DelayDeleteVec.erase(iter);
			delete pSkill;
		}
	}
	if (FALSE == m_PassiveSkillMap.empty()){
		for (PassiveSkillMap::iterator iter = m_PassiveSkillMap.begin(); iter != m_PassiveSkillMap.end(); ){
			CSSPassiveSkill* pSkill = iter->second;
			if (NULL == pSkill || pSkill->m_pcMasterGO == NULL){
				iter = m_PassiveSkillMap.erase(iter);
				delete pSkill;
				continue;
			}
			pSkill->OnHeartBeat(tUTCMilsec,tTickSpan);


			++iter;
		}
	}

	return eNormal;
}

void CSSPassiveSkillMgr::Trigger(INT32 n32PassiveSkillUniqueID, CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1, INT32 para2, INT32 para3){
	CSSPassiveSkill* pPSkill = GetPassiveSkill(n32PassiveSkillUniqueID);
	if(pPSkill != NULL){
		pPSkill->Trigger(pTarget,passiveType,para1,para2,para3);
	}
}

CSSPassiveSkill* CSSPassiveSkillMgr::CreatePassiveSkill(INT32	n32PassiveSkillID){
	const SSPassiveSkilllCfg* pCfg =  CSSCfgMgr::GetSSCfgInstance().GetPassitiveSkillCfg(n32PassiveSkillID);
	if(NULL == pCfg) return NULL;
	CSSPassiveSkill*pSkill = new CSSPassiveSkill(pCfg);
	pSkill->SetBattleMgr(m_pCSSBattleMgr);
	return pSkill;
}

INT32 CSSPassiveSkillMgr::AddPassiveSkill(CSSGameUnit* pTarget, INT32	n32PassiveSkillID){
	CSSPassiveSkill* pPSkill = CreatePassiveSkill(n32PassiveSkillID);
	if(pPSkill == NULL){
		return eNormal;
	}
	//检查攻击类型，看是否允许添加被动
	if((pTarget->GetAttackWay() != EAttackWay_Near && pPSkill->m_pCfg->eRangeType == EPassiveRangeType_Near)
		|| (pTarget->GetAttackWay() != EAttackWay_Range && pPSkill->m_pCfg->eRangeType == EPassiveRangeType_Range)){
			return eNormal;
	}

	pPSkill->m_pcMasterGO = pTarget;
	pPSkill->m_un32UniqueID = pTarget->GetCurBattle()->GetUniqueEffectID();
	pPSkill->m_tLastEffectMilsec = GetUTCMiliSecond() - pPSkill->m_pCfg->n32Cooldown;
	for(int i=0;i<n_cPassiveSkillTriggerNum;i++){
		EPassiveSkillTriggerType eType = pPSkill->m_pCfg->lePassiveSkillTriggerType[i];
		if(eType != EPassiveSkillTriggerType_None){
			pTarget->AddPassiveSkill(eType, pPSkill->m_un32UniqueID);
		}
	}
	pPSkill->OnAdd();
	m_PassiveSkillMap[pPSkill->m_un32UniqueID] = pPSkill;
	return pPSkill->m_un32UniqueID;
}

void CSSPassiveSkillMgr::RevmovePassiveSkill(INT32	n32PassiveSkillUniqueID){
	//将准备删除的技能加入延迟删除列表
	m_DelayDeleteVec.push_back(n32PassiveSkillUniqueID);
}

void CSSPassiveSkillMgr::TryUpgradeSkill(INT32 n32PassiveSkillUniqueID, INT32 n32Level){
	CSSPassiveSkill* pPSkill = GetPassiveSkill(n32PassiveSkillUniqueID);
	if(pPSkill == NULL){
		return;
	}

	for(int i=0;i<99;i++){
		INT32 nextLevelSkillID = pPSkill->m_pCfg->un32PassiveSkillID + 1;
		const SSPassiveSkilllCfg* pNewCfg =  CSSCfgMgr::GetSSCfgInstance().GetPassitiveSkillCfg(nextLevelSkillID);
		if(pNewCfg != NULL && pNewCfg->n32UpgradeLevel <= n32Level){
			pPSkill->m_pCfg = pNewCfg;
		}
		else{
			break;
		}
	}
}

void CSSPassiveSkillMgr::SetBattleMgr(CSSBattleMgr* pMgr){
	m_pCSSBattleMgr = pMgr;
}

void CSSPassiveSkillMgr::ExpiredPassiveSkill(INT32 n32PassiveSkillUniqueID){
	CSSPassiveSkill* pSkill = GetPassiveSkill(n32PassiveSkillUniqueID);
	if(pSkill == NULL){
		return;
	}
	pSkill->SetExpired();
}

}