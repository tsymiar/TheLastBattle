#include "SSSkillEffect_Purification.h"
#include "SSGameUnit.h"
#include "SSBattle.h"
#include "SSSkillEffect_Buf.h"
#include "SSEffectMgr.h"

namespace SceneServer{

	INT32 CSSSkillEffect_Purification::Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
		return eEC_EffectEnd;
	} 
	//发送净化消息
	INT32 CSSSkillEffect_Purification::NotifySkillModelPurificationToGC(){
		GSToGC::NotifySkillModelPurification sMsg;
		sMsg.set_guid(m_pcMasterGO->GetObjGUID());
		if (m_pcTarGU){
			sMsg.set_targetguid(m_pcTarGU->GetObjGUID());
		}

		sMsg.set_effectid(m_pCfg->un32SkillModelID);

		GetBattle()->SyncMsgToGC(sMsg, sMsg.msgid(), m_pcMasterGO->GetObjGUID(), m_pcMasterGO, NULL);
		return eNormal;
	}

	CSSSkillEffect_Purification::CSSSkillEffect_Purification(){
		m_SkillEffectType = eSkillEffectType_Purification;
	}

	INT32 CSSSkillEffect_Purification::Begin(){
		if (NULL == m_pcMasterGO || FALSE == CheckBattle() || NULL == m_pcTarGU){
			return eEC_EffectEnd;
		}

		if(FALSE == CheckHitTargetCamp(m_pcTarGU) || FALSE == CheckHitTargetType(m_pcTarGU)){
			return eEC_EffectEnd;
		}

		vector<CSSSkillEffect_Buf*> removeVec;
		SSkillModelPurificationCfg* pPurCfg = (SSkillModelPurificationCfg*)m_pCfg;
		for (UINT32* iter = m_pcTarGU->GetBufArr().Begin(); iter != m_pcTarGU->GetBufArr().End(); iter = m_pcTarGU->GetBufArr().Next()){
			CSSSkillEffect_Buf* pBuf = (CSSSkillEffect_Buf*)m_pcTarGU->GetCurBattle()->GetEffectMgr()->GetEffect(*iter);
			if (NULL != pBuf){
				SSkillModelBufCfg* pTempCfg = (SSkillModelBufCfg*)pBuf->m_pCfg;
				bool ifRemove = FALSE;
				if(ePurType_byType == pPurCfg->ePurType){
					if(pPurCfg->lPurParameters[0] == pTempCfg->eBuffType){
						ifRemove = TRUE;
					}
				}
				else if(ePurType_byID == pPurCfg->ePurType){
					for(int i=0;i<c_n32MaxNextSkillEffectCfgNum;i++){
						if(pPurCfg->lPurParameters[i] == 0) break;
						if(pPurCfg->lPurParameters[i] == pTempCfg->un32SkillModelID){
							ifRemove = TRUE;
							break;
						}
					}
				}
				else if(ePurType_byEffect == pPurCfg->ePurType){
					if(pPurCfg->lPurParameters[0] != 0 && pPurCfg->lPurParameters[0] == pTempCfg->sEffectInfo.eEffectCate){
						if(FALSE == ((pTempCfg->sEffectInfo.eEffectCate == eEffectCate_AttackSpeed || pTempCfg->sEffectInfo.eEffectCate == eEffectCate_MoveSpeed) && pTempCfg->sEffectInfo.n32EffectBaseValue >= 0)){
							ifRemove = TRUE;
						}
					}
				}

				//将其加入准备移除列表
				if(ifRemove){
					removeVec.push_back(pBuf);
				}
				//检查移除的数量是否达到配置要求了。
				if(pPurCfg->n32MaxNum != 0 && removeVec.size() >= pPurCfg->n32MaxNum){
					break;
				}
			}
		}
		bool ifRemoved = removeVec.size() > 0;
		for(auto iter = removeVec.begin(); iter != removeVec.end(); iter++){
			(*iter)->ForceStop();
		}

		if(ifRemoved){
			//如果净化成功，则发送消息
			NotifySkillModelPurificationToGC();
		}
		return eEC_EffectEnd;
	}

	INT32 CSSSkillEffect_Purification::End(){
		return eNormal;
	}

}