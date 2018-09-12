#include "../stdafx.h"
#include "GateSession.h"

#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\SSWorkThreadMgr.h"
using namespace SceneServer;

CMsgHandle_Gate CGateSession::mHandles;

CMsgHandle_Gate::CMsgHandle_Gate()
	:IMsgHandle(GSToSS::eMsgToSSFromGS_Begin,GSToSS::eMsgToSSFromGS_End-GSToSS::eMsgToSSFromGS_Begin)
{
	RegisterMsgFunc(GSToSS::eMsgToSSFromGS_AskRegiste,  CGateSession::Msg_Handle_Init,true);
	SetUnKnownMsgHandle(CGateSession::Msg_Handle_Dispath);
}

CGateSession::CGateSession()
{

}

CGateSession::~CGateSession()
{

}

void CGateSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	SSToGS::AskRegisteRet sMsg;
	sMsg.set_state(eNormal);
	CSSWorkThreadMgr::GetInstance().PostMsgToGS(GetID(), sMsg, sMsg.mgsid());
}

void CGateSession::OnRealEstablish(){
	ISSGSInfo *pcGSInfo = CSSWorkThreadMgr::GetInstance().GetGSInfoByNSID(GetID());
	if (pcGSInfo){
		ELOG(LOG_INFO,"GS(%u) Connected.",pcGSInfo->GetGSID());
	}
}

void CGateSession::OnClose()
{
	ISSGSInfo *pcGSInfo = CSSWorkThreadMgr::GetInstance().GetGSInfoByNSID(GetID());
	if (pcGSInfo)
	{
		ELOG(LOG_INFO,"GS(%u) DisConnect.",pcGSInfo->GetGSID());
	}
}

bool CGateSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	int n32RealMsgID = *(int*)(pMsg);
	int n32GcNetID = *(int*)(pMsg+4);
	pMsg += 2*sizeof(int);
	n32MsgLength -= 2*sizeof(int);
	INT32 n32RetFlag = CSSWorkThreadMgr::GetInstance().OnMsgFromGS_GSAskRegiste(vthis->GetID(), pMsg, n32MsgLength);
	if (n32RetFlag!=eNormal) return false;
	vthis->SetInited(true,true);
	return true;
}

bool CGateSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	int n32RealMsgID = *(int*)(pMsg);
	int n32GcNetID = *(int*)(pMsg+4);
	pMsg += 2*sizeof(int);
	n32MsgLength -= 2*sizeof(int);
	ISSGSInfo *pcGSInfo = CSSWorkThreadMgr::GetInstance().GetGSInfoByNSID(vthis->GetID());
	if (pcGSInfo && n32MsgID==GSToSS::eMsgToSSFromGS_ReportGCMsg){
		CSSWorkThreadMgr::GetInstance().SendGSMsgToLogicThread(pcGSInfo, n32GcNetID, pMsg, n32MsgLength, n32RealMsgID);
	}
	return true;
}
