#include "../stdafx.h"
#include "CentralSession.h"

#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "..\SSWorkThreadMgr.h"
using namespace SceneServer;
CMsgHandle_Central CCentralSession::mHandles;

CMsgHandle_Central::CMsgHandle_Central()
	:IMsgHandle(CSToSS::eMsgToSSFromCS_Begin,CSToSS::eMsgToSSFromCS_End-CSToSS::eMsgToSSFromCS_Begin)
{
	RegisterMsgFunc(CSToSS::eMsgToSSFromCS_AskRegisteRet,  CCentralSession::Msg_Handle_Init,true);
	SetUnKnownMsgHandle(CCentralSession::Msg_Handle_Dispath);
}

CCentralSession::CCentralSession()
{

}

CCentralSession::~CCentralSession()
{

}

void CCentralSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	ELOG(LOG_INFO,"CS Connected, try to register me.");

	SSToCS::AskRegiste sMsg;
	sMsg.set_ssid(CSSWorkThreadMgr::GetInstance().GetSSConfig().n32SSID);
	sMsg.set_ip(CSSWorkThreadMgr::GetInstance().GetSSConfig().sGSListenIP.aszIPAddr, sizeof(SIPAddr));
	sMsg.set_port(CSSWorkThreadMgr::GetInstance().GetSSConfig().n32GSListenPort);
	sMsg.set_userpwd(CSSWorkThreadMgr::GetInstance().GetSSConfig().aszUserPwd, strlen(CSSWorkThreadMgr::GetInstance().GetSSConfig().aszUserPwd));
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_CLIENT_S2C, 0, sMsg, sMsg.mgsid());
}

void CCentralSession::OnRealEstablish(){
	ELOG(LOG_INFO, "CS Connected and register ok, try to ask global config.");
	CSSWorkThreadMgr::GetInstance().OnEvent(EVT_ON_CS_REGISTERED, NULL);
	CSSWorkThreadMgr::GetInstance().OnEvent(EVT_SS2CS_SyncAllGameInfo, NULL);
}

void CCentralSession::OnClose(){
	ELOG(LOG_INFO, "CS DisConnect.");
}

bool CCentralSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	std::auto_ptr<CSToSS::AskRegisteRet> pAskRegisteRet(new CSToSS::AskRegisteRet);
	pAskRegisteRet->ParseFromArray(pMsg, n32MsgLength);
	if (eNormal != pAskRegisteRet->ret()){
		ELOG(LOG_ERROR, "Login to central server fail with error code %d \nPlease resolve this proper and restart again!", pAskRegisteRet->ret());
		return false;
	}
	TIME_MILSEC tSelfUTCMilSec = GetUTCMiliSecond();
	CSSWorkThreadMgr::GetInstance().m_tCSTimeError = pAskRegisteRet->time() - tSelfUTCMilSec;
	CSSWorkThreadMgr::GetInstance().m_un32GSBaseIdx = pAskRegisteRet->basegsid();
	if (CSSWorkThreadMgr::GetInstance().m_un32MaxGSNum!=0){
		// it had parsed when in the first connected.
		Assert(CSSWorkThreadMgr::GetInstance().m_un32MaxGSNum==pAskRegisteRet->gsinfo_size());
	}
	else{
		CSSWorkThreadMgr::GetInstance().m_un32MaxGSNum = pAskRegisteRet->gsinfo_size();
		UINT32 un32DefinedGSNum = 0;
		if(CSSWorkThreadMgr::GetInstance().m_un32MaxGSNum > 100000)
		{
			ELOG(LOG_WARNNING, "unimagable GSNum %d.", CSSWorkThreadMgr::GetInstance().m_un32MaxGSNum);
			return  false;
		}

		if (0 < CSSWorkThreadMgr::GetInstance().m_un32MaxGSNum)
		{
			CHAR szPass[c_n32DefaultUserPwdLen + 1];

			for(UINT32 i = 0; i < CSSWorkThreadMgr::GetInstance().m_un32MaxGSNum; i++){
				const CSToSS::AskRegisteRet_GSInfo& pGSInfo = pAskRegisteRet->gsinfo(i);
				// ÏûÏ¢½âÂë//
				INT32 n32GSID = pGSInfo.gsid();
				if (0 == n32GSID) continue;
				un32DefinedGSNum++;
				CSSGSInfo* pInfo = new CSSGSInfo;
				pInfo->m_n32GSID = n32GSID;
				memcpy(pInfo->m_szUserPwd, pGSInfo.userpwd().c_str(), pGSInfo.userpwd().size());
				CSSWorkThreadMgr::GetInstance().m_pcGSInfoList[n32GSID] = pInfo;
			}
			ELOG(LOG_INFO, "GSKernel Start success, Got %d GSInfo and %d GS defined, Milsec error %lld.", 
				CSSWorkThreadMgr::GetInstance().m_un32MaxGSNum, un32DefinedGSNum, CSSWorkThreadMgr::GetInstance().m_tCSTimeError);
		}
	}

	vthis->SetInited(true,true);
	return true;
}

bool CCentralSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	CSSWorkThreadMgr::GetInstance().SendCSMsgToLogicThread(pMsg, n32MsgLength, n32MsgID);
	return true;
}
