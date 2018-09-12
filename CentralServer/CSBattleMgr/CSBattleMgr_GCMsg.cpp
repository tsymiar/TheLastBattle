// CSBattleMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSBattleMgr.h"
#include "CSBattle.h"

#include "CSGameLogMgr.h"
#include "CSUserMgr.h"
#include "CSKernel.h"

namespace CentralServer{

INT32	CCSBattleMgr::OnMsgFromGC_AskLeaveRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	INT32 n32AskRet = AskLeaveRoom(piUser);
	if (eNormal != n32AskRet){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskLeaveRoom, n32AskRet);
	}
	else{
		GSToGC::LeaveBattleSuccess sMsg;
		return piUser->PostMsgToGC(sMsg, sMsg.msgid());
	}
	return eNormal;
}

INT32	CCSBattleMgr::OnMsgFromGC_AskChangeRoomSeat(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskChangeRoomSeat> pMsg = ParseProtoMsg<GCToCS::AskChangeRoomSeat>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	INT32 n32AskRet = AskChangeBattleSeat(piUser, pMsg->newpos());
	if (eNormal != n32AskRet){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskQuickBattle, n32AskRet);
	}
	return eNormal;
}

INT32	CCSBattleMgr::OnMsgFromGC_AskReadyRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	INT32 n32AskRet = AskReadyBattle(piUser,true);
	if (eNormal != n32AskRet){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskReadyRoom, n32AskRet);
	}
	return eNormal;
}

INT32	CCSBattleMgr::OnMsgFromGC_AskCancelRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	INT32 n32AskRet = AskReadyBattle(piUser,false);
	if (eNormal != n32AskRet){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskCancelRoom, n32AskRet);
	}
	return eNormal;
}

INT32	CCSBattleMgr::OnMsgFromGC_AskStartRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	INT32 n32AskRet = AskStartBattle(piUser); 
	if (eNormal != n32AskRet){ 
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskStartRoom, n32AskRet);
	}
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskChatInRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskChatInRoom> pMsg = ParseProtoMsg<GCToCS::AskChatInRoom>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	INT32 n32AskRet = AskChatInRoom(piUser,(char*)pMsg->chat().c_str(),pMsg->chat().size());
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskCreateRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskCreateRoom> pMsg = ParseProtoMsg<GCToCS::AskCreateRoom>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	INT32 n32AskRet  = AskCreateRoom(piUser, pMsg->mapid(), pMsg->passwd(),false);
	if (eNormal != n32AskRet){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskCreateRoom, n32AskRet);
	}
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskRoomList(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	INT32 n32AskRet  = AskRoomList(piUser);
	if (eNormal != n32AskRet){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskRoomList, n32AskRet);
	}
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskAddRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	boost::shared_ptr<GCToCS::AskAddRoom> pMsg = ParseProtoMsg<GCToCS::AskAddRoom>(pData, n32DataLength);
	if (!pMsg){
		return 0;
	}
	INT32 n32AskRet  = AskAddRoom(piUser, pMsg->battleid(), pMsg->passwd(),false);
	if (eNormal != n32AskRet){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskAddRoom, n32AskRet);
		AskRoomList(piUser);
	}
	return eNormal;
}

INT32 CCSBattleMgr::OnMsgFromGC_AskReEnterRoom(ICSGSInfo *piGSInfo, INT32 n32GCNSID, const char* pData, int n32DataLength){
	CheckGetUserByNetInfo(piGSInfo, n32GCNSID);
	INT32 n32AskRet  = AskReEnterRoom(piUser);
	if (eNormal != n32AskRet){
		piUser->PostMsgToGC_AskRetMsg(GCToCS::eMsgToGSToCSFromGC_AskReEnterRoom, n32AskRet);
	}
	return eNormal;
}



} 
