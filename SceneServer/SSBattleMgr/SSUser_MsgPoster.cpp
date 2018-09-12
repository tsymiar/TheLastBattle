// SSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SSUser.h"
#include "SSBattleMgr.h"
#include "SSWorkThreadMgr.h"
using namespace std;

namespace SceneServer{

INT32	CSSUser::PostMsgToGC_AskRet(INT32 n32AskProtocalID, INT32 n32RetFlag){
	GSToGC::AskRet sAskRet;
	sAskRet.set_askid(n32AskProtocalID);
	sAskRet.set_errorcode(n32RetFlag);

	return CSSWorkThreadMgr::GetInstance().PostMsgToGC(m_sUserNetInfo, sAskRet, sAskRet.msgid());
}

INT32	CSSUser::PostMsgToGC(google::protobuf::Message& sMsg, int n32MsgID){
	return CSSWorkThreadMgr::GetInstance().PostMsgToGC(m_sUserNetInfo, sMsg, n32MsgID);
}

INT32	CSSUser::PostMsgToCS_UserOffline(){
	SSToCS::ReportUserOffline sMsg;
	sMsg.set_guid(m_sUserGUID.un64GUID);
	SSToCS::UserNetInfo* sUserNetInfo = new SSToCS::UserNetInfo;
	sUserNetInfo->set_gcnsid(m_sUserNetInfo.n32GCNSID);
	sUserNetInfo->set_gsid(m_sUserNetInfo.n32GSID);
	sMsg.set_allocated_usernetinfo(sUserNetInfo);

	CSSWorkThreadMgr::GetInstance().PostMsgToCS(sMsg, sMsg.mgsid());
	return eNormal;
}

INT32	CSSUser::PostMsgToCS_AskAddGold(INT64 n64Gold){
	SSToCS::AddGold sMsg;
	sMsg.set_guid(m_sUserGUID.un64GUID);
	sMsg.set_gold(n64Gold);
	CSSWorkThreadMgr::GetInstance().PostMsgToCS(sMsg, sMsg.mgsid());
	return eNormal;
}

INT32	CSSUser::PostMsgToCS_AskChangeFirstWinTime(TIME_SECOND tCurFirstWintime){
	SSToCS::AskChangeFirstWinTime sMsg;
	sMsg.set_guid(m_sUserGUID.un64GUID);
	sMsg.set_curtime(tCurFirstWintime);
	CSSWorkThreadMgr::GetInstance().PostMsgToCS(sMsg, sMsg.mgsid());
	return eNormal;
}
}