#include "../stdafx.h"
#include "GateSession.h"
#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "../ThirdFunc/MsgLists/BSToGS.pb.h"
#include "../ThirdFunc/MsgLists/GSToBS.pb.h"

CMsgHandle_Gate CGateSession::mHandles;

CMsgHandle_Gate::CMsgHandle_Gate()
	:IMsgHandle(GSToBS::eMsgToBSFromGS_Begin,GSToBS::eMsgToBSFromGS_End-GSToBS::eMsgToBSFromGS_Begin){
	RegisterMsgFunc(GSToBS::eMsgToBSFromGS_AskRegister, CGateSession::Msg_AskRegister,true);
	RegisterMsgFunc(GSToBS::eMsgToBSFromGS_ReportAllClientInfo, CGateSession::Msg_Handle_ReportGsInfo);
	RegisterMsgFunc(GSToBS::eMsgToBSFromGS_OneUserLoginTokenRet,CGateSession::Msg_Handle_OneUserLoginTokenRet);
	SetUnKnownMsgHandle(CGateSession::Msg_Handle_Dispath);
}

CGateSession::CGateSession()
{

}

CGateSession::~CGateSession()
{

}

void CGateSession::SendInitData(){
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	BSToGS::AskRegister sAskRegister;
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_BS_OnlyGS, GetID(), sAskRegister, sAskRegister.mgsid());
}

void CGateSession::OnRealEstablish()
{
	ELOG(LOG_INFO,"GS(%u) Connected.",GetLogicID());
}

void CGateSession::OnClose()
{
	map<UINT32,sOneGsInfo>::iterator findit = gAllGsInfo.find(GetLogicID());

	if (findit!=gAllGsInfo.end()) 
	{
		findit->second.gs_isLost = true;
		findit->second.gs_nets = 0;
		ELOG(LOG_INFO,"GS(%u) DisConnect.",GetLogicID());
	}
}

bool CGateSession::Msg_AskRegister(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID){
	//std::auto_ptr<GSToBS::AskRegister> sAskRegister(new GSToBS::AskRegister());
	//sAskRegister->ParseFromArray(pMsg, n32MsgLength);
	boost::shared_ptr<GSToBS::AskRegister> sAskRegister = ParseProtoMsg<GSToBS::AskRegister>(pMsg, n32MsgLength);
	if (!sAskRegister){
		return 0;
	}

	UINT32 gsid = sAskRegister->gsid();
	UINT32 gsListener = sAskRegister->listenport();

	map<UINT32, sOneGsInfo>::iterator findit = gAllGsInfo.find(gsid);

	if (findit==gAllGsInfo.end()) {
		vthis->DelayClose(1000);
		return true;
	}

	sOneGsInfo& theGsInfo = findit->second;

	if (!theGsInfo.gs_isLost){
		vthis->DelayClose(1000);
		return true;
	}

	if (theGsInfo.gs_Port!=gsListener || theGsInfo.gs_Ip!=vthis->GetRemoteIp())
	{
		vthis->DelayClose(1000);
		return true;
	}

	theGsInfo.gs_isLost = false;
	theGsInfo.gs_nets = vthis->GetID();
	vthis->SetLogicID(gsid);
	vthis->SetInited(true,true);
	return true;
}

bool CGateSession::Msg_Handle_ReportGsInfo(const char* pMsg, int n32MsgLength, INetSession* vthis , int n32MsgID){
	map<UINT32,sOneGsInfo>::iterator findit = gAllGsInfo.find(vthis->GetLogicID());

	if (findit != gAllGsInfo.end()){
		//std::auto_ptr<GSToBS::ReportAllClientInf> sMsg(new GSToBS::ReportAllClientInf);
		//sMsg->ParseFromArray(pMsg, n32MsgLength);
		boost::shared_ptr<GSToBS::ReportAllClientInf> sMsg = ParseProtoMsg<GSToBS::ReportAllClientInf>(pMsg, n32MsgLength);
		if (!sMsg){
			return 0;
		}

		//ELOG(LOG_INFO,"gs(%u) client(%u)",vthis->GetLogicID(), sMsg->tokenlist_size());
		findit->second.gs_gc_count = sMsg->tokenlist_size();
		return true;
	}
	return true;
}

bool CGateSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	return true;
}

bool CGateSession::Msg_Handle_OneUserLoginTokenRet( const char* pMsg, int n32MsgLength, INetSession* vthis , int n32MsgID )
{
	// 发送第4消息：gs允许该用户连接，告知gs地址和连接密码
	map<UINT32,sOneGsInfo>::iterator findit = gAllGsInfo.find(vthis->GetLogicID());

	if (findit != gAllGsInfo.end())
	{
		//std::auto_ptr<BSToGS::OneUserLoginToken> sMsg(new BSToGS::OneUserLoginToken);
		//bool bRet = sMsg->ParseFromArray(pMsg, n32MsgLength);
		boost::shared_ptr<BSToGS::OneUserLoginToken> sMsg = ParseProtoMsg<BSToGS::OneUserLoginToken>(pMsg, n32MsgLength);
		if (!sMsg){
			return 0;
		}
		std::auto_ptr<BSToGC::AskGateAddressRet> sMsgSend(new BSToGC::AskGateAddressRet);
		sMsgSend->set_user_name(sMsg->user_name());
		sMsgSend->set_token(sMsg->token());
		sMsgSend->set_ip(sMsg->ip());
		sMsgSend->set_port(sMsg->port());
		INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_BS_OnlyGC,sMsg->gateclient(),*sMsgSend,sMsgSend->mgsid());
		return true;
	}
	ELOG(LOG_ERROR, "Can Not Find An GS For Loginer.");
	return true;
}

