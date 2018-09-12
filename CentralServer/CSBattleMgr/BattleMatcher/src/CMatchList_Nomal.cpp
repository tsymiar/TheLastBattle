#include "stdafx.h"
#include "..\CMatchList_Nomal.h"
#include "..\..\CSCfgMgr.h"
#include "..\..\CSUser.h"
#include "..\..\CSBattleMgr.h"
#include "..\..\CSUserMgr.h"
using namespace CentralServer;

CCSMatchList_Nomal::CCSMatchList_Nomal(UINT32 mapId):ICSMatchList(mapId)
{

}

CCSMatchList_Nomal::~CCSMatchList_Nomal()
{

}

bool CCSMatchList_Nomal::AddOneTeam(CCSMatchTeam* pTeam)
{
	for (auto it=mRoomList.begin();it!=mRoomList.end();++it){
		if (it->second->AddOneTeam(pTeam)){
			return true;
		}
	}
	CCSMatchRoom_Nomal* pRoom = new CCSMatchRoom_Nomal(mMapLogicCfg->MapId);
	bool bRet = pRoom->AddOneTeam(pTeam);
	mRoomList.insert(make_pair(pRoom->GetRoomID(),pRoom));
	return true;
}
bool CCSMatchList_Nomal::RemoveOneTeam(CCSMatchTeam* pTeam)
{
	for (auto it=mRoomList.begin();it!=mRoomList.end();++it){
		if (it->second->RemoveOneTeam(pTeam)){
			return true;
		}
	}
	return false;
}
void CCSMatchList_Nomal::Update()
{
	CCSMatchRoom_Nomal* pRoom = NULL;
	for (auto it=mRoomList.begin();it!=mRoomList.end();){
		pRoom = it->second;
		if (pRoom->Update()){
			delete pRoom;
			it = mRoomList.erase(it);
			continue;
		}
		else{
			++it;
		}
	}
}

bool CCSMatchList_Nomal::AddInvitePlayer( IMatchPlayer* pPlayer, UINT32 matchRoomId, bool isAccpet )
{
	if (pPlayer->m_pUser->GetUserBattleInfoEx().GetBattleState()!=eBattleState_Free) return false;
	auto it = mRoomList.find(matchRoomId);
	if (it==mRoomList.end()) return false;
	return it->second->AddInvitePlayer(pPlayer,isAccpet);
}

//////////////////////////////////////////////////////////////////////////

CCSMatchRoom_Nomal::CCSMatchRoom_Nomal(UINT32 mapId)
{
	static UINT32 gRoomId = 0;
	mRoomID = ++gRoomId;
	mUserCount = 0;
	m_pMapLogicCfg = CCSCfgMgr::getInstance().GetMapLogicCfg(mapId);
	Assert(m_pMapLogicCfg!=NULL);
	UINT32 perCount = (m_pMapLogicCfg->SeatCount-1)/(eRoomTime_Total-1);
	mCurRoomTime = eRoomTime_Start;
	mCheckTime[eRoomTime_Start] = time(NULL); mMustCount[eRoomTime_Start] = 1+perCount*0;
	int rand = RandInt(10,30);//26
	mCheckTime[eRoomTime_Finish] = mCheckTime[0] + rand; mMustCount[eRoomTime_Finish] = m_pMapLogicCfg->SeatCount;
	if (rand>0) rand = RandInt(rand/2,rand-1);//21
	mCheckTime[eRoomTime_Random4] = mCheckTime[0] + rand; mMustCount[eRoomTime_Random4] = 1+perCount*4;
	if (rand>0) rand = RandInt(rand/2,rand-1);//16
	mCheckTime[eRoomTime_Random3] = mCheckTime[0] + rand; mMustCount[eRoomTime_Random3] = 1+perCount*3;
	if (rand>0) rand = RandInt(rand/2,rand-1);//10
	mCheckTime[eRoomTime_Random2] = mCheckTime[0] + rand; mMustCount[eRoomTime_Random2] = 1+perCount*2;
	if (rand>0) rand = RandInt(rand/2,rand-1);//8
	mCheckTime[eRoomTime_Random1] = mCheckTime[0] + rand; mMustCount[eRoomTime_Random1] = 1+perCount*1;
	ELOG(LOG_SpecialDebug,"房间号%d：开始时间%s",mRoomID,ctime(&mCheckTime[eRoomTime_Start]));
	ELOG(LOG_SpecialDebug,"房间号%d：至少2人时间%s",mRoomID,ctime(&mCheckTime[eRoomTime_Random1]));
	ELOG(LOG_SpecialDebug,"房间号%d：至少3人时间%s",mRoomID,ctime(&mCheckTime[eRoomTime_Random2]));
	ELOG(LOG_SpecialDebug,"房间号%d：至少4人时间%s",mRoomID,ctime(&mCheckTime[eRoomTime_Random3]));
	ELOG(LOG_SpecialDebug,"房间号%d：至少5人时间%s",mRoomID,ctime(&mCheckTime[eRoomTime_Random4]));
	ELOG(LOG_SpecialDebug,"房间号%d：结束时间%s",mRoomID,ctime(&mCheckTime[eRoomTime_Finish]));
	mIsInvalid = false;
}

CCSMatchRoom_Nomal::~CCSMatchRoom_Nomal()
{

}

bool CCSMatchRoom_Nomal::AddOneTeam(CCSMatchTeam* pTeam)
{
	if (mIsInvalid) return false;
	for (int i=1;i<=m_pMapLogicCfg->PlayerModel.size();++i){
		int curTeamSize = 0;
		for (auto it=mTeamMap[i].begin();it!=mTeamMap[i].end();++it){
			curTeamSize += (*it)->GetPlayerCount();
		}
		if (curTeamSize+pTeam->GetPlayerCount()<=m_pMapLogicCfg->PlayerModel[i-1]){
			mTeamMap[i].push_back(pTeam);
			mUserCount += pTeam->GetPlayerCount();
			pTeam->Switch(true);
			GSToGC::BattleMatcherCount msg;
			msg.set_count(mUserCount>=mMustCount[mCurRoomTime]?mUserCount:mMustCount[mCurRoomTime]);
			msg.set_maxcount(m_pMapLogicCfg->SeatCount);
			PostMsgToAll(msg,msg.msgid());
			return true;
		}
	}
	return false;
}

bool CCSMatchRoom_Nomal::RemoveOneTeam(CCSMatchTeam* pTeam)
{
	for (int i=1;i<=m_pMapLogicCfg->PlayerModel.size();++i){
		for (auto it=mTeamMap[i].begin();it!=mTeamMap[i].end();++it){
			if ((*it)==pTeam){
				mTeamMap[i].erase(it);
				mUserCount -= pTeam->GetPlayerCount();
				if (mUserCount==0) mIsInvalid = true;
				pTeam->Switch(false);
				GSToGC::BattleMatcherCount msg;
				msg.set_count(mUserCount>=mMustCount[mCurRoomTime]?mUserCount:mMustCount[mCurRoomTime]);
				msg.set_maxcount(m_pMapLogicCfg->SeatCount);
				PostMsgToAll(msg,msg.msgid());
				return true;
			}
		}
	}
	return false;
}

bool CCSMatchRoom_Nomal::Update()
{
	if (mUserCount==0 || mIsInvalid) return true;
	if (mUserCount==m_pMapLogicCfg->SeatCount ||
		mMustCount[mCurRoomTime]==m_pMapLogicCfg->SeatCount)
	{
		map<UINT32,UINT32> aiRobots;
		for (int i=1;i<=m_pMapLogicCfg->PlayerModel.size();++i){
			int curTeamSize = 0;
			for (auto it=mTeamMap[i].begin();it!=mTeamMap[i].end();++it){
				curTeamSize += (*it)->GetPlayerCount();
			}
			while (curTeamSize<m_pMapLogicCfg->PlayerModel[i-1]){
				aiRobots.insert(make_pair(i+curTeamSize*m_pMapLogicCfg->PlayerModel.size(),i));
				++curTeamSize;
			}
		}
		GetBattleMgrInstance()->OnBattleMached(eMatchType_Nomal,m_pMapLogicCfg->MapId,mTeamMap,&aiRobots);
		return true;
	}
	time_t curTime = time(NULL);
	for (int i=mCurRoomTime+1;i<=eRoomTime_Finish;++i){
		if (curTime>mCheckTime[i]){//跳转到下一状态//
			ELOG(LOG_SpecialDebug,"房间号%d：跳转时间%s",mRoomID,ctime(&curTime));
			mCurRoomTime = (ERoomTime)i;
			GSToGC::BattleMatcherCount msg;
			msg.set_count(mUserCount>=mMustCount[mCurRoomTime]?mUserCount:mMustCount[mCurRoomTime]);
			msg.set_maxcount(m_pMapLogicCfg->SeatCount);
			PostMsgToAll(msg,msg.msgid());
			break;
		}
	}
	// 下面邀请玩家流程被屏蔽
	if (false && mCurRoomTime==eRoomTime_Random4 && mCheckTime[eRoomTime_Random4]+15<=mCheckTime[eRoomTime_Finish]){//66+15<=82
		if (mUserCount+1==m_pMapLogicCfg->SeatCount && mInvitePlayer.size()==0){
			CentralServer::CCSUser* pUser = GetCSUserMgrInstance()->GetIdlePlayer(mIgorePlayerMap);
			if (pUser!=NULL){
				mInvitePlayer = pUser->GetNickName();
				GSToGC::NotifyOneMatchNeedOne msg;
				msg.set_mapid(m_pMapLogicCfg->MapId);
				msg.set_fightid(mRoomID);
				pUser->PostMsgToGC(msg,msg.msgid());
			}
		}
	}
	return false;
}

void CCSMatchRoom_Nomal::PostMsgToAll( google::protobuf::Message& sMsg, int n32MsgID )
{
	for (auto it=mTeamMap.begin();it!=mTeamMap.end();++it){
		for (auto it2=it->second.begin();it2!=it->second.end();++it2){
			for (auto it3=(*it2)->mVPlayer.begin();it3!=(*it2)->mVPlayer.end();++it3){
				(*it3)->m_pUser->PostMsgToGC(sMsg, n32MsgID);
			}
		}
	}
}

bool CCSMatchRoom_Nomal::AddInvitePlayer( IMatchPlayer* pPlayer, bool isAccpet )
{
	if (mInvitePlayer!=pPlayer->m_pUser->GetNickName()) return false;
	if (isAccpet==false){ mIgorePlayerMap.insert(make_pair(mInvitePlayer,false)); mInvitePlayer.clear(); return true; };
	if (mUserCount==m_pMapLogicCfg->SeatCount) return false;
	CCSMatchTeam* pTeam = GetBattleMgrInstance()->GetMatcher()->UserCreateTeam(pPlayer,eMatchType_Nomal,m_pMapLogicCfg->MapId);
	if (pTeam==NULL) return false;
	bool bRet = AddOneTeam(pTeam);
	Assert(bRet==true);
	mInvitePlayer.clear();
	return true;
}
