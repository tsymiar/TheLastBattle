#pragma once

#include "../../Share/Net/IMsgHandle.h"
#include "../../Share/Net/ISrvCliSession.h"

class CMsgHandle_Gate : public IMsgHandle
{
public:
	CMsgHandle_Gate();
	virtual ~CMsgHandle_Gate() {}
};

class CGateSession : public ISrvCliSession
{
public:
	CGateSession();
	virtual ~CGateSession();
protected:
	virtual void SendInitData();
	virtual void OnRealEstablish();
	virtual void OnClose();
public:
	static CMsgHandle_Gate mHandles;
	
	static bool Msg_Handle_Init(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
	static bool Msg_Handle_Dispath(const char* pMsg, int n32MsgLength, INetSession* vthis, int n32MsgID);
};
