#include "stdafx.h"
#include "SSSkillEffect_Link.h"
#include "SSGameObject.h"
#include "SSGameUnit.h"
#include "SSBattle.h"
#include "SSHero.h"
#include "SSEffectMgr.h"

namespace SceneServer{

	CSSSkillEffect_Link::CSSSkillEffect_Link():m_eLinkState(eLink_Begin)
	{
		m_SkillEffectType = eSkillEffectType_Link;
	}


	CSSSkillEffect_Link::~CSSSkillEffect_Link(){
	}

	

	INT32 CSSSkillEffect_Link::Begin(){
		m_pLinkCfg = (SSkillModelLinkCfg*)m_pCfg;
		if (NULL == m_pLinkCfg){
			return eEC_EffectEnd;
		}

		if( NULL == m_pcTarGU){
			return eEC_EffectEnd;
		}

		if (NULL != m_pcMasterGO){
			m_cLinkPos = m_pcMasterGO->GetCurPos();
			m_tBeginMilsec = GetUTCMiliSecond();
			m_eLinkState = eLink_Begin;
			//加入视野系统
			m_pcTarGU->GetCurBattle()->AddChildSightObject(m_pcTarGU, this);
			return eNormal;
		}

		return eEC_EffectEnd;
	}

	INT32 CSSSkillEffect_Link::Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){	
		if (NULL == m_pcMasterGO || NULL == m_pcTarGU){
			m_eLinkState = eLink_Break;
			return eEC_EffectEnd;
		}

		//检查打断
		if (TRUE == m_pcMasterGO->IsDead() || ( m_pLinkCfg->bIfCanBeBreak && (m_pcMasterGO->GetFPData(eEffectCate_Dizziness) > 0 || m_pcMasterGO->GetFPData(eEffectCate_Silence) > 0)) ){
			m_eLinkState = eLink_Break;
			return eEC_EffectEnd;
		}

		//检查距离
		if(m_pLinkCfg->n32FarthestDistance > 0){
			CVector3D pos = m_pLinkCfg->eStartPointType == eLinkStartPointType_Releaser?m_pcMasterGO->GetCurPos():m_cLinkPos;
			if(FALSE == pos.CanWatch(m_pLinkCfg->n32FarthestDistance,m_pcTarGU->GetCurPos())){
				m_eLinkState = eLink_Break;
				return eEC_EffectEnd;
			}
		}

		if (tUTCMilsec - m_tBeginMilsec >= m_pLinkCfg->n32LastTime){
			m_eLinkState = eLink_Finish;
			return eEC_EffectEnd;
		}

		return eNormal;
	}

	INT32 CSSSkillEffect_Link::End(){
		if (NULL == m_pcMasterGO){
			return eEC_EffectEnd;
		}
		//从视野系统中移除
		GetBattle()->RemoveChildSightObject(m_pcTarGU, this);
		
		if(m_eLinkState == eLink_Break){
			GetBattle()->GetEffectMgr()->AddEffectsFromCfg((SSNextSkillEffectCfg*)m_pLinkCfg->asBreakModelList, m_pcMasterGO
				, m_pcTarGU, m_pcMasterGO->GetCurPos(), m_pcMasterGO->GetCurDir(),m_pSkillRuntimeData, GetUTCMiliSecond());
		}
		else if(m_eLinkState == eLink_Finish){
			GetBattle()->GetEffectMgr()->AddEffectsFromCfg((SSNextSkillEffectCfg*)m_pLinkCfg->asFinishModelList, m_pcMasterGO
				, m_pcTarGU, m_pcMasterGO->GetCurPos(), m_pcMasterGO->GetCurDir(),m_pSkillRuntimeData, GetUTCMiliSecond());
		}

		StopDependedEffect();

		return eNormal;
	}


	//发送在视野中出现的消息
	void CSSSkillEffect_Link::OnAppearInSight(std::vector<CSSUser*>& cTempUserMap){
		if(NULL == m_pcTarGU || NULL == m_pcMasterGO){
			return;
		}

		GSToGC::NotifySkillModelStartLink sMsg;
		if(m_pLinkCfg->eStartPointType == eLinkStartPointType_Releaser){
			sMsg.set_guid(m_pcMasterGO->GetObjGUID());
		}
		else{
			GSToGC::Pos* pos = new GSToGC::Pos;
			CSSBattle::SetPos(m_cLinkPos, *pos);
			sMsg.set_allocated_beginpos(pos);
		}
		sMsg.set_targetguid(m_pcTarGU->GetObjGUID());
		sMsg.set_uniqueid(m_un32UniqueID);
		sMsg.set_effectid(m_pCfg->un32SkillModelID);

		m_pcTarGU->GetCurBattle()->SendMsgToWatchers(sMsg, sMsg.msgid(), cTempUserMap);
	}
	//发送在视野中消失的消息
	void CSSSkillEffect_Link::OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap){
		if(NULL == m_pcTarGU || NULL == m_pcMasterGO){
			return;
		}

		GSToGC::NotifySkillModelStopLink sMsg;
		sMsg.set_uniqueid(m_un32UniqueID);

		m_pcTarGU->GetCurBattle()->SendMsgToWatchers(sMsg, sMsg.msgid(), cTempUserMap);
	}

	float CSSSkillEffect_Link::GetSightPosX(){
		if(NULL == m_pcTarGU){
			return 0;
		}
		return m_pcTarGU->GetCurPos().m_fX;
	}
	float CSSSkillEffect_Link::GetSightPosY(){
		if(NULL == m_pcTarGU){
			return 0;
		}
		return m_pcTarGU->GetCurPos().m_fZ;
	}

}