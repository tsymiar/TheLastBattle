
#include "SSSkillEffect_Range.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSBattle.h"
#include "SSEffectMgr.h"
 



namespace SceneServer{

CSSSkillEffect_Range::CSSSkillEffect_Range(): m_tLauchUTCMilsec(0)
	,m_tNextUTCMilsec(0)		    
	,m_RangeTimes(0)
	,m_Camp(eGOCamp_AllPeace)
	,m_CurPos(0,0,0)
	,m_RangEffectCfg(NULL)
{
	m_SkillEffectType = eSkillEffectType_Range;
}
 
CSSSkillEffect_Range::~CSSSkillEffect_Range(){
	     
	m_RangeTimes = 0;  
	m_SkillEffectType = eSkillEffectType_None;
}

INT32 CSSSkillEffect_Range::Update(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan){
	if (tUTCMilsec < m_tNextUTCMilsec){
		return eNormal;
	}

	if (NULL == m_pcMasterGO){ 
		return eEC_NullPointer;
	}

	if (m_RangEffectCfg->n32ReleaseTimeDelay > 0 && tUTCMilsec - m_tLauchUTCMilsec < m_RangEffectCfg->n32ReleaseTimeDelay){ 
		return eNormal;
	}

	if (m_tLauchUTCMilsec > 0 && tUTCMilsec -  m_tLauchUTCMilsec > m_RangEffectCfg->n32LifeTime){ 
		return eEC_TimeOut;
	}

	if(m_RangeTimes >= m_RangEffectCfg->n32RangeTimes && tUTCMilsec -  m_tLauchUTCMilsec > m_RangEffectCfg->n32LifeTime){ 
		return eEC_EffectEnd;
	} 

	if (m_RangeTimes < m_RangEffectCfg->n32RangeTimes){
		GetHitObjList();

		++m_RangeTimes;

		m_tNextUTCMilsec = tUTCMilsec + m_RangEffectCfg->n32RangeInterval;  
	}

	return eNormal;
} 

INT32	CSSSkillEffect_Range::Begin(){
	m_RangEffectCfg = (SSkillModelRangeCfg*)m_pCfg;
	if (NULL == m_RangEffectCfg){
		return eEC_EffectEnd;
	}

	m_tNextUTCMilsec = 0;	    
	m_RangeTimes = 0; 
	m_tLauchUTCMilsec = GetUTCMiliSecond(); 
	m_CurPos.Zero();

	if(m_RangEffectCfg->eSkillAOECate == eSAOECate_SelfCenter){ 
		m_cPos = m_pcMasterGO->GetCurPos();
		m_CurPos = m_cPos;
	}
	else if(m_RangEffectCfg->eSkillAOECate == eSAOECate_FixDist_Ex){
		m_CurPos = m_pcMasterGO->GetCurPos() +  m_pcMasterGO->GetCurDir() * m_RangEffectCfg->n32ReleaseDist; 
	}  
	else{
		if (NULL != m_pcTarGU){
			m_CurPos = m_pcTarGU->GetCurPos();
		} 
	}

	m_Camp = m_pcMasterGO->GetCampID();
	if(m_RangEffectCfg->bIfHasEffect){ 
		GetBattle()->AddSightLight(this, this, m_Camp);
		GetBattle()->AddSightObject(this);
	}

	Update(GetUTCMiliSecond(), 0);

	return eNormal;
}

INT32 CSSSkillEffect_Range::End(){ 
	if(m_RangEffectCfg->bIfHasEffect) {
		GetBattle()->RemoveSightObject(this);
		GetBattle()->RemoveSightLight(this, this, m_Camp);
	}

	m_tLauchUTCMilsec = 0;
	m_tNextUTCMilsec = 0;	    
	m_RangeTimes = 0; 
	m_IsForceStop = FALSE;
	m_CurPos.Zero();

	return eNormal;
}

INT32  CSSSkillEffect_Range::GetHitObjList(){ 
	CSSGameUnit* pArray[128];
	UINT16 length = 0;

	GetRangeModuleTargetVec(pArray, length);
	if (length == 0){
		return eEc_NoAtkObj;
	} 
	 
	return  DoNextModule(pArray, length); 
}


INT32 CSSSkillEffect_Range::GetRangeModuleTargetVec(CSSGameUnit** pGUArray, UINT16 &length){
	length = 0;
	for(GameObjectMap::iterator iter = GetBattle()->GetBattleGameObjectList().begin(); iter != GetBattle()->GetBattleGameObjectList().end(); ++iter){
		CSSGameUnit *pcTempGO = iter->second;   

		if(!CheckHitTargetType(pcTempGO)){
			continue;
		}
		if (!CheckHitTargetCamp(pcTempGO)) {
			continue;
		} 

		CVector3D& checkPos = pcTempGO->GetCurPos();
 
		bool bResult = false;

		switch(m_RangEffectCfg->eSkillShapeType){ 
		case eSkillShapeType_Cicle:
			bResult = GetBattle()->IsInCircle(checkPos, m_CurPos, m_RangEffectCfg->n32RangePar1);
			break;
		case eSkillShapeType_Rectangle:
			bResult = GetBattle()->IsInRect(checkPos, m_CurPos, m_RangEffectCfg->n32RangePar1, m_RangEffectCfg->n32RangePar2, m_cDir);  
			break;
		case eSkillShapeType_Sector:
			{
				if (checkPos.Equal(m_CurPos)){
					bResult = true;
				}
				else{ 
					float radius = (FLOAT)m_RangEffectCfg->n32RangePar1;
					float radian = (FLOAT)m_RangEffectCfg->n32RangePar2;
					bResult = CSSBattle::IsPointInCircularSector(checkPos, m_CurPos, radian  / 2, radius, m_cDir);
				}
			}
			break;
		}

		if(bResult){ 
			Assert(length<128 && "the buffer is so small, try and wacth how to crash...");
			pGUArray[length++] = pcTempGO;
		}
	}
	return eNormal;
}


INT32    CSSSkillEffect_Range::DoNextModule(CSSGameUnit** pGUArray, UINT16 length) {
	if (NULL == m_pcMasterGO){
		return eEC_NullPointer;
	}

	INT32  tEffectObjNum = m_RangEffectCfg->n32MaxEffectObj;
	if (1 == tEffectObjNum){  
		CSSGameUnit *pUnit = NULL;
		INT32 n32RandInt =  CFunction::GetGameRandomN(m_pBattle->GetRandomEngine(),0,length-1);
		pUnit = pGUArray[n32RandInt];  
		CallNextModule(pUnit);
		return eNormal;
	}

	if (tEffectObjNum < 1){
		tEffectObjNum = 9999;
	} 

	for (int i =0; i< length; ++i){ 
		CSSGameUnit *pUnit = pGUArray[i]; 
		if (NULL == pUnit){
			continue;
		}
		
		CallNextModule(pUnit);

		if (i > tEffectObjNum){
			break;
		} 
	}
	return eNormal;
}

CVector3D CSSSkillEffect_Range::GetCurPos(){
	return m_cPos;
}

INT32	CSSSkillEffect_Range::CallNextModule(CSSGameUnit *pUnit){
	if (NULL == pUnit){
		return eEC_NullPointer;
	}

	CVector3D tarDir = pUnit->GetCurPos() - m_cPos;
	tarDir.m_fY = 0;
	tarDir.unit(); 

	GetBattle()->GetEffectMgr()->AddEffectsFromCfg(m_RangEffectCfg->asSkillModelList, m_pcMasterGO, pUnit, pUnit->GetCurPos()
		, tarDir, m_pSkillRuntimeData, GetUTCMiliSecond()); 
	
	OnSkillHitTarget(pUnit);

	return eNormal;
}

void	CSSSkillEffect_Range::OnAppearInSight(std::vector<CSSUser*>& cTempUserMap){
	if (FALSE == CheckBattle()){
		return;
	} 
	
	if ( NULL == m_pcMasterGO){
		return;
	} 

	GSToGC::RangeEffect sMsg;
	sMsg.set_guid(m_pcMasterGO->GetObjGUID());
	sMsg.set_effectid(m_pCfg->un32SkillModelID);
	sMsg.set_uniqueid(m_un32UniqueID);
	GSToGC::Dir* dir = new GSToGC::Dir;
	CSSBattle::SetDir(m_cDir, *dir);
	sMsg.set_allocated_dir(dir);
	GSToGC::Pos* pos = new GSToGC::Pos;
	CSSBattle::SetPos(m_CurPos, *pos);
	sMsg.set_allocated_pos(pos);
	GetBattle()->SendMsgToWatchers(sMsg, sMsg.msgid(), cTempUserMap);
}

void	CSSSkillEffect_Range::OnDisappearInSight(std::vector<CSSUser*>& cTempUserMap){ 
	if(m_pcMasterGO == NULL || FALSE == CheckBattle()){
		return;
	} 

	GSToGC::RangeEffectEnd sMsg;
	sMsg.set_uniqueid(m_un32UniqueID);
	GetBattle()->SendMsgToWatchers(sMsg, sMsg.msgid(), cTempUserMap);
}

float	CSSSkillEffect_Range::GetSightX(){
	return m_CurPos.m_fX;
}

float	CSSSkillEffect_Range::GetSightY(){
	return m_CurPos.m_fZ;
}

bool	CSSSkillEffect_Range::IfParaInvalid(){
	return TRUE == IsForceStop();
}

float CSSSkillEffect_Range::GetSightPosX(){return m_CurPos.m_fX;}
float CSSSkillEffect_Range::GetSightPosY(){return m_CurPos.m_fZ;}

}