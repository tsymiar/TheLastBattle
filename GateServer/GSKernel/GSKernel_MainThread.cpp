#include "stdafx.h"
#include "GSKernel.h"
#include <iostream>
#include <fstream>
#include "GSSSInfo.h"

using namespace std;

namespace GateServer{

void CGSKernel::MainLoop(){
	m_tHeartBeatTicks = 0;
	TIME_TICK	tHeartBeatCDTick = 1;
	TIME_TICK tHeartPack = 100;
	TIME_MILSEC tLastHeartBeatMilsec = GetMiliSecond();
	INT32 n32CurHeartBeatTime = 0;
	TIME_MILSEC tCurUTCMilsec= 0;
	while(true){
		tCurUTCMilsec = GetMiliSecond();
		ProfileReport(tCurUTCMilsec);
		TIME_TICK tTickElaps = tCurUTCMilsec - tLastHeartBeatMilsec;
		if (tTickElaps >= tHeartPack){			
			m_tHeartBeatTicks += tTickElaps;
			++m_n32HeartBeatTimes;
			if (eNormal != OnHeartBeat(tCurUTCMilsec, tTickElaps)){
				ELOG(LOG_ERROR,  "fail with error code %d!, please amend the error and try again!");
				break;
			}
			tLastHeartBeatMilsec = tCurUTCMilsec;
		}
#ifdef USE_PROFILE
		TIME_MILSEC begin = GetMiliSecond();
#endif
		GSNetSessionMgr::GetInstance()->Update();
#ifdef USE_PROFILE
		gProfileNet.mTimeForDealWithMsg += (GetMiliSecond()-begin);
#endif
		Sleep((INT32)tHeartBeatCDTick);
	}

	ELOG(LOG_WARNNING,  "exited!");
}

INT32	CGSKernel::OnHeartBeat(TIME_MILSEC tUTCMilsec, TIME_TICK tTickSpan)
{
#ifdef USE_PROFILE
	TIME_MILSEC begin = GetMiliSecond();
#endif
	CheckCSConnect(tUTCMilsec);
	CheckSSConnect(tUTCMilsec);
	ChechUserToken(tUTCMilsec);
	ReportGsInfo(tUTCMilsec);
#ifdef USE_PROFILE
	gProfileNet.mTimeForDealWithOther += (GetMiliSecond()-begin);
#endif
	return eNormal;
}

void CGSKernel::ProfileReport(TIME_TICK tUTCMilsec)
{
	static DWORD runCounts = 0;
	static TIME_TICK lastReprot = tUTCMilsec;
	++runCounts;
	if (lastReprot>tUTCMilsec) return;
	DWORD fps = 1000.0f*runCounts/(5000+tUTCMilsec-lastReprot);
	lastReprot = tUTCMilsec + 5000;
	//////////////////////////////////////////////////////////////////////////
	int max[3][2] = {0};
	for (auto it=gProfileNet.mMessageCount.begin(); it!=gProfileNet.mMessageCount.end(); ++it)
	{
		if (it->second>max[0][1])
		{
			max[2][0] = max[1][0];
			max[2][1] = max[1][1];
			max[1][0] = max[0][0];
			max[1][1] = max[0][1];
			max[0][0] = it->first;
			max[0][1] = it->second;
		}
		else if (it->second>max[1][1])
		{
			max[2][0] = max[1][0];
			max[2][1] = max[1][1];
			max[1][0] = it->first;
			max[1][1] = it->second;
		}
		else if (it->second>max[2][1])
		{
			max[2][0] = it->first;
			max[2][1] = it->second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TCHAR text[1024];
	swprintf_s(text,_countof(text),_T("gs id=%u fps=%u users=%u time-msg=%u time-other=%u max(%u->%u,%u->%u,%u->%u)"),
		m_sGSConfig.n32GSID,fps,gProfileNet.mClientCount,
		gProfileNet.mTimeForDealWithMsg,gProfileNet.mTimeForDealWithOther,
		max[0][0],max[0][1],max[1][0],max[1][1],max[2][0],max[2][1]);
	gProfileNet.mTimeForDealWithMsg = 0;
	gProfileNet.mTimeForDealWithOther = 0;
	SetConsoleTitle(text);
	runCounts = 0;
}

INT32 CGSKernel::HandleMsgFromCS(const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID)
{
	return OnMsgFromCS(pMsg, n32MsgLength, n32MsgID, n32TransID,n32GcNetID);
}

INT32 CGSKernel::HandleMsgFromSS(CGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID){
	return OnMsgFromSS(piSSInfo, pMsg, n32MsgLength, n32MsgID, n32TransID, n32GcNetID);
}

INT32 CGSKernel::HandleMsgFromGC(INT32 n32NSID, const char* pMsg, int n32MsgLength, int n32MsgID){
	OnMsgFromGC(n32NSID, pMsg, n32MsgLength, n32MsgID);
	return eNormal;
}

INT32	CGSKernel::CheckCSConnect(TIME_MILSEC tTime)
{
	if (tTime - m_tLastPingCSTickCounter >= c_tDefaultPingCDTick)
	{
		TIME_MILSEC tCurMilsec = GetMiliSecond();
		GSToCS::Asking sPing;
		sPing.set_time(tCurMilsec);
		string sData = sPing.SerializeAsString();
		CGSKernel::GetStaticGSKernel().TransToCS(sData.c_str(), sData.size(), sPing.mgsid(), 0, 0);
		m_tLastPingCSTickCounter = tTime;
	}

	return eNormal;
}

INT32	CGSKernel::CheckSSConnect(TIME_MILSEC tCurTime)
{
	if (m_CGSSSInfoMap.empty()){
		return eNormal;
	}

	TIME_MILSEC tCurUTCMilsec = GetMiliSecond();
	for (CGSSSInfoMap::iterator iter = m_CGSSSInfoMap.begin(); iter != m_CGSSSInfoMap.end(); ++iter){
		CGSSSInfo* pSSInfo = iter->second;
		if (NULL != pSSInfo && 0 != pSSInfo->GetSSID() && 0 != pSSInfo->GetNSID()){
			if (tCurTime - pSSInfo->m_tPingTickCounter >= c_tDefaultPingCDTick)
			{
				GSToSS::AskPing sMsg;
				sMsg.set_time(tCurUTCMilsec);
				string sData = sMsg.SerializeAsString();
				TransToSS(pSSInfo,sData.c_str(),sData.size(),sMsg.mgsid(),0,0);
				pSSInfo->m_tPingTickCounter = tCurTime;
			}
		}
	}

	return eNormal;
}

INT32 CGSKernel::ReportGsInfo(TIME_TICK tUICMilsec){
	static TIME_TICK nextReportTime = tUICMilsec;
	if(nextReportTime > tUICMilsec) 
		return eNormal;
	nextReportTime = tUICMilsec + 2000;//2秒间隔

	GSToBS::ReportAllClientInf sMsg;
	sMsg.set_tokenlist_size(m_UserTokenList.size());
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_CLIENT_G2B, 0, sMsg, sMsg.msgid());
	return eNormal;
}

void CGSKernel::ClearAllUserToken()
{
	map<string,SUserToken*>::iterator it = m_UserTokenList.begin();
	for (;it!=m_UserTokenList.end();++it)
	{
		delete it->second;
	}
	m_UserTokenList.clear();
	m_UserTokenListByNsId.clear();
}

void CGSKernel::AddUserToken(const std::string& sUserName,const std::string sToken)
{
	SUserToken* &userToken = m_UserTokenList[sUserName];
	if (userToken==NULL) 
		userToken = new SUserToken;
	userToken->UserName = sUserName;
	userToken->UserToken = sToken;
	userToken->OverTime = GetMiliSecond()+60000;
	if (userToken->NetSessionID>0){
		m_UserTokenListByNsId.erase(userToken->NetSessionID);
		userToken->NetSessionID = 0;
	}
	userToken->ReconnectCount = 0;
}

INT32 CGSKernel::ChechUserToken( TIME_TICK tUICMilsec )
{
	static TIME_TICK lastReprot = tUICMilsec;
	if (lastReprot>tUICMilsec) return eNormal;
	lastReprot = tUICMilsec + 2000;

	map<string,SUserToken*>::iterator it = m_UserTokenList.begin();
	for (;it!=m_UserTokenList.end();)
	{
		SUserToken* userToken = it->second;
		if (userToken->NetSessionID==0 && 
			tUICMilsec>userToken->OverTime)//最大重练超时限制//
		{
			for (auto it2=m_UserTokenListByNsId.begin();it2!=m_UserTokenListByNsId.end();++it2){
				Assert(it2->second!=userToken);//检查没有对象引用它
			}
			it = m_UserTokenList.erase(it);
			delete userToken;
		}
		else
		{
			++it;
		}
	}
	return eNormal;
}

bool CGSKernel::IsUserCanLogin(const string& sUserName,const std::string& sToken,INT32 nsId)
{
	if (m_sGSConfig.n32SkipBalance) return true;

	map<string,SUserToken*>::iterator it = m_UserTokenList.find(sUserName);
	if (it!=m_UserTokenList.end())
	{
		SUserToken* userToken = it->second;
		if (userToken->UserToken == sToken)
		{
			++userToken->ReconnectCount;
			if (userToken->ReconnectCount>INT_MAX)
			{
				Assert(false);
				m_UserTokenList.erase(it);//最大重练次数限制//
			}
			else
			{
				if (userToken->NetSessionID>0)
				{
					m_UserTokenListByNsId.erase(userToken->NetSessionID);
				}
				for (auto it=m_UserTokenListByNsId.begin();it!=m_UserTokenListByNsId.end();++it){
					Assert(it->second!=userToken);//检查没有对象引用它
				}
				userToken->OverTime = 0;
				userToken->NetSessionID = nsId;
				m_UserTokenListByNsId[nsId] = userToken;
			}
			return true;
		}
	}
	return false;
}

void CGSKernel::OnUserLost(INT32 nsId)
{
	auto it = m_UserTokenListByNsId.find(nsId);
	if (it!=m_UserTokenListByNsId.end())
	{
		SUserToken* userToken = it->second;
		userToken->NetSessionID = 0;
		userToken->OverTime = GetMiliSecond()+60000;
		m_UserTokenListByNsId.erase(it);
	}

	GSToCS::UserOffLine sUserOffLineToCs;
	sUserOffLineToCs.set_usernetid(nsId);
	string sData = sUserOffLineToCs.SerializeAsString();
	CGSKernel::GetStaticGSKernel().TransToCS(sData.c_str(),sData.size(),sUserOffLineToCs.mgsid(),0,0);
}

}
