#include "stdafx.h"
#include "M2SSession.h"

#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\GSKernel.h"

CMsgHandle_M2S CM2SSession::mHandles;
using namespace GateServer;
CMsgHandle_M2S::CMsgHandle_M2S()
	:IMsgHandle(SSToGS::eMsgToGSFromSS_Begin, SSToGS::eMsgToGSFromSS_End-SSToGS::eMsgToGSFromSS_Begin)
{
	RegisterMsgFunc(SSToGS::eMsgToGSFromSS_AskRegisteRet, CM2SSession::Msg_Handle_Init, true);
	SetUnKnownMsgHandle(CM2SSession::Msg_Handle_Dispath);
}

CM2SSession::CM2SSession()
{

}

CM2SSession::~CM2SSession()
{

}

void CM2SSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	CGSKernel::GetStaticGSKernel().m_n32SuccessSSConnectNum++;
	CGSSSInfo *pcSSInfo = CGSKernel::GetStaticGSKernel().GetGSSSInfoBySSID(GetLogicID());
	if (pcSSInfo==NULL) {
		ELOG(LOG_ERROR, "");
		return;
	}
	ELOG(LOG_INFO,"SS(%u) Connected, try to register me.", pcSSInfo->GetSSID());
	pcSSInfo->m_n32NSID = GetID();
	GSToSS::AskRegiste sMsg;
	sMsg.set_gsid(CGSKernel::GetStaticGSKernel().GetGSConfig()->n32GSID);
	sMsg.set_pwd(CGSKernel::GetStaticGSKernel().GetGSConfig()->aszMyUserPwd, strlen(CGSKernel::GetStaticGSKernel().GetGSConfig()->aszMyUserPwd));
	string sData = sMsg.SerializeAsString();
	CGSKernel::GetStaticGSKernel().TransToSS(pcSSInfo,sData.c_str(),sData.size(),sMsg.msgid(),0,0);
}

void CM2SSession::OnRealEstablish()
{
	CGSSSInfo *pcSSInfo = CGSKernel::GetStaticGSKernel().GetGSSSInfoBySSID(GetLogicID());
	if (pcSSInfo==NULL) {
		ELOG(LOG_ERROR, "");
		return;
	}
	ELOG(LOG_INFO,"SS(%u) Connected and register ok.",pcSSInfo->GetSSID());
	pcSSInfo->m_n32NSID = GetID();//网络对象知道逻辑对象，现在让逻辑对象知道网络对象//
	pcSSInfo->m_tLastConnMilsec = GetMiliSecond();
	pcSSInfo->m_tPingTickCounter = 0;
}

void CM2SSession::OnClose(){
	CGSSSInfo *pcSSInfo = CGSKernel::GetStaticGSKernel().GetGSSSInfoBySSID(GetLogicID());
	if (pcSSInfo==NULL) {
		ELOG(LOG_ERROR, "");
		return;
	}
	ELOG(LOG_INFO,"SS(%u) DisConnect.",pcSSInfo->GetSSID());
	CGSKernel::GetStaticGSKernel().OnEvent(EVT_ON_SS_DISCONNECT, pcSSInfo);
	pcSSInfo->m_n32NSID = 0;//逻辑对象不再知道网络对象//
}

bool CM2SSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	int n32RealMsgID = *(int*)(pMsg);
	int n32GcNetID = *(int*)(pMsg+4);
	pMsg += 2*sizeof(int);
	n32MsgLength -= 2*sizeof(int);
	// don't send any message until it init success.
	CGSSSInfo *pcSSInfo = CGSKernel::GetStaticGSKernel().GetGSSSInfoBySSID(vthis->GetLogicID());
	if (pcSSInfo==NULL || pMsg==NULL) {
		ELOG(LOG_ERROR, "");
		return false;
	}

	boost::shared_ptr<SSToGS::AskRegisteRet> pAskRegisteRet = ParseProtoMsg<SSToGS::AskRegisteRet>(pMsg, n32MsgLength);
	if (!pAskRegisteRet){
		return 0;
	}

	if (eNormal != pAskRegisteRet->state()){		
		ELOG(LOG_WARNNING, "register to SS %d Fail with error code %d.", pcSSInfo->GetSSID(), pAskRegisteRet->state());
		return false;
	}

	pcSSInfo->m_eSSNetState = eSNS_Free;
	ELOG(LOG_INFO, "register to SS %d success at session %d.", pcSSInfo->GetSSID(), pcSSInfo->GetNSID());
	vthis->SetInited(true,true);

	return true;
}

bool CM2SSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	int n32RealMsgID = *(int*)(pMsg);
	int n32GcNetID = *(int*)(pMsg+4);
	pMsg += 2*sizeof(int);
	n32MsgLength -= 2*sizeof(int);
	CGSSSInfo *pcSSInfo = CGSKernel::GetStaticGSKernel().GetGSSSInfoBySSID(vthis->GetLogicID());
	if (pcSSInfo){
		CGSKernel::GetStaticGSKernel().HandleMsgFromSS(pcSSInfo, pMsg, n32MsgLength, n32RealMsgID, n32MsgID, n32GcNetID);
	}
	return true;
}
