#include "stdafx.h"
#include "M2CSession.h"

#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\GSKernel.h"
#include "../../ThirdFunc/MsgLists/CSToGS.pb.h"
#include "../../ThirdFunc/MsgLists/GSToCS.pb.h"

CMsgHandle_M2C CM2CSession::mHandles;
using namespace GateServer;
CMsgHandle_M2C::CMsgHandle_M2C()
	:IMsgHandle(CSToGS::eMsgToGSFromCS_Begin,CSToGS::eMsgToGSFromCS_End-CSToGS::eMsgToGSFromCS_Begin)
{
	RegisterMsgFunc(CSToGS::eMsgToGSFromCS_AskRegisteRet,  CM2CSession::Msg_Handle_Init, true);
	RegisterMsgFunc(CSToGS::eMsgToGSFromCS_OneSSConnected,  CM2CSession::Msg_Handle_OneSSConnected);
	SetUnKnownMsgHandle(CM2CSession::Msg_Handle_Dispath);
}

CM2CSession::CM2CSession()
{

}

CM2CSession::~CM2CSession()
{

}

void CM2CSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	GateServer::CGSKernel::GetStaticGSKernel().m_tLastPingCSTickCounter = 0;
	ELOG(LOG_INFO,"CS Connected, try to register me.");
	GSToCS::AskRegiste aAskRegiste;
	aAskRegiste.set_port(GateServer::CGSKernel::GetStaticGSKernel().GetGSConfig()->n32GCListenPort);
	aAskRegiste.set_ip((const char*)(&CGSKernel::GetStaticGSKernel().GetGSConfig()->sGCListenIP), sizeof(SIPAddr));
	aAskRegiste.set_gsid(GateServer::CGSKernel::GetStaticGSKernel().GetGSConfig()->n32GSID);
	aAskRegiste.set_usepwd(GateServer::CGSKernel::GetStaticGSKernel().GetGSConfig()->aszMyUserPwd);
	string sData = aAskRegiste.SerializeAsString();
	CGSKernel::GetStaticGSKernel().TransToCS(sData.c_str(),sData.size(),aAskRegiste.msgid(),0,0);
}

void CM2CSession::OnRealEstablish()
{
	ELOG(LOG_INFO,"CS Connected and register ok");
	GateServer::CGSKernel::GetStaticGSKernel().m_CSNetSessionId = GetID();
}

void CM2CSession::OnClose()
{
	ELOG(LOG_INFO,"CS DisConnect.");
	GateServer::CGSKernel::GetStaticGSKernel().m_CSNetSessionId = 0;
}

bool CM2CSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	int n32RealMsgID = *(int*)(pMsg);
	int n32GcNetID = *(int*)(pMsg+4);
	pMsg += 2*sizeof(int);
	n32MsgLength -= 2*sizeof(int);
	//don't send any message until it init success.
	boost::shared_ptr<CSToGS::AskRegisteRet> pAskRegisteRet = ParseProtoMsg<CSToGS::AskRegisteRet>(pMsg, n32MsgLength);
	if (!pAskRegisteRet){
		return 0;
	}

	if (eNormal != pAskRegisteRet->registe()){
		ELOG(LOG_WARNNING, "CS Register Error(ret=%u)!", pAskRegisteRet->registe());
		return false;
	}
	TIME_MILSEC tCSMilsec = pAskRegisteRet->curtime();
	TIME_MILSEC tSelfMilsec = GetMiliSecond();
	GateServer::CGSKernel::GetStaticGSKernel().m_tCSTimeError = tCSMilsec - tSelfMilsec;
	GateServer::CGSKernel::GetStaticGSKernel().m_un32SSBaseIdx = pAskRegisteRet->ssbaseid();
	GateServer::CGSKernel::GetStaticGSKernel().SetMaxSSNum(pAskRegisteRet->ssinfo_size());
	ELOG(LOG_DBBUG, "m_un32MaxSSNum:%d", CGSKernel::GetStaticGSKernel().GetMaxSSNum());
	if (GateServer::CGSKernel::GetStaticGSKernel().GetMaxSSNum() > 100000){
		ELOG(LOG_WARNNING, "CS Register Error(ss max=%u)!", CGSKernel::GetStaticGSKernel().GetMaxSSNum());
		return false;
	}

	if (0 < CGSKernel::GetStaticGSKernel().GetMaxSSNum()){
		EServerNetState eTempSNState = eSNS_Closed;
		CGSKernel::GetStaticGSKernel().m_n32PostSSConnectNum = 0;

		for(UINT32 i = 0; i < pAskRegisteRet->ssinfo_size(); i++){
			// ÏûÏ¢½âÂë//
			if (0 == pAskRegisteRet->ssinfo(i).ssid()) 
				continue;

			if (CGSKernel::GetStaticGSKernel().GetCGSSSInfoMap().find(pAskRegisteRet->ssinfo(i).ssid())
				!=CGSKernel::GetStaticGSKernel().GetCGSSSInfoMap().end())
				continue;

			CGSSSInfo* pSSInfo = new CGSSSInfo();
			
			pSSInfo->m_n32SSID = pAskRegisteRet->ssinfo(i).ssid();
			memcpy(pSSInfo->m_sListenIP.aszIPAddr, pAskRegisteRet->ssinfo(i).ip().c_str(), pAskRegisteRet->ssinfo(i).ip().size());
			pSSInfo->m_n32ListenPort = pAskRegisteRet->ssinfo(i).port();
			pSSInfo->m_eSSNetState = (EServerNetState)pAskRegisteRet->ssinfo(i).netstate();
			pSSInfo->m_n32NSID = 0;
			pSSInfo->m_un32ConnTimes = 0;
			pSSInfo->m_tLastConnMilsec = 0;
			pSSInfo->m_tPingTickCounter = 0;
			pSSInfo->m_n64MsgReceived = 0;
			pSSInfo->m_n64MsgSent = 0;
			pSSInfo->m_n64DataReceived = 0;
			pSSInfo->m_n64DataSent = 0;
			CGSKernel::GetStaticGSKernel().GetCGSSSInfoMap()[pSSInfo->m_n32SSID] = pSSInfo;
			if (pSSInfo->m_eSSNetState == eSNS_Closed) 
				continue;

			INT32 n32PostRet = INetSessionMgr::GetInstance()->CreateConnector(ST_CLIENT_G2S, pAskRegisteRet->ssinfo(i).ip().c_str(), pAskRegisteRet->ssinfo(i).port(), 10240, 10240, pSSInfo->m_n32SSID);
			CGSKernel::GetStaticGSKernel().m_n32PostSSConnectNum++;
		}
	}
	vthis->SetInited(true,true);
	return true;
}

bool CM2CSession::Msg_Handle_OneSSConnected(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	int n32RealMsgID = *(int*)(pMsg);
	int n32GcNetID = *(int*)(pMsg+4);
	pMsg += 2*sizeof(int);
	n32MsgLength -= 2*sizeof(int);
	boost::shared_ptr<CSToGS::OneSSConnected> pOneSSConnected = ParseProtoMsg<CSToGS::OneSSConnected>(pMsg, n32MsgLength);
	if (!pOneSSConnected){
		return 0;
	}

	EServerNetState eTempSNState = (EServerNetState)pOneSSConnected->netstate();
	CGSSSInfo *pcSSInfo = CGSKernel::GetStaticGSKernel().GetGSSSInfoBySSID(pOneSSConnected->ssid());
	if (pcSSInfo){
		memcpy(pcSSInfo->m_sListenIP.aszIPAddr, pOneSSConnected->ip().c_str(), pOneSSConnected->ip().size());
		pcSSInfo->m_n32ListenPort = pOneSSConnected->port();
		if (pcSSInfo->m_eSSNetState == eSNS_Closed){
			pcSSInfo->m_eSSNetState = eTempSNState;
			pcSSInfo->m_n32NSID = 0;
			INT32 n32PostRet = INetSessionMgr::GetInstance()->CreateConnector(ST_CLIENT_G2S, pcSSInfo->m_sListenIP.aszIPAddr
				, pcSSInfo->m_n32ListenPort, 10240, 10240, pOneSSConnected->ssid());
		}
	}
	return true;
}

bool CM2CSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	int n32RealMsgID = *(int*)(pMsg);
	int n32GcNetID = *(int*)(pMsg+4);
	pMsg += 2*sizeof(int);
	n32MsgLength -= 2*sizeof(int);
	CGSKernel::GetStaticGSKernel().HandleMsgFromCS(pMsg, n32MsgLength, n32RealMsgID, n32MsgID, n32GcNetID);
	return true;
}
