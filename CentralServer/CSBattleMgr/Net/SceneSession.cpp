#include "../stdafx.h"
#include "SceneSession.h"

using namespace CentralServer;
#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\CSKernel.h"
#include "../CSSSInfo.h"
#include "../../../ThirdFunc/MsgLists/SSToCS.pb.h"
#include "../../../ThirdFunc/MsgLists/CSToSS.pb.h"

CMsgHandle_Scene CSceneSession::mHandles;

CMsgHandle_Scene::CMsgHandle_Scene()
	:IMsgHandle(SSToCS::eMsgToCSFromSS_Begin, SSToCS::eMsgToCSFromSS_End-SSToCS::eMsgToCSFromSS_Begin)
{
	RegisterMsgFunc(SSToCS::eMsgToCSFromSS_AskRegiste, CSceneSession::Msg_Handle_Init,true);
	SetUnKnownMsgHandle(CSceneSession::Msg_Handle_Dispath);
}

CSceneSession::CSceneSession()
{

}

CSceneSession::~CSceneSession()
{

}

void CSceneSession::SendInitData(){
	TIME_MILSEC tCurMilSec = GetUTCMiliSecond();
	CSToSS::AskRegisteRet aAskRegisteRet;
	aAskRegisteRet.set_ret(eNormal);
	aAskRegisteRet.set_time(tCurMilSec);
	aAskRegisteRet.set_basegsid(GetCSKernelInstance()->GetKernelCfg().un32GSBaseIdx);
	for (UINT32 i= 0; i<GetCSKernelInstance()->GetMaxGSNum(); i++){
		CSToSS::AskRegisteRet::GSInfo* pGSInfo = aAskRegisteRet.add_gsinfo();
		pGSInfo->set_gsid(GetCSKernelInstance()->m_pcGSInfoList[i].m_n32GSID);
		pGSInfo->set_userpwd(GetCSKernelInstance()->m_pcGSInfoList[i].m_szUserPwd, sizeof(CHAR) * c_n32DefaultUserPwdLen);
	}
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_CS_OnlySS, GetID(), aAskRegisteRet, aAskRegisteRet.msgid());

	//////////////////////////////////////////////////////////////////////////
	CentralServer::CCSSSInfo *pcSSInfo = GetCSKernelInstance()->GetCSSInfoByNSID(GetID());
	CSToGS::OneSSConnected sOneSSConnected;
	sOneSSConnected.set_state(eNormal);
	sOneSSConnected.set_time(tCurMilSec);
	sOneSSConnected.set_ssid(pcSSInfo->m_n32SSID);
	sOneSSConnected.set_ip(pcSSInfo->m_sListenIP.aszIPAddr, sizeof(SIPAddr));
	sOneSSConnected.set_port(pcSSInfo->m_n32ListenPort);
	sOneSSConnected.set_netstate(pcSSInfo->m_eSSNetState);
	sOneSSConnected.set_basessid(GetCSKernelInstance()->GetKernelCfg().un32SSBaseIdx);
	
	GetCSKernelInstance()->BroadCastMsgToGS(sOneSSConnected, sOneSSConnected.msgid());
}

void CSceneSession::OnRealEstablish(){
	CCSSSInfo *pcSSInfo = GetCSKernelInstance()->GetCSSInfoByNSID(GetID());
	if (pcSSInfo){
		ELOG(LOG_INFO,"SS(%u) Connected",pcSSInfo->GetSSID());
	}
}

void CSceneSession::OnClose(){
	CCSSSInfo *pcSSInfo = GetCSKernelInstance()->GetCSSInfoByNSID(GetID());
	if (pcSSInfo){
		ELOG(LOG_INFO,"SS(%u) DisConnected",pcSSInfo->GetSSID());
		UINT32 pos = pcSSInfo->GetSSID() - GetCSKernelInstance()->GetKernelCfg().un32SSBaseIdx;
		pcSSInfo->m_eSSNetState = eSNS_Closed;
		pcSSInfo->m_n32NSID = 0;
		pcSSInfo->m_tLastConnMilsec = 0;
		pcSSInfo->m_un32ConnTimes = 0;
		GetCSKernelInstance()->m_psSSNetInfoList[pos].tConnMilsec = 0;
		GetCSKernelInstance()->m_psSSNetInfoList[pos].pcSSInfo = NULL;
	}
}

bool CSceneSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	boost::shared_ptr<SSToCS::AskRegiste> aAskRegiste = ParseProtoMsg<SSToCS::AskRegiste>(pMsg, n32MsgLength);
	if (!aAskRegiste){
		return 0;
	}

	INT32 ssPos = aAskRegiste->ssid() - GetCSKernelInstance()->GetKernelCfg().un32SSBaseIdx;
	INT32 n32Registe = eNormal;
	CCSSSInfo *pcSSInfo = GetCSKernelInstance()->GetCSSInfoBySSID(aAskRegiste->ssid());
	if (NULL == pcSSInfo){
		n32Registe = eEC_InvalidSSID;
		return false;
	}

	INT32 n32Compare = strcmp(pcSSInfo->m_szUserPwd, aAskRegiste->userpwd().c_str());
	if (0 != n32Compare){
		n32Registe = eEC_InvalidUserPwd;
		vthis->DelayClose(111);
		return false;
	}

	// ¼ÓÈëSS
	pcSSInfo->m_eSSNetState = eSNS_Connecting;
	pcSSInfo->m_n32NSID = vthis->GetID();
	vthis->SetLogicID(pcSSInfo->GetSSID());
	pcSSInfo->m_tLastConnMilsec = GetUTCMiliSecond();
	memcpy(&pcSSInfo->m_sListenIP, aAskRegiste->ip().c_str(), sizeof(SIPAddr));
	pcSSInfo->m_n32ListenPort = aAskRegiste->port();
	GetCSKernelInstance()->m_psSSNetInfoList[ssPos].pcSSInfo = pcSSInfo;
	pcSSInfo->m_un32ConnTimes++;
	pcSSInfo->m_tLastConnMilsec = GetUTCMiliSecond();
	pcSSInfo->m_tLastPingMilSec = pcSSInfo->m_tLastConnMilsec;

	vthis->SetInited(true, true);

	return true;
}

bool CSceneSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	CCSSSInfo *pcSSInfo = GetCSKernelInstance()->GetCSSInfoByNSID(vthis->GetID());
	if (pcSSInfo)
		GetCSKernelInstance()->HandleMsgFromSS(pcSSInfo, pMsg, n32MsgLength, n32MsgID);
	return true;
}
