// SSBattleMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SSBattleMgr.h"

#include "SSUser.h"
#include "SSWorkThreadMgr.h"

using namespace std;

namespace SceneServer{

INT32	CSSBattleMgr::OnMsgFromGS(ISSGSInfo *piGSInfo, const char* pData, int n32DataLength, int n32MsgID){
	GSMsgHandlerMap::iterator iter = m_asGSMsgHandler.find(n32MsgID);
	if (m_asGSMsgHandler.end() != iter){
		return (this->*iter->second)(piGSInfo, pData, n32DataLength);
	}
	return eEC_NullMsgHandler;
}

}