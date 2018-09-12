/*
* Copyright (c) 2010,WangYanYu
* All rights reserved.
*
* file name			:SSArea.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:WangYanYu
* complete date		:March 26 2014
* summary			:
*
*/

#pragma once
#include "stdafx.h"

namespace SceneServer{

class CSSGameUnit;
class CSSArea
{
private:
	INT32							m_n32XAreaIdx;
	INT32							m_n32ZAreaIdx;
	UINT32							m_un32AreaID;
	map<UINT64, CSSGameUnit*>		m_cObjMap;

public:
	CSSArea(void);
	~CSSArea(void);

	UINT32			GetAreaID(){return m_un32AreaID;}
	void			SetAreaID(UINT32 un32AreaID){m_un32AreaID = un32AreaID;}
	INT32			GetXAreaIdx(){return m_n32XAreaIdx;}
	void			SetXAreaIdx(INT32 n32Idx){m_n32XAreaIdx = n32Idx;}
	INT32			GetZAreaIdx(){return m_n32ZAreaIdx;}
	void			SetZAreaIdx(INT32 n32Idx){m_n32ZAreaIdx = n32Idx;}

	INT32			AddObj(CSSGameUnit *pcGO);
	CSSGameUnit*			GetAreaUnitByIdx(UINT64 idx);
	INT32			RemoveObj(const SGUID_KEY &crsGUID);
	INT32			GetGOList(vector<CSSGameUnit*> &rcGOVec );
	INT32			GetEnemyGUList(INT32 n32SelfCampID, vector<CSSGameUnit*> &rcGUVec);
	INT32			GetUserSightMap(map<CSSUser*, INT32> &rcUserSightMap);
	INT32			GetGameObject(const CVector3D &crsCenterPos, INT32 n32Sight, CSSGameUnit **ppcGOBuff, INT32 n32BuffSize, bool bIgnoreHero, bool bIgnoreNPC, bool bIgnoreGoods);
	INT32			GetGameObject(CSSGameUnit **ppcGOBuff, INT32 n32BuffSize, bool bIgnoreHero, bool bIgnoreNPC, bool bIgnoreGoods );
	INT32			GetCollideGameObject(CSSGameUnit **ppcGOBuff, INT32 n32BuffSize);
	INT32			Clear(); 
	bool			IsObjExitAtPos(const CVector3D & cPos);
	
};

}
