#include "stdafx.h"
#include "SSArea.h"
#include "SSGameUnit.h"
#include "SSHero.h"
#include "SSNPC.h"
#include "SSGameObject.h"
#include "SSBattle.h"
#include "SSAltarSev.h"
namespace SceneServer{

CSSArea::CSSArea(void){
	m_un32AreaID = 0;
	m_n32XAreaIdx = -1;
	m_n32ZAreaIdx = -1;
}

CSSArea::~CSSArea(void){
	Clear();
}

INT32	CSSArea::Clear(){
	m_un32AreaID = 0;
	m_n32XAreaIdx = -1;
	m_n32ZAreaIdx = -1;
	m_cObjMap.clear();
	return eNormal;
}

INT32	CSSArea::AddObj(CSSGameUnit *pcGO){

	if (NULL == pcGO){
		return eEC_NullPointer;
	}
	bool bAltar = false;
	UINT64 un64ObjIdx = pcGO->GetObjGUID().GetGUIDIndex();
	map<UINT64, CSSGameUnit*>::iterator iterObj = m_cObjMap.find(un64ObjIdx);
	if (m_cObjMap.end() != iterObj){
		return eEC_GameObjectAlreadyExist;
	}
	m_cObjMap[un64ObjIdx] = pcGO;  
	
	return eNormal;
}

INT32	CSSArea::RemoveObj(const SGUID_KEY &crsGUID){
	UINT64 un64ObjIdx = crsGUID.GetGUIDIndex();
	map<UINT64, CSSGameUnit*>::iterator iterObj = m_cObjMap.find(un64ObjIdx);
	if (m_cObjMap.end() == iterObj){
		return eEC_CannotFindoutTheObject;
	}
	m_cObjMap.erase(iterObj);
	return eNormal;
}

INT32	CSSArea::GetGOList(vector<CSSGameUnit*> &rcGOVec){ 

	INT32 n32GotNum = 0;

	map<UINT64, CSSGameUnit*>::iterator iterObj = m_cObjMap.begin();
	while (m_cObjMap.end() != iterObj){
		CSSGameUnit *pUnit = iterObj->second;
		if (NULL != pUnit)
		{ 
			rcGOVec.push_back(iterObj->second);
			n32GotNum++; 
		} 
		iterObj++;
	}
	return n32GotNum;
}

INT32	CSSArea::GetEnemyGUList(INT32 n32SelfCampID, vector<CSSGameUnit*> &rcGUVec){
	INT32 n32GotNum = 0;

	map<UINT64, CSSGameUnit*>::iterator iterObj = m_cObjMap.begin();
	while (m_cObjMap.end() != iterObj){
		CSSGameUnit *pcTempGO = iterObj->second;
		if (NULL == pcTempGO){
			iterObj++;
			continue;
		}
		if (!CFunction::IfEnemy(n32SelfCampID, pcTempGO->GetCampID())){
			iterObj++;
			continue;
		}
		if (eOAS_Dead == pcTempGO->GetGOActionStateInfo().eOAS){
			iterObj++;
			continue;
		}
		rcGUVec.push_back(pcTempGO);
		iterObj++;
	}
	return n32GotNum;
}

INT32	CSSArea::GetUserSightMap(map<CSSUser*, INT32> &rcUserSightMap){
	INT32 n32GotNum = 0;
	map<UINT64, CSSGameUnit*>::iterator iterObj = m_cObjMap.begin();
	while (m_cObjMap.end() != iterObj){
		CSSGameUnit *pcTempGO = iterObj->second;
		if (NULL != pcTempGO && pcTempGO->IfHero()){
			CSSHero* pHero = (CSSHero*)pcTempGO;
			if (pHero->GetUser()){
				rcUserSightMap[pHero->GetUser()] = 0;
				n32GotNum++;
			}
		}
		iterObj++;
	}
	return n32GotNum;
}

bool	CSSArea::IsObjExitAtPos(const CVector3D & cPos)
{
	CVector3D tPos = cPos;

	map<UINT64, CSSGameUnit*>::iterator iterObj = m_cObjMap.begin();
	while (m_cObjMap.end() != iterObj){
		CSSGameUnit *pObj = iterObj->second;
		if ((NULL != pObj) && (abs(pObj->GetCurPos().m_fX - tPos.m_fX) < 0.001) && (abs(pObj->GetCurPos().m_fZ - tPos.m_fZ) < 0.001))
		{ 
			return true;
		} 
		iterObj++;
	}
	return false;
}
 

INT32	CSSArea::GetGameObject(const CVector3D &crsCenterPos, INT32 n32Sight, CSSGameUnit **ppcGOBuff, INT32 n32BuffSize, bool bIgnoreHero, bool bIgnoreNPC, bool bIgnoreGoods){
	if (NULL == ppcGOBuff || 0>= n32BuffSize){
		return 0;
	}
	INT32 n32GotNum = 0;
	map<UINT64, CSSGameUnit*>::iterator iterGO = m_cObjMap.begin();
	while (m_cObjMap.end() != iterGO){
		CSSGameUnit *pcTempGO = iterGO->second;
		if (NULL == pcTempGO){
			iterGO++;
			continue;
		}
		if (bIgnoreHero &&pcTempGO->IfHero()){
			iterGO++;
			continue;
		}
		if (bIgnoreNPC && pcTempGO->IfNPC()){
			iterGO++;
			continue;
		}
		if (!crsCenterPos.CanWatch(n32Sight, &pcTempGO->GetGOActionStateInfo().cPos)){
			continue;
		}
		ppcGOBuff[n32GotNum] = pcTempGO;
		n32GotNum++;
		if (n32GotNum >= n32BuffSize){
			break;
		}
		iterGO++;
	}
	return n32GotNum;	
}

INT32	CSSArea::GetGameObject(CSSGameUnit **ppcGOBuff, INT32 n32BuffSize, bool bIgnoreHero, bool bIgnoreNPC, bool bIgnoreGoods){
	if (NULL == ppcGOBuff || 0 >= n32BuffSize){
		return eEC_NullPointer;
	}

	INT32 n32GotNum = 0;
	map<UINT64, CSSGameUnit*>::iterator iterGO = m_cObjMap.begin();
	while (m_cObjMap.end() != iterGO){
		CSSGameUnit *pcTempGO = iterGO->second;
		if (NULL == pcTempGO){
			iterGO++;
			continue;
		}
		if (bIgnoreHero &&pcTempGO->IfHero()){
			iterGO++;
			continue;
		}
		if (bIgnoreNPC && pcTempGO->IfNPC()){
			iterGO++;
			continue;
		}
		ppcGOBuff[n32GotNum] = pcTempGO;
		n32GotNum++;
		if (n32GotNum >= n32BuffSize){
			break;
		}
		iterGO++;
	}
	return n32GotNum;
}

INT32	CSSArea::GetCollideGameObject(CSSGameUnit **ppcGOBuff, INT32 n32BuffSize){
	if (NULL == ppcGOBuff || 0 >= n32BuffSize){
		return eEC_NullPointer;
	}

	INT32 n32GotNum = 0;
	map<UINT64, CSSGameUnit*>::iterator iterGO = m_cObjMap.begin();
	while (m_cObjMap.end() != iterGO){
		CSSGameUnit *pcTempGO = iterGO->second;
		if (NULL == pcTempGO){
			iterGO++;
			continue;
		}
		if (eOAS_Dead == pcTempGO->GetGOActionStateInfo().eOAS){
			iterGO++;
			continue;
		}
		if (pcTempGO->IfNPC_Building()){
			iterGO++;
			continue;
		}
		ppcGOBuff[n32GotNum] = pcTempGO;
		n32GotNum++;
		if (n32GotNum >= n32BuffSize){
			break;
		}
		iterGO++;
	}

	return n32GotNum;
}


 CSSGameUnit*	CSSArea::GetAreaUnitByIdx(UINT64 idx)
 {
	 map<UINT64, CSSGameUnit*>::iterator itr =		m_cObjMap.find(idx);
	 if (itr != m_cObjMap.end())
	 {
		 return itr->second;
	 }
	 return NULL;
 }

}