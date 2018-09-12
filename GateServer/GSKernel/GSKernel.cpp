#include "stdafx.h"
#include "GSKernel.h"
#include <iostream>
#include <fstream>
#include "../../ThirdFunc/MsgLists/BSToGS.pb.h"
#include "../../ThirdFunc/MsgLists/GSToBS.pb.h"
#include "tinyxml.h" 
#include "tinystr.h"

using namespace std;

namespace GateServer{
CGSKernel::CGSKernel()
{
	timeBeginPeriod(1);
	memset(&m_sGSConfig, 0, sizeof(m_sGSConfig));
	m_n32HeartBeatTimes = 0;
	m_tHeartBeatTicks = 0;
	m_tLastPingCSTickCounter = 0;
	m_tCSTimeError = 0;
	m_un32SSBaseIdx = 0;
	m_n32MaxSSNum = 0;
	m_CSNetSessionId = 0;
}

CGSKernel::~CGSKernel()
{
	ClearAllUserToken();
	timeEndPeriod(1);
}

INT32	CGSKernel::LoadConfig()
{
	const char* filepath = "./GSKernel/GSCfg.xml";
	TiXmlDocument doc(filepath);
	bool loadOkay = doc.LoadFile();
	if (!loadOkay){
		printf("load GSCfg.xml failed for %s\n", doc.ErrorDesc());
	}
	else{
		TiXmlElement* m_pXMLEle = doc.RootElement();
		const char* pIP = m_pXMLEle->FirstChildElement("IP")->GetText();
		memcpy(m_sGSConfig.sCSIP.aszIPAddr, pIP, strlen(pIP));

		m_sGSConfig.n32CSPort = atoi(m_pXMLEle->FirstChildElement("Port")->GetText());
		m_sGSConfig.n32CSMaxMsgSize = atoi(m_pXMLEle->FirstChildElement("MsgMaxSize")->GetText());

		const char* pPWD = m_pXMLEle->FirstChildElement("PWD")->GetText();
		memcpy(m_sGSConfig.aszMyUserPwd, pPWD, strlen(pPWD));

		m_sGSConfig.n32GSID = atoi(m_pXMLEle->FirstChildElement("GSID")->GetText());

		const char* pListenIP = m_pXMLEle->FirstChildElement("ListenIP")->GetText();
		memcpy(m_sGSConfig.sGCListenIP.aszIPAddr, pListenIP, strlen(pListenIP));

		m_sGSConfig.n32GCListenPort = atoi(m_pXMLEle->FirstChildElement("ListenPort")->GetText());
		m_sGSConfig.n32GCMaxMsgSize = atoi(m_pXMLEle->FirstChildElement("MsgMaxSize")->GetText());
		m_sGSConfig.n32MaxGCNum = atoi(m_pXMLEle->FirstChildElement("MaxGCNum")->GetText());


		const char* pBSIP = m_pXMLEle->FirstChildElement("BSIP")->GetText();
		memcpy(m_sGSConfig.sBSListenIP.aszIPAddr, pBSIP, strlen(pBSIP));

		m_sGSConfig.n32BSListenPort = atoi(m_pXMLEle->FirstChildElement("BSPort")->GetText());
		m_sGSConfig.n32SkipBalance = atoi(m_pXMLEle->FirstChildElement("IfSkipBS")->GetText());
	}

	return eNormal;
}

INT32	CGSKernel::UnloadConfig()
{
	return eNormal;
}

INT32	CGSKernel::Initialize()
{
	memset(&m_sGSConfig, 0, sizeof(m_sGSConfig));
	m_tHeartBeatTicks = 0;
	m_tCSTimeError = 0;
	m_tLastPingCSTickCounter = 0;
	m_un32SSBaseIdx = 0;
	m_n32MaxSSNum = 0;
	m_n32PostSSConnectNum = 0;
	m_n32SuccessSSConnectNum = 0;

	memset(m_asCSMsgHandler, 0, sizeof(m_asCSMsgHandler));
	memset(m_asSSMsgHandler, 0, sizeof(m_asSSMsgHandler));

	m_asCSMsgHandler[CSToGS::eMsgToGSFromCS_AskPingRet - CSToGS::eMsgToGSFromCS_Begin] = (&CGSKernel::OnMsgFromCS_AskPingRet);
	m_asCSMsgHandler[CSToGS::eMsgToGSFromCS_OrderOpenListen - CSToGS::eMsgToGSFromCS_Begin] = &CGSKernel::OnMsgFromCS_OrderOpenListen;
	m_asCSMsgHandler[CSToGS::eMsgToGSFromCS_OrderCloseListen - CSToGS::eMsgToGSFromCS_Begin] = (&CGSKernel::OnMsgFromCS_OrderCloseListen);
	m_asCSMsgHandler[CSToGS::eMsgToGSFromCS_OrderKickoutGC - CSToGS::eMsgToGSFromCS_Begin] = (&CGSKernel::OnMsgFromCS_OrderKickoutGC);
	m_asCSMsgHandler[CSToGS::eMsgToGSFromCS_UserConnectedSS - CSToGS::eMsgToGSFromCS_Begin] = (&CGSKernel::OnMsgFromCS_UserConnectedToSS);
	m_asCSMsgHandler[CSToGS::eMsgToGSFromCS_UserDisConnectedSS - CSToGS::eMsgToGSFromCS_Begin] = (&CGSKernel::OnMsgFromCS_UserDisConnectedToSS);

	m_asSSMsgHandler[SSToGS::eMsgToGSFromSS_AskPingRet - SSToGS::eMsgToGSFromSS_Begin] = &CGSKernel::OnMsgFromSS_AskPingRet;
	m_asSSMsgHandler[SSToGS::eMsgToGSFromSS_OrderKickoutGC - SSToGS::eMsgToGSFromSS_Begin] = (&CGSKernel::OnMsgFromSS_OrderKickoutGC);

	INT32 n32Init = eNormal;
	memset(&m_sGSConfig, 0, sizeof(m_sGSConfig));
	n32Init = LoadConfig();

	//initialize fail.
	if (eNormal == n32Init)
	{
		ELOG(LOG_INFO, "CGSKernel Initialize success");
	}

	return n32Init;
}

INT32	CGSKernel::Start()
{
	GSNetSessionMgr* pNetSession = new GSNetSessionMgr;
	// 允许客户端连接
	pNetSession->CreateListener(m_sGSConfig.n32GCListenPort,102400,102400,0);
	// 连接中心服务器
	pNetSession->CreateConnector(ST_CLIENT_G2C,m_sGSConfig.sCSIP.aszIPAddr,m_sGSConfig.n32CSPort,10240000,40960000,0);
	// 连接DBServer
	pNetSession->CreateConnector(ST_CLIENT_G2B,m_sGSConfig.sBSListenIP.aszIPAddr,m_sGSConfig.n32BSListenPort,1024000,1024000,0);

	//reset member variables.
	m_tHeartBeatTicks = 0;
	MainLoop();

	return 0;
}

INT32	CGSKernel::Stop()
{
	ELOG(LOG_INFO, "GSKernel Stop success!");
	return eNormal;
}

INT32	CGSKernel::Uninitialize()
{
	UnloadConfig();

	ELOG(LOG_INFO, "GSKernel Uninitialize success ");

	return eNormal;
}

void CGSKernel::OnEvent(UINT32 event_type,PVOID params)
{
	switch(event_type)
	{
	case EVT_ON_SS_DISCONNECT:
		{
			CGSSSInfo* pGSSSInfo = (CGSSSInfo*)params;
			User2SSInfoMap::iterator it = m_User2SSInfoMap.begin();
			for (;it!=m_User2SSInfoMap.end();)
			{
				if (it->second==pGSSSInfo)
				{
					PostGameClientDisconnect(it->first);
					it = m_User2SSInfoMap.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		break;
	}
}

INT32 CGSKernel::PostToGameClient(INT32 n32NetSessioNID, const char* pRealMsg, int n32MsgLen, int n32MsgID)
{
#ifdef USE_PROFILE
	gProfileNet.mMessageCount[n32MsgID] += 1;
#endif
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_GS, n32NetSessioNID, pRealMsg, n32MsgLen, n32MsgID);
	return 0;
}

INT32 CGSKernel::PostToGameClient(INT32 n32NetSessioNID, google::protobuf::Message& sMsg, int n32MsgID)
{
#ifdef USE_PROFILE
	gProfileNet.mMessageCount[n32MsgID] += 1;
#endif
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_GS, n32NetSessioNID, sMsg, n32MsgID);
	return 0; 
}

INT32 CGSKernel::BroadcastToGameClient(const char* pRealMsg, int n32MsgLen, int n32MsgID)
{
#ifdef USE_PROFILE
	gProfileNet.mMessageCount[n32MsgID] += 1;
#endif
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_GS, -1, pRealMsg, n32MsgLen, n32MsgID);
	return 0;
}

INT32 CGSKernel::PostGameClientDisconnect(INT32 n32NSID)
{
	INetSessionMgr::GetInstance()->DisconnectOne(n32NSID);
	return 0;
}

INT32 CGSKernel::TransToCS(const char* sMsgBuffer,int n32MsgLen,int n32MsgID,int n32TransID,int gcNetID)
{
	INetSessionMgr::GetInstance()->TranMsgToSession(ST_CLIENT_G2C, 0, sMsgBuffer, n32MsgLen, n32MsgID, n32TransID, gcNetID);
	return 0;
}

INT32 CGSKernel::TransToSS(CGSSSInfo *piSSInfo,const char* sMsgBuffer,int n32MsgLen,int n32MsgID,int n32TransID,int gcNetID)
{
	Assert(piSSInfo->GetNSID()>0);
	INetSessionMgr::GetInstance()->TranMsgToSession(ST_CLIENT_G2S, piSSInfo->GetNSID(), sMsgBuffer, n32MsgLen, n32MsgID, n32TransID, gcNetID);
	return 0;
}

CGSSSInfo*	CGSKernel::GetGSSSInfoBySSID(INT32 n32SSID)
{
	CGSSSInfoMap::iterator iter = m_CGSSSInfoMap.find(n32SSID);
	if (m_CGSSSInfoMap.end() != iter){
		return iter->second;
	}
	return NULL;	
}

GateServer::CGSKernel& CGSKernel::GetStaticGSKernel(){

	static CGSKernel lCGSKernel;
	return lCGSKernel;
}
}
