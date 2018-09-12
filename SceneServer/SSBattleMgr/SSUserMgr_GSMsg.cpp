// SSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SSUserMgr.h"
#include "SSUser.h"
#include "SSLoggingHeader.h"
using namespace std;

Begin_JT_FBAll_SceneServer_SSUserMgr

extern CSSUserMgr g_cSSUserMgrInstance;

INT32	CSSUserMgr::OnMsgFromGS(ISSGSInfo *piGSInfo, const char* pMsg, int n32MsgLength, int n32MsgID){
	if (n32MsgID <= GSToSS::eMsgToSSFromGS_Begin || n32MsgID >= GSToSS::eMsgToSSFromGS_End){
		return eEC_InvalidMsgProtocalID;
	}

	INT32 n32Index = n32MsgID - GSToSS::eMsgToSSFromGS_Begin;
	SSSGSMsgHandler psHandler = m_asGSMsgHandler[n32Index];
	if (NULL != psHandler.pfHandler){
		return (this->*psHandler.pfHandler)(piGSInfo, pMsg, n32MsgLength);
	}

	return eEC_NullMsgHandler;
}

End_JT_FBAll_SceneServer_SSUserMgr