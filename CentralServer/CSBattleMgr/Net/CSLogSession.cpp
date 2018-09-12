#include "../stdafx.h"
#include "CSLogSession.h"

#include "..\..\..\Share\Net\INetSessionMgr.h"
CMsgHandle_CSLog CCSLogSession::mHandles;

CMsgHandle_CSLog::CMsgHandle_CSLog()
	:IMsgHandle(ToLog::eMsgToLog_Begin, ToLog::eMsgToLog_End - ToLog::eMsgToLog_Begin)
{
	SetUnKnownMsgHandle(CCSLogSession::Msg_Handle_Dispath);
}

CCSLogSession::CCSLogSession()
{
}

CCSLogSession::~CCSLogSession()
{

}

void CCSLogSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	ELOG(LOG_INFO," LogServer Connected, try to register me.");
}

void CCSLogSession::OnRealEstablish(){
	ELOG(LOG_INFO, "Connected and register ok, try to ask global config.");
}

void CCSLogSession::OnClose(){
	ELOG(LOG_INFO, "DisConnect.");
}

bool CCSLogSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	vthis->SetInited(true,true);
	return true;
}

bool CCSLogSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	return true;
}
