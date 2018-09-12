#include "SSGUParameterMgr.h"
#include "SSGameUnit.h"
#include "SSBattle.h"

namespace SceneServer{

	CSSGUParameterMgr::CSSGUParameterMgr():m_pMasterGU(NULL)
	{
		for(int i=0;i<eEffectCate_End;i++){
			paras[i] = new CSSGUParameter();
		}


		paras[eEffectCate_CurHP]->SetMinBaseValue(0);
		paras[eEffectCate_CurMP]->SetMinBaseValue(0);
		paras[eEffectCate_MoveSpeed]->SetIfOnlyOneMinusePercent(TRUE);
		paras[eEffectCate_MoveSpeed]->SetMinValue(1);
		paras[eEffectCate_MoveSpeed]->SetMinBaseValue(1);
		paras[eEffectCate_MoveSpeed]->SetMaxValue(c_n32MaxMoveSpeed);
		paras[eEffectCate_AttackSpeed]->SetCountDown(TRUE);
		paras[eEffectCate_AttackSpeed]->SetIfOnlyOneMinusePercent(TRUE);
		paras[eEffectCate_AttackSpeed]->SetMinValue(1);
		paras[eEffectCate_AttackSpeed]->SetMinBaseValue(1);
		paras[eEffectCate_AttackSpeed]->SetMaxValue(5000);
		paras[eEffectCate_Dizziness]->SetType(eParaType_Status);
		paras[eEffectCate_Silence]->SetType(eParaType_Status);
		paras[eEffectCate_Dizziness]->SetType(eParaType_Status);
		paras[eEffectCate_Silence]->SetType(eParaType_Status);

		paras[eEffectCate_MaxHP]->SetMaxValueChangeCallback(paras[eEffectCate_CurHP]);
		paras[eEffectCate_MaxMP]->SetMaxValueChangeCallback(paras[eEffectCate_CurMP]);
	}

	CSSGUParameterMgr::~CSSGUParameterMgr(){
		for(int i=0;i<eEffectCate_End;i++){
			delete paras[i];
			paras[i] = NULL;
		}
	}

	VOID	CSSGUParameterMgr::AddBaseGroup(SFightProperty fp, bool isBaseValue){
		AddBaseValue(eEffectCate_PhyAttack,fp.n32PhyAttPower);
		AddBaseValue(eEffectCate_MagicAttack,fp.n32MagicAttPower);
		AddBaseValue(eEffectCate_PhyDefense,fp.n32PhyDefAbility);
		AddBaseValue(eEffectCate_MagicDefense,fp.n32MagicDefAbility);
		AddBaseValue(eEffectCate_MoveSpeed,fp.n32MoveSpeed);
		if(isBaseValue)
			AddBaseValue(eEffectCate_AttackSpeed,fp.n32AttackSpeed);
		else
			AddPercentValue(eEffectCate_AttackSpeed,fp.n32AttackSpeed);
		AddBaseValue(eEffectCate_MaxHP,fp.n32MaxHP);
		AddBaseValue(eEffectCate_MaxMP,fp.n32MaxMP);
		AddBaseValue(eEffectCate_HPRecoverRate,fp.n32HPRecover);
		AddBaseValue(eEffectCate_MPRecoverRate,fp.n32MPRecover);
		AddBaseValue(eEffectCate_ReliveTime,fp.n32ReliveSecond);
		AddBaseValue(eEffectCate_CriPersent,fp.n32CriPersent);
		AddBaseValue(eEffectCate_CriHarm,fp.n32CriHarm);
		AddBaseValue(eEffectCate_CPRecover,fp.n32CPRecover);
		AddBaseValue(eEffectCate_Dizziness,fp.n32DizzinessCounter);
		AddBaseValue(eEffectCate_Silence,fp.n32SilenceCounter);
		AddBaseValue(eEffectCate_Disarm,fp.n32DisarmCounter);
		AddBaseValue(eEffectCate_Restrain,fp.n32RestrainCounter);
		AddBaseValue(eEffectCate_Invisible,fp.n32InvisibleCounter);
		AddBaseValue(eEffectCAte_AttackDist,fp.n32AttackDist);
	}

	VOID	CSSGUParameterMgr::AddPercentGroup(SFightProperty fp){
		AddPercentValue(eEffectCate_PhyAttack,fp.n32PhyAttPower);
		AddPercentValue(eEffectCate_MagicAttack,fp.n32MagicAttPower);
		AddPercentValue(eEffectCate_PhyDefense,fp.n32PhyDefAbility);
		AddPercentValue(eEffectCate_MagicDefense,fp.n32MagicDefAbility);
		AddPercentValue(eEffectCate_MoveSpeed,fp.n32MoveSpeed);
		AddPercentValue(eEffectCate_AttackSpeed,fp.n32AttackSpeed);
		AddPercentValue(eEffectCate_MaxHP,fp.n32MaxHP);
		AddPercentValue(eEffectCate_MaxMP,fp.n32MaxMP);
		AddPercentValue(eEffectCate_HPRecoverRate,fp.n32HPRecover);
		AddPercentValue(eEffectCate_MPRecoverRate,fp.n32MPRecover);
		AddPercentValue(eEffectCate_ReliveTime,fp.n32ReliveSecond);
		AddPercentValue(eEffectCate_CriPersent,fp.n32CriPersent);
		AddPercentValue(eEffectCate_CriHarm,fp.n32CriHarm);
		AddPercentValue(eEffectCate_CPRecover,fp.n32CPRecover);
		AddPercentValue(eEffectCate_Dizziness,fp.n32DizzinessCounter);
		AddPercentValue(eEffectCate_Silence,fp.n32SilenceCounter);
		AddPercentValue(eEffectCate_Disarm,fp.n32DisarmCounter);
		AddPercentValue(eEffectCate_Restrain,fp.n32RestrainCounter);
		AddPercentValue(eEffectCate_Invisible,fp.n32InvisibleCounter);
		AddPercentValue(eEffectCAte_AttackDist,fp.n32AttackDist);
	}

	INT32	CSSGUParameterMgr::GetValue(INT32 type){
		if(type < 0 || type >= eEffectCate_End) return 0;
		return paras[type]->GetValue();
	}

	VOID	CSSGUParameterMgr::AddBaseValue(INT32 type, INT32 value, INT32 uniqueID){
		if(type < 0 || type >= eEffectCate_End) return;
		CSSGUParameter* para = paras[type];
		para->AddBase(value,uniqueID);
		CheckMaxValue(type);

		if(type == eEffectCate_Dizziness && value != 0){
			OnDizzChanged(true,value);
		}
	}

	VOID	CSSGUParameterMgr::RemoveBaseValue(INT32 type, INT32 value, INT32 uniqueID){
		if(type < 0 || type >= eEffectCate_End) return;
		CSSGUParameter* para = paras[type];
		para->RemoveBase(value,uniqueID);
		CheckMaxValue(type);

		if(type == eEffectCate_Dizziness && value != 0){
			OnDizzChanged(false,value);
		}
	}

	VOID	CSSGUParameterMgr::AddPercentValue(INT32 type, INT32 value, INT32 uniqueID){
		if(type < 0 || type >= eEffectCate_End) return;
		CSSGUParameter* para = paras[type];
		para->AddPercent(value,uniqueID);
		CheckMaxValue(type);
	}

	VOID	CSSGUParameterMgr::RemovePercentValue(INT32 type, INT32 value, INT32 uniqueID){
		if(type < 0 || type >= eEffectCate_End) return;
		CSSGUParameter* para = paras[type];
		para->RemovePercent(value,uniqueID);
		CheckMaxValue(type);
	}

	VOID	CSSGUParameterMgr::CheckMaxValue(INT32 type){
		if(type == eEffectCate_MaxHP  || type == eEffectCate_None){
			paras[eEffectCate_CurHP]->SetMaxBaseValue(GetValue(eEffectCate_MaxHP));
		}
		if(type == eEffectCate_MaxMP  || type == eEffectCate_None){
			paras[eEffectCate_CurMP]->SetMaxBaseValue(GetValue(eEffectCate_MaxMP));
		}
	}


	INT32	CSSGUParameterMgr::SyncFPToGC(){
		GSToGC::NotifyAFPData msgFP;
		msgFP.set_guid(m_pMasterGU->GetObjGUID());
		for(int i=1;i<eEffectCate_End;i++){
			if(i == eEffectCate_CurHP || i == eEffectCate_CurMP) continue;
			if(paras[i]->IfChanged()){
				GSToGC::NotifyAFPData_FPInfo* pFpInfo = msgFP.add_info();
				pFpInfo->set_type(i);
				pFpInfo->set_value(paras[i]->GetValueForSend());
			}
		}
		CheckMaxValue(eEffectCate_None);
		m_pMasterGU->GetCurBattle()->SendMsgToAllWatcher(msgFP, msgFP.msgnum(), m_pMasterGU);
		return eNormal;
	}

	VOID	CSSGUParameterMgr::ChangeHP(INT32 value,GSToGC::HPMPChangeReason reason){
		INT32 noLimitValue = GetValue(eEffectCate_CurHP) + value;
		AddBaseValue(eEffectCate_CurHP,value);
		SynHP(value > 0?GetValue(eEffectCate_CurHP):noLimitValue,reason);
	}

	VOID	CSSGUParameterMgr::ChangeMP(INT32 value,GSToGC::HPMPChangeReason reason){
		AddBaseValue(eEffectCate_CurMP,value);
		SynMP(GetValue(eEffectCate_CurMP),reason);
	}

	VOID	CSSGUParameterMgr::SynHP(INT32 value,GSToGC::HPMPChangeReason reason){
		GSToGC::HPChange sHpChange;
		sHpChange.set_guid(m_pMasterGU->GetObjGUID());
		sHpChange.set_hp(value);
		sHpChange.set_reason(reason);
		m_pMasterGU->GetCurBattle()->SendMsgToAllWatcher(sHpChange, sHpChange.msgid(), m_pMasterGU);
	}

	VOID	CSSGUParameterMgr::SynMP(INT32 value,GSToGC::HPMPChangeReason reason){
		GSToGC::MpChange sMsg;
		sMsg.set_guid(m_pMasterGU->GetObjGUID());
		sMsg.set_mp(value);
		sMsg.set_reason(reason);
		m_pMasterGU->GetCurBattle()->SendMsgToAllWatcher(sMsg, sMsg.msgid(), m_pMasterGU);
	}

	INT32	CSSGUParameterMgr::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
		for(int i=0;i<eEffectCate_End;i++){
			if(i == eEffectCate_CurHP || i == eEffectCate_CurMP) continue;
			if(paras[i]->IfChanged()){
				SyncFPToGC();
				break;
			}
		}

		if(paras[eEffectCate_CurHP]->IfChanged()){
			SynHP(paras[eEffectCate_CurHP]->GetValueForSend(),GSToGC::HPMPChangeReason::RecoverHurt);
		}
		if(paras[eEffectCate_CurMP]->IfChanged()){
			SynMP(paras[eEffectCate_CurMP]->GetValueForSend(),GSToGC::HPMPChangeReason::RecoverHurt);
		}

		for(int i=0;i<eEffectCate_End;i++){
			paras[i]->OnSend();
		}
		return eNormal;
	}


	void	CSSGUParameterMgr::OnDizzChanged(bool ifAdd, INT32 changeValue){
		INT32 nowValue = paras[eEffectCate_Dizziness]->GetValue();
		INT32 oldValue = ifAdd?nowValue - changeValue:nowValue + changeValue;
		if(nowValue > 0 && oldValue <= 0){
			m_pMasterGU->OnDizziness();
		}
		else if(nowValue <= 0 && oldValue > 0){
			m_pMasterGU->OnDisDizziness();
		}
	}

	void	CSSGUParameterMgr::SetValueChangeCallBack(INT32 type, ISSParameterChangeCallBack* pCallBack){
		if(type < 0 || type >= eEffectCate_End) return;
		CSSGUParameter* para = paras[type];
		para->SetValueChangeCallback(pCallBack);
	}
}