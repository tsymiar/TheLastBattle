#pragma once

#include "CMatchTeam.h"
#include "message.h"

class ICSMatchList
{
public:
	ICSMatchList(UINT32 mapId);
	virtual ~ICSMatchList();
public:
	virtual bool AddOneTeam(CCSMatchTeam* pTeam) = 0;
	virtual bool RemoveOneTeam(CCSMatchTeam* pTeam) = 0;
	virtual void Update() = 0;
	virtual bool AddInvitePlayer(IMatchPlayer* pPlayer,UINT32 matchRoomId,bool isAccpet) { return false; }
public:
	SMapLogicCfg* mMapLogicCfg;
};
