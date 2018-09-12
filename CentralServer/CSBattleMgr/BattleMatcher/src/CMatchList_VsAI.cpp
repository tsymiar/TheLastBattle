#include "stdafx.h"
#include "..\CMatchList_VsAI.h"
#include "..\..\CSBattleMgr.h"

CCSMatchList_VsAI::CCSMatchList_VsAI(UINT32 mapId):ICSMatchList(mapId)
{
	mUserCount = 0;
}

CCSMatchList_VsAI::~CCSMatchList_VsAI()
{

}

bool CCSMatchList_VsAI::AddOneTeam(CCSMatchTeam* pTeam)
{
	pTeam->Switch(true);
	mVTeam.push_back(pTeam);
	mUserCount += pTeam->GetPlayerCount();
	GSToGC::BattleMatcherCount msg;
	msg.set_count(mUserCount);
	Assert(mUserCount>=0 && mUserCount<100);
	msg.set_maxcount(mMapLogicCfg->PlayerModel[0]);
	PostMsgToAll(msg,msg.msgid());
	return true;
}
bool CCSMatchList_VsAI::RemoveOneTeam(CCSMatchTeam* pTeam)
{
	for (auto it=mVTeam.begin();it!=mVTeam.end();++it){
		if ((*it)==pTeam){
			pTeam->Switch(false);
			mVTeam.erase(it);
			mUserCount -= pTeam->GetPlayerCount();
			GSToGC::BattleMatcherCount msg;
			msg.set_count(mUserCount);
			Assert(mUserCount>=0 && mUserCount<100);
			msg.set_maxcount(mMapLogicCfg->PlayerModel[0]);
			PostMsgToAll(msg,msg.msgid());
			return true;
		}
	}
	return false;
}
void CCSMatchList_VsAI::Update()
{
	vector<CCSMatchTeam*> mVLeaved = mVTeam;
	map<UINT32,vector<CCSMatchTeam*>> mVTeamStart;
	vector<CCSMatchTeam*> mVTeamAI;
	int curUserCount = 0;
	int virUserCount = 0;
	for (auto it=mVLeaved.begin();it!=mVLeaved.end();){
		virUserCount += (*it)->GetPlayerCount();
		if (virUserCount>mMapLogicCfg->PlayerModel[0]){
			virUserCount = curUserCount;
			++it;//超重下班电梯//
			continue;
		}
		mVTeamStart[1].push_back(*it);//排队进电梯//
		curUserCount = virUserCount;
		it = mVLeaved.erase(it);
		if (virUserCount==mMapLogicCfg->PlayerModel[0]){
			map<UINT32,UINT32> aiRobots;
			UINT32 pos = 1;
			UINT32 campId = 1;
			for (auto it=mMapLogicCfg->PlayerModel.begin();it!=mMapLogicCfg->PlayerModel.end();++it){
				if (pos==1) { ++pos; continue; }//阵营开始位置//
				campId = pos;
				UINT32 count = (*it);
				while(count-->0) { aiRobots.insert(make_pair(pos,campId)); pos += mMapLogicCfg->PlayerModel.size(); }
			}
			GetBattleMgrInstance()->OnBattleMached(eMatchType_VsAI,mMapLogicCfg->MapId,mVTeamStart,&aiRobots);
			mVTeamStart.clear();
			mUserCount -= virUserCount;
			curUserCount = 0;
			virUserCount = 0;
			it = mVLeaved.begin();//下班电梯马上就来了哦//
			GSToGC::BattleMatcherCount msg;
			msg.set_count(mUserCount);
			Assert(mUserCount>=0 && mUserCount<100);
			msg.set_maxcount(mMapLogicCfg->PlayerModel[0]);
			PostMsgToAll(msg,msg.msgid());
		}
	}
	mVTeam = mVTeamStart[1];
	mVTeam.insert(mVTeam.end(),mVLeaved.begin(),mVLeaved.end());
}

void CCSMatchList_VsAI::PostMsgToAll( google::protobuf::Message& sMsg,int n32MsgID )
{
	CCSMatchTeam* pTeam = NULL;
	for (auto it=mVTeam.begin();it!=mVTeam.end();++it){
		pTeam = (*it);
		for (auto it2=pTeam->mVPlayer.begin();it2!=pTeam->mVPlayer.end();++it2){
			(*it2)->m_pUser->PostMsgToGC(sMsg, n32MsgID);
		}
	}
}
