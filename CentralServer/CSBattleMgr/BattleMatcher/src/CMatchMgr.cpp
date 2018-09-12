#include "stdafx.h"
#include "..\CMatchMgr.h"
#include "..\..\CSCfgMgr.h"
#include "..\..\CSUser.h"
using namespace CentralServer;

CCSMatchMgr::CCSMatchMgr() 
{
}

CCSMatchMgr::~CCSMatchMgr()
{
	for (int i=0;i<eMatchType_Total;++i)
	{
		for (auto it=mAllMatchList[i].begin(); it!=mAllMatchList[i].end(); ++it)
		{
			delete it->second;
		}
	}
	for (auto it=mAllTeamMap.begin();it!=mAllTeamMap.end();++it)
	{
		delete it->second;
	}
}

CCSMatchTeam* CCSMatchMgr::UserCreateTeam(IMatchPlayer* pPlayer,EBattleMatchType matchType,UINT32 mapId)
{
	Assert(pPlayer->m_pUser->GetUserBattleInfoEx().GetBattleType()==eBattleType_Free);
	CCSMatchTeam* pTeam = new CCSMatchTeam(matchType,mapId);
	mAllTeamMap.insert(make_pair(pTeam->mTeamId,pTeam));
	bool bRet = pTeam->AddOneUser(pPlayer);
	Assert(bRet==true);
	return pTeam;
}

bool CCSMatchMgr::UserAddTeam(IMatchPlayer* pPlayer,IMatchPlayer* pTeamLeader)
{
	Assert(pPlayer->m_pUser->GetUserBattleInfoEx().GetBattleType()==eBattleType_Free);
	auto it = mAllTeamMap.find(pTeamLeader->m_MatchTeamId);
	if (it==mAllTeamMap.end()) return false;
	CCSMatchTeam* pTeam = it->second;
	if (pTeam->GetLeader()!=pTeamLeader) return false;
	return pTeam->AddOneUser(pPlayer);
}

bool CCSMatchMgr::IsUserTeamInMatch(IMatchPlayer* pPlayer)
{
	if (pPlayer->m_MatchTeamId==0) return false;
	auto it = mAllTeamMap.find(pPlayer->m_MatchTeamId);
	if (it==mAllTeamMap.end()) return false;
	CCSMatchTeam* pTeam = it->second;
	return pTeam->mInMatch;
}

bool CCSMatchMgr::UserRemoveTeam(IMatchPlayer* pPlayer)
{
	if (pPlayer->m_pUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Match) return false;
	if (pPlayer->m_pUser->GetUserBattleInfoEx().GetBattleState()!=eBattleState_Wait) return false;
	auto it = mAllTeamMap.find(pPlayer->m_MatchTeamId);
	if (it==mAllTeamMap.end()) return false;
	CCSMatchTeam* pTeam = it->second;
	if (pPlayer->bIfMaster){
		if (pTeam->Dissolve(pPlayer)){
			delete pTeam;
			mAllTeamMap.erase(it);
			return true;
		}
	}
	else if (pTeam->RemoveOneUser(pPlayer)){
		return true;
	}
	//log

	return false;
}

INT32 CCSMatchMgr::TeamStartMatch(IMatchPlayer* pPlayer)
{
	auto it = mAllTeamMap.find(pPlayer->m_MatchTeamId);
	if (it==mAllTeamMap.end()) return eEC_UnKnownError;
	CCSMatchTeam* pTeam = it->second;
	if (pTeam->mStopedPlayer.size()>0) return eEC_MatchTeamateStoped;
	if (!pTeam->IsCanMatch()) return eEC_UnKnownError;
	Assert(CCSCfgMgr::getInstance().GetMapLogicCfg(pTeam->mMapId)!=NULL);
	ICSMatchList* &pOwnerList = mAllMatchList[pTeam->mMatchType][pTeam->mMapId];
	if (pOwnerList==NULL) 
	{
		switch(pTeam->mMatchType)
		{
		case eMatchType_VsAI:
			pOwnerList = new CCSMatchList_VsAI(pTeam->mMapId);
			break;
		case eMatchType_Nomal:
			pOwnerList = new CCSMatchList_Nomal(pTeam->mMapId);
			break;
		case eMatchType_Tianti:
			pOwnerList = new CCSMatchList_Tianti(pTeam->mMapId);
			break;
		}
	}
	bool bRet = pOwnerList->AddOneTeam(pTeam);
	//log
	pTeam->mStartTime = time(NULL);
	return bRet?eNormal:eEC_UnKnownError;
}

bool CCSMatchMgr::TeamStopMatch(IMatchPlayer* pPlayer)
{
	auto it = mAllTeamMap.find(pPlayer->m_MatchTeamId);
	if (it==mAllTeamMap.end()) return false;
	CCSMatchTeam* pTeam = it->second;
	if (pTeam->mInMatch==false) return false;
	ICSMatchList* &pOwnerList = mAllMatchList[pTeam->mMatchType][pTeam->mMapId];
	Assert(pOwnerList!=NULL);
	return pOwnerList->RemoveOneTeam(pTeam);
}

bool CCSMatchMgr::TeamAddInvitePlayer( IMatchPlayer* pPlayer,UINT32 roomId,UINT32 mapId,bool isAccpet )
{
	ICSMatchList* &pOwnerList = mAllMatchList[eMatchType_Nomal][mapId];
	if (pOwnerList)
		return pOwnerList->AddInvitePlayer(pPlayer,roomId,isAccpet);
	return false;
}

void CCSMatchMgr::Update(int64_t tUTCMilsec, int64_t tTickSpan)
{
	for (int i=0;i<eMatchType_Total;++i)
	{
		for (auto it=mAllMatchList[i].begin(); it!=mAllMatchList[i].end(); ++it)
		{
			it->second->Update();
		}
	}
}

void CCSMatchMgr::UserStopTeam(IMatchPlayer* pPlayer)
{
	if (pPlayer->m_MatchTeamId==0) return;
	auto it = mAllTeamMap.find(pPlayer->m_MatchTeamId);
	if (it==mAllTeamMap.end()) return;
	CCSMatchTeam* pTeam = it->second;
	pTeam->Stop(pPlayer);
}

bool CCSMatchMgr::PushDataOnlinePlayer(IMatchPlayer* pPlayer)
{
	if (pPlayer->m_MatchTeamId==0) return false;
	auto it = mAllTeamMap.find(pPlayer->m_MatchTeamId);
	if (it==mAllTeamMap.end()) return false;
	CCSMatchTeam* pTeam = it->second;
	return pTeam->PushDataOnlinePlayer(pPlayer);
}
