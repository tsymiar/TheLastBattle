#include "../stdafx.h"
#include "B2LSession.h"
#include "..\..\..\Share\Net\INetSessionMgr.h"

CMsgHandle_B2L CB2LSession::mHandles;

CMsgHandle_B2L::CMsgHandle_B2L()
	:IMsgHandle(LSToBS::eMsgToBSFromLS_Begin,LSToBS::eMsgToBSFromLS_End-LSToBS::eMsgToBSFromLS_Begin)
{
	RegisterMsgFunc(LSToBS::eMsgToBSFromLS_AskRegisterRet, CB2LSession::Msg_Handle_Init,true);
	RegisterMsgFunc(LSToBS::eMsgToBSFromLS_OneClinetLoginCheckRet, CB2LSession::Msg_User_Login,true);
	SetUnKnownMsgHandle(CB2LSession::Msg_Handle_Dispath);
}

CB2LSession::CB2LSession()
{

}

CB2LSession::~CB2LSession()
{

}

void CB2LSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	ELOG(LOG_INFO,"LS Connected, try to register me.");
	BSToLS::AskRegister sMsg;
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_CLIENT_B2L, GetID(), sMsg, sMsg.msgid());
}

void CB2LSession::OnRealEstablish()
{
	ELOG(LOG_INFO,"LS Connected and register ok");
}

void CB2LSession::OnClose()
{
	ELOG(LOG_INFO,"LS DisConnect.");
}

bool CB2LSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	// don't send any message until it init success.
	vthis->SetInited(true,true);

	return true;
}

bool CB2LSession::Msg_User_Login(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	std::auto_ptr<GCToBS::OneClinetLogin> sUserLoginInfo(new GCToBS::OneClinetLogin);
	sUserLoginInfo->ParseFromArray(pMsg, n32MsgLength);

	// 发送第3消息：用户验证是否成功，如果验证成功，请求分配gs给用户
	BSToGC::ClinetLoginCheckRet sMsg;
	sMsg.set_login_success(sUserLoginInfo->login_success());
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_BS_OnlyGC,sUserLoginInfo->nsid(),sMsg, sMsg.mgsid());
	ELOG(LOG_SpecialDebug,"user(%s)(%s)(%u) ask login ret",sUserLoginInfo->uin().c_str(),sUserLoginInfo->sessionid().c_str(),sUserLoginInfo->nsid());

	if (sUserLoginInfo->login_success()==false){
		INetSessionMgr::GetInstance()->DisconnectOne(sUserLoginInfo->nsid());
	}
	else{
		sOneGsInfo* pLittleOne = NULL;
		for (map<UINT32, sOneGsInfo>::iterator it = gAllGsInfo.begin(); it != gAllGsInfo.end(); ++it){
			sOneGsInfo* theGsInfo = &(it->second);
			if (theGsInfo->gs_isLost) 
				continue;
			if (pLittleOne==NULL || theGsInfo->gs_gc_count < pLittleOne->gs_gc_count){
				pLittleOne = theGsInfo;
			}
		}

		if (!pLittleOne){
			return false;
		}
		++pLittleOne->gs_gc_count;//only ++ as cache.

		GUID guid;
		CoCreateGuid(&guid);
		char guidStr[100] = {0};
		sprintf_s(guidStr,"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

		BSToGS::OneUserLoginToken sOneUserLoginToken;
		sOneUserLoginToken.set_gateclient(sUserLoginInfo->nsid());
		sOneUserLoginToken.set_token(guidStr, strlen(guidStr));
		sOneUserLoginToken.set_user_name(sUserLoginInfo->uin());
		sOneUserLoginToken.set_ip(pLittleOne->gs_IpExport.c_str(), pLittleOne->gs_IpExport.size());
		sOneUserLoginToken.set_port(pLittleOne->gs_Port);
		INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_BS_OnlyGS, pLittleOne->gs_nets,sOneUserLoginToken, sOneUserLoginToken.msgid());
	}

	return true;
}

bool CB2LSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	return true;
}
