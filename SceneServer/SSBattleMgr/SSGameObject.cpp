#include "stdafx.h"
#include "SSGameObject.h"
#include "SSGameUnit.h"
#include "SSBattle.h"
#include "SSNPC.h"

#include "SSUser.h"
#include "SSBattleMgr.h"

namespace SceneServer{

CSSGameObject::CSSGameObject(UINT64 guid, EGameObjectCamp sCamp, UINT32 objectID, UINT64 sMasterGUID, CSSBattle* pBattle, CSSBattleMgr* pMgr):m_pCSSBattleMgr(pMgr)
	,m_tCurAtkMilsec(0)
	, m_n32CollideRadius(0.0f)
	, m_eGOCamp(sCamp)
	, m_n32Seat(0)
, m_bExpire(FALSE)
, m_tEnterBattleUTCMilsec(0)
, m_tLastSyncMoveStateMilsec(0)
, m_objType(objectID)
, m_pNormalAttackSkill(NULL)
, m_pAI(NULL)
, m_pcBattle(pBattle)
, m_sMasterGUID(sMasterGUID)
, m_sObjGUID(guid)
, m_FatherHeroGUID(0){
}

CSSGameObject::~CSSGameObject(void){
	if(m_pAI != NULL) delete m_pAI;
	m_pAI = NULL;
}

UINT64	CSSGameObject::GetCurBattleID(){
	if (NULL != m_pcBattle){
		return m_pcBattle->GetBattleID();
	}
	return 0;
}

bool CSSGameObject::IsSameCamp( CSSGameObject *pcGo )
{ 
	INT32 camp1 = (INT32)this->GetCampID();
	camp1 = camp1 % 2;
	INT32 camp2 = (INT32)pcGo->GetCampID();
	camp2 = camp2 % 2;
	if(camp2 != camp1)
		return FALSE;
	return TRUE; 
}

TIME_TICK	CSSGameObject::GetFreeActionTick(){
	if (eOAS_Free != m_sCurOASI.eOAS){
		return 0;
	}
	TIME_MILSEC tCurUTCMilsec = GetUTCMiliSecond();
	TIME_TICK tTickSpan = 0;
	if (tCurUTCMilsec >= m_sCurOASI.tASUTCMilsec){
		tTickSpan = tCurUTCMilsec - m_sCurOASI.tASUTCMilsec;
	}
	return tTickSpan;
}

INT32	CSSGameObject::SetGOActionState(EGOActionState eState){
	m_sCurOASI.eOAS = eState;
	m_sCurOASI.tASUTCMilsec = GetUTCMiliSecond();
	return eNormal;
}

INT32	CSSGameObject::SetGOActionState_ASUTCMilsec(TIME_MILSEC tMilsec){
	m_sCurOASI.tASUTCMilsec = tMilsec;
	return eNormal;
}

INT32	CSSGameObject::SetGOActionState_Pos(const CVector3D &crcPos){
	Assert(!IsNan(crcPos.m_fX));
	Assert(!IsNan(crcPos.m_fY));
	Assert(!IsNan(crcPos.m_fZ));
	Assert(100000 > crcPos.m_fX);
	Assert(100000 > crcPos.m_fZ);
	m_sCurOASI.cPos = crcPos;
	return eNormal;
}

INT32	CSSGameObject::SetGOActionState_Dir(const CVector3D &crcDir){
	Assert(!IsNan(crcDir.m_fX));
	Assert(!IsNan(crcDir.m_fY));
	Assert(!IsNan(crcDir.m_fZ));
	Assert(1 >= crcDir.m_fX);
	Assert(1 >= crcDir.m_fZ);
	m_sCurOASI.cDir = crcDir;
	return eNormal;
}

INT32	CSSGameObject::SetGOActionState_SkillID(UINT32 un32SkillID){
	m_sCurOASI.un32SkillID = un32SkillID;
	return eNormal;
}

INT32	CSSGameObject::SetGOActionState_SkillTarGUID(const SGUID_KEY &crsTarGUID){
	m_sCurOASI.sSkillTarGUID = crsTarGUID;
	return eNormal;
}

bool CSSGameObject::IfEnemy(CSSGameObject *pcGo){
	if (NULL == pcGo){
		return FALSE;
	}

	return CFunction::IfEnemy(GetCampID(), pcGo->GetCampID());
}

bool CSSGameObject::IfCanBeTarget()
{
	return eOAS_Reliving != m_sCurOASI.eOAS && m_sCurOASI.eOAS != eOAS_Dead;
}

void CSSGameObject::SetCurAtkGUID(const SGUID_KEY &crsGUID){
	m_CurAtkGUID = crsGUID;
	m_tCurAtkMilsec = GetUTCMiliSecond();
}

const INT32 MaxAtkGuidTime = 3000;
const SGUID_KEY & CSSGameObject::GetCurAtkGUID(){
	TIME_MILSEC curTime = GetUTCMiliSecond();
	if (m_tCurAtkMilsec > 0 && curTime - m_tCurAtkMilsec > MaxAtkGuidTime){
		m_CurAtkGUID = 0;
	}

	return m_CurAtkGUID;
}

bool CSSGameObject::SetExpire(bool bExpire){
	m_bExpire = bExpire;
	return true;
}

}