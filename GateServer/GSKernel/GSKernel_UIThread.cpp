#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "GSKernel.h"

using namespace std;

Begin_JT_FBAll_GateServer_GSKernel

//INT32	CGSKernel::PrintGCNetInfo()
//{
//	m_cGCNetListener.PrintNetInfo();
//	return eNormal;
//}
//
//INT32	CGSKernel::PrintGSNetInfo()
//{
//	return eNormal;
//}

//INT32	CGSKernel::RegisteMsgHandler(INT32 n32ProtocolID, IMsgHandler* piHandler)
//{
//	//if (eMsgGSTOGS_End <= n32ProtocolID)
//	//{
//	//	return eEC_InvalidProtocalID;
//	//}
//
//	//m_acGSTOGSMsgRegister[n32ProtocolID].piHandler = piHandler;
//	//m_acGSTOGSMsgRegister[n32ProtocolID].pfMsgFunc = psMsgFunc;
//
//	//INT32 n32HandlerIndex = 0;
//	//BOOLEAN bRegisteSuccess = false;
//	//if (n32ProtocolID > eMsgGameCommon_GSTOGS_Begin &&  n32ProtocolID < eMsgGameCommon_GSTOGS_End)
//	//{
//	//	n32HandlerIndex = n32ProtocolID - eMsgGameCommon_GSTOGS_Begin;
//	//	m_acGSTOGSMsgRegister[n32HandlerIndex].piHandler = piHandler;
//	//	//m_acGSTOGSMsgRegister[n32HandlerIndex].pfMsgFunc = psMsgFunc;
//	//	bRegisteSuccess = true;
//	//}
//	//else if (n32ProtocolID > eMsgGameCommon_GCTOGS_Begin &&  n32ProtocolID < eMsgGameCommon_GCTOGS_End)
//	//{
//	//	n32HandlerIndex = n32ProtocolID - eMsgGameCommon_GCTOGS_Begin;
//	//	m_acGCTOGSMsgRegister[n32HandlerIndex].piHandler = piHandler;
//	//	//m_acGCTOGSMsgRegister[n32HandlerIndex].pfMsgFunc = psMsgFunc;
//	//	bRegisteSuccess = true;
//	//}
//
//	//if (!bRegisteSuccess)
//	//{
//	//	return eEC_InvalidProtocalID;
//	//}
//
//	return eNormal;
//}


////----executed in any thread--------
//INT32	CGSKernel::DispatchMsg(IMsg::PsMsgHeader psMsgHeader)
//{
//	//psMsgHeader->n32Time = c_n32MsgFromGateServer;
//	CThreadSync cts0(&m_cGSMsgQueueLock);
//	return m_piGSMsgQueue->PushBack((UINT8*)psMsgHeader, psMsgHeader->n32Size);
//}
//
//
//INT32		CGSKernel::HandleMsg(IMsg *piMsg, IMsg *piMsgRet)
//{
//	return eNormal;
//}
//
//INT32		CGSKernel::DeliverMsg(IMsg *piMsg, IMsg *piMsgRet)
//{
//	INT32 n32MsgID = piMsg->GetMsgHeader()->n32Type;
//	INT32 n32HandlerIndex = 0;
//	IMsgHandler *piHandler = NULL;
//
//	if (n32MsgID > eMsgGameCommon_TOGS_Begin &&  n32MsgID < eMsgGateServer_TOGS_End)
//	{
//		n32HandlerIndex = n32MsgID - eMsgGameCommon_TOGS_Begin;
//		piHandler = m_asGSMsgRegister[n32HandlerIndex].piHandler;
//		if (NULL != piHandler)
//		{
//			piHandler->HandleMsg(piMsg, piMsgRet);
//		}
//	}
//	else if (n32MsgID > eMsgTools_Begin &&  n32MsgID < eMsgTools_End)
//	{
//		n32HandlerIndex = n32MsgID - eMsgTools_Begin;
//		piHandler = m_asToolsMsgRegister[n32HandlerIndex].piHandler;
//		if (NULL != piHandler)
//		{
//			piHandler->HandleMsg(piMsg, piMsgRet);
//		}
//	}
//
//	return eNormal;
//}


//----export function implement--------


End_JT_FBAll_GateServer_GSKernel
