#include "stdafx.h"
#include "M2BSession.h"

#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\GSKernel.h"
#include "../../ThirdFunc/MsgLists/BSToGS.pb.h"
#include "../../ThirdFunc/MsgLists/GSToBS.pb.h"

CMsgHandle_M2B CM2BSession::mHandles;

CMsgHandle_M2B::CMsgHandle_M2B()
	:IMsgHandle(BSToGS::eMsgToGSFromBS_Begin, BSToGS::eMsgToGSFromBS_End-BSToGS::eMsgToGSFromBS_Begin)
{
	RegisterMsgFunc(BSToGS::eMsgToGSFromBS_AskRegisterRet, CM2BSession::Msg_Handle_Init, true);
	RegisterMsgFunc(BSToGS::eMsgToGSFromBS_OneUserLoginToken,  CM2BSession::Msg_Handle_OneUserLoginToken);
	SetUnKnownMsgHandle(CM2BSession::Msg_Handle_Dispath);
}

CM2BSession::CM2BSession()
{

}

CM2BSession::~CM2BSession()
{

}

void CM2BSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	ELOG(LOG_INFO,"BS Connected, try to register me.");
	GSToBS::AskRegister askRegister;
	askRegister.set_gsid(GateServer::CGSKernel::GetStaticGSKernel().GetGSConfig()->n32GSID);
	askRegister.set_listenport(GateServer::CGSKernel::GetStaticGSKernel().GetGSConfig()->n32GCListenPort);
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_CLIENT_G2B, 0, askRegister, askRegister.mgsid());
}

void CM2BSession::OnRealEstablish()
{
	ELOG(LOG_INFO,"BS Connected and register ok.");
}

void CM2BSession::OnClose()
{
	ELOG(LOG_INFO,"BS DisConnect.");
}

bool CM2BSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	vthis->SetInited(true,true);
	return true;
}

bool CM2BSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	return true;
}

bool CM2BSession::Msg_Handle_OneUserLoginToken(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	boost::shared_ptr<BSToGS::OneUserLoginToken> pReportAllClientInf = ParseProtoMsg<BSToGS::OneUserLoginToken>(pMsg, n32MsgLength);
	if (!pReportAllClientInf){
		return 0;
	}

	GateServer::CGSKernel::GetStaticGSKernel().AddUserToken(pReportAllClientInf->user_name(), pReportAllClientInf->token());
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_CLIENT_G2B, 0, *pReportAllClientInf, GSToBS::eMsgToBSFromGS_OneUserLoginTokenRet);
	return true;
}
