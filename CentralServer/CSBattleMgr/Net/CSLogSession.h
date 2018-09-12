#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ICliSession.h"

class CMsgHandle_CSLog : public IMsgHandle
{
public:
	CMsgHandle_CSLog();
	virtual ~CMsgHandle_CSLog() {}
};

class CCSLogSession : public ICliSession
{
public:
	CCSLogSession();
	virtual ~CCSLogSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_CSLog mHandles;
	
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
