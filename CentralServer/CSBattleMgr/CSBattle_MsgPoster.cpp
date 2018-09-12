#include "stdafx.h"
#include "CSBattle.h"
#include "CSBattleMgr.h"

#include "CSUser.h"
#include "CSKernel.h"

namespace CentralServer{

INT32 CCSBattle::BroadcastMsgToAllUser(google::protobuf::Message& sMsg, int n32MsgID){
	for (auto it=m_UserMap.begin();it!=m_UserMap.end(); ++it){
		it->second->PostMsgToGC(sMsg, n32MsgID);
	}
	return eNormal;
}

INT32 CCSBattle::PostMsgToBattleSS(google::protobuf::Message& sMsg, int n32MsgID){
	ICSSSInfo *piSSInfo = GetCSKernelInstance()->GetSSInfoBySSID(m_n32SSID);
	if (NULL == piSSInfo){
		return eEC_InvalidSSID;
	}
	return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, n32MsgID);
}

INT32 CCSBattle::PostMsgToSS_NotifyUserIsOnline(CCSUser *pUser, bool isOnline){
	ICSSSInfo *piSSInfo = GetCSKernelInstance()->GetSSInfoBySSID(m_n32SSID);
	if (NULL == piSSInfo){
		return eEC_NullBattle;
	}
	if (isOnline){
		CSToSS::NotifyUserOnline sMsg;
		sMsg.set_battleid(m_BattleID);
		sMsg.set_userguid(pUser->GetGUID());
		sMsg.mutable_usernetinfo()->set_n32gsid(pUser->GetUserNetInfo().n32GSID);
		sMsg.mutable_usernetinfo()->set_n32gcnsid(pUser->GetUserNetInfo().n32GCNSID);
		GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
	}
	else{
		CSToSS::NotifyUserOffline sMsg;
		sMsg.set_battleid(m_BattleID);
		sMsg.set_userguid(pUser->GetGUID());
		return GetCSKernelInstance()->PostMsgToSS(piSSInfo, sMsg, sMsg.msgid());
	}
	return eNormal;
}

}