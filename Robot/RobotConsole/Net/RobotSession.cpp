#include "stdafx.h"
#include "RobotSession.h"
#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\RobotMgr.h"
using namespace ReBot;

CMsgHandle_Central CRobotSession::mHandles;

CMsgHandle_Central::CMsgHandle_Central()
	:IMsgHandle(CSToSS::eMsgToSSFromCS_Begin,CSToSS::eMsgToSSFromCS_End-CSToSS::eMsgToSSFromCS_Begin)
{
	RegisterMsgFunc(CSToSS::eMsgToSSFromCS_AskRegisteRet,CRobotSession::Msg_Handle_Init,true); 
	SetUnKnownMsgHandle(CRobotSession::Msg_Handle_Dispath);
}

CRobotSession::CRobotSession()
{
	mLogicInited = false;
	mConnected = false;
	SetSendBufferMaxSize(10*1024);//»º´æ10K//
}

CRobotSession::~CRobotSession()
{

}

void CRobotSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	mLogicInited = false;
	mConnected = true;
	--CRobotMgr::m_n32ConnectingRobotNum;
	//ELOG(LOG_ERROR,"cur ping count(%u)",CRobotMgr::m_n32ConnectingRobotNum);
	CCClient* pcRobot = gRobotMgr->GetRobotByID(GetLogicID());
	pcRobot->OnNetConnected(GetID());
}

void CRobotSession::OnRealEstablish()
{
	mLogicInited = true;
	CCClient* pcRobot = gRobotMgr->GetRobotByID(GetLogicID());
	pcRobot->OnNetRealConnected(GetID());
}

void CRobotSession::OnClose()
{
	mLogicInited = false;
	if (!mConnected){
		--CRobotMgr::m_n32ConnectingRobotNum;
		//ELOG(LOG_ERROR,"cur ping count(%u)",CRobotMgr::m_n32ConnectingRobotNum);
	}
	CCClient* pcRobot = gRobotMgr->GetRobotByID(GetLogicID());
	pcRobot->OnNetDisconnected(GetID());
}

bool CRobotSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	vthis->SetInited(true,true);
	return true;
}

bool CRobotSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	CRobotSession* pNetSession = (CRobotSession*)vthis;
	if (!pNetSession->mLogicInited) pNetSession->SetInited(true,true);
	CCClient* pcRobot = gRobotMgr->GetRobotByID(vthis->GetLogicID());
	pcRobot->HandleMsg(vthis->GetID(), pMsg, n32MsgLength, n32MsgID);
	return true;
}
