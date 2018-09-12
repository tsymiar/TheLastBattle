#include "stdafx.h"
#include "GSKernel.h"
#include <iostream>
#include <fstream>
#include "GSSSInfo.h"

using namespace std;

namespace GateServer{


	INT32	CGSKernel::OnMsgFromSS(CGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength, int n32MsgID, int n32TransID, int n32GcNetID){
		if (n32TransID < SSToGS::eMsgToGSFromSS_Begin || n32TransID >= SSToGS::eMsgToGSFromSS_End){
			return eEC_InvalidMsgProtocalID;
		}
		if (n32TransID==SSToGS::eMsgToGSFromSS_OrderPostToGC){
			if (n32GcNetID==0){
				BroadcastToGameClient(pMsg,n32MsgLength,n32MsgID);
			}
			else{
				if (n32MsgID==GSToGC::eMsgToGCFromGS_NotifyBattleBaseInfo){
					m_User2SSInfoMap[n32GcNetID] = piSSInfo;
				}
				PostToGameClient(n32GcNetID,pMsg,n32MsgLength,n32MsgID);
			}
		}
		else{
			INT32	n32Index = n32TransID - SSToGS::eMsgToGSFromSS_Begin;
			PFSSMsgHandler rsHandler = m_asSSMsgHandler[n32Index];
			if ( NULL != rsHandler){
				return (this->*rsHandler)(piSSInfo, pMsg, n32MsgLength);
			}
		}

		return eEC_NullMsgHandler;
}


INT32	CGSKernel::OnMsgFromSS_AskPingRet(CGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength){
	if (NULL == piSSInfo){
		return eEC_NullPointer;
	}

	SSToGS::AskPingRet pPingRet;
	if (!ParseProtoMsg(pMsg, n32MsgLength, pPingRet)){
		return 0;
	}

	TIME_MILSEC tCurUTCMilsec = GetMiliSecond();
	TIME_TICK tTickSpan = tCurUTCMilsec - pPingRet.time();

	ELOG(LOG_INFO, "Ping SS %d returned, Tick span %lld. ", piSSInfo->GetSSID(), tTickSpan);

	return eNormal;
}

INT32	CGSKernel::OnMsgFromSS_OrderKickoutGC(CGSSSInfo *piSSInfo, const char* pMsg, int n32MsgLength){
	SSToGS::OrderKickoutGC pOrderKickoutGC;
	if (!ParseProtoMsg(pMsg, n32MsgLength, pOrderKickoutGC)){
		return 0;
	}

	PostGameClientDisconnect(pOrderKickoutGC.gsnid());
	return eNormal;
}

}
