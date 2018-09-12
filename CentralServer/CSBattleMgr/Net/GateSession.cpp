#include "../stdafx.h"
#include "GateSession.h"

using namespace CentralServer;
#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\CSKernel.h"
#include "../../ThirdFunc/MsgLists/CSToGS.pb.h"
#include "../../ThirdFunc/MsgLists/GSToCS.pb.h"

CMsgHandle_Gate CGateSession::mHandles;

CMsgHandle_Gate::CMsgHandle_Gate()
	:IMsgHandle(GSToCS::eMsgToCSFromGS_Begin,GSToCS::eMsgToCSFromGS_End-GSToCS::eMsgToCSFromGS_Begin)
{
	RegisterMsgFunc(GSToCS::eMsgToCSFromGS_AskRegiste, CGateSession::Msg_Handle_Init, true);
	SetUnKnownMsgHandle(CGateSession::Msg_Handle_Dispath);
}

CGateSession::CGateSession()
{

}

CGateSession::~CGateSession()
{

}

void CGateSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	//发送消息//
	TIME_MILSEC tCurUTCMilsec = GetUTCMiliSecond();

	CSToGS::AskRegisteRet sMsg;
	sMsg.set_registe(eNormal);
	sMsg.set_curtime(tCurUTCMilsec);
	sMsg.set_ssbaseid(GetCSKernelInstance()->GetKernelCfg().un32SSBaseIdx);

	for (UINT32 i = 0; i < GetCSKernelInstance()->GetMaxSSNum(); i++)
	{
		CSToGS::AskRegisteRet::SSInfo* pSsinfo = sMsg.add_ssinfo();
		pSsinfo->set_ssid(GetCSKernelInstance()->m_pcSSInfoList[i].m_n32SSID);
		pSsinfo->set_ip((const char*)&GetCSKernelInstance()->m_pcSSInfoList[i].m_sListenIP, sizeof(GetCSKernelInstance()->m_pcSSInfoList[i].m_sListenIP));
		pSsinfo->set_port(GetCSKernelInstance()->m_pcSSInfoList[i].m_n32ListenPort);
		pSsinfo->set_netstate(GetCSKernelInstance()->m_pcSSInfoList[i].m_eSSNetState);
	}

	string sData = sMsg.SerializeAsString();
	INetSessionMgr::GetInstance()->TranMsgToSession(ST_SERVER_CS_OnlyGS, GetID(), sData.c_str(), sData.size(), sMsg.msgid(), 0, 0);
}

void CGateSession::OnRealEstablish(){
	CCSGSInfo *pcGSInfo = GetCSKernelInstance()->GetCGSInfoByNSID(GetID());
	if (pcGSInfo){
		ELOG(LOG_INFO,"GS(%u) Connected",pcGSInfo->GetGSID());
	}
}

void CGateSession::OnClose()
{
	CCSGSInfo *pcGSInfo = GetCSKernelInstance()->GetCGSInfoByNSID(GetID());
	if (pcGSInfo){
		ELOG(LOG_INFO,"GS(%u) DisConnected",pcGSInfo->GetGSID());
		UINT32 pos = pcGSInfo->GetGSID()-GetCSKernelInstance()->GetKernelCfg().un32GSBaseIdx;
		pcGSInfo->m_eGSNetState = eSNS_Closed;
		pcGSInfo->m_n32NSID = 0;
		pcGSInfo->m_tLastConnMilsec = 0;
		GetCSKernelInstance()->m_psGSNetInfoList[pos].tConnMilsec = 0;
		GetCSKernelInstance()->m_psGSNetInfoList[pos].pcGSInfo = NULL;
	}
}

bool CGateSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){

	int n32RealMsgID = *(int*)(pMsg);
	int n32GcNetID = *(int*)(pMsg+4);
	pMsg += 2*sizeof(int);
	n32MsgLength -= 2*sizeof(int);

	boost::shared_ptr<GSToCS::AskRegiste> sMsg = ParseProtoMsg<GSToCS::AskRegiste>(pMsg, n32MsgLength);
	if (!sMsg){
		return 0;
	}

	//找到位置号//
	INT32 gsPos = -1;
	CCSGSInfo *pcGSInfo = NULL;
	for (UINT32 i = 0; i < GetCSKernelInstance()->GetMaxGSNum(); i++){
		if (sMsg->gsid() == GetCSKernelInstance()->m_pcGSInfoList[i].m_n32GSID){
			pcGSInfo = &GetCSKernelInstance()->m_pcGSInfoList[i];
			gsPos = i;
			break;
		}
	}

	INT32 n32Registe = eNormal;
	if (NULL == pcGSInfo){
		n32Registe = eEC_InvalidGSID;
	}

	if (eNormal == n32Registe){
		INT32 n32Compare = strcmp(pcGSInfo->m_szUserPwd, sMsg->usepwd().c_str());
		if (0 != n32Compare){
			n32Registe = eEC_InvalidUserPwd;
		}
	}

	if (n32Registe!=eNormal) {
		vthis->DelayClose(111);
		return false;
	}

	// 加入GS
	TIME_MILSEC tCurUTCMilsec = GetUTCMiliSecond();
	pcGSInfo->m_eGSNetState = eSNS_Free;
	pcGSInfo->m_n32NSID = vthis->GetID();
	vthis->SetLogicID(pcGSInfo->GetGSID());
	pcGSInfo->m_tLastConnMilsec = GetUTCMiliSecond();
	memcpy(&pcGSInfo->m_sListenIP, sMsg->ip().c_str(), sMsg->ip().size());
	pcGSInfo->m_n32ListenPort = sMsg->port();
	GetCSKernelInstance()->m_psGSNetInfoList[gsPos].pcGSInfo = pcGSInfo;
	pcGSInfo->m_n64MsgReceived++;
	pcGSInfo->m_n64DataReceived += sMsg->ByteSize();
	pcGSInfo->m_un32ConnTimes++;
	pcGSInfo->m_tLastConnMilsec = tCurUTCMilsec;
	pcGSInfo->m_tLastPingMilSec = tCurUTCMilsec;

	vthis->SetInited(true,true);
	return true;
}

bool CGateSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	int n32RealMsgID = *(int*)(pMsg);
	int n32GcNetID = *(int*)(pMsg+4);
	pMsg += 2*sizeof(int);
	n32MsgLength -= 2*sizeof(int);

	CCSGSInfo *pcGSInfo = GetCSKernelInstance()->GetCGSInfoByNSID(vthis->GetID());
	if (pcGSInfo)
		GetCSKernelInstance()->HandleMsgFromGS(pcGSInfo, pMsg, n32MsgLength, n32RealMsgID, n32MsgID, n32GcNetID);
	return true;
}
