#include "SSSkillEffect_Summon.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSEffectMgr.h"
#include "SSNPC.h"
 
namespace SceneServer{

CSSSkillEffect_Summon::CSSSkillEffect_Summon(): m_pSSkillModuleSummonCfg(NULL)
{
	m_SkillEffectType = eSkillEffectType_Summon;
}

CSSSkillEffect_Summon::~CSSSkillEffect_Summon(){
	Clear();
	m_SkillEffectType = eSkillEffectType_None; 
}

INT32 CSSSkillEffect_Summon::Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){  
	if ( NULL == m_pcMasterGO){
		return eEC_NullPointer;
	}
	return eNormal;
} 

INT32	CSSSkillEffect_Summon::Begin(){
	if (NULL == m_pcMasterGO){
		return eEC_NullPointer;
	}

	m_pSSkillModuleSummonCfg = (SSkillModuleSummonCfg*)m_pCfg;
	if (NULL == m_pSSkillModuleSummonCfg){
		return eEC_NULLCfg;
	}   

	CVector3D tMasterPos = m_pcMasterGO->GetCurPos();
	CVector3D tarPos; 
	if(m_pSSkillModuleSummonCfg->eSummonWayType == eSummonType_DistDir){
		tarPos = tMasterPos +  (float)m_pSSkillModuleSummonCfg->n32Distance * m_cDir;
	}
	else if(m_pSSkillModuleSummonCfg->eSummonWayType == eSummonType_Tarpos){
		tarPos = m_cPos;
	}else{
		tarPos = tMasterPos;
	} 
	
	for (int i = 0; i < n_c32NPCNum; ++i){ 
		INT32 num = m_pSSkillModuleSummonCfg->n32SummonNum[i];
		INT32 npcTypeID = m_pSSkillModuleSummonCfg->nNPCId[i];
		if(num <= 0 || npcTypeID <= 0) break;
		for(int j=0;j<num;++j){
			CSSNPC* pNPC = (CSSNPC*)GetBattle()->CreateNPCAndAdd(m_pcMasterGO, npcTypeID , tarPos, GetUTCMiliSecond() + m_pSSkillModuleSummonCfg->n32LifeTime);
			pNPC->SetFatherHeroGUID(m_pcMasterGO->GetObjGUID()); 
			pNPC->SetExpire(false);//CreateNPCAndAdd的时候，因为EndLife尚未读取配置为0，所以Expire为ture，需要重置为false否则会立刻消失。
		 
			//pNPC->SetObjEndLife(GetUTCMiliSecond() + m_pSSkillModuleSummonCfg->n32LifeTime);
			SendSummonMsg(pNPC);
		}
		
	}
	
	return eNormal;
}

INT32	CSSSkillEffect_Summon::End(){
	return eNormal;
}

INT32	CSSSkillEffect_Summon::SendSummonMsg(CSSNPC* pBornNPC){ 
	if (NULL == m_pcMasterGO || NULL == pBornNPC){
		return eEC_NullPointer;
	}

	GSToGC::SummonEffect sSummonEffect;
	sSummonEffect.set_guid(m_pcMasterGO->GetObjGUID());
	sSummonEffect.set_effectid(m_pSSkillModuleSummonCfg->un32SkillModelID);
	sSummonEffect.set_npcguid(pBornNPC->GetObjGUID());
	GetBattle()->SendMsgToAllWatcher(sSummonEffect, sSummonEffect.msgid(), m_pcMasterGO);

	//pBornNPC->SetObjEndLife(GetUTCMiliSecond() + m_pSSkillModuleSummonCfg->n32LifeTime);
	pBornNPC->SetObjTotalLife(m_pSSkillModuleSummonCfg->n32LifeTime);
	pBornNPC->SendSummonLifeMsg(eSummonFunc_All, m_pcMasterGO);

	return eNormal;
}

}