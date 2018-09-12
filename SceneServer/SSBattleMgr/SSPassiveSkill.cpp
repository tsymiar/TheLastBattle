#include "SSPassiveSkill.h"
#include "SSEffectMgr.h"
#include "SSGameUnit.h"
#include "SSEffectMgr.h"
#include "SSBattle.h"
#include "SSBattleMgr.h"
#include "SSCfgMgr.h"
#include "SSPassitiveEffect.h"
#include "SSPassitiveEffect_BloodSeek.h"
#include "SSPassitiveEffect_Rebound.h"
#include "SSPassitiveEffect_Relive.h"
#include "SSHero.h"

namespace SceneServer{

CSSPassiveSkill::CSSPassiveSkill(const SSPassiveSkilllCfg* pCfg):
	m_pCfg(pCfg),
	m_tLastEffectMilsec(0),
	m_pcMasterGO(NULL),
	m_un32UniqueID(0),
	m_un32TriggerTimes(0)
	, m_IfExpired(false)
{
	for (INT32 i = 0; i < c_n32MaxNextSkillEffectCfgNum; ++i){
		m_lKeepedEffects[i] = nullptr;
	}
}


CSSPassiveSkill::~CSSPassiveSkill(){
	OnRemove();
	RemoveAllKeepedEffect();
}

INT32 CSSPassiveSkill::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	for(int i=0; i<c_n32MaxNextSkillEffectCfgNum; i++){
		if(m_lKeepedEffects[i] != NULL){
			if(m_IfExpired || eNormal != m_lKeepedEffects[i]->OnHeartBeat(tUTCMilsec,tTickSpan)){
				DestroyAFreePassitiveEffect(m_lKeepedEffects[i]);
			}
		}
	}
	return eNormal;
}

void CSSPassiveSkill::Trigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1, INT32 para2, INT32 para3){
	//死亡检测
	if(m_pcMasterGO->IsDead() && FALSE == m_pCfg->bIfEffectOnDis){
		return;
	}

	//检查触发目标
	if(FALSE == m_pCfg->bIfAffectBuilding && pTarget != NULL && pTarget->IfNPC_Building()){
		return;
	}
	if(FALSE == m_pCfg->bIfAffectHero && pTarget != NULL && pTarget->IfHero()){
		return;
	}
	if(FALSE == m_pCfg->bIfAffectMonster && pTarget != NULL && pTarget->IfNPC()){
		return;
	}
	//检查CD
	if(m_pCfg->n32Cooldown > 0){
		TIME_MILSEC curTime = GetUTCMiliSecond();
		if(curTime < m_tLastEffectMilsec + m_pCfg->n32Cooldown){
			return;
		}
	}
	//检查概率触发
	if(m_pCfg->n32TriggerRate > 0 && m_pCfg->n32TriggerRate < 1000){
		if(FALSE == CSSEffectMgr::CanEffectBeTriggered(m_pcMasterGO->GetCurBattle()->GetRandomEngine(),m_pCfg->n32TriggerRate)){
			return;
		}
	}
	//检查耗血蓝
	if (m_pcMasterGO->IfHero()){
		CSSHero *pcHero = (CSSHero*)m_pcMasterGO.get();
		if (pcHero->GetCP(eCPCP_Personnal) < m_pCfg->n32UseCP){
			return;
		}
		if (pcHero->GetCurMP() < m_pCfg->n32UseMP){
			return;
		}
		if (pcHero->GetCurHP() < m_pCfg->n32UseHP){
			return;
		}
	}

	//检查触发频率
	if(FALSE == CheckTriggerTimes()){
		return;
	}
	//刷新CD
	if(m_pCfg->n32Cooldown > 0){
		m_tLastEffectMilsec = GetUTCMiliSecond();
	}
	//扣血蓝
	if (m_pcMasterGO->IfHero()){
		CSSHero *pcHero = (CSSHero*)m_pcMasterGO.get();
		if(m_pCfg->n32UseCP > 0) pcHero->ChangeCP(eCPCP_Personnal,-m_pCfg->n32UseCP);
		if(m_pCfg->n32UseHP > 0) pcHero->ChangeCurHP(m_pcMasterGO,GSToGC::HPMPChangeReason::SkillHurt, -m_pCfg->n32UseHP);
		if(m_pCfg->n32UseMP > 0) pcHero->ChangeCurMP(m_pcMasterGO,GSToGC::HPMPChangeReason::SkillHurt, -m_pCfg->n32UseMP);
	}

	//获取触发效果对象列表
	CSSGameUnit* targetVec[2];
	INT8 targetVecSize = 0;
	switch(m_pCfg->ePassiveSkillTargetType){
	case EPassiveSkillTargetType_Self:
		targetVec[0] = m_pcMasterGO;
		targetVecSize = 1;
		break;
	case EPassiveSkillTargetType_Trigger:
		targetVec[0] = pTarget;
		targetVecSize = 1;
		break;
	case EPassiveSkillTargetType_Both:
		targetVec[0] = m_pcMasterGO;
		targetVec[1] = pTarget;
		targetVecSize = 2;
		break;
	}

	//发送被触发消息给客户端
	if(m_pCfg->bIfHasReleaseEffect || m_pCfg->bIfSendColdDown)
		NotifyPassitiveSkillReleaseToGC();
	//调用主动技能
	for(int i=0 ;i<targetVecSize; i++)
		m_pcMasterGO->GetCurBattle()->GetEffectMgr()->AddEffectsFromCfg((SSNextSkillEffectCfg*)m_pCfg->asSkillModelList,m_pcMasterGO,targetVec[i],m_pcMasterGO->GetCurPos(),m_pcMasterGO->GetCurDir(),NULL,GetUTCMiliSecond());
	

	//调用被动效果
	for(int i=0; i < c_n32MaxNextSkillEffectCfgNum; i++){
		bool isOldEffect = TRUE;
		CSSPassiveEffect* pEffect = GetKeepedEffect(m_pCfg->lPassitiveEffectList[i]);
		if(pEffect == NULL){
			pEffect = CreateEffect(m_pCfg->lPassitiveEffectList[i],m_pcMasterGO);
			isOldEffect = FALSE;
		}
		if(pEffect == NULL) continue;
		pEffect->Trigger(pTarget,passiveType,para1,para2,para3);
		if(FALSE == isOldEffect){
			if(pEffect->m_bIfKeeped == FALSE && pEffect->IfLongTimeEffect()){
				AddKeepedEffect(pEffect);
			}
			else{
				DestroyAFreePassitiveEffect(pEffect);
			}
		}
	}
}

void CSSPassiveSkill::OnTrigger(CSSGameUnit* pTarget , EPassiveSkillTriggerType passiveType, INT32 para1, INT32 para2, INT32 para3){
	
}

void CSSPassiveSkill::SetCooldown(){
	if(m_pCfg->n32Cooldown > 0){
		m_tLastEffectMilsec = GetUTCMiliSecond();
	}
}

CSSPassiveEffect* CSSPassiveSkill::CreateEffect(INT32	n32EffectID, CSSGameUnit* pMaster){
	if(n32EffectID == 0) 
		return NULL;
	CSSPassiveEffect* pReturnEffect = NULL;
	EPassiveSkillType pEffectType = CSSCfgMgr::GetSSCfgInstance().GetPassitiveSkillType(n32EffectID);
	switch(pEffectType){
	case EPassiveSkillType_BloodSeek:
		{
			const SSPassiveEffectCfg_BloodSeek* pCfg = CSSCfgMgr::GetSSCfgInstance().GetPassitiveEffectBloodSeekCfg(n32EffectID);
			if(pCfg == NULL){
				break;
			}
			CSSPassiveEffect_BloodSeek* pEffect = new CSSPassiveEffect_BloodSeek;
			pEffect->m_pCfg = pCfg;
			pReturnEffect =  pEffect;
		}
		break;
	case EPassiveSkillType_Rebound:
		{
			const SSPassiveEffectCfg_Rebound* pCfg = CSSCfgMgr::GetSSCfgInstance().GetPassitiveEffectReboundCfg(n32EffectID);
			if(pCfg == NULL){
				break;
			}
			CSSPassiveEffect_Rebound* pEffect = new CSSPassiveEffect_Rebound;
			pEffect->m_pCfg = pCfg;
			pReturnEffect =  pEffect;
		}
		break;
	case EPassiveSkillType_Relive:
		{
			const SSPassiveEffectCfg_Relive* pCfg = CSSCfgMgr::GetSSCfgInstance().GetPassitiveEffectReliveCfg(n32EffectID);
			if(pCfg == NULL){
				break;
			}
			CSSPassiveEffect_Relive* pEffect = new CSSPassiveEffect_Relive;
			pEffect->m_pCfg = pCfg;
			pReturnEffect =  pEffect;
		}
		break;
	}
	if(pReturnEffect != NULL){
		pReturnEffect->m_effectType = pEffectType;
		pReturnEffect->m_pEffectUniqueID = pMaster->GetCurBattle()->GetUniqueEffectID();
		pReturnEffect->m_pcMasterGO = pMaster;
	}
	return pReturnEffect;
}

INT32 CSSPassiveSkill::DestroyAFreePassitiveEffect(CSSPassiveEffect*& pcEffect)
{
	delete pcEffect;
	pcEffect = nullptr;
	return eNormal;
}

void CSSPassiveSkill::AddKeepedEffect(CSSPassiveEffect* pEffect){
	for(int i=0; i<c_n32MaxNextSkillEffectCfgNum; i++){
		if(m_lKeepedEffects[i] == NULL){
			m_lKeepedEffects[i] = pEffect;
			break;
		}
	}
}

CSSPassiveEffect* CSSPassiveSkill::GetKeepedEffect(INT32 pEffectUniqueID){
	for(int i=0; i<c_n32MaxNextSkillEffectCfgNum; i++){
		if(m_lKeepedEffects[i] != NULL && m_lKeepedEffects[i]->m_pEffectUniqueID == pEffectUniqueID){
			return m_lKeepedEffects[i];
		}
	}
	return NULL;
}

void CSSPassiveSkill::RemoveAllKeepedEffect(){
	for(int i=0; i<c_n32MaxNextSkillEffectCfgNum; i++){
		if(m_lKeepedEffects[i] != NULL){
			DestroyAFreePassitiveEffect(m_lKeepedEffects[i]);
		}
	}
}

void CSSPassiveSkill::OnAdd(){
	if(m_pCfg->bIfHasStartEffect && m_pcMasterGO){
		m_pcMasterGO->GetCurBattle()->AddChildSightObject(m_pcMasterGO, this);
	}
	//调用启动时的模块效果
	m_pcMasterGO->GetCurBattle()->GetEffectMgr()->AddEffectsFromCfg((SSNextSkillEffectCfg*)m_pCfg->asStartSkillModelList,m_pcMasterGO,m_pcMasterGO,m_pcMasterGO->GetCurPos(),m_pcMasterGO->GetCurDir(),NULL,GetUTCMiliSecond());
}

void CSSPassiveSkill::OnRemove(){ 
	if(NULL != m_pCfg && m_pCfg->bIfHasStartEffect && m_pcMasterGO) {
		m_pcMasterGO->GetCurBattle()->RemoveChildSightObject(m_pcMasterGO, this);
	}
	//调用结束时的模块效果
	if (m_pcMasterGO){
		m_pcMasterGO->GetCurBattle()->GetEffectMgr()->AddEffectsFromCfg((SSNextSkillEffectCfg*)m_pCfg->asEndSkillModelList,m_pcMasterGO,m_pcMasterGO,m_pcMasterGO->GetCurPos(),m_pcMasterGO->GetCurDir(),NULL,GetUTCMiliSecond());
	}
}

INT32 CSSPassiveSkill::NotifyPassitiveSkillReleaseToGC(){
	if(m_pcMasterGO == NULL){
		return eEC_EffectEnd;
	}
	GSToGC::NotifyPassitiveSkillRelease sMsg;
	sMsg.set_guid(m_pcMasterGO->GetObjGUID());
	sMsg.set_skillid(m_pCfg->un32PassiveSkillID);
	int timeLeft = m_tLastEffectMilsec + m_pCfg->n32Cooldown - GetUTCMiliSecond();
	if(timeLeft < 0) timeLeft = 0;
	sMsg.set_timeleft(timeLeft);

	m_pcMasterGO->GetCurBattle()->SendMsgToAllWatcher(sMsg,sMsg.msgid(),m_pcMasterGO);
	return eNormal;
}


void CSSPassiveSkill::OnAppearInSight(std::vector<CSSUser*>& cTempUserMap){
	if(m_pcMasterGO == NULL){
		return;
	}

	GSToGC::NotifyPassitiveSkillLoad sMsg;
	sMsg.set_guid(m_pcMasterGO->GetObjGUID());
	sMsg.set_skillid(m_pCfg->un32PassiveSkillID);
	sMsg.set_uniqueid(m_un32UniqueID);

	m_pcMasterGO->GetCurBattle()->SendMsgToWatchers(sMsg, sMsg.msgid(), cTempUserMap);
}

void CSSPassiveSkill::OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap){
	if(m_pcMasterGO == NULL){
		return;
	}

	GSToGC::NotifyPassitiveSkillUnLoad sMsg;
	sMsg.set_guid(m_pcMasterGO->GetObjGUID());
	sMsg.set_skillid(m_pCfg->un32PassiveSkillID);
	sMsg.set_uniqueid(m_un32UniqueID);

	m_pcMasterGO->GetCurBattle()->SendMsgToWatchers(sMsg, sMsg.msgid(), cTempUserMap);
}

void CSSPassiveSkill::SetBattleMgr(CSSBattleMgr* pMgr){
	m_pCSSBattleMgr = pMgr;
}

}