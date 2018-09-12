#include "stdafx.h"
#include "..\CMatchTeam.h"
#include "..\..\CSCfgMgr.h"
#include "..\..\CSUser.h"
#include "..\..\CSUserMgr.h"

using namespace CentralServer;

CCSMatchTeam::CCSMatchTeam(EBattleMatchType matchType, UINT32 mapId)
{
	static UINT32 sTeamId = 0;
	mTeamId = ++sTeamId;
	mMatchType = matchType;
	mMapId = mapId;
	m_pMapLogicCfg = CCSCfgMgr::getInstance().GetMapLogicCfg(mapId);
	Assert(m_pMapLogicCfg);
	Assert(mMatchType>eMatchType_None && mMatchType<eMatchType_Total);
	mInMatch = false;
}

CCSMatchTeam::~CCSMatchTeam()
{
	Assert(mInMatch==false);
	GSToGC::NotifyMatchTeamBaseInfo msgBaseInfo;
	msgBaseInfo.set_teamid(0);
	msgBaseInfo.set_matchtype(eMatchType_None);
	msgBaseInfo.set_mapid(0);
	IMatchPlayer* curPlayer = NULL;
	for (auto it=mVPlayer.begin();it!=mVPlayer.end();++it){
		curPlayer = *it;
		curPlayer->m_MatchTeamId = 0;
		curPlayer->bIfMaster = false;
		curPlayer->m_pUser->PostMsgToGC(msgBaseInfo,msgBaseInfo.msgid());
		curPlayer->m_pUser->GetUserBattleInfoEx().Clear(33);
	}
}

bool CCSMatchTeam::AddOneUser(IMatchPlayer* pPlayer)
{
	if (mInMatch) return false;
	bool isOk = false;
	for (auto it=m_pMapLogicCfg->PlayerModel.begin();it!=m_pMapLogicCfg->PlayerModel.end();++it){
		if (mVPlayer.size()<(*it)){
			isOk = true;
			break;
		}
	}
	if (!isOk) return false;

	GSToGC::NotifyMatchTeamBaseInfo msgBaseInfo;
	msgBaseInfo.set_teamid(mTeamId);
	msgBaseInfo.set_matchtype(mMatchType);
	msgBaseInfo.set_mapid(mMapId);
	pPlayer->m_pUser->PostMsgToGC(msgBaseInfo,msgBaseInfo.msgid());

	GSToGC::NotifyMatchTeamPlayerInfo msgMyInfo;
	msgMyInfo.set_postion(mVPlayer.size());
	msgMyInfo.set_nickname(pPlayer->m_pUser->GetNickName());
	msgMyInfo.set_headid(pPlayer->m_pUser->GetHeadID());
	msgMyInfo.set_userlevel(pPlayer->m_pUser->GetUserLv());
	msgMyInfo.set_isinsert(true);

	GSToGC::NotifyMatchTeamPlayerInfo msgPlayerInfo;
	int pos = 0;
	for (auto it=mVPlayer.begin();it!=mVPlayer.end();++it,++pos){
		Assert((*it)!=pPlayer);
		msgPlayerInfo.set_postion(pos);
		msgPlayerInfo.set_nickname((*it)->m_pUser->GetNickName());
		msgPlayerInfo.set_headid((*it)->m_pUser->GetHeadID());
		msgPlayerInfo.set_userlevel((*it)->m_pUser->GetUserLv());
		msgPlayerInfo.set_isinsert(true);
		pPlayer->m_pUser->PostMsgToGC(msgPlayerInfo,msgPlayerInfo.msgid());
		(*it)->m_pUser->PostMsgToGC(msgMyInfo,msgMyInfo.msgid());
	}

	pPlayer->m_pUser->PostMsgToGC(msgMyInfo,msgMyInfo.msgid());
	mVPlayer.push_back(pPlayer);
	pPlayer->m_MatchTeamId = mTeamId;
	if (mVPlayer.size()==1) pPlayer->bIfMaster = true;
	pPlayer->m_pUser->GetUserBattleInfoEx().ChangeTypeWithState(eBattleType_Match,eBattleState_Wait);
	return true;
}

bool CCSMatchTeam::RemoveOneUser(IMatchPlayer* pPlayer)
{
	Assert(pPlayer->bIfMaster==false);
	if (mInMatch) return false;
	int pos = 0;
	for (auto it=mVPlayer.begin();it!=mVPlayer.end();++it,++pos){
		if ((*it)==pPlayer){
			GSToGC::NotifyMatchTeamBaseInfo msgBaseInfo;
			msgBaseInfo.set_teamid(0);
			msgBaseInfo.set_matchtype(eMatchType_None);
			msgBaseInfo.set_mapid(0);
			pPlayer->m_pUser->PostMsgToGC(msgBaseInfo,msgBaseInfo.msgid());
			pPlayer->m_MatchTeamId = 0;
			mVPlayer.erase(it);
			GSToGC::NotifyMatchTeamPlayerInfo msgMyInfo;
			msgMyInfo.set_postion(pos);
			msgMyInfo.set_nickname(pPlayer->m_pUser->GetNickName());
			msgMyInfo.set_headid(0);
			for (auto it2=mVPlayer.begin();it2!=mVPlayer.end();++it2){
				(*it2)->m_pUser->PostMsgToGC(msgMyInfo,msgMyInfo.msgid());
			}
			pPlayer->m_pUser->GetUserBattleInfoEx().Clear(11);
			mStopedPlayer.erase(pPlayer->m_pUser->GetGUID());
			return true;
		}
	}
	return false;
}

bool CCSMatchTeam::Dissolve(IMatchPlayer* pPlayer)
{
	Assert(pPlayer->bIfMaster==true);
	if (mInMatch) return false;
	GSToGC::NotifyMatchTeamBaseInfo msgBaseInfo;
	msgBaseInfo.set_teamid(0);
	msgBaseInfo.set_matchtype(eMatchType_None);
	msgBaseInfo.set_mapid(0);
	IMatchPlayer* curPlayer = NULL;
	for (auto it=mVPlayer.begin();it!=mVPlayer.end();++it){
		curPlayer = *it;
		curPlayer->m_MatchTeamId = 0;
		curPlayer->bIfMaster = false;
		curPlayer->m_pUser->PostMsgToGC(msgBaseInfo,msgBaseInfo.msgid());
		curPlayer->m_pUser->GetUserBattleInfoEx().Clear(22);
		mStopedPlayer.erase(pPlayer->m_pUser->GetGUID());
	}
	Assert(pPlayer->bIfMaster==false);
	return true;
}

void CCSMatchTeam::Switch(bool isMatch)
{
	if (isMatch) Assert(mStopedPlayer.size()==0);
	mInMatch = isMatch;
	GSToGC::NotifyMatchTeamSwitch switchMsg;
	switchMsg.set_startflag(isMatch);
	IMatchPlayer* curPlayer = NULL;
	for (auto it=mVPlayer.begin();it!=mVPlayer.end();++it){
		curPlayer = *it;
		curPlayer->m_pUser->PostMsgToGC(switchMsg,switchMsg.msgid());
	}
}

void CCSMatchTeam::Stop(IMatchPlayer* pPlayer)
{
	mStopedPlayer.insert(make_pair(pPlayer->m_pUser->GetGUID(),true));
}

bool CCSMatchTeam::PushDataOnlinePlayer(IMatchPlayer* pPlayer)
{
	auto it = mStopedPlayer.find(pPlayer->m_pUser->GetGUID());
	if (it==mStopedPlayer.end()) return false;
	mStopedPlayer.erase(it);

	GSToGC::NotifyMatchTeamBaseInfo msgBaseInfo;
	msgBaseInfo.set_teamid(mTeamId);
	msgBaseInfo.set_matchtype(mMatchType);
	msgBaseInfo.set_mapid(mMapId);
	pPlayer->m_pUser->PostMsgToGC(msgBaseInfo,msgBaseInfo.msgid());

	GSToGC::NotifyMatchTeamPlayerInfo msgPlayerInfo;
	int pos = 0;
	for (auto it=mVPlayer.begin();it!=mVPlayer.end();++it,++pos){
		msgPlayerInfo.set_postion(pos);
		msgPlayerInfo.set_nickname((*it)->m_pUser->GetNickName());
		msgPlayerInfo.set_headid((*it)->m_pUser->GetHeadID());
		msgPlayerInfo.set_userlevel((*it)->m_pUser->GetUserLv());
		msgPlayerInfo.set_isinsert(true);
		pPlayer->m_pUser->PostMsgToGC(msgPlayerInfo,msgPlayerInfo.msgid());
	}

	return true;
}
