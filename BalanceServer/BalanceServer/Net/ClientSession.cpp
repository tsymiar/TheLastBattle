#include "../stdafx.h"
#include "ClientSession.h"
#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "../ThirdFunc/MsgLists/BSToGC.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "../ThirdFunc/MsgLists/BSToGC.pb.h"
#include "../ThirdFunc/MsgLists/GSToBS.pb.h"
#include "../ThirdFunc/MsgLists/GCToBS.pb.h"

CMsgHandle_Client CClientSession::mHandles;

CMsgHandle_Client::CMsgHandle_Client()
	:IMsgHandle(GCToBS::eMsgToBSFromGC_Begin, GCToBS::eMsgToBSFromGC_End-GCToBS::eMsgToBSFromGC_Begin){
	RegisterMsgFunc(GCToBS::eMsgToBSFromGC_OneClinetLogin, CClientSession::MSG_OneClientLogin);
	SetUnKnownMsgHandle(CClientSession::Msg_Handle_Dispath);
}

CClientSession::CClientSession(){
	mLogicInited = false;
}

CClientSession::~CClientSession(){

}

void CClientSession::SendInitData(){
}

void CClientSession::OnRealEstablish(){
}

void CClientSession::OnClose(){
}

bool CClientSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	vthis->SetInited(true,true);
	return true;
}

bool CClientSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	return true;
}

bool CClientSession::MSG_OneClientLogin( const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID )
{
	// 收到第2消息：客户端连接bs，bs向ls请求用户是否合法连接
	std::auto_ptr<GCToBS::OneClinetLogin> sOneClientLogin(new GCToBS::OneClinetLogin);

	bool bRet = sOneClientLogin->ParseFromArray(pMsg,n32MsgLength);

	if (!bRet){
		ELOG(LOG_ERROR, "Fail with Msg Error.");
		return true;
	}

	ELOG(LOG_SpecialDebug,"user(%s)(%s)(%u) ask login bs",sOneClientLogin->uin().c_str(),sOneClientLogin->sessionid().c_str(),vthis->GetID());
	sOneClientLogin->set_nsid(vthis->GetID());

	INetSessionMgr::GetInstance()->SendMsgToSession(ST_CLIENT_B2L, 0, *sOneClientLogin, BSToLS::eMsgToLSFromBC_OneClinetLoginCheck);

	return true;
}
