#include "SSSkillEffect_Caculate.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSPassiveSkillMgr.h"

namespace SceneServer{

INT32 CSSSkillEffect_Caculate::Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	return eEC_EffectEnd;
} 
//发送结算消息
INT32 CSSSkillEffect_Caculate::NotifySkillModelHitObjectToGC(){
	if(m_pcMasterGO == NULL || FALSE == CheckBattle()){
		return eEC_EffectEnd;
	}

	GSToGC::HitTar sMsg;
	sMsg.set_guid(m_pcMasterGO->GetObjGUID());
	if (m_pcTarGU){
		sMsg.set_targuid(m_pcTarGU->GetObjGUID());
	}

	sMsg.set_effectid(m_pCfg->un32SkillModelID);

	GetBattle()->SyncMsgToGC(sMsg, sMsg.msgid(), m_pcMasterGO->GetObjGUID(), m_pcMasterGO, NULL);

	return eNormal;
}

CSSSkillEffect_Caculate::CSSSkillEffect_Caculate(){
	m_SkillEffectType = eSkillEffectType_Caculate;
}

INT32 CSSSkillEffect_Caculate::Begin(){
	if (NULL == m_pcMasterGO || FALSE == CheckBattle() || NULL == m_pcTarGU){
		return eEC_EffectEnd;
	}

	if(FALSE == CheckHitTargetCamp(m_pcTarGU) || FALSE == CheckHitTargetType(m_pcTarGU)){
		return eEC_EffectEnd;
	}

	//根据是否普通攻击，调用对应的被动技能回调接口
	bool bIfNormalAttack = FALSE;
	SSkillModelAccountCfg* pCurCfg = (SSkillModelAccountCfg*)m_pCfg;
	if (NULL != m_pSkillRuntimeData && NULL != m_pSkillRuntimeData->cpsCfg){
		bIfNormalAttack = m_pSkillRuntimeData->cpsCfg->bIfNomalAttack;
	}
	if(TRUE == bIfNormalAttack){
		m_pcMasterGO->OnPassitiveSkillCalled(EPassiveSkillTriggerType_NormalAttackCaculate_Before, m_pcTarGU);
		m_pcTarGU->OnPassitiveSkillCalled(EPassiveSkillTriggerType_Attacked, m_pcMasterGO);
	}
	//进行结算
	INT32 n32Res = CaculateSkillEffectOnce(m_pcMasterGO, m_pcTarGU, pCurCfg->sEffectInfo, m_pSkillRuntimeData == NULL? 0:m_pSkillRuntimeData->cpsCfg->un32SkillID, bIfNormalAttack);

	//如果是野怪受到伤害，需要调用野怪的对应接口，将攻击目标加上，用于野怪的AI
	if (eNormal == n32Res){
		if (m_pcTarGU && m_pcTarGU->IfWildNPC()){
			m_pcTarGU->GetCurBattle()->SetWildNPCGroupCurEnemy(m_pcTarGU, m_pcMasterGO);
		}
	} 

	NotifySkillModelHitObjectToGC();
	//回调被普通攻击命中的被动技能接口
	if(TRUE == bIfNormalAttack){
		m_pcMasterGO->OnPassitiveSkillCalled(EPassiveSkillTriggerType_NormalAttackCaculate_After,m_pcTarGU);
	}

	return eEC_EffectEnd;
}

INT32 CSSSkillEffect_Caculate::End(){
	return eNormal;
}


INT32 CSSSkillEffect_Caculate::CaculateSkillEffectOnce(CSSGameUnit *pcMasterGO, CSSGameUnit *pcTarGU, const SEffectInfo &rsEffectInfo, UINT32 skillID, bool bIfNormalAtk, bool ifAdd){
	if (NULL == pcTarGU){
		return eEC_NullPointer;
	}
	//不允许攻击的对象
	if (  !pcMasterGO->GetCurBattle()->CheckObjCanBeHurt(pcTarGU,pcMasterGO ) && (rsEffectInfo.eEffectCate <= eEffectCate_CurMP) )
	{
		return eEc_InvalidTarget;
	}

	//开始计算效果值
	float value = rsEffectInfo.n32EffectBaseValue;//基础值
	//先计算加法加成
	for(int i=0;i<c_n32MaxEffectCaculateCateNum;i++){
		if(rsEffectInfo.eEffectAddCacuCate[i] == EEffectCaculateCate_None){
			continue;
		}
		switch(rsEffectInfo.eEffectAddCacuCate[i]){
		case EEffectCaculateCate_SelfPhyAttack ://1.自己物攻加成
			value += (float)pcMasterGO->GetFPData(eEffectCate_PhyAttack) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfMagicAttack : //2.自己魔攻加成
			value += (float)pcMasterGO->GetFPData(eEffectCate_MagicAttack) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfPhyDefence : //3.自己物防加成
			value += (float)pcMasterGO->GetFPData(eEffectCate_PhyDefense) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfMagicDefence : //4.自己魔防加成
			value += (float)pcMasterGO->GetFPData(eEffectCate_MagicDefense) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfNowHP : //5.自己当前生命值加成
			value += (float)pcMasterGO->GetFPData(eEffectCate_CurHP) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfMaxHP : //6.自己最大生命值加成
			value += (float)pcMasterGO->GetFPData(eEffectCate_MaxHP) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfLostHpPercent : //7.自己损失生命比加成(每1%加成)
			{
				float nowHP = pcMasterGO->GetFPData(eEffectCate_CurHP);
				float maxHP = pcMasterGO->GetFPData(eEffectCate_MaxHP);
				float lostPercent = (1 - (nowHP / maxHP)) * 100.0f;
				value += lostPercent * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			}
			break;
		case EEffectCaculateCate_SelfNowMP : //8.自己当前魔法值加成
			value += (float)pcMasterGO->GetFPData(eEffectCate_CurMP) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfMaxMP : //9.自己最大魔法值加成
			value += (float)pcMasterGO->GetFPData(eEffectCate_MaxMP) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfLostMpPercent : //10.自己损失魔法比加成(每1%加成)
			{
				float nowMP = pcMasterGO->GetFPData(eEffectCate_CurHP);
				float maxMP = pcMasterGO->GetFPData(eEffectCate_MaxHP);
				float lostPercent = (1 - (nowMP / maxMP)) * 100.0f;
				value += lostPercent * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			}
			break;
		case EEffectCaculateCate_SelfLevel : //11.自己等级加成
			{
				CSSHero* pHero = pcMasterGO->TryCastToHero();
				if(NULL != pHero){
					value += (float)pHero->GetLevel() * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
				}
			}
			break;
		case EEffectCaculateCate_SelfMoveSpeed : //12.自己移动速度加成(每1厘米加成)
			value += (float)pcMasterGO->GetFPData(eEffectCate_MoveSpeed) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;

		case EEffectCaculateCate_TarPhyAttack ://21.目标物攻加成
			value += (float)pcTarGU->GetFPData(eEffectCate_PhyAttack) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarMagicAttack : //22.目标魔攻加成
			value += (float)pcTarGU->GetFPData(eEffectCate_MagicAttack) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarPhyDefence : //23.目标物防加成
			value += (float)pcTarGU->GetFPData(eEffectCate_PhyDefense) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarMagicDefence : //24.目标魔防加成
			value += (float)pcTarGU->GetFPData(eEffectCate_MagicDefense) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarNowHP : //25.目标当前生命值加成
			value += (float)pcTarGU->GetFPData(eEffectCate_CurHP) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarMaxHP : //26.目标最大生命值加成
			value += (float)pcTarGU->GetFPData(eEffectCate_MaxHP) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarLostHpPercent : //27.目标损失生命比加成(每1%加成)
			{
				float nowHP = pcTarGU->GetFPData(eEffectCate_CurHP);
				float maxHP = pcTarGU->GetFPData(eEffectCate_MaxHP);
				float lostPercent = (1 - (nowHP / maxHP)) * 100.0f;
				value += lostPercent * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			}
			break;
		case EEffectCaculateCate_TarNowMP : //28.目标当前魔法值加成
			value += (float)pcTarGU->GetFPData(eEffectCate_CurMP) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarMaxMP : //29.目标最大魔法值加成
			value += (float)pcTarGU->GetFPData(eEffectCate_MaxMP) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarLostMpPercent : //30.目标损失魔法比加成(每1%加成)
			{
				float nowMP = pcTarGU->GetFPData(eEffectCate_CurHP);
				float maxMP = pcTarGU->GetFPData(eEffectCate_MaxHP);
				float lostPercent = (1 - (nowMP / maxMP)) * 100.0f;
				value += lostPercent * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			}
			break;
		case EEffectCaculateCate_TarLevel : //31.目标等级加成
			{
				CSSHero* pHero = pcTarGU->TryCastToHero();
				if(NULL != pHero){
					value += (float)pHero->GetLevel() * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
				}
			}
			break;
		case EEffectCaculateCate_TarMoveSpeed : //32.目标移动速度加成(每1厘米加成)
			value += (float)pcTarGU->GetFPData(eEffectCate_MoveSpeed) * (float)rsEffectInfo.eEffectAddCacuValue[i] / 1000.0f;
			break;
		default:
			break;
		}
	}
	//再计算乘法加成
	float multAdd = 0;
	for(int i=0;i<c_n32MaxEffectCaculateCateNum;i++){
		if(rsEffectInfo.eEffectMultCacuCate[i] == EEffectCaculateCate_None){
			continue;
		}
		switch(rsEffectInfo.eEffectMultCacuCate[i]){
		case EEffectCaculateCate_SelfPhyAttack ://1.自己物攻加成
			multAdd += (float)pcMasterGO->GetFPData(eEffectCate_PhyAttack) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfMagicAttack : //2.自己魔攻加成
			multAdd += (float)pcMasterGO->GetFPData(eEffectCate_MagicAttack) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfPhyDefence : //3.自己物防加成
			multAdd += (float)pcMasterGO->GetFPData(eEffectCate_PhyDefense) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfMagicDefence : //4.自己魔防加成
			multAdd += (float)pcMasterGO->GetFPData(eEffectCate_MagicDefense) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfNowHP : //5.自己当前生命值加成
			multAdd += (float)pcMasterGO->GetFPData(eEffectCate_CurHP) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfMaxHP : //6.自己最大生命值加成
			multAdd += (float)pcMasterGO->GetFPData(eEffectCate_MaxHP) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfLostHpPercent : //7.自己损失生命比加成(每1%加成)
			{
				float nowHP = pcMasterGO->GetFPData(eEffectCate_CurHP);
				float maxHP = pcMasterGO->GetFPData(eEffectCate_MaxHP);
				float lostPercent = (1 - (nowHP / maxHP)) * 100.0f;
				multAdd += lostPercent * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			}
			break;
		case EEffectCaculateCate_SelfNowMP : //8.自己当前魔法值加成
			multAdd += (float)pcMasterGO->GetFPData(eEffectCate_CurMP) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfMaxMP : //9.自己最大魔法值加成
			multAdd += (float)pcMasterGO->GetFPData(eEffectCate_MaxMP) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_SelfLostMpPercent : //10.自己损失魔法比加成(每1%加成)
			{
				float nowMP = pcMasterGO->GetFPData(eEffectCate_CurHP);
				float maxMP = pcMasterGO->GetFPData(eEffectCate_MaxHP);
				float lostPercent = (1 - (nowMP / maxMP)) * 100.0f;
				multAdd += lostPercent * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			}
			break;
		case EEffectCaculateCate_SelfLevel : //11.自己等级加成
			{
				CSSHero* pHero = pcMasterGO->TryCastToHero();
				if(NULL != pHero){
					multAdd += (float)pHero->GetLevel() * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
				}
			}
			break;
		case EEffectCaculateCate_SelfMoveSpeed : //12.自己移动速度加成(每1厘米加成)
			multAdd += (float)pcMasterGO->GetFPData(eEffectCate_MoveSpeed) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;

		case EEffectCaculateCate_TarPhyAttack ://21.目标物攻加成
			multAdd += (float)pcTarGU->GetFPData(eEffectCate_PhyAttack) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarMagicAttack : //22.目标魔攻加成
			multAdd += (float)pcTarGU->GetFPData(eEffectCate_MagicAttack) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarPhyDefence : //23.目标物防加成
			multAdd += (float)pcTarGU->GetFPData(eEffectCate_PhyDefense) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarMagicDefence : //24.目标魔防加成
			multAdd += (float)pcTarGU->GetFPData(eEffectCate_MagicDefense) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarNowHP : //25.目标当前生命值加成
			multAdd += (float)pcTarGU->GetFPData(eEffectCate_CurHP) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarMaxHP : //26.目标最大生命值加成
			multAdd += (float)pcTarGU->GetFPData(eEffectCate_MaxHP) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarLostHpPercent : //27.目标损失生命比加成(每1%加成)
			{
				float nowHP = pcTarGU->GetFPData(eEffectCate_CurHP);
				float maxHP = pcTarGU->GetFPData(eEffectCate_MaxHP);
				float lostPercent = (1 - (nowHP / maxHP)) * 100.0f;
				multAdd += lostPercent * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			}
			break;
		case EEffectCaculateCate_TarNowMP : //28.目标当前魔法值加成
			multAdd += (float)pcTarGU->GetFPData(eEffectCate_CurMP) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarMaxMP : //29.目标最大魔法值加成
			multAdd += (float)pcTarGU->GetFPData(eEffectCate_MaxMP) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		case EEffectCaculateCate_TarLostMpPercent : //30.目标损失魔法比加成(每1%加成)
			{
				float nowMP = pcTarGU->GetFPData(eEffectCate_CurHP);
				float maxMP = pcTarGU->GetFPData(eEffectCate_MaxHP);
				float lostPercent = (1 - (nowMP / maxMP)) * 100.0f;
				multAdd += lostPercent * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			}
			break;
		case EEffectCaculateCate_TarLevel : //31.目标等级加成
			{
				CSSHero* pHero = pcTarGU->TryCastToHero();
				if(NULL != pHero){
					multAdd += (float)pHero->GetLevel() * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
				}
			}
			break;
		case EEffectCaculateCate_TarMoveSpeed : //32.目标移动速度加成(每1厘米加成)
			multAdd += (float)pcTarGU->GetFPData(eEffectCate_MoveSpeed) * (float)rsEffectInfo.eEffectMultCacuValue[i] / 1000.0f;
			break;
		default:
			break;
		}
	}
	value = value * (1 + multAdd);
	if(value <= 0){
		value = 0;
	}
	else{
		value++;
	}

	//计算暴击
	INT32 n32BlastHitRate = rsEffectInfo.n32BlastAttPercent;
	INT32 n32BlastHitVal = rsEffectInfo.n32BlastAttRate + 2000;
	//如果是普通伤害，暴击率需要加上基础值
	if(bIfNormalAtk){
		n32BlastHitRate += pcMasterGO->GetFPData(eEffectCate_CriPersent);
		n32BlastHitVal += pcMasterGO->GetFPData(eEffectCate_CriHarm);
	}
	bool bIfBlast = FALSE;
	//根据概率计算是否暴击
	if(FALSE == pcTarGU->IfNPC_Building() &&  n32BlastHitRate > 0){
		INT32 n32RandInt = CFunction::GetGameRandomN(pcMasterGO->GetCurBattle()->GetRandomEngine(), 0,1000 );
		if (n32RandInt <= n32BlastHitRate){
			bIfBlast = TRUE;
		}
	}
	//如果暴击了，计算暴击伤害
	if(bIfBlast){
		value *= ( n32BlastHitVal / 1000.0f );
	}

	switch (rsEffectInfo.eEffectCate){
	case eEffectCate_PhyHurt:
	case eEffectCate_MagicHurt:
	case eEffectCate_TrueHurt:
		{
			//记录伤害日志 
			pcTarGU->CelObjHurtLog( pcMasterGO, skillID, value, rsEffectInfo.eEffectCate);
			  
			//开始进行伤害结算
			INT32 hurtValue = 0;
			INT32 rst =  pcTarGU->ApplyHurt(pcMasterGO, value, rsEffectInfo.eEffectCate, hurtValue, bIfNormalAtk , bIfBlast);
			//计算吸血
			float fSuckBloodPercent = 0;
			if(eEffectCate_PhyHurt == rsEffectInfo.eEffectCate) fSuckBloodPercent = pcMasterGO->GetFPData(eEffectCate_PhySuckBlood);
			if(eEffectCate_MagicHurt == rsEffectInfo.eEffectCate) fSuckBloodPercent = pcMasterGO->GetFPData(eEffectCate_MagicSuckBlood);
			if(fSuckBloodPercent > 0 && FALSE == pcMasterGO->IsDead()){
				pcMasterGO->ChangeCurHP(pcMasterGO, GSToGC::HPMPChangeReason::SkillConsume, (float)hurtValue * fSuckBloodPercent / 1000.0f);
			}
			return rst;
		}
	case eEffectCate_CurHP:
		return pcTarGU->ChangeCurHP(pcMasterGO, GSToGC::SkillConsume, value, skillID, eEffectCate_CurHP); 
	case eEffectCate_CurMP:
		return pcTarGU->ChangeCurMP(pcMasterGO, GSToGC::SkillConsume, value);
	}

	if (NULL == pcTarGU){
		return eEC_NullPointer;
	}
	return pcTarGU->ChangeFPData(rsEffectInfo.eEffectCate, rsEffectInfo.n32EffectBaseValue, rsEffectInfo.n32EffectRate, ifAdd);
}

}