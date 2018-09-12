#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "CSBattleMgr.h"
#include "CSUserMgr.h"
#include "CSBattle.h"
#include "CSKernel.h"

using namespace std;

namespace CentralServer{

INT32 CCSBattleMgr::OnMsgFromRC_AskServerInfo(ICSRCInfo *piRCInfo, const char* pData, int n32DataLength){
	boost::shared_ptr<RCToCS::AskServerInfo> sMsp = ParseProtoMsg<RCToCS::AskServerInfo>(pData, n32DataLength);
	if (!sMsp){
		ELOG(LOG_ERROR,"---error: RCToCS::AskServerInfo---");
		return 0;
	}
	//ELOG(LOG_INFO,"===from gm tool askserverinfo msg==");
	CSToRC::AllServerInfo sMsg;
	INT32 ssNum = 0;
	INT32 csNum = m_BattleRoomList.size();
	for(auto iter = m_cAllBattleMap.begin(); iter!=m_cAllBattleMap.end(); iter++){
		CCSBattle* pBattle = iter->second;
		if(pBattle == NULL) continue;
		pBattle->IfAsync()?csNum++:ssNum++;
	}
	sMsg.set_roomusernum(csNum);
	sMsg.set_battleusernum(ssNum);
	sMsg.set_allusernum(GetCSUserMgrInstance()->GetCurtTotalServerUser()); 
	sMsg.set_curttime(CFunction::GetCurtFormatTime(time(NULL)));  
	return GetCSKernelInstance()->PostMsgToRC(piRCInfo, sMsg, sMsg.msgid());
}

}