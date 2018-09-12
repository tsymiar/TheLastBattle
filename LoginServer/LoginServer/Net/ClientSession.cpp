#include "../stdafx.h"
#include "ClientSession.h"
#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "../SDKAsynHandler.h"
#include "../SdkConnector.h"

CMsgHandle_Client CClientSession::mHandles;

CMsgHandle_Client::CMsgHandle_Client()
	:IMsgHandle(GCToLS::eMsgToLSFromGC_Begin, GCToLS::eMsgToLSFromGC_End-GCToLS::eMsgToLSFromGC_Begin)
{
	RegisterMsgFunc(GCToLS::eMsgToLSFromGC_AskLogin, CClientSession::Msg_Handle_Init, true);
	SetUnKnownMsgHandle(CClientSession::Msg_Handle_Dispath);
}

CClientSession::CClientSession()
{
	mLogicInited = false;
}

CClientSession::~CClientSession()
{

}

void CClientSession::SendInitData()
{

}

void CClientSession::OnRealEstablish()
{
}

void CClientSession::OnClose()
{
}

bool CClientSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	// 收到第1消息：请求登录，放入登录队列
	boost::shared_ptr<GCToLS::AskLogin> sLogin = ParseProtoMsg<GCToLS::AskLogin>(pMsg, n32MsgLength);
	if (!sLogin){
		//ELOG(LOG_ERROR, "Login Fail With Msg Analysis Error.");
		SDKAsynHandler::GetInstance().PostToLoginFailQueue(eEC_TBInvalidToken, vthis->GetID());
		return 0;
	}

	SDKAsynHandler::GetInstance().CheckLogin(*sLogin, vthis->GetID());
	vthis->SetInited(true,true);
	return true;
}

bool CClientSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	return true;
}