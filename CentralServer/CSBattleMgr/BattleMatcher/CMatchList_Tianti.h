#pragma once

#include "IMatchList.h"

class CCSMatchList_Tianti : public ICSMatchList
{
public:
	CCSMatchList_Tianti(UINT32 mapId);
	virtual ~CCSMatchList_Tianti();
public:
	virtual bool AddOneTeam(CCSMatchTeam* pTeam) { return false; }
	virtual bool RemoveOneTeam(CCSMatchTeam* pTeam) { return false; }
	virtual void Update() {}
};
