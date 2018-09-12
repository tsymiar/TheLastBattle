#pragma once
#include "stdafx.h"
#include "CSCfgMgr.h"
#include <unordered_map>
#include <hash_map>
#include <map>
#include "CSBattleRoom.h"
 

class CCSMatchMgr;

namespace CentralServer{

class CCSBattle;
class CCSUser;
typedef	hash_map<UINT64, CCSBattle*> BattleMap;

class CCSBattleMgr
{
private:
	CCSBattleMgr();
public:
	~CCSBattleMgr();
	static CCSBattleMgr&	GetInstance();

private:
	UINT64						m_un64MaxBattleID;
	BattleMap					m_cAllBattleMap;
	CCSMatchMgr*				m_pMatchMgr;
	map<UINT32,CCSBattleRoom*>	m_BattleRoomList;

public:
	void					RegisterMsgHandle(SSMsgHandlerMap& m_SSMsgHandlerMap, GSMsgHandlerMap& m_GSMsgHandlerMap, GCMsgHandlerMap& m_GCMsgHandlerMap, RCMsgHandlerMap& m_RCMsgHandlerMap);
	INT32					Initialize();
	INT32					Uninitialize();
	void					GetProfileReport(wstringstream &report);
	CCSMatchMgr*			GetMatcher() { return m_pMatchMgr; }

	 
public:
	UINT32					GetTotalBattleNum(){ return (UINT32)m_cAllBattleMap.size(); }
	UINT64					GenerateBattleID() { return ++m_un64MaxBattleID; }
	const UINT32			GetBattleMapID(UINT64 un64BattleID);
	CCSBattle*				GetBattle(UINT64 un64BattleID);

public:
	bool					CheckWordFilter(char* pStr);
	INT32					NotifyBattleSSUserIsOnline(CCSUser *piUser, bool isOnline);
	INT32					PostMsgToBattleSS(UINT64 un64BattleID, google::protobuf::Message& sMsg, int n32MsgID);
	INT32					SyncMsgForRemoveBattle(UINT64 un64BattleID);

public:
	INT32					AskCreateGuideBattle(CCSUser* piUser, INT32 mapId, INT32 ntype);

public:
	INT32					AskRoomList(CCSUser *piUser);
	INT32					AskCreateRoom(CCSUser *piUser, UINT32 un32MapId, const string& stPwd, bool isRestart, UINT32 rePostion=0);
	INT32					AskAddRoom(CCSUser *piUser, UINT64 un32RoomId, const string& stPwd, bool isRestart, UINT32 rePostion=0);
	INT32					AskLeaveRoom(CCSUser *piUser);
	INT32					AskChangeBattleSeat(CCSUser *piUser, INT32 n32TarPos);
	INT32					AskReadyBattle(CCSUser *piUser,bool isReady);
	INT32					AskStartBattle(CCSUser *piUser);
	INT32					AskChatInRoom(CCSUser *piUser,const char* chatContent,UINT32 charLen);
	INT32					AskReEnterRoom(CCSUser *piUser);
	CCSBattleRoom*			GetRoom(UINT64 roomid);
	INT32					OnBattleRoomStart(CCSBattleRoom* pRoom, CCSUserListMap& sList, map<UINT32,UINT32>* aiRobots, UINT32 mapID);
	void					OnBattleRoomAbort(CCSBattleRoom* pRoom);

public:
	INT32					RemoveMatchUser(CCSUser *piUser);
	void					OnBattleMached(CCSUserListMap& pList, map<UINT32,UINT32>* aiRobots, UINT32 mapID, EBattleMatchType type);
	void					OnBattleMached(EBattleMatchType type,UINT32 mapId,map<UINT32,vector<CCSMatchTeam*>>& teamList,map<UINT32,UINT32>* aiRobots);

private:
	//////////////////////////////////////////////////////////////////////////
	INT32					OnMsgFromRC_AskServerInfo(ICSRCInfo *piRCInfo, const char* pData, int n32DataLength);
	//////////////////////////////////////////////////////////////////////////
	INT32					OnMsgFromGC_AskCreateMatchTeam(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskAddMatchTeam(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskRemoveMatchTeam(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskInviteJoinMatchTeam(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskStartMatch(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskStopMatch(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_QuestMatchTeamList(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_NotifyOneMatchNeedOneRet(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskGuideBattle(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskCreateRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskRoomList(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskAddRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskLeaveRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskChatInRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskChangeRoomSeat(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskReadyRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskCancelRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskStartRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	INT32					OnMsgFromGC_AskReEnterRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength);
	//////////////////////////////////////////////////////////////////////////
	INT32					OnMsgFromSS_AllBattleInfo(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength);
	INT32					OnMsgFromSS_CreateBattleResponse(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength);
	INT32					OnMsgFromSS_FinishedBattle(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength);
	INT32					OnMsgFromSS_AskAllCfgList(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength);
	INT32					OnMsgFromSS_RepostGotMsg(ICSSSInfo *piSSInfo, const char* pData, int n32DataLength);
	//////////////////////////////////////////////////////////////////////////
	INT32					PostMsgToSS_NotifyNPCCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifyHeroCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifyGoodsCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifyShopCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifyRobotAICfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifyRobotAIPathCfgList(ICSSSInfo *piSSInfo);

	INT32					PostMsgToSS_NotifyNewSkillCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelAccountCfgCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelLeadingCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelEmitCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelRangeCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelBufCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelSummonCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelMoveCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelSwitchCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelPurificationCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifySkillModelLinkCfgList(ICSSSInfo *piSSInfo);

	INT32					PostMsgToSS_NotifyPassitiveSkillCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifyPassitiveEffectBloodSeekCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifyPassitiveEffectReboundCfgList(ICSSSInfo *piSSInfo);
	INT32					PostMsgToSS_NotifyPassitiveEffectReliveCfgList(ICSSSInfo *piSSInfo);

	INT32					PostMsgToSS_NotifyGameLogSwitch(ICSSSInfo *piSSInfo);
};

}
