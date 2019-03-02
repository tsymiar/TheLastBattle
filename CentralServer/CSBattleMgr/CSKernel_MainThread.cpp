#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSKernel.h"
#include "CSUserMgr.h"
#include "CSBattleMgr.h"
#include <conio.h>
using namespace std;

namespace CentralServer{

bool DoUserCmd(const char* cmd);

void CCSKernel::MainLoop(){
	TIME_TICK	tHeartBeatCDTick = 10;

	//侦听端口10002
    INetSessionMgr::GetInstance()->CreateListener(m_sCSKernelCfg.n32GSNetListenerPort,1024000,10240000,0,&gGateSessionFactory);
    //侦听端口10001
	INetSessionMgr::GetInstance()->CreateListener(m_sCSKernelCfg.n32SSNetListenerPort,1024000,10240000,1,&gSceneSessionFactory);
	//侦听端口10010
    INetSessionMgr::GetInstance()->CreateListener(m_sCSKernelCfg.n32RCNetListenerPort,1024000,10240000,2,&gRemoteConsoleFactory);
    //连接LogMgr 1234端口
	INetSessionMgr::GetInstance()->CreateConnector(ST_CLIENT_C2Log, m_sCSKernelCfg.LogAddress.c_str(), m_sCSKernelCfg.LogPort, 102400,102400,0);

    //连接redis 6379
	if (m_sCSKernelCfg.redisAddress != "0"){
		INetSessionMgr::GetInstance()->CreateConnector(ST_CLIENT_C2R, m_sCSKernelCfg.redisAddress.c_str(), m_sCSKernelCfg.redisPort,102400,102400,0);
	}
    //连接redis 6380，也是redis？
	if (m_sCSKernelCfg.redisLogicAddress != "0"){
		INetSessionMgr::GetInstance()->CreateConnector(ST_CLIENT_C2LogicRedis, m_sCSKernelCfg.redisLogicAddress.c_str(), m_sCSKernelCfg.redisLogicPort,102400,102400,0);
	}
	while(true)
	{
		if (kbhit())
		{
			static char CmdArray[1024] = {0};
			static int CmdPos = 0;
			char CmdOne = getche();
			CmdArray[CmdPos++] = CmdOne;
			bool bRet = 0;
			if (CmdPos>=1024 || CmdOne==13) { CmdArray[--CmdPos]=0; bRet = DoUserCmd(CmdArray); CmdPos=0; if (bRet) break; }
		}

		INetSessionMgr::GetInstance()->Update();

		GetCSUserMgrInstance()->OnHeartBeatImmediately();

		++m_RunCounts;

		m_BattleTimer.Run();

		Sleep(1);
	}
}

bool DoUserCmd(const char* cmd)
{
	printf("start parse cmd %s\n",cmd);

	if (stricmp(cmd,"q")==0) return true;
	else if(stricmp(cmd,"s")==0) GetCSUserMgrInstance()->PostSaveCmd();

	return false;
}

INT32 CCSKernel::HandleMsgFromSS(ICSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength, int n32MsgID){
	return OnMsgFromSS(piSSInfo, pMsg, n32MsgLength, n32MsgID);
}

INT32 CCSKernel::HandleMsgFromGS(ICSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID){
	return OnMsgFromGS(piGSInfo, pMsg, n32MsgLength, n32MsgID, n32TransID, n32GcNetID);
}

INT32 CCSKernel::HandleMsgFromRC(ICSRCInfo *piRCInfo, const char* pMsg, int n32MsgLength, int n32MsgID){

	auto iter = m_RCMsgHandlerMap.find(n32MsgID);
	if (m_RCMsgHandlerMap.end() != iter){
		return (iter->second)(piRCInfo, pMsg, n32MsgLength);
	}
	return eNormal;
}

INT32	CCSKernel::OnMsgFromSS(ICSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength, int n32MsgID){
	if (NULL == piSSInfo || NULL == pMsg){
		return eEC_NullPointer;
	}

	auto iter = m_SSMsgHandlerMap.find(n32MsgID);
	if (m_SSMsgHandlerMap.end() != iter){
		return (iter->second)(piSSInfo, pMsg, n32MsgLength);
	}

	return eNormal;
}

INT32	CCSKernel::OnMsgFromGS(ICSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID){

	if (n32TransID==GSToCS::eMsgToCSFromGS_ReportGCMsg)
	{
		GCMsgHandlerMap::iterator iter = m_GCMsgHandlerMap.find(n32MsgID);
		if (m_GCMsgHandlerMap.end() != iter){
			return (iter->second)(piGSInfo, n32GcNetID, pMsg, n32MsgLength);
		}
	}
	else
	{
		GSMsgHandlerMap::iterator iter = m_GSMsgHandlerMap.find(n32MsgID);
		if (m_GSMsgHandlerMap.end() != iter){
			return (iter->second)(piGSInfo, pMsg, n32MsgLength);
		}
	}
	return eEC_NullMsgHandler;
}

INT32	CCSKernel::OnMsgFromSS_AskPing(ICSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength){
	boost::shared_ptr<SSToCS::AskPing> spAskPing = ParseProtoMsg<SSToCS::AskPing>(pMsg, n32MsgLength);
	if (!spAskPing){
		return 0;
	}

	CSToSS::PingRet sPingRet;
	sPingRet.set_time(spAskPing->time());
	piSSInfo->ResetPing();

	return PostMsgToSS(piSSInfo, sPingRet, sPingRet.msgid());
}

INT32	CCSKernel::OnMsgFromGS_AskRegiste(ICSGSInfo *cpiGSInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<GSToCS::AskRegiste> sMsg = ParseProtoMsg<GSToCS::AskRegiste>(pData, n32DataLength);
	if (!sMsg){
		return 0;
	}

	// 找到位置号
	INT32 gsPos = -1;
	CCSGSInfo *pcGSInfo = NULL;
	for (UINT32 i = 0; i < GetMaxGSNum(); i++){
		if (sMsg->gsid() == m_pcGSInfoList[i].m_n32GSID){
			pcGSInfo = &m_pcGSInfoList[i];
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

	TIME_MILSEC tCurUTCMilsec = GetUTCMiliSecond();
	if (eNormal == n32Registe){
		pcGSInfo->m_eGSNetState = eSNS_Free;
		pcGSInfo->m_n32NSID = cpiGSInfo->GetNSID();
		pcGSInfo->m_tLastConnMilsec = GetUTCMiliSecond();
		memcpy(&pcGSInfo->m_sListenIP, sMsg->ip().c_str(), sizeof(SIPAddr));
		pcGSInfo->m_n32ListenPort = sMsg->port();
		m_psGSNetInfoList[gsPos].pcGSInfo = pcGSInfo;

		pcGSInfo->m_n64MsgReceived++;
		pcGSInfo->m_n64DataReceived += sMsg->ByteSize();
		pcGSInfo->m_un32ConnTimes++;
		pcGSInfo->m_tLastConnMilsec = tCurUTCMilsec;
		pcGSInfo->m_tLastPingMilSec = tCurUTCMilsec;
		ELOG(LOG_INFO, "Gate Server with GSID %d registed at net session %d, total conn times %d", pcGSInfo->GetGSID(), gsPos, pcGSInfo->m_un32ConnTimes);
	}

	CSToGS::AskRegisteRet sAskRegisteRet;
	sAskRegisteRet.set_registe(n32Registe);
	sAskRegisteRet.set_curtime(tCurUTCMilsec);
	if (eNormal == n32Registe){
		sAskRegisteRet.set_ssbaseid(m_sCSKernelCfg.un32SSBaseIdx);
		for (UINT32 i = 0; i < GetMaxSSNum(); i++){
			CSToGS::AskRegisteRet::SSInfo* pSSInfo = sAskRegisteRet.add_ssinfo();
			pSSInfo->set_ssid(m_pcSSInfoList[i].m_n32SSID);
			pSSInfo->set_ip((const char*)&m_pcSSInfoList[i].m_sListenIP, sizeof(SIPAddr));
			pSSInfo->set_port(m_pcSSInfoList[i].m_n32ListenPort);
			pSSInfo->set_netstate(m_pcSSInfoList[i].m_eSSNetState);
		}
	}

	GetCSKernelInstance()->PostMsgToGS(cpiGSInfo, 0, sAskRegisteRet, sAskRegisteRet.msgid());

	if (eNormal != n32Registe){
		INetSessionMgr::GetInstance()->DisconnectOne(cpiGSInfo->GetNSID());
	}

	return eNormal;
}

INT32	CCSKernel::OnMsgFromGS_AskPing(ICSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength){
	boost::shared_ptr<GSToCS::Asking> sAskPing = ParseProtoMsg<GSToCS::Asking>(pMsg, n32MsgLength);
	if (!sAskPing){
		return 0;
	}

	CSToGS::AskPing sMsg;
	sMsg.set_time(sAskPing->time());

	PostMsgToGS(piGSInfo, 0, sMsg, sMsg.mgsid());

	return eNormal;
}

INT32	CCSKernel::OnMsgFromGS_ReportGCMsg(ICSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength){
	boost::shared_ptr<GSToCS::ReportGCMsg> pReportGCMsg = ParseProtoMsg<GSToCS::ReportGCMsg>(pMsg, n32MsgLength);
	if (!pReportGCMsg){
		return 0;
	}

	if (pReportGCMsg->othermsgid() < GCToCS::eMsgToGSToCSFromGC_Begin || pReportGCMsg->othermsgid() >= GCToCS::eMsgToGSToCSFromGC_End){
		ELOG(LOG_WARNNING, "GS %d at NSIdx %d, size %d error.", piGSInfo->GetGSID(), piGSInfo->GetNSID(), n32MsgLength);
		return eNormal;
	}

	GCMsgHandlerMap::iterator iter = m_GCMsgHandlerMap.find(pReportGCMsg->othermsgid());
	if (m_GCMsgHandlerMap.end() != iter){
		return (iter->second)(piGSInfo, pReportGCMsg->nsid(), pReportGCMsg->othermsg().c_str(), pReportGCMsg->othermsg().size());
	}

	return eNormal;
}

void CCSKernel::ProfileReport(int64_t tUTCMilsec, int64_t tTickSpan)
{
	DWORD fps = 1000.0f*m_RunCounts/(5000+tUTCMilsec-m_LastReport);
	m_LastReport = tUTCMilsec;
	wstringstream strTitle;
	strTitle << "cs" << " id=" << m_sCSKernelCfg.unCSId << " fps=" << fps;
	GetBattleMgrInstance()->GetProfileReport(strTitle);
	GetCSUserMgrInstance()->GetProfileReport(strTitle);
	SetConsoleTitle(strTitle.str().c_str());
	m_RunCounts = 0;
}

ICSSSInfo*	CCSKernel::GetMiniBattleNumSS(){
	//check parameters.
	if (NULL == m_pcSSInfoList){
		return NULL;
	}
	//define local variables.
	CCSSSInfo *pcFindSSInfo = NULL;
	TIME_MILSEC curTime = GetUTCMiliSecond();
	//loop max SS num to find out the mini battle one.
	for (UINT32 i = 0; i < m_sCSKernelCfg.un32MaxSSNum; i++){
		CCSSSInfo &rsSSInfo = m_pcSSInfoList[i];
		if (eSNS_Free != rsSSInfo.m_eSSNetState && eSNS_Busy != rsSSInfo.m_eSSNetState){
			continue;
		}
		if (rsSSInfo.m_tLastPingMilSec+c_tSSPingCSCDTick*2<curTime){
			continue;
		}
		if (0 == rsSSInfo.m_n32NSID){
			continue;
		}
		if (NULL == pcFindSSInfo){
			pcFindSSInfo = &rsSSInfo;
		}else if (pcFindSSInfo->m_n32BattleNum > rsSSInfo.m_n32BattleNum){
			pcFindSSInfo = &rsSSInfo;
		}
	}

	return pcFindSSInfo;
}
}