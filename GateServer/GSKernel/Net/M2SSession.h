#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ICliSession.h"

class CMsgHandle_M2S : public IMsgHandle
{
public:
	CMsgHandle_M2S();
	virtual ~CMsgHandle_M2S() {}
};

class CM2SSession : public ICliSession
{
public:
	CM2SSession();
	virtual ~CM2SSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_M2S mHandles;
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
