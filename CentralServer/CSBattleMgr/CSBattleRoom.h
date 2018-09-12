#pragma once

#include "CSUser.h"
using namespace CentralServer;

#define MAX_ROOM_PLAYERS 100

/*·¿¼ä*/
class CCSBattleRoom
{
public:
	CCSBattleRoom(UINT32 mapID, const string& password, UINT32 maxplayernum);
	virtual ~CCSBattleRoom();

public:
	UINT32 GetID() { return m_RoomID; }
	UINT32 GetMapID() { return m_RoomMapID; }
	UINT32 GetCurUserNum() { return m_curPlayerNum; }
	UINT32 GetMaxUserNum() { return m_maxPlayerNum; }
	const char* GetMasterNickName() { if (m_pMaster) return m_pMaster->m_pUser->GetNickName().c_str(); return "";}
	bool IsHasPWD() { return m_RoomPwd.empty()?false:true; }
	bool IsPWDMatch(const string& stPWD) { return m_RoomPwd==stPWD; }
	string	GetPwd(){return m_RoomPwd;}

public:
	bool AddPlayer(IRoomPlayer* pRoomPlayer,UINT32 pos,bool isRestart, UINT32 rePostion=0);
	bool RestartEnter(IRoomPlayer* pRoomPlayer);
	bool RemoverPlayer(IRoomPlayer* pRoomPlayer);
	bool ChangeSeat(IRoomPlayer* pRoomPlayer,UINT32 pos);
	bool ReadyCancel(IRoomPlayer* pRoomPlayer,bool isReady);
	INT32 Start(IRoomPlayer* pRoomPlayer);
	bool Chat(IRoomPlayer* pRoomPlayer,const char* chatContent,UINT32 charLen);

private:
	void BroadcastMsg_BattleSeatPosInfo(UINT32 pos, IRoomPlayer* pRoomPlayer);
	void BroadcastMsg_ToAllUser(google::protobuf::Message& sMsg, int n32MsgID);

private:
	UINT32 m_RoomID;
	UINT32 m_RoomMapID;
	string m_RoomPwd;
	UINT32 m_curPlayerNum;
	UINT32 m_maxPlayerNum;
	IRoomPlayer* m_pMaster;
	IRoomPlayer* m_pPlayerList[MAX_ROOM_PLAYERS];

	//log
public:
	void GetRommUserInfoLog();
};
