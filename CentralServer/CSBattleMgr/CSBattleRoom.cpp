#include "stdafx.h"
#include "CSBattleRoom.h"
#include "CSBattleMgr.h"
#include "CSGameLogMgr.h"

CCSBattleRoom::CCSBattleRoom( UINT32 mapID, const string& password, UINT32 maxplayernum )
{
	Assert(maxplayernum<=MAX_ROOM_PLAYERS);
	static UINT32 u32MaxRoomID = 0;
	m_RoomID = ++u32MaxRoomID;
	m_RoomMapID = mapID;
	m_RoomPwd = password;
	m_curPlayerNum = 0;
	m_maxPlayerNum = maxplayernum;
	m_pMaster = NULL;
	memset(m_pPlayerList,0,sizeof(IRoomPlayer*)*MAX_ROOM_PLAYERS);
}

CCSBattleRoom::~CCSBattleRoom()
{
	IRoomPlayer* pCurPlayer = NULL;
	for (UINT32 i = 0; i < m_maxPlayerNum; i++)
	{
		pCurPlayer = m_pPlayerList[i];
		if (pCurPlayer)
		{
			pCurPlayer->m_RoomID = 0;
			pCurPlayer->SetFindPos(-1);
			pCurPlayer->bIfReady = false;
			pCurPlayer->bIfMaster = false;
			pCurPlayer->bIfReStart = false;
			if (pCurPlayer->m_pUser->GetUserBattleInfoEx().GetBattleState()<eBattleState_Async)
			{
				pCurPlayer->m_pUser->GetUserBattleInfoEx().Clear(6);
			}
		}
	}
}

bool CCSBattleRoom::AddPlayer( IRoomPlayer* pRoomPlayer, UINT32 pos, bool isRestart, UINT32 rePostion/*=0*/ )
{
	if (pRoomPlayer->m_RoomID>0) return false;
	if (isRestart){ pos=rePostion; }

	if (pos==0)
	{
		for(UINT32 i=0;i<m_maxPlayerNum;++i)
		{
			if (m_pPlayerList[i]==NULL)
			{
				pRoomPlayer->m_RoomID = m_RoomID;
				pRoomPlayer->SetFindPos(i);
				pRoomPlayer->bIfReady = false;
				pRoomPlayer->bIfMaster = false;
				pRoomPlayer->bIfReStart = isRestart;
				if (m_pMaster==NULL)
				{
					pRoomPlayer->bIfReady = true;
					pRoomPlayer->bIfMaster = true;
					m_pMaster = pRoomPlayer;
				}
				m_pPlayerList[i] = pRoomPlayer;
				++m_curPlayerNum;
				Assert(pRoomPlayer->m_pUser->GetUserName().size()>0);
				ELOG(LOG_SpecialDebug,"玩家%s%s房间(%u)",pRoomPlayer->m_pUser->GetUserName(),pRoomPlayer->bIfMaster?"创建":"加入",m_RoomID);
				BroadcastMsg_BattleSeatPosInfo(pRoomPlayer->GetRoomPos(), NULL);
				if (!isRestart)
				{
					GSToGC::RoomBaseInfo sMsg;
					sMsg.set_roomid(m_RoomID);
					sMsg.set_mapid(m_RoomMapID);
					pRoomPlayer->m_pUser->PostMsgToGC(sMsg, sMsg.msgid());
					BroadcastMsg_BattleSeatPosInfo(0, pRoomPlayer);
				}
				return true;
			}
		}
	}
	else
	{
		--pos;
		if (pos<m_maxPlayerNum && m_pPlayerList[pos]==NULL)
		{
			pRoomPlayer->m_RoomID = m_RoomID;
			pRoomPlayer->SetFindPos(pos);
			pRoomPlayer->bIfReady = false;
			pRoomPlayer->bIfMaster = false;
			pRoomPlayer->bIfReStart = isRestart;
			if (m_pMaster==NULL)
			{
				pRoomPlayer->bIfReady = true;
				pRoomPlayer->bIfMaster = true;
				m_pMaster = pRoomPlayer;
			}
			m_pPlayerList[pos] = pRoomPlayer;
			++m_curPlayerNum;
			Assert(pRoomPlayer->m_pUser->GetUserName().size()>0);
			ELOG(LOG_SpecialDebug,"玩家%s%s房间(%u)",pRoomPlayer->m_pUser->GetUserName(),pRoomPlayer->bIfMaster?"创建":"加入",m_RoomID);
			BroadcastMsg_BattleSeatPosInfo(pRoomPlayer->GetRoomPos(), NULL);
			if (!isRestart)
			{
				GSToGC::RoomBaseInfo sMsg;
				sMsg.set_roomid(m_RoomID);
				sMsg.set_mapid(m_RoomMapID);
				pRoomPlayer->m_pUser->PostMsgToGC(sMsg, sMsg.msgid());
				BroadcastMsg_BattleSeatPosInfo(0, pRoomPlayer);
			}
			return true;
		}
	}
	return false;
}

bool CCSBattleRoom::RestartEnter(IRoomPlayer* pRoomPlayer)
{
	if (pRoomPlayer->m_RoomID==m_RoomID &&
		m_pPlayerList[pRoomPlayer->GetFindPos()]==pRoomPlayer &&
		pRoomPlayer->bIfReStart)
	{
		pRoomPlayer->bIfReStart = false;
		GSToGC::RoomBaseInfo sMsg;
		sMsg.set_roomid(m_RoomID);
		sMsg.set_mapid(m_RoomMapID);
		pRoomPlayer->m_pUser->PostMsgToGC(sMsg, sMsg.msgid());
		BroadcastMsg_BattleSeatPosInfo(0, pRoomPlayer);
		return true;
	}
	return false;
}

bool CCSBattleRoom::RemoverPlayer( IRoomPlayer* pRoomPlayer )
{
	if (pRoomPlayer->m_RoomID==m_RoomID &&
		m_pPlayerList[pRoomPlayer->GetFindPos()]==pRoomPlayer)
	{
		m_pPlayerList[pRoomPlayer->GetFindPos()] = NULL;
		--m_curPlayerNum;
		ELOG(LOG_SpecialDebug,"玩家%s离开房间(%u)",pRoomPlayer->m_pUser->GetUserName(),m_RoomID);
		{
			GSToGC::RoomBaseInfo sMsg;
			sMsg.set_roomid(0);
			sMsg.set_mapid(0);
			pRoomPlayer->m_pUser->PostMsgToGC(sMsg, sMsg.msgid());
		}
		BroadcastMsg_BattleSeatPosInfo(pRoomPlayer->GetRoomPos(), NULL);
		pRoomPlayer->m_RoomID = 0;
		pRoomPlayer->SetFindPos(-1);
		pRoomPlayer->bIfReady = false;
		pRoomPlayer->bIfReStart = false;
		if (pRoomPlayer->bIfMaster)
		{
			pRoomPlayer->bIfMaster = false;
			m_pMaster = NULL;
			for(UINT32 j=0;j<m_maxPlayerNum;++j)
			{
				if (m_pPlayerList[j]!=NULL)
				{
					m_pPlayerList[j]->bIfReady = true;
					m_pPlayerList[j]->bIfMaster = true;
					m_pMaster = m_pPlayerList[j];
					GSToGC::NotifyBattleManagerChange sMsg;
					m_pMaster->m_pUser->PostMsgToGC(sMsg, sMsg.msgid());
					BroadcastMsg_BattleSeatPosInfo(m_pPlayerList[j]->GetRoomPos(), NULL);
					break;
				}
			}
			if (m_pMaster==NULL)
			{
				GetBattleMgrInstance()->OnBattleRoomAbort(this);
			}
		}
		return true;
	}
	return false;
}

bool CCSBattleRoom::ChangeSeat( IRoomPlayer* pRoomPlayer,UINT32 pos )
{
	--pos;
	if (pos<m_maxPlayerNum && m_pPlayerList[pos]==NULL)
	{
		if (pRoomPlayer->m_RoomID==m_RoomID &&
			m_pPlayerList[pRoomPlayer->GetFindPos()]==pRoomPlayer)
		{
			m_pPlayerList[pRoomPlayer->GetFindPos()] = NULL;
			pRoomPlayer->SetFindPos(pos);
			if (!pRoomPlayer->bIfMaster)
				pRoomPlayer->bIfReady = false;
			m_pPlayerList[pos] = pRoomPlayer;
			BroadcastMsg_BattleSeatPosInfo(pRoomPlayer->GetRoomPos(), NULL);
			return true;
		}
	}
	return false;
}

bool CCSBattleRoom::ReadyCancel( IRoomPlayer* pRoomPlayer, bool isReady )
{
	if (pRoomPlayer->m_RoomID==m_RoomID &&
		m_pPlayerList[pRoomPlayer->GetFindPos()]==pRoomPlayer)
	{
		m_pPlayerList[pRoomPlayer->GetFindPos()]->bIfReady = isReady;
		BroadcastMsg_BattleSeatPosInfo(pRoomPlayer->GetRoomPos(), NULL); 
		return true;
	}
	return false;
}

INT32 CCSBattleRoom::Start( IRoomPlayer* pRoomPlayer )
{
	if (pRoomPlayer->m_RoomID==m_RoomID &&
		m_pPlayerList[pRoomPlayer->GetFindPos()]==pRoomPlayer &&
		pRoomPlayer->bIfMaster==true)
	{
		CCSUserListMap pListMap;
		for(UINT32 j=0;j<m_maxPlayerNum;++j)
		{
			if (m_pPlayerList[j]!=NULL)
			{
				if (!m_pPlayerList[j]->bIfReady)
				{
					return eEC_NotAllUserReady;
				}
				pListMap[j%2+1].AddElement(m_pPlayerList[j]->m_pUser);
			}
		}
		// create battle.
		return GetBattleMgrInstance()->OnBattleRoomStart(this, pListMap, NULL, m_RoomMapID);
	}
	return eEC_YouAreNotBattleManager;
}

bool CCSBattleRoom::Chat( IRoomPlayer* pRoomPlayer,const char* chatContent,UINT32 charLen )
{
	if (pRoomPlayer->m_RoomID==m_RoomID &&
		m_pPlayerList[pRoomPlayer->GetFindPos()]==pRoomPlayer)
	{
		GSToGC::ChatInRoom sMsg;
		sMsg.set_chat(chatContent, charLen);
		sMsg.set_pos(pRoomPlayer->GetRoomPos());
		BroadcastMsg_ToAllUser(sMsg, sMsg.msgid());
		return true;
	}
	return false;
}

void CCSBattleRoom::BroadcastMsg_BattleSeatPosInfo( UINT32 pos, IRoomPlayer* pRoomPlayer )
{
	Assert(pos<MAX_ROOM_PLAYERS);
	GSToGC::BattleSeatPosInfo sBattleSeatPosInfoMsg;
	IRoomPlayer* pCurPlayer = NULL;
	if (pos==0)
	{
		for (UINT32 i = 0; i < m_maxPlayerNum; i++)
		{
			pCurPlayer = m_pPlayerList[i];
			GSToGC::BattleSeatPosInfo::PosInfo* pPosInfo = sBattleSeatPosInfoMsg.add_posinfo();
			pPosInfo->set_pos(i+1);
			if (pCurPlayer)
			{
				pPosInfo->set_ifmaster(pCurPlayer->bIfMaster);
				pPosInfo->set_ifready(pCurPlayer->bIfReady);
				pPosInfo->set_guid(pCurPlayer->m_pUser->GetGUID());
				pPosInfo->set_nickname(pCurPlayer->m_pUser->GetNickName());
				pPosInfo->set_headid(pCurPlayer->m_pUser->GetHeadID());
				pPosInfo->set_gold(pCurPlayer->m_pUser->GetGold());
			}
		}
	}
	else
	{
		--pos;
		if (pos<m_maxPlayerNum)
		{
			GSToGC::BattleSeatPosInfo::PosInfo* pPosInfo = sBattleSeatPosInfoMsg.add_posinfo();
			pPosInfo->set_pos(pos+1);
			pCurPlayer = m_pPlayerList[pos];
			if (pCurPlayer)
			{
				pPosInfo->set_ifmaster(pCurPlayer->bIfMaster);
				pPosInfo->set_ifready(pCurPlayer->bIfReady);
				pPosInfo->set_guid(pCurPlayer->m_pUser->GetGUID());
				pPosInfo->set_nickname(pCurPlayer->m_pUser->GetNickName());
				pPosInfo->set_headid(pCurPlayer->m_pUser->GetHeadID());
				pPosInfo->set_gold(pCurPlayer->m_pUser->GetGold());
			}
		}
	}

	if (pRoomPlayer)
	{
		pRoomPlayer->m_pUser->PostMsgToGC(sBattleSeatPosInfoMsg, sBattleSeatPosInfoMsg.msgid());
	}
	else
	{
		BroadcastMsg_ToAllUser(sBattleSeatPosInfoMsg, sBattleSeatPosInfoMsg.msgid());
	}
}

void CCSBattleRoom::BroadcastMsg_ToAllUser( google::protobuf::Message& sMsg, int n32MsgID )
{
	IRoomPlayer* pCurPlayer = NULL;
	for (UINT32 i = 0; i < m_maxPlayerNum; i++)
	{
		pCurPlayer = m_pPlayerList[i];
		if (pCurPlayer)
		{
			pCurPlayer->m_pUser->PostMsgToGC(sMsg, n32MsgID);
		}
	}
}

//room  user log
void CCSBattleRoom::GetRommUserInfoLog()
{
	stringstream mystream;
	mystream<<m_curPlayerNum<<LOG_SIGN;

	for (UINT32 i = 0; i < m_maxPlayerNum; i++)
	{ 
		if (m_pPlayerList[i])
		{ 
			mystream<<m_pPlayerList[i]->m_pUser->GetUserDBData().sPODUsrDBData.un64ObjIdx<<LOG_SIGN;
		} 
	} 
	//log
	CSSGameLogMgr::GetInstance().AddGameLog(eLog_HouseStart,m_RoomID, mystream.str());
}