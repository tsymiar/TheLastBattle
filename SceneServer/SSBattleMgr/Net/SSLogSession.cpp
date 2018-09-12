#include "../stdafx.h"
#include "SSLogSession.h"

#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\SSWorkThreadMgr.h"
using namespace SceneServer;
CMsgHandle_SSLog CSSLogSession::mHandles;

CMsgHandle_SSLog::CMsgHandle_SSLog()
	:IMsgHandle(ToLog::eMsgToLog_Begin, ToLog::eMsgToLog_End - ToLog::eMsgToLog_Begin)
{
	SetUnKnownMsgHandle(CSSLogSession::Msg_Handle_Dispath);
}

CSSLogSession::CSSLogSession()
{
}

CSSLogSession::~CSSLogSession()
{

}

void CSSLogSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	ELOG(LOG_INFO," LogServer Connected, try to register me.");
}

void CSSLogSession::OnRealEstablish(){
	ELOG(LOG_INFO, "Connected and register ok, try to ask global config.");
}

void CSSLogSession::OnClose(){
	ELOG(LOG_INFO, "DisConnect.");
}

bool CSSLogSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	vthis->SetInited(true,true);
	return true;
}

bool CSSLogSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	return true;
}
