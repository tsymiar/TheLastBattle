#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ICliSession.h"

class CMsgHandle_M2B : public IMsgHandle
{
public:
	CMsgHandle_M2B();
	virtual ~CMsgHandle_M2B() {}
};

class CM2BSession : public ICliSession
{
public:
	CM2BSession();
	virtual ~CM2BSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_M2B mHandles;
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_OneUserLoginToken(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
