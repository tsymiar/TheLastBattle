#include "ClientSession.h"
#include "..\Share\Net\INetSessionMgr.h"
#include "..\LogHandler.h"

CMsgHandle_Client CClientSession::mHandles;

CMsgHandle_Client::CMsgHandle_Client():IMsgHandle(ToLog::eMsgToLog_Begin, ToLog::eMsgToLog_End)
{
	
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
	ELOG(LOG_INFO, "");
}

void CClientSession::OnClose()
{
	ELOG(LOG_INFO, "");
}

bool CClientSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	ELOG(LOG_INFO, "");
	vthis->SetInited(true,true);
	return true;
}

bool CClientSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	ELOG(LOG_DBBUG, "");
	if (n32MsgID == ToLog::eMsgToLog_WriteLog){
		CLogHandler::GetInstance().HandleLogMsg(pMsg, n32MsgLength);
	}
	return true;
}