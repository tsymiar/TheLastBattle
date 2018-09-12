#include "StdAfx.h"
#include "BSHanlder.h"
#include "RobotMgr.h"
#include "WorkThreadMgr.h"

namespace ReBot
{

	INT32 CBSHanlder::OnConnected(INT32 n32NetSessionID){
		//ELOG(LOG_INFO, "n32NetSessionID:%d", n32NetSessionID);
		DefineMsgBuilder_Middle
		tMsgBuilder.GetMsgHeader()->n32ProtocolID = 32769;
		tMsgBuilder.AddINT32(0);
		return eNormal;
	}

	UINT32 CBSHanlder::OnReceieved(INT32 n32NetSessionID, BYTE *pbData, UINT32 un32DataLen){
		return eNormal;
	}

	INT32 CBSHanlder::OnConnectorHeartBeart(){
		return eNormal;
	}

	VOID CBSHanlder::OnDisconnect(INT32 n32NetSessionID){

	}

	INT32 CBSHanlder::OnConnectFail(INT32 n32NetSessionID, INT32 n32ErrorCode){
		return 0;
	}

	CBSHanlder::CBSHanlder(){
		m_piNetConnector = WYYTools_NetMgr->CreateNetConnector(eTCPNetConnector, this);

		m_piNetConnector->Initialize(1);
		m_piNetConnector->Start();

		//UINT16 un16NSIdx = INetMgr::GetNetSessionIdx(1);
		//map<EServerLinkType,SessionInfo *>::iterator iter = WorkThreadMgr::GetInstance().m_IpLinkInfo.find(eServerLink_GateType);
		//if(iter == WorkThreadMgr::GetInstance().m_IpLinkInfo.end())
		//	return;	 	  
		//string str = WorkThreadMgr::GetInstance().m_IpLinkInfo[eServerLink_GateType]->ipAddr;
		//m_piNetConnector->PostConnect(str.c_str(), WorkThreadMgr::GetInstance().m_IpLinkInfo[eServerLink_GateType]->ipPort, un16NSIdx);
	}

}
