#include "SSPassitiveEffect_Rebound.h"
#include "SSGameUnit.h"

namespace SceneServer{

CSSPassiveEffect_Rebound::CSSPassiveEffect_Rebound():
	m_pCfg(NULL)
{

}

CSSPassiveEffect_Rebound::~CSSPassiveEffect_Rebound(){

}

void CSSPassiveEffect_Rebound::Trigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1, INT32 para2, INT32 para3){
	INT32 n32ReboundValue = m_pCfg->n32BaseValue 
		+ ((float)m_pCfg->n32Percent*para1/1000) 
		+ (pTarget==NULL?0:((float)m_pCfg->n32TargetHealthPercent * pTarget->GetFPData(eEffectCate_MaxHP)/1000))
		+((float)m_pCfg->n32TargetHealthPercent * m_pcMasterGO->GetFPData(eEffectCate_MaxHP)/1000);


	pTarget->ChangeCurHP(m_pcMasterGO,GSToGC::HPMPChangeReason::SkillHurt,-n32ReboundValue);
}

}