// CSUserMgr.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSUserMgr.h"
#include "CSUser.h"

#include "CSCfgMgr.h"

using namespace std;

namespace CentralServer{

INT32 CCSUserMgr::OnMsgFromGS_UserOffline(ICSGSInfo *piGSInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<GSToCS::UserOffLine> sMsp = ParseProtoMsg<GSToCS::UserOffLine>(pData, n32DataLength);
	if (!sMsp){
		return eNormal;
	}
	CCSUser *pUser = GetUserByNetInfo(piGSInfo, sMsp->usernetid());
	if (NULL != pUser){
		pUser->OnOffline();

		if (pUser->GetUserBattleInfoEx().GetBattleState() == eBattleState_Free){
			RemoveUser(pUser);
		}
	}
	return eNormal;
}

}