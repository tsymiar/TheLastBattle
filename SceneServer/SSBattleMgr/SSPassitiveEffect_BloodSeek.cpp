#include "SSPassitiveEffect_BloodSeek.h"
#include "SSGameUnit.h"

namespace SceneServer{

CSSPassiveEffect_BloodSeek::CSSPassiveEffect_BloodSeek():
	m_pCfg(NULL)
{

}

CSSPassiveEffect_BloodSeek::~CSSPassiveEffect_BloodSeek(){

}

void CSSPassiveEffect_BloodSeek::Trigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1, INT32 para2, INT32 para3){
	INT32 n32BloodSeekValue = m_pCfg->n32BaseValue 
		+ ((float)m_pCfg->n32Percent*para1/1000) 
		+ (pTarget==NULL?0:((float)m_pCfg->n32TargetHealthPercent * pTarget->GetFPData(eEffectCate_MaxHP)/1000))
		+((float)m_pCfg->n32SelfHealthPercent * m_pcMasterGO->GetFPData(eEffectCate_MaxHP)/1000);

	m_pcMasterGO->ChangeCurHP(m_pcMasterGO,GSToGC::HPMPChangeReason::SkillHurt,n32BloodSeekValue);
}

}