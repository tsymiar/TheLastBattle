#include "stdafx.h"
#include "SSSkillEffect_Leading.h"
#include "SSGameObject.h"
#include "SSGameUnit.h"
#include "SSBattle.h"
#include "SSHero.h"
#include "SSEffectMgr.h"

namespace SceneServer{

CSSSkillEffect_Leading::CSSSkillEffect_Leading():m_LeadingState(eLeading_Failed)
	, m_pLeadingCfg(NULL)
	, m_isUsingSkill(TRUE)
{
	m_SkillEffectType = eSkillEffectType_Leading;
}


CSSSkillEffect_Leading::~CSSSkillEffect_Leading(){
}

INT32 CSSSkillEffect_Leading::Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){	
	if (NULL == m_pcMasterGO){
		return eEC_EffectEnd;
	}

	if (TRUE == m_pLeadingCfg->bIsCheckTargetSta){
		if (NULL == m_pcTarGU){
			return eEC_EffectEnd;
		}

		if (m_pcTarGU->IsDead()){
			return eEC_EffectEnd;
		}

		if (m_pcMasterGO->IfHero()){
			CSSHero* pHero = (CSSHero*)m_pcMasterGO.get();
			if (pHero->GetLockedTargetGUID() != m_pcTarGU->GetObjGUID()){
				return eEC_EffectEnd;
			}
		}
	}

	if (m_pcMasterGO->GetFPData(eEffectCate_Dizziness) > 0 || m_pcMasterGO->GetFPData(eEffectCate_Silence) > 0){
			return eEC_EffectEnd;
	}

	if (tUTCMilsec - m_tBeginMilsec >= m_pLeadingCfg->eLeadingTime){
		m_LeadingState = eLeading_Success;
		return eEC_EffectEnd;
	}

	return eNormal;
}

INT32 CSSSkillEffect_Leading::Begin(){
	m_isUsingSkill = TRUE;
	m_pLeadingCfg = (SSkillModelLeadingCfg*)m_pCfg;
	if (NULL == m_pLeadingCfg){
		return eEC_EffectEnd;
	} 

	if (NULL != m_pcMasterGO){
		m_tBeginMilsec = GetUTCMiliSecond();
		NotifyGS(eLeading_Begin);
		m_LeadingState = eLeading_Failed;
		return eNormal;
	}

	return eEC_EffectEnd;
}

INT32 CSSSkillEffect_Leading::End(){
	m_isUsingSkill = FALSE;

	if (NULL == m_pcMasterGO){
		return eEC_EffectEnd;
	}

	NotifyGS(m_LeadingState);

	if(m_LeadingState == eLeading_Success){
		GetBattle()->GetEffectMgr()->AddEffectsFromCfg((SSNextSkillEffectCfg*)m_pCfg->asSkillModelList, m_pcMasterGO
			, m_pcTarGU, m_pcMasterGO->GetCurPos(), m_pcMasterGO->GetCurDir(),m_pSkillRuntimeData, GetUTCMiliSecond());
	}

	StopDependedEffect();

	return eNormal;
}


INT32 CSSSkillEffect_Leading::NotifyGS(ELeadingState eLeadingState){
	GSToGC::NotifySkillModelLeading sNotifySkillModelLeading;
	sNotifySkillModelLeading.set_effectid(m_pCfg->un32SkillModelID);
	sNotifySkillModelLeading.set_guid(m_pcMasterGO->GetObjGUID());
	sNotifySkillModelLeading.set_uniqueid(m_un32UniqueID);
	if (NULL != m_pcTarGU){
		sNotifySkillModelLeading.set_targuid(m_pcTarGU->GetObjGUID());
	}
	sNotifySkillModelLeading.set_state(eLeadingState);

	GetBattle()->SendMsgToAllWatcher(sNotifySkillModelLeading, sNotifySkillModelLeading.msgid(), m_pcMasterGO);

	return eNormal;
}

}