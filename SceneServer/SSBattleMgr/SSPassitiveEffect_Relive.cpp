#include "SSPassitiveEffect_Relive.h"
#include "SSGameUnit.h"
#include "SSNPC.h"

namespace SceneServer{

CSSPassiveEffect_Relive::CSSPassiveEffect_Relive():
	m_pCfg(NULL),
	m_tStartReliveTime(0)
{}

CSSPassiveEffect_Relive::~CSSPassiveEffect_Relive(){
	
}

void CSSPassiveEffect_Relive::Trigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1, INT32 para2, INT32 para3){
	m_tStartReliveTime = GetUTCMiliSecond();
	CSSNPC* pcNPC = m_pcMasterGO->TryCastToNPC();
	if (NULL != pcNPC){
		m_pcMasterGO->BeginAction_Reliving(TRUE,m_pcMasterGO);
	}
}

INT32 CSSPassiveEffect_Relive::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if(tUTCMilsec < m_pCfg->n32WaitMilSec + m_tStartReliveTime){
		return eNormal;
	}

	m_pcMasterGO->ChangeCurHP(m_pcMasterGO, GSToGC::HPMPChangeReason::SkillHurt,m_pCfg->n32BaseHP + ((float)m_pCfg->n32PercentHP*m_pcMasterGO->GetFPData(eEffectCate_MaxHP)/1000) - m_pcMasterGO->GetCurHP());
	m_pcMasterGO->ChangeCurMP(m_pcMasterGO, GSToGC::HPMPChangeReason::SkillHurt,m_pCfg->n32BaseMP + ((float)m_pCfg->n32PercentMP*m_pcMasterGO->GetFPData(eEffectCate_MaxMP)/1000) - m_pcMasterGO->GetCurMP());
	m_pcMasterGO->EndAction_Reliving(TRUE,m_pcMasterGO);
	return eEC_EffectEnd;
}

}