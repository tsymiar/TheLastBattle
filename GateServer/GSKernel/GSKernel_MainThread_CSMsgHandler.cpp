#include "stdafx.h"
#include "GSKernel.h"
#include <iostream>
#include <fstream>
#include "GSSSInfo.h"

using namespace std;

namespace GateServer{

INT32	CGSKernel::OnMsgFromCS(const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID){
	if (n32TransID < CSToGS::eMsgToGSFromCS_Begin || n32TransID >= CSToGS::eMsgToGSFromCS_End){
		return eEC_InvalidMsgProtocalID;
	}
	if (n32TransID==CSToGS::eMsgToGSFromCS_OrderPostToGC){
		if (n32GcNetID==0){
			BroadcastToGameClient(pMsg,n32MsgLength,n32MsgID);
		}
		else{
			PostToGameClient(n32GcNetID,pMsg,n32MsgLength,n32MsgID);
		}
	}
	else{
		INT32 n32Index = n32TransID - CSToGS::eMsgToGSFromCS_Begin;
		PFCSMsgHandler rsHandler = m_asCSMsgHandler[n32Index];
		if ( NULL != rsHandler){
			return (this->*rsHandler)(pMsg, n32MsgLength);
		}
	}
	return eEC_NullMsgHandler;
}


INT32	CGSKernel::OnMsgFromCS_AskPingRet(const char* pMsg, int n32MsgLength){
	CSToGS::AskPing pPingRet;
	if (!ParseProtoMsg(pMsg, n32MsgLength, pPingRet)){
		return 0;
	}
	TIME_MILSEC tCurMilsec = GetMiliSecond();
	TIME_TICK tTickSpan = tCurMilsec - pPingRet.time();
	ELOG(LOG_INFO, "Ping CS returned, tick span %lld.", tTickSpan);

	return eNormal;
}

// 中心服务器要求网关开服，允许客户端链接
INT32	CGSKernel::OnMsgFromCS_OrderOpenListen(const char* pMsg, int n32MsgLength){
	INetSessionMgr::GetInstance()->CreateListener(m_sGSConfig.n32GCListenPort,10240,10240,0);
	return eNormal;
}

INT32	CGSKernel::OnMsgFromCS_OrderCloseListen(const char* pMsg, int n32MsgLength){
	INetSessionMgr::GetInstance()->StopListener(0);
	return eNormal;
}

INT32	CGSKernel::OnMsgFromCS_OrderKickoutGC(const char* pMsg, int n32MsgLength){
	CSToGS::OrderKickoutGC pOrderKickoutGC;
	if (!ParseProtoMsg(pMsg, n32MsgLength, pOrderKickoutGC)){
		return 0;
	}
	PostGameClientDisconnect(pOrderKickoutGC.gcnid());
	return eNormal;
}

INT32 CGSKernel::OnMsgFromCS_UserConnectedToSS(const char* pMsg, int n32MsgLength)
{
	CSToGS::UserConnectedSS pUserConnectedSS;
	if (!ParseProtoMsg(pMsg, n32MsgLength, pUserConnectedSS)){
		return 0;
	}

	CGSSSInfo* pSSInfo = GetGSSSInfoBySSID(pUserConnectedSS.ssid());
	if (NULL == pSSInfo){
		ELOG(LOG_ERROR, "pSSInfo is null with ssid(%d)", pUserConnectedSS.ssid());
		return eNormal;
	}

	for (INT32 i = 0; i < pUserConnectedSS.gcnid_size(); ++i){
		m_User2SSInfoMap[pUserConnectedSS.gcnid(i)] = pSSInfo;
		ELOG(LOG_DBBUG, "User NetID(%d) connect With SS(%d)", pUserConnectedSS.gcnid(i), pSSInfo->GetSSID());
	}
	return eNormal;
}

INT32 CGSKernel::OnMsgFromCS_UserDisConnectedToSS(const char* pMsg, int n32MsgLength)
{
	CSToGS::UserDisConnectedSS pUserConnectedSS;
	if (!ParseProtoMsg(pMsg, n32MsgLength, pUserConnectedSS)){
		return 0;
	}

	for (INT32 i = 0; i < pUserConnectedSS.gcnid_size(); ++i){
		INT32 gcNetId = pUserConnectedSS.gcnid(i);
		m_User2SSInfoMap.erase(gcNetId);
	}

	return eNormal;
}

}
