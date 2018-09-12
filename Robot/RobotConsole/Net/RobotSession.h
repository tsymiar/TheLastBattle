#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ICliSession.h"

class MessageLite;

class CMsgHandle_Central : public IMsgHandle
{
public:
	CMsgHandle_Central();
	virtual ~CMsgHandle_Central() {}
};

class CRobotSession : public ICliSession
{
public:
	CRobotSession();
	virtual ~CRobotSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_Central mHandles;
	static MessageLite* Msg_Create_None() { return NULL; }
	//pb±ä¶¯
	//static bool Msg_Handle_Init(MessageLite* pBaseMsg,INetSession* vthis);
	//static bool Msg_Handle_Dispath(MessageLite* pBaseMsg,INetSession* vthis);
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	bool mLogicInited;
	bool mConnected;
};
