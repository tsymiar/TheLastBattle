#include "SSSkillEffect_Switch.h"
#include "SSGameUnit.h"
#include "SSBattle.h"
#include "SSPassiveSkillMgr.h"

namespace SceneServer{

INT32 CSSkillModel_Switch::Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
		return eEC_EffectEnd;
} 

CSSkillModel_Switch::CSSkillModel_Switch(){
	m_SkillEffectType = eSkillEffectType_Switch;
}

INT32 CSSkillModel_Switch::Begin(){
	if (NULL == m_pcMasterGO || m_pSkillRuntimeData == NULL){
			return eEC_EffectEnd;
	}

	SSkillModelSwitchCfg* pCurCfg = (SSkillModelSwitchCfg*)m_pCfg;

	if(FALSE == m_pSkillRuntimeData->bIfOpen){
		//如果是关闭状态，启动被动技能，设置为不可冷却
		m_pSkillRuntimeData->bIfCanCooldown = FALSE;
		m_pSkillRuntimeData->bIfOpen = TRUE;
		for(int i=0; i<c_n32MaxNextSkillEffectCfgNum; i++){
			INT32 un32PassiveSkillID = pCurCfg->lPassitiveEffectList[i];
			if(0 != un32PassiveSkillID){
				INT32 uniID = GetBattle()->GetPassiveSkillMgr()->AddPassiveSkill(m_pcMasterGO,un32PassiveSkillID);
				if(uniID > 0)
					m_pSkillRuntimeData->un32PassitiveSkillArr[i] = uniID;
			}
		}
	}
	else{
		//如果是开启状态，关闭被动技能，设置为可冷却
		m_pSkillRuntimeData->bIfCanCooldown = FALSE;
		m_pSkillRuntimeData->bIfOpen = FALSE;
		for(int i=0; i<c_n32MaxNextSkillEffectCfgNum; i++){
			INT32 un32PassiveSkillID = m_pSkillRuntimeData->un32PassitiveSkillArr[i];
			if(0 != un32PassiveSkillID){
				GetBattle()->GetPassiveSkillMgr()->RevmovePassiveSkill(un32PassiveSkillID);
			}
			m_pSkillRuntimeData->un32PassitiveSkillArr[i] = 0;
		}
	}


	return eEC_EffectEnd;
}

INT32 CSSkillModel_Switch::End(){
	return eNormal;
}

}