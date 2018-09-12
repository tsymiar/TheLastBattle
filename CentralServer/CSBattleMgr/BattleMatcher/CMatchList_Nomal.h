#pragma once

#include "IMatchList.h"

class CCSMatchRoom_Nomal;

class CCSMatchList_Nomal : public ICSMatchList
{
public:
	CCSMatchList_Nomal(UINT32 mapId);
	virtual ~CCSMatchList_Nomal();
public:
	virtual bool AddOneTeam(CCSMatchTeam* pTeam);
	virtual bool RemoveOneTeam(CCSMatchTeam* pTeam);
	virtual void Update();
	virtual bool AddInvitePlayer(IMatchPlayer* pPlayer,UINT32 matchRoomId,bool isAccpet);
public:
	map<UINT32,CCSMatchRoom_Nomal*> mRoomList;
};

enum ERoomTime
{
	eRoomTime_Start,
	eRoomTime_Random1,
	eRoomTime_Random2,
	eRoomTime_Random3,
	eRoomTime_Random4,
	eRoomTime_Finish,
	eRoomTime_Total,
};

inline int RandInt(int Low, int Up)
{
	return int(double(rand()) / (RAND_MAX + 1) * (Up - Low + 1)) + Low;
}

class CCSMatchRoom_Nomal
{
public:
	CCSMatchRoom_Nomal(UINT32 mapId);
	virtual ~CCSMatchRoom_Nomal();
public:
	bool AddOneTeam(CCSMatchTeam* pTeam);
	bool RemoveOneTeam(CCSMatchTeam* pTeam);
	bool Update();
	void PostMsgToAll(google::protobuf::Message& sMsg,int n32MsgID);
	UINT32 GetRoomID() { return mRoomID; }
	bool AddInvitePlayer(IMatchPlayer* pPlayer,bool isAccpet);
public:
	SMapLogicCfg* m_pMapLogicCfg;
	ERoomTime mCurRoomTime;
	time_t mCheckTime[eRoomTime_Total];//检查补足时间//
	UINT32 mMustCount[eRoomTime_Total];//必定数量(机器人补足)//
	UINT32 mUserCount;//玩家数量//
	UINT32 mRoomID;
	string mInvitePlayer;
	map<string,bool> mIgorePlayerMap;
	map<UINT32,vector<CCSMatchTeam*>> mTeamMap;
	bool mIsInvalid;//当没玩家时设置为无效
};
