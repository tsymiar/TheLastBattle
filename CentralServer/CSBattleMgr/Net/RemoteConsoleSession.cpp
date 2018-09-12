#include "../stdafx.h"
#include "RemoteConsoleSession.h"

using namespace CentralServer;
#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\CSKernel.h"
#include "../../ThirdFunc/MsgLists/RCToCS.pb.h"
#include "../../ThirdFunc/MsgLists/CSToRC.pb.h"
#include "../CSKernel.h"

CMsgHandle_RemoteConsole CRemoteConsoleSession::mHandles;

CMsgHandle_RemoteConsole::CMsgHandle_RemoteConsole()
	:IMsgHandle(RCToCS::eMsgRC2CS_Begin,RCToCS::eMsgRC2CS_End-RCToCS::eMsgRC2CS_Begin)
{
	RegisterMsgFunc(RCToCS::eMsgRC2CS_AskRegist, CRemoteConsoleSession::Msg_Handle_Init,true);
	SetUnKnownMsgHandle(CRemoteConsoleSession::Msg_Handle_Dispath);
}

CRemoteConsoleSession::CRemoteConsoleSession()
{

}

CRemoteConsoleSession::~CRemoteConsoleSession()
{

}

void CRemoteConsoleSession::SendInitData()
{
	CSToRC::AskRegistRsp eMsg;
	eMsg.set_rst(true);
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_CS_OnlyRC, GetID(), eMsg, eMsg.msgid());
}

void CRemoteConsoleSession::OnRealEstablish(){
	CCSRCInfo *pcRCInfo = GetCSKernelInstance()->GetCRCInfoByNSID(GetID());
	if (pcRCInfo){
		ELOG(LOG_INFO,"RC(%u) Connected",pcRCInfo->GetNSID());
	}
}

void CRemoteConsoleSession::OnClose()
{
	CCSRCInfo *pcRCInfo = GetCSKernelInstance()->GetCRCInfoByNSID(GetID());
	if (pcRCInfo){
		ELOG(LOG_INFO,"RC(%u) DisConnected",pcRCInfo->GetNSID());
		pcRCInfo->m_eNetState = eSNS_Closed;
		pcRCInfo->m_n32NSID = 0;
	}
}

bool CRemoteConsoleSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	boost::shared_ptr<RCToCS::AskRegist> eAskMsg = ParseProtoMsg<RCToCS::AskRegist>(pMsg, n32MsgLength);
	if (!eAskMsg){
		return 0;
	}
	//ELOG(LOG_INFO,"==from gm tool Regist msg==");
	const string key = eAskMsg->key();
	if(key == GetCSKernelInstance()->m_szRemoteConsolekey){
		CCSRCInfo *pcRCInfo = GetCSKernelInstance()->GetCRCInfoByNSID(vthis->GetID());
		if(NULL == pcRCInfo){
			pcRCInfo = GetCSKernelInstance()->GetEmptyCRCInfo();
			if (pcRCInfo){
				pcRCInfo->m_eNetState = eSNS_Free;
				pcRCInfo->m_n32NSID = vthis->GetID();
				CSToRC::AskRegistRsp eMsg;
				eMsg.set_rst(true);
				INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_CS_OnlyRC, vthis->GetID(), eMsg, eMsg.msgid());
			}
			else{
				Assert(false && "so many remote client connected...");
			}
		}
	}
	else{
		CSToRC::AskRegistRsp eMsg;
		eMsg.set_rst(false);
		INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_CS_OnlyRC, vthis->GetID(), eMsg, eMsg.msgid());
		vthis->SetInited(true, true);
	}

	return true;
}

bool CRemoteConsoleSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	CCSRCInfo *pcRCInfo = GetCSKernelInstance()->GetCRCInfoByNSID(vthis->GetID());
	if (pcRCInfo)
		GetCSKernelInstance()->HandleMsgFromRC(pcRCInfo, pMsg, n32MsgLength, n32MsgID);
	return true;
}
