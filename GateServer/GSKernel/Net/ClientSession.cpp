#include "stdafx.h"
#include "ClientSession.h"
#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\GSKernel.h"

CMsgHandle_Client CClientSession::mHandles;

CMsgHandle_Client::CMsgHandle_Client()
	:IMsgHandle(GCToCS::eMsgToGSToCSFromGC_Begin,GCToCS::eMsgToGSToCSFromGC_End-GCToCS::eMsgToGSToCSFromGC_Begin)
{
	RegisterMsgFunc(GCToCS::eMsgToGSToCSFromGC_Begin, CClientSession::Msg_Handle_Init,true);
	SetUnKnownMsgHandle(CClientSession::Msg_Handle_Dispath);
}

CClientSession::CClientSession()
{
	mLogicInited = false;
	SetSendBufferMaxSize(10*1024);//·¢ËÍ»º´æÎª10K//
}

CClientSession::~CClientSession()
{

}

void CClientSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
}

void CClientSession::OnRealEstablish()
{
	if (!mLogicInited)
	{
		++gProfileNet.mClientCount;
		mLogicInited = true;
		ELOG(LOG_DBBUG,"client(%d)(%s:%d) connected.", GetID(), GetRemoteIp(), GetRemotePort());
	}
}

void CClientSession::OnClose()
{
	if (mLogicInited)
	{
		ELOG(LOG_DBBUG,"client(%d)(%s:%d) disconnected.", GetID(), GetRemoteIp(), GetRemotePort());
		--gProfileNet.mClientCount;
		GateServer::CGSKernel::GetStaticGSKernel().OnUserLost(GetID());
		mLogicInited = false;
	}
}

void UCAPI CClientSession::SetConnection(ISDConnection* poConnection)
{
	INetSession::SetConnection(poConnection);
	SSendStrategy sendopt;
	sendopt.dwSendBuffFullDelayInMilliSec = 0;
	sendopt.dwSendBuffFullRetryCountBeforeDisconnect = 0;
	poConnection->SetOpt(SEND_STRATEGY,&sendopt);
}

bool CClientSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	vthis->SetInited(true,true);
	return true;
}

bool CClientSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	CClientSession* pCliSession = (CClientSession*)vthis;
	if (!pCliSession->mLogicInited){
		pCliSession->SetInited(true,true);
	}
	GateServer::CGSKernel::GetStaticGSKernel().HandleMsgFromGC(pCliSession->GetID(), pMsg, n32MsgLength, n32MsgID);
	return true;
}
