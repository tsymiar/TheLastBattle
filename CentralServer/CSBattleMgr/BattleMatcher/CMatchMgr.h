#pragma once

#include "CMatchTeam.h"
#include "IMatchList.h"

class CCSMatchMgr
{
public:
	CCSMatchMgr();
	virtual ~CCSMatchMgr();
public:
	CCSMatchTeam* UserCreateTeam(IMatchPlayer* pPlayer,EBattleMatchType matchType,UINT32 mapId);
	bool UserAddTeam(IMatchPlayer* pPlayer,IMatchPlayer* pTeamLeader);
	bool UserRemoveTeam(IMatchPlayer* pPlayer);
	bool IsUserTeamInMatch(IMatchPlayer* pPlayer);
	INT32 TeamStartMatch(IMatchPlayer* pPlayer);
	bool TeamStopMatch(IMatchPlayer* pPlayer);
	bool TeamAddInvitePlayer(IMatchPlayer* pPlayer,UINT32 roomId,UINT32 mapId,bool isAccpet);
	void Update(int64_t tUTCMilsec, int64_t tTickSpan);
	void UserStopTeam(IMatchPlayer* pPlayer);
	bool PushDataOnlinePlayer(IMatchPlayer* pPlayer);
private:
	map<UINT32,ICSMatchList*> mAllMatchList[eMatchType_Total];
	map<UINT32,CCSMatchTeam*> mAllTeamMap;
};