
#include "SSSkillEffect_Buf.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSEffectMgr.h"
#include "SSPassiveSkillMgr.h"
#include "SSSkillEffect_Caculate.h"
namespace SceneServer{

CSSSkillEffect_Buf::CSSSkillEffect_Buf():m_tBeginDotMilsec(0)
	, m_pBufCfg(NULL)
	, m_bIfBuffAdded(FALSE)
	, m_passitiveSkillID(0)
	, m_repeatTimes(0)
{
	m_SkillEffectType = eSkillEffectType_Buf;
}

CSSSkillEffect_Buf::~CSSSkillEffect_Buf(){
}

const SSkillModelBufCfg* CSSSkillEffect_Buf::GetModelConfig(){
	return (const SSkillModelBufCfg*)m_pCfg;
}

INT32	CSSSkillEffect_Buf::Begin(){
	m_pBufCfg = (SSkillModelBufCfg*)m_pCfg;
	if (NULL == m_pBufCfg){
		return eEC_EffectEnd;
	}

	if(m_pBufCfg->eBuffTarget == eSBTC_Releaser){
		m_pcTarGU = m_pcMasterGO;
	}

	if(NULL == m_pcTarGU || m_pcTarGU->IsDead()){
		return eEC_EffectEnd;
	}

	if(FALSE == m_pcTarGU->CanAddBuf()){
		return eEC_EffectEnd;
	}

	if(FALSE == CheckHitTargetCamp(m_pcTarGU) || FALSE == CheckHitTargetType(m_pcTarGU)){
		return eEC_EffectEnd;
	}
	

	CSSSkillEffect_Buf* pSameBuf = NULL;//可重复的buf
	INT32 n32SameBuffNum = 0;

	CSSSkillEffect_Buf* pRejectBuf = NULL;//相互冲突的buf

	CSSSkillEffect_Buf* pReplaceBuf = NULL;//会覆盖的buf

	TIME_MILSEC tCurTime = GetUTCMiliSecond();

	for (UINT32* iter = m_pcTarGU->GetBufArr().Begin(); iter != m_pcTarGU->GetBufArr().End(); iter = m_pcTarGU->GetBufArr().Next()){
		CSSSkillEffect_Buf* pBuf = (CSSSkillEffect_Buf*)m_pcTarGU->GetCurBattle()->GetEffectMgr()->GetEffect(*iter);
		if (NULL != pBuf){
			SSkillModelBufCfg* pTempCfg = (SSkillModelBufCfg*)pBuf->m_pCfg;
			if (NULL == pTempCfg){
				m_pcTarGU->GetBufArr().RemoveElement(*iter);
				continue;
			}
			//获取三种(堆叠，冲突，覆盖)buff，如果有的话
			if(pTempCfg->un32SkillModelID == m_pBufCfg->un32SkillModelID){
				n32SameBuffNum++;
				pSameBuf = pBuf;
			}
			if(m_pBufCfg->n32RejectId != 0 && pTempCfg->n32RejectId == m_pBufCfg->n32RejectId){
				pRejectBuf = pBuf;
			}
			if(m_pBufCfg->n32ReplaceId != 0 && pTempCfg->n32ReplaceId == m_pBufCfg->n32ReplaceId){
				pReplaceBuf = pBuf;
			}
		}
	}

	//如果有互斥的buff，则本buff失效
	if(pRejectBuf != NULL){
		return eEC_EffectEnd;
	}

	if(pReplaceBuf != NULL){
		//有需要覆盖的buff。被覆盖的buff直接终结
		pReplaceBuf->SetExpired();
	}
	//如果有相同的buff
	else if(pSameBuf != NULL){
		//如果不可堆叠，则返回
		if(m_pBufCfg->eReplaceType == EReplaceType_Forbit){
			return eEC_EffectEnd;
		}
		//如果为重置时间类型的堆叠
		else if(m_pBufCfg->eReplaceType == EReplaceType_Reset){
			//如果堆叠次数未满，则给原有buff添加效果
			if(pSameBuf->GetRepeateTimes() < m_pBufCfg->n32ReplaceTimes){
				pSameBuf->AddRepeatEffect();
			}
			//不管是否添加了层数，都需要重置持续时间
			pSameBuf->ResetTime();
			//本buff失效结束
			return eEC_EffectEnd;
		}
		//如果是各自计算持续时间的堆叠
		else if(m_pBufCfg->eReplaceType == EReplaceType_SingleCaculate){
			//如果堆叠的次数大于最大可堆叠次数，本次buff失效
			if(m_pBufCfg->n32ReplaceTimes > 0 && n32SameBuffNum >= m_pBufCfg->n32ReplaceTimes){
				return eEC_EffectEnd;
			}
		}
		
	}

	m_tBeginDotMilsec = tCurTime - m_pBufCfg->n32EffectInterval;
	m_tBeginMilsec = tCurTime;

	if(FALSE == m_bIfBuffAdded){
		//添加buff效果
		AddBuffEffect(FALSE);
		//将buff加入到对象的buff列表中去
		m_pcTarGU->AddBuf(GetUniqueID());
		//buff作为对象的子视野对象，加入到视野系统中
		m_pcTarGU->GetCurBattle()->AddChildSightObject(m_pcTarGU, this);
	}
	
	m_pcTarGU->GetCurBattle()->GetEffectMgr()->AddEffectsFromCfg(m_pBufCfg->asSkillStartModelList
		, m_pcMasterGO, m_pcTarGU, m_pcTarGU->GetCurPos(),  m_pcTarGU->GetCurDir() - m_pcMasterGO->GetCurDir(), m_pSkillRuntimeData, tCurTime);
	return eNormal;
}

void CSSSkillEffect_Buf::AddRepeatEffect(){
	//添加buff效果
	AddBuffEffect(TRUE);
}

void CSSSkillEffect_Buf::AddBuffEffect(bool bIfRepeat){
	m_bIfBuffAdded = TRUE;
	if(m_pBufCfg->sEffectInfo.eEffectCate != eEffectCate_None){
		//如果buff类型为被动，则给对象挂上被动技能
		if(eEffectCAte_PassitiveSkill == m_pBufCfg->sEffectInfo.eEffectCate){
			if(FALSE == bIfRepeat){//被动只能添加一次
				m_passitiveSkillID = GetBattle()->GetPassiveSkillMgr()->AddPassiveSkill(m_pcTarGU,m_pBufCfg->sEffectInfo.n32EffectBaseValue);
			}
		}
		//如果不是被动技能，则给对象加上对应的属性
		else{
			INT32 n32OriVal = m_pcTarGU->GetFPData(m_pBufCfg->sEffectInfo.eEffectCate);
			CSSSkillEffect_Caculate::CaculateSkillEffectOnce(m_pcMasterGO, m_pcTarGU, m_pBufCfg->sEffectInfo, 0); 
			m_repeatTimes++;//堆叠层数+1
		}
	}
}

INT32 CSSSkillEffect_Buf::Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if (m_ifExpired){
		return eEC_EffectEnd;
	}

	if(m_tBeginMilsec + m_pBufCfg->n32EffectLastTick <= tUTCMilsec){
		return eEC_EffectEnd;
	}

	if(NULL == m_pcTarGU){
		return eEC_EffectEnd;
	}
	//如果非英雄的NPC死亡了，他施放的buff要移除
	if( m_pcMasterGO->IsDead() && FALSE == m_pcMasterGO->IfHero() ){
		return eEC_EffectEnd;
	}
	//目标死亡，且设置为死亡后清除buff(大部分buff都是)，则清除
	if(m_pcTarGU->IsDead() && TRUE == m_pBufCfg->bIfClearAtDie){
		return eEC_EffectEnd;
	}
	//每到了时间点，就调用一次dot效果
	if(m_pBufCfg->n32EffectInterval > 0 && m_tBeginDotMilsec + m_pBufCfg->n32EffectInterval <= tUTCMilsec){
		m_tBeginDotMilsec += m_pBufCfg->n32EffectInterval;
		GetBattle()->GetEffectMgr()->AddEffectsFromCfg(m_pBufCfg->asSkillIntervalModelList, m_pcMasterGO, m_pcTarGU, m_pcTarGU->GetCurPos()
			, m_pcTarGU->GetCurDir(), m_pSkillRuntimeData, tUTCMilsec);
	}

	return eNormal;
} 

INT32	CSSSkillEffect_Buf::End(){
	if (FALSE == m_bIfBuffAdded){
		return eEC_EffectEnd;
	}
	m_bIfBuffAdded = FALSE;

	if(NULL == m_pcTarGU){
		return eEC_EffectEnd;
	}
	//buf结束的时候，调用结束时候的模块(大部分buff都木有)
	GetBattle()->GetEffectMgr()->AddEffectsFromCfg(m_pBufCfg->asSkillEndModelList, m_pcMasterGO, m_pcTarGU, m_pcTarGU->GetCurPos()
		, m_pcTarGU->GetCurDir(), m_pSkillRuntimeData, GetUTCMiliSecond());

	if(m_pBufCfg->sEffectInfo.eEffectCate != eEffectCate_None){
		//如果是被动效果的buff，需要将被动从对象身上移除
		if(eEffectCAte_PassitiveSkill == m_pBufCfg->sEffectInfo.eEffectCate){
			if(m_passitiveSkillID > 0){
				GetBattle()->GetPassiveSkillMgr()->RevmovePassiveSkill(m_passitiveSkillID);
				m_passitiveSkillID = 0;
			}
		}
		//如果不是被动效果，则需要将添加的属性从对象上移除
		else{
			//添加了多少次属性，就需要移除多少次
			for(int i=0;i<m_repeatTimes;i++){
				CSSSkillEffect_Caculate::CaculateSkillEffectOnce(m_pcMasterGO, m_pcTarGU, m_pBufCfg->sEffectInfo, 0,FALSE,FALSE);
			}
		}
	}
	//从对象身上移除buff的ID
	// 注意：当过期时，说明是tar主动删的，不能再卸载
	m_pcTarGU->RemoveBuf(m_un32UniqueID);
	//从视野系统中移除buff的视野
	GetBattle()->RemoveChildSightObject(m_pcTarGU, this);

	return eNormal;
}
void CSSSkillEffect_Buf::Clear(){
	End();
}

void CSSSkillEffect_Buf::ResetTime(){
	m_tBeginMilsec = GetUTCMiliSecond();
}

//发送buff在视野中出现的消息
void CSSSkillEffect_Buf::OnAppearInSight(std::vector<CSSUser*>& cTempUserMap){
	if(NULL == m_pcTarGU || NULL == m_pcMasterGO){
		return;
	}

	GSToGC::BuffEffect sMsg;
	sMsg.set_guid(m_pcMasterGO->GetObjGUID());
	sMsg.set_uniqueid(m_un32UniqueID);
	sMsg.set_effectid(m_pCfg->un32SkillModelID);
	sMsg.set_time(GetUTCMiliSecond() - m_tBeginDotMilsec - m_pBufCfg->n32EffectInterval);
	sMsg.set_state(EBuffMsgStatus_Start);
	if (m_pcTarGU){
		sMsg.set_targuid(m_pcTarGU->GetObjGUID());
	}
	
	m_pcTarGU->GetCurBattle()->SendMsgToWatchers(sMsg, sMsg.msgid(), cTempUserMap);
}
//发送buff在视野中消失的消息
void CSSSkillEffect_Buf::OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap){
	if(NULL == m_pcTarGU || NULL == m_pcMasterGO){
		return;
	}

	GSToGC::BuffEffect sMsg;
	sMsg.set_guid(m_pcMasterGO->GetObjGUID());
	sMsg.set_uniqueid(m_un32UniqueID);
	sMsg.set_effectid(m_pCfg->un32SkillModelID);
	sMsg.set_time(GetUTCMiliSecond() - m_tBeginDotMilsec - m_pBufCfg->n32EffectInterval);
	sMsg.set_state(EBuffMsgStatus_End);
	if (m_pcTarGU){
		sMsg.set_targuid(m_pcTarGU->GetObjGUID());
	}

	m_pcTarGU->GetCurBattle()->SendMsgToWatchers(sMsg, sMsg.msgid(), cTempUserMap);
}

float CSSSkillEffect_Buf::GetSightPosX(){
	if(NULL == m_pcTarGU){
		return 0;
	}
	return m_pcTarGU->GetCurPos().m_fX;
}
float CSSSkillEffect_Buf::GetSightPosY(){
	if(NULL == m_pcTarGU){
		return 0;
	}
	return m_pcTarGU->GetCurPos().m_fZ;
}

CVector3D CSSSkillEffect_Buf::GetCurPos(){
	if(NULL == m_pcTarGU){
		return CVector3D();
	}
	return m_pcTarGU->GetCurPos();
}

bool CSSSkillEffect_Buf::IfParaInvalid(){
	return TRUE == IsForceStop();
}

void CSSSkillEffect_Buf::SetExpired(){
	m_ifExpired = true;
}

}