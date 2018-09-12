#include "SSSkillEffect.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSPassiveSkillMgr.h"
#include "SSBattleMgr.h"

namespace SceneServer{
 
CSSSkillEffect::CSSSkillEffect(): m_SkillEffectType(eSkillEffectType_None)
	, m_pSkillRuntimeData(NULL)
	, m_pCfg(NULL)
	, m_EffectDelayTime(0)
	, m_pcMasterGO(NULL)
	, m_pcStartSkillGU(NULL)
	, m_tBeginMilsec(0)
	, m_pcTarGU(NULL)
	, m_cDir(0,0,0)
	, m_cPos(0,0,0)
	, m_IsForceStop(FALSE)
	, m_un32UniqueID(0)
	, m_pBattle(NULL)
	, m_pCSSBattleMgr(NULL)
	, m_ifExpired(false)
{
		for (INT32 i = 0; i < c_n32MaxDependSkillEffectCfgNum; ++i){
			m_lDependEffect[i] = nullptr;
		}
}
CSSSkillEffect::~CSSSkillEffect(){
	 
}
 

bool CSSSkillEffect::IfNeedWait(TIME_MILSEC tUTCMilsec){
	return tUTCMilsec - m_tBeginMilsec < m_EffectDelayTime;
}

INT32 CSSSkillEffect::CheckCooldown(){
	if(m_pCfg == NULL || m_pSkillRuntimeData == NULL){
		return eNormal;
	}


	if (m_pSkillRuntimeData->bIfCanCooldown && m_pCfg->bIsCoolDown){
		return m_pSkillRuntimeData->DoCoolDown();
	}
	return eNormal;
}

bool CSSSkillEffect::CheckHitTargetType(CSSGameUnit* pTarget){
	if (NULL == pTarget)	{
		return FALSE;
	}
	if (pTarget->IsDead()){
		return FALSE;
	} 

	if(pTarget->IfHero() && m_pCfg->bIfAffectHero){
		return true;
	}

	if(pTarget->IfNPC() && !pTarget->IfNPC_Building()  && m_pCfg->bIfAffectMonster){
		return true;
	}

	if(pTarget->IfNPC() && pTarget->IfNPC_Building() && m_pCfg->bIfAffectBuilding){
		return true;
	}

	return FALSE;
}

bool CSSSkillEffect::CheckHitTargetCamp(CSSGameUnit* pTarget){
	if (NULL == pTarget)	{
		return FALSE;
	}

	if (pTarget->IsDead()){
		return FALSE;
	} 

	switch (m_pCfg->eTargetType){
	case eSMTC_AllObject:
		return TRUE;
	case eSMTC_Enemy:
		return CFunction::IfEnemy(m_pcMasterGO->GetCampID(), pTarget->GetCampID());
	case eSMTC_Self:
		return FALSE == CFunction::IfEnemy(m_pcMasterGO->GetCampID(), pTarget->GetCampID());
	default:
		return FALSE;
	} 
}

INT32 CSSSkillEffect::Clear(){
	m_pSkillRuntimeData = NULL;
	m_pCfg = NULL;
	m_EffectDelayTime = 0;
	m_pcMasterGO = NULL;
	m_tBeginMilsec = 0;
	m_pcTarGU = 0;
	m_IsForceStop = FALSE;
	m_un32UniqueID = 0;
	m_pBattle = NULL;
	return eNormal;
}

INT32 CSSSkillEffect::ForceStop(){
	End();

	m_IsForceStop = TRUE;
	m_un32UniqueID = 0;

	return eNormal;
}

INT32 CSSSkillEffect::AddSelfToUsingList(){
	if(m_pSkillRuntimeData == NULL){
		return eNormal;
	}
	if(m_pcMasterGO == NULL){
		return eNormal;
	}
	for (INT32 i = 0; i < c_n32UsingEffectsNum; ++i){
		if (m_pSkillRuntimeData->un32UsingEffectsArr[i] == 0){
			m_pSkillRuntimeData->un32UsingEffectsArr[i]  = m_un32UniqueID;
			return eNormal;
		}
	}

	return eEC_AddEffectFailed;
}

bool CSSSkillEffect::IsForceStop(){
	return m_IsForceStop;
}

INT32 CSSSkillEffect::StopDependedEffect(){
	for (INT32 i = 0; i < c_n32MaxDependSkillEffectCfgNum; ++i){
		CSSSkillEffect* pDependEffect = m_lDependEffect[i];
		if (NULL != pDependEffect && 0 != pDependEffect->m_un32UniqueID){
			pDependEffect->ForceStop();
			m_lDependEffect[i] = NULL;
		}
	}

	return eNormal;
}

void CSSSkillEffect::OnSkillHitTarget(CSSGameUnit* pTarget){
	//调用技能命中时的被动技能
	if(m_pSkillRuntimeData != NULL && m_pSkillRuntimeData->cpsCfg != NULL && FALSE == m_pSkillRuntimeData->cpsCfg->bIfNomalAttack){
		m_pcMasterGO->OnPassitiveSkillCalled(EPassiveSkillTriggerType_SkillAttack, pTarget);
	}
}

bool CSSSkillEffect::CheckBattle(){
	return m_pBattle && m_pBattle->GetBattleState() == eSSBS_Playing;
}
CSSBattle* CSSSkillEffect::GetBattle(){
	return m_pBattle;
}

bool CSSSkillEffect::IfParaInvalid(){
	return TRUE == IsForceStop() || NULL == m_pSkillRuntimeData || NULL == m_pSkillRuntimeData->cpsCfg;
}

void CSSSkillEffect::SetBattleMgr(CSSBattleMgr* pMgr){
	m_pCSSBattleMgr = pMgr;
}

void CSSSkillEffect::Clean(){
	ELOG(LOG_WARNNING, "");
	End();
}

}