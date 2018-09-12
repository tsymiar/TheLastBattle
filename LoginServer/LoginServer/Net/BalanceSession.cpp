#include "../stdafx.h"
#include "BalanceSession.h"
#include "..\..\..\Share\Net\INetSessionMgr.h"
#include "../SdkConnector.h"

CMsgHandle_Balance CBalanceSession::mHandles;
static vector<sOneBsInfo> m_BS_List;

CMsgHandle_Balance::CMsgHandle_Balance()
	:IMsgHandle(BSToLS::eMsgToLSFromBS_Begin,BSToLS::eMsgToLSFromBS_End-BSToLS::eMsgToLSFromBS_Begin)
{
	RegisterMsgFunc(BSToLS::eMsgToLSFromBS_AskRegister, CBalanceSession::Msg_Handle_Init,true);
	RegisterMsgFunc(BSToLS::eMsgToLSFromBC_OneClinetLoginCheck, CBalanceSession::Msg_Handle_OneClientLoginCheck);
	SetUnKnownMsgHandle(CBalanceSession::Msg_Handle_Dispath);
}

CBalanceSession::CBalanceSession()
{
	m_BS_List.clear();
}

CBalanceSession::~CBalanceSession()
{
	m_BS_List.clear();
}

void CBalanceSession::SendInitData()
{
	// send init data to remote and a stop message at the end.
	// you can't send anything until you recv remote init messages.
	LSToBS::AskRegisterRet sMsg;
	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_LS_OnlyBS, GetID(), sMsg,  sMsg.msgid());
}

void CBalanceSession::OnRealEstablish()
{
	ELOG(LOG_INFO,"BS(%u) Connected.",GetLogicID());
}

void CBalanceSession::OnClose()
{
	ELOG(LOG_INFO,"BS(%u) DisConnected.",GetLogicID());
}

bool CBalanceSession::Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	vthis->SetInited(true,true);

// 	std::auto_ptr<BSToLS::AskRegister> pRegister(new BSToLS::AskRegister);
// 	pRegister->ParseFromArray(pMsg, n32MsgLength);
// 
// 	bool IfInVec = false;
// 	vector<sOneBsInfo>::iterator iter = m_BS_List.begin();
// 	for(; iter != m_BS_List.end(); iter ++){
// 		if (0 == strcmp((iter->bs_IpExport).c_str(), pRegister->bsip().c_str())){
// 			IfInVec = true;
// 		}
// 	}
// 
// 	if (!IfInVec){
// 		sOneBsInfo temp;
// 		memset(&temp, 0, sizeof(temp));
// 		temp.bs_IpExport = pRegister->bsip();
// 		temp.bs_Port = pRegister->port();
// 		m_BS_List.push_back(temp);
// 	}

	return true;
}

bool CBalanceSession::Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID)
{
	return true;
}

bool CBalanceSession::Msg_Handle_OneClientLoginCheck( const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID )
{
	std::auto_ptr<GCToBS::OneClinetLogin> sOneClientLogin(new GCToBS::OneClinetLogin);

	bool bRet = sOneClientLogin->ParseFromArray(pMsg,n32MsgLength);

	if (!bRet){
		ELOG(LOG_ERROR, "Fail with Msg Error.");
		return true;
	}

	LoginUserMap& lAllLoginUserInfo = SdkConnector::GetInstance().GetLoginUserMap();

	string sessionid = "";
	if (sOneClientLogin->plat() == ePlatform_PC){
		stringstream ss;
		ss<<sOneClientLogin->plat();
		ss<<sOneClientLogin->uin();
		ss>>sessionid;
	}
	else{
		sessionid = sOneClientLogin->sessionid();
	}
	auto it = lAllLoginUserInfo.find(sessionid);
	if (it != lAllLoginUserInfo.end()){
		sOneClientLogin->set_login_success(true);
		sOneClientLogin->set_uin(it->second.uin);
		lAllLoginUserInfo.erase(it);
		ELOG(LOG_DBBUG, "Erase uid:%s", sOneClientLogin->uin().c_str());
	}
	else{
		ELOG(LOG_WARNNING, "Fail With User with uin(%s) Not Find.", sOneClientLogin->uin().c_str());
		sOneClientLogin->set_login_success(false);
	}

	INetSessionMgr::GetInstance()->SendMsgToSession(ST_SERVER_LS_OnlyBS,vthis->GetID(),*sOneClientLogin, LSToBS::eMsgToBSFromLS_OneClinetLoginCheckRet);

	return true;
}

