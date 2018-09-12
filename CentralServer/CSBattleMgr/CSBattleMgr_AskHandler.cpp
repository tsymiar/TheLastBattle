// CSBattleMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSBattle.h"
#include "CSBattleMgr.h"
 
#include "CSGameLogMgr.h"
#include "CSCfgMgr.h"
#include "CSKernel.h"
#include "CSUser.h"
#include "CSUserMgr.h"

using namespace std;

namespace CentralServer{
	
INT32	CCSBattleMgr::RemoveMatchUser(CCSUser *piUser){
	if (NULL == piUser) return eEC_NullUser;
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Match) return eEC_JustInBattle;
	m_pMatchMgr->TeamStopMatch(piUser->GetMatchPlayer());
	bool bRet = m_pMatchMgr->UserRemoveTeam(piUser->GetMatchPlayer());
	if (!bRet) return eEC_JustInBattle;
	piUser->GetUserBattleInfoEx().Clear(3);

	return eNormal;
}

void CCSBattleMgr::OnBattleMached(CCSUserListMap& pListMap, map<UINT32,UINT32>* aiRobots, UINT32 mapID, EBattleMatchType type){
	ICSSSInfo* piSSInfo = GetCSKernelInstance()->GetMiniBattleNumSS();
	if (piSSInfo){
		CCSBattle* pBattle = new CCSBattle(type, eBattleType_Match,piSSInfo->GetSSID(),GenerateBattleID(),mapID,pListMap,aiRobots);
		m_cAllBattleMap.insert(make_pair(pBattle->GetBattleId(), pBattle));
		pBattle->Start();
		piSSInfo->AddBattleNum(1);
	}
	else{
		for (auto iter = pListMap.begin(); iter != pListMap.end(); ++iter){
			for (auto itUser = iter->second.Begin(); itUser != iter->second.End(); itUser = iter->second.Next()){
				CCSUser* pUser = *itUser;
				Assert(pUser->GetUserBattleInfoEx().GetBattleType()==eBattleType_Match);
				Assert(pUser->GetUserBattleInfoEx().GetBattleState()==eBattleState_Wait);
				pUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskQuickBattle, eEC_NullSceneServer);
			}
		}
	}
}

const UINT32 CCSBattleMgr::GetBattleMapID(UINT64 un64BattleID)
{
	CCSBattle *pBattle = GetBattle(un64BattleID);
	if (NULL == pBattle){
		return NULL;
	}
	return pBattle->GetBattleMapId();
}


INT32 CCSBattleMgr::AskRoomList(CCSUser *piUser)
{
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Free){
		return eEC_JustInBattle;
	}
	GSToGC::AskRoomListRet sMsg;
	CCSBattleRoom* pRoom;
	for (auto it=m_BattleRoomList.begin();it!=m_BattleRoomList.end();++it){
		pRoom = it->second;
		GSToGC::RoomInfo* pRoomInfo = sMsg.add_roomlist();
		pRoomInfo->set_roomid(pRoom->GetID());
		pRoomInfo->set_ifpwd(pRoom->IsHasPWD());
		pRoomInfo->set_mapid(pRoom->GetMapID());
		pRoomInfo->set_master(pRoom->GetMasterNickName());
		pRoomInfo->set_curusercount(pRoom->GetCurUserNum());
		pRoomInfo->set_maxusercount(pRoom->GetMaxUserNum());
	}
	piUser->PostMsgToGC(sMsg, sMsg.msgid());
	return eNormal;
}

INT32 CCSBattleMgr::AskCreateRoom(CCSUser *piUser, UINT32 un32MapId, const string& stPwd, bool isRestart, UINT32 rePostion/*=0*/)
{
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Free){
		return eEC_JustInBattle;
	}
	set<UINT32>& lMapCfdMap = CCSCfgMgr::getInstance().GetMapCfgMap();
	set<UINT32>::iterator mapIter = lMapCfdMap.find(un32MapId);
	if (lMapCfdMap.end() == mapIter){
		return eEC_InvalidMapID;
	}
	SMapLogicCfg* pLogicCfg = CCSCfgMgr::getInstance().GetMapLogicCfg(un32MapId);
	if (pLogicCfg==NULL){
		return eEC_InvalidMapID;
	}
	if (stPwd.length()>c_n32MAxBattlePwdLen){
		return eEC_InvalidPwdLength;
	}
	CCSBattleRoom* pBattleRoom = new CCSBattleRoom(un32MapId,stPwd,6);
	if (pBattleRoom==NULL){
		return eEC_UnKnownError;
	}
	m_BattleRoomList.insert(make_pair(pBattleRoom->GetID(),pBattleRoom));
	if (!pBattleRoom->AddPlayer(piUser->GetRoomPlayer(),0,isRestart,rePostion)){
		m_BattleRoomList.erase(pBattleRoom->GetID());
		delete pBattleRoom;
		return eEC_UnKnownError;
	}
	piUser->GetUserBattleInfoEx().ChangeTypeWithState(eBattleType_Room,eBattleState_Wait);
	//log
	CSSGameLogMgr::GetInstance().AddGameLog(eLog_CreateHouse,piUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,pBattleRoom->GetID(),un32MapId);
	return eNormal;
}

INT32 CCSBattleMgr::AskAddRoom(CCSUser *piUser, UINT64 un32RoomId, const string& stPwd, bool isRestart, UINT32 rePostion/*=0*/)
{
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Free){
		return eEC_JustInBattle;
	}
	map<UINT32,CCSBattleRoom*>::iterator it = m_BattleRoomList.find(un32RoomId);
	if (it==m_BattleRoomList.end()){
		return eEC_NullBattle;
	}
	CCSBattleRoom* pBattleRoom = it->second;
	if (pBattleRoom->IsHasPWD()){
		if (stPwd.empty()){
			return eEC_PleaseEnterPwd;
		}
		if (!pBattleRoom->IsPWDMatch(stPwd)){ 
			return eEC_BattlePDWNotMatch;
		}
	}
	if (!pBattleRoom->AddPlayer(piUser->GetRoomPlayer(),0,isRestart,rePostion)){
		return eEC_AddBattleFailForAllFull;
	}
	piUser->GetUserBattleInfoEx().ChangeTypeWithState(eBattleType_Room,eBattleState_Wait);
	//log
	CSSGameLogMgr::GetInstance().AddGameLog(eLog_AddHouse,piUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,un32RoomId);
	return eNormal;
}

INT32 CCSBattleMgr::AskLeaveRoom(CCSUser *piUser)
{
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Room){
		return eEC_JustNotInBattle;
	}
	map<UINT32,CCSBattleRoom*>::iterator it = m_BattleRoomList.find(piUser->GetRoomPlayer()->m_RoomID);
	if (it==m_BattleRoomList.end()){
		return eEC_NullBattle;
	}
	CCSBattleRoom* pBattleRoom = it->second;
	if (!pBattleRoom->RemoverPlayer(piUser->GetRoomPlayer())){
		return eEC_JustNotInBattle;
	}
	piUser->GetUserBattleInfoEx().Clear(5);
	return eNormal;
}

INT32 CCSBattleMgr::AskChangeBattleSeat(CCSUser *piUser, INT32 n32TarPos)
{
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Room){
		return eEC_JustNotInBattle;
	}
	map<UINT32,CCSBattleRoom*>::iterator it = m_BattleRoomList.find(piUser->GetRoomPlayer()->m_RoomID);
	if (it==m_BattleRoomList.end()){
		return eEC_NullBattle;
	}
	CCSBattleRoom* pBattleRoom = it->second;
	if (!pBattleRoom->ChangeSeat(piUser->GetRoomPlayer(),n32TarPos)){
		return eEC_UnKnownError;
	}
	return eNormal;
}

INT32 CCSBattleMgr::AskReadyBattle(CCSUser *piUser,bool isReady)
{
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Room){
		return eEC_JustNotInBattle;
	}
	map<UINT32,CCSBattleRoom*>::iterator it = m_BattleRoomList.find(piUser->GetRoomPlayer()->m_RoomID);
	if (it==m_BattleRoomList.end()){
		return eEC_NullBattle;
	}
	CCSBattleRoom* pBattleRoom = it->second;
	if (!pBattleRoom->ReadyCancel(piUser->GetRoomPlayer(),isReady)){
		return eEC_JustNotInBattle;
	}
	
	return eNormal;
}

CCSBattleRoom*	CCSBattleMgr::GetRoom(UINT64 roomid)
{
	map<UINT32, CCSBattleRoom*>::iterator iter = m_BattleRoomList.find(roomid);
	if (iter != m_BattleRoomList.end()){
		return iter->second;
	}
	else
		return NULL;
}

INT32 CCSBattleMgr::AskChatInRoom( CCSUser *piUser, const char* chatContent, UINT32 charLen )
{
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Room){
		return eEC_JustNotInBattle;
	}
	map<UINT32,CCSBattleRoom*>::iterator it = m_BattleRoomList.find(piUser->GetRoomPlayer()->m_RoomID);
	if (it==m_BattleRoomList.end()){
		return eEC_NullBattle;
	}
	CCSBattleRoom* pBattleRoom = it->second;
	if (!pBattleRoom->Chat(piUser->GetRoomPlayer(),chatContent,charLen)){
		return eEC_JustNotInBattle;
	}
	//log
	CSSGameLogMgr::GetInstance().AddGameLog(eLog_Chat,piUser->GetUserDBData().sPODUsrDBData.un64ObjIdx,1);
	return eNormal;
}

INT32 CCSBattleMgr::AskReEnterRoom(CCSUser *piUser)
{
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Room){
		return eEC_ReEnterRoomFail;
	}
	map<UINT32,CCSBattleRoom*>::iterator it = m_BattleRoomList.find(piUser->GetRoomPlayer()->m_RoomID);
	if (it==m_BattleRoomList.end()){
		return eEC_ReEnterRoomFail;
	}
	CCSBattleRoom* pBattleRoom = it->second;
	if (!pBattleRoom->RestartEnter(piUser->GetRoomPlayer())){
		return eEC_ReEnterRoomFail;
	}
	return eNormal;
}

INT32 CCSBattleMgr::AskStartBattle(CCSUser *piUser)
{
	if (piUser->GetUserBattleInfoEx().GetBattleType()!=eBattleType_Room){
		return eEC_UnKnownError;
	}
	map<UINT32,CCSBattleRoom*>::iterator it = m_BattleRoomList.find(piUser->GetRoomPlayer()->m_RoomID);
	if (it==m_BattleRoomList.end()){
		return eEC_UnKnownError;
	}
	CCSBattleRoom* pBattleRoom = it->second;
	INT32 ret = pBattleRoom->Start(piUser->GetRoomPlayer());
	//LOG
	{
		pBattleRoom->GetRommUserInfoLog();
	}
	return ret;
}

INT32 CCSBattleMgr::OnBattleRoomStart( CCSBattleRoom* pRoom, CCSUserListMap& sListMap, map<UINT32,UINT32>* aiRobots, UINT32 mapID )
{
	ICSSSInfo* piSSInfo = GetCSKernelInstance()->GetMiniBattleNumSS();
	if (piSSInfo){
		CCSBattle* pBattle = new CCSBattle(eMatchType_None, eBattleType_Room, piSSInfo->GetSSID(),GenerateBattleID(),mapID, sListMap, aiRobots);
		pBattle->SetPWD(pRoom->GetPwd().c_str());
		m_cAllBattleMap.insert(make_pair(pBattle->GetBattleId(),pBattle));
		pBattle->Start();
		piSSInfo->AddBattleNum(1);
		ELOG(LOG_SpecialDebug,"房间(%u)因战斗而结束",pRoom->GetID());
		m_BattleRoomList.erase(pRoom->GetID());
		delete pRoom;
		pRoom = NULL;
		return eNormal;
	}
	return eEC_InvalidSSID;
}

void CCSBattleMgr::OnBattleRoomAbort(CCSBattleRoom* pRoom)
{
	ELOG(LOG_SpecialDebug,"房间(%u)无房主而中止",pRoom->GetID());
	m_BattleRoomList.erase(pRoom->GetID());
	delete pRoom;
	pRoom = NULL;
}

}