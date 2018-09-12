#include "stdafx.h"
#include "GSKernel.h"
#include <iostream>
#include <fstream>
#include "GSSSInfo.h"

using namespace std;

namespace GateServer{

INT32	CGSKernel::OnMsgFromGC(INT32 n32NSID, const char* pMsg, int n32MsgLength, int n32MsgID)
{
	if (0 > n32MsgLength || m_sGSConfig.n32GCMaxMsgSize < n32MsgLength)
	{
		ELOG(LOG_WARNNING, "msg with n32NSID:%d protocal id %d, size %d, cfg max msg size %d, so disconnect it.", n32NSID, n32MsgID, m_sGSConfig.n32GCMaxMsgSize);
		PostGameClientDisconnect(n32NSID);
		return eNormal;
	}

	bool bLogMsgFlag = false; 
	GCToCS::Login ploginMsg;

	if (n32MsgID > GCToCS::eMsgToGSToCSFromGC_Begin && n32MsgID < GCToCS::eMsgToGSToCSFromGC_End)
	{
		if (n32MsgID == GCToCS::eMsgToGSToCSFromGC_AskLogin)
		{
			GCToCS::Login pLogin;
			if (!ParseProtoMsg(pMsg, n32MsgLength, pLogin)){
				ELOG(LOG_ERROR, "");
				return 0;
			}
			//验证token
			if (!IsUserCanLogin(pLogin.name(), pLogin.passwd(), n32NSID)){
				ELOG(LOG_ERROR, "user %s can't login with token %s", pLogin.name().c_str(), pLogin.passwd().c_str());
				GSToGC::NetClash sMsg;
				PostToGameClient(n32NSID, sMsg, sMsg.msgid());
				PostGameClientDisconnect(n32NSID);
				return eNormal;
			}
			//获取IP
			INetSession* pClient = INetSessionMgr::GetInstance()->GetSession(n32NSID);
			if (NULL != pClient){
				const char* ipAddr = pClient->GetRemoteIp();
				if (NULL == ipAddr)
				{
					ELOG(LOG_ERROR, "user %s can't login with IP is null", pLogin.name().c_str() );
					PostGameClientDisconnect(n32NSID);
					return eNormal;
				}
				bLogMsgFlag = true;  
				ploginMsg.set_platform(pLogin.platform() );
				ploginMsg.set_sdk(pLogin.sdk());
				ploginMsg.set_name(pLogin.name());
				ploginMsg.set_passwd(pLogin.passwd());
				ploginMsg.set_equimentid(pLogin.equimentid());
				ploginMsg.set_ipaddress(  std::string(ipAddr) ); 
				ELOG(LOG_DBBUG,"client(%d) ask login(%s)(%s)",n32NSID,ploginMsg.name().c_str(),ploginMsg.passwd().c_str());
			}
		}
		else if (n32MsgID == GCToCS::eMsgToGSToCSFromGC_AskReconnectGame)
		{
			GCToCS::ReconnectToGame pReconnectToGame;
			if (!ParseProtoMsg(pMsg, n32MsgLength, pReconnectToGame)){
				ELOG(LOG_ERROR, "");
				return 0;
			}
			//验证token
			if (!IsUserCanLogin(pReconnectToGame.name(), pReconnectToGame.passwd(), n32NSID)){
				ELOG(LOG_ERROR, "user %s can't login with token %s", pReconnectToGame.name().c_str(), pReconnectToGame.passwd().c_str());
				GSToGC::NetClash sMsg;
				PostToGameClient(n32NSID, sMsg, sMsg.msgid());
				PostGameClientDisconnect(n32NSID);
				return eNormal;
			}
		}
	 
		if (m_CSNetSessionId>0)
		{  
			if (bLogMsgFlag){
				string sData = ploginMsg.SerializeAsString();
				TransToCS(sData.c_str(), sData.size(), n32MsgID, GSToCS::eMsgToCSFromGS_ReportGCMsg, n32NSID);
			}else{
				TransToCS(pMsg, n32MsgLength, n32MsgID, GSToCS::eMsgToCSFromGS_ReportGCMsg, n32NSID);
			}
		}
		else
		{
			ELOG(LOG_WARNNING, "m_CSNetSessionId < 0");
		}
	}
	else if (n32MsgID > GCToSS::eMsgToGSToSSFromGC_Begin && n32MsgID < GCToSS::eMsgToGSToSSFromGC_End)
	{
		if (n32MsgID==GCToSS::eMsgToGSToSSFromGC_AskPingSS)
		{
			GCToSS::AskPingSS msgPing; 
			if (ParseProtoMsg(pMsg, n32MsgLength, msgPing)){
				GSToGC::PingRet retMsg;
				retMsg.set_time(msgPing.time());
				retMsg.set_flag(1);
				PostToGameClient(n32NSID,retMsg,retMsg.msgid());
			}
			return eNormal;
		}

		User2SSInfoMap::iterator iter = m_User2SSInfoMap.find(n32NSID);
		if (m_User2SSInfoMap.end() != iter)
		{
			CGSSSInfo* pSSInfo = iter->second;
			if (pSSInfo->GetNSID()>0)
			{
				TransToSS(pSSInfo,pMsg,n32MsgLength,n32MsgID,GSToSS::eMsgToSSFromGS_ReportGCMsg,n32NSID);
			}
			else
			{
				ELOG(LOG_ERROR, "pSSInfo->GetNSID() <= 0!!");
			}
		}
		else
		{
			ELOG(LOG_ERROR, "nsid(%d) send msg(%d) error, can't get ssinfo.", n32NSID, n32MsgID);
		}
	}
	else
	{
		ELOG(LOG_ERROR, "unknown msg with Protocal id %d, size %d, n32NSID:%d", n32MsgID, n32MsgLength, n32NSID);
	}
	return eNormal;
}


}
